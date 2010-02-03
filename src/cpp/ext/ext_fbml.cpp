/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 Facebook, Inc. (http://www.facebook.com)          |
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include <cpp/ext/ext_fbml.h>
#include <fbml.h>
#include <slimmer.h>
#include <cpp/base/resource_data.h>

using namespace std;

#define MAXTAGSIZE 512

namespace HPHP {

static ReadWriteMutex s_mutex;
///////////////////////////////////////////////////////////////////////////////
// helpers

class FBMLTree : public SweepableResourceData {
public:
  DECLARE_OBJECT_ALLOCATION(FBMLTree);

  FBMLTree(fbml_node *tree) : m_tree(tree) {
  }

  virtual ~FBMLTree() {
    if (m_tree) {
      fbml_node_free(m_tree);
      m_tree = NULL;
    }
  }

  // overriding ResourceData
  const char *o_getClassName() const { return "FBMLTree";}

private:
  fbml_node *m_tree;
};
IMPLEMENT_OBJECT_ALLOCATION(FBMLTree);

/**
 * This class doesn't need to be SweepableResourceData, as there is nothing
 * in its data members to free during garbage collection time, nor itself.
 * Both m_tree and itself are object allocation that will be collected
 * automatically. This is a very special case.
 */
class FBMLNode : public ResourceData {
public:
  DECLARE_OBJECT_ALLOCATION(FBMLNode);

  FBMLNode(fbml_node *node, CObjRef tree) : m_node(node), m_tree(tree) {
  }

  // overriding ResourceData
  const char *o_getClassName() const { return "FBMLNode";}

  fbml_node *m_node;
  Object m_tree; // this is just to make sure m_node is not free-d by anyone
};
IMPLEMENT_OBJECT_ALLOCATION(FBMLNode);

static void copy_hash_to_strings(CArrRef contains_hash, char ** strings,
                                 vector<String> &holder) {
  int i = 0;
  for (ArrayIter iter(contains_hash); iter; ++iter) {
    String s = iter.second().toString();
    holder.push_back(s); // so "s" won't be deallocated
    strings[i++] = (char*)s.data();
  }
  strings[i] = NULL;
}

static fbml_schema *make_fbml_schema() {
  fbml_schema *in_schema = new fbml_schema();
  in_schema->illegal_children = (char**)malloc(MAXTAGSIZE*sizeof(char*));
  in_schema->illegal_children_attr = (char**)malloc(MAXTAGSIZE*sizeof(char*));
  return in_schema;
}

static void free_fbml_schema(fbml_schema *schema) {
  free(schema->illegal_children);
  free(schema->illegal_children_attr);
  delete schema;
}

static fbml_context_schema *
make_fbml_context_schema(char * key, fbml_schema * in_schema) {
  fbml_context_schema *new_context = new fbml_context_schema();
  new_context->context_tag = key;
  new_context->schema = (fbml_schema **)malloc(2 * sizeof(fbml_schema*));
  new_context->schema[0] = in_schema;
  new_context->schema[1] = NULL;
  return new_context;
}

static void free_fbml_context_schema(fbml_context_schema *context) {
  free_fbml_schema(context->schema[0]);
  free(context->schema);
  delete context;
}

///////////////////////////////////////////////////////////////////////////////
// sanitizers

struct callback_parameter {
  String  user_func_name;
  Variant user_data;
  const char *fallback; /* what string to return when a failure occurs */

  String  id_prefix;
  String  id_selector;
};

static char *callback_user_function(char *input, void *data) {
  struct callback_parameter *param = (struct callback_parameter*)data;

  Array params;
  params.append(param->user_data);
  params.append(input);
  Variant vret = invoke(param->user_func_name.data(), params);
  String sret = vret.toString();
  const char *ret = sret.data();

  return strdup((ret && *ret) ? ret : param->fallback);
}

static char *callback_attr_function(char *tag, char *attr, char *val,
                                    void *data) {
  struct callback_parameter *param = (struct callback_parameter*)data;

  Array params;
  params.append(param->user_data);
  params.append(tag);
  params.append(attr);
  params.append(val);
  Variant ret = invoke(param->user_func_name.data(), params);

  return strdup(ret.toString().data());
}
struct node_callback_parameter {
  String  user_func_name;
  Variant user_data;
  Object  tree;
  const char *fallback; /* what string to return when a failure occurs */
};

static char *callback_node_function(struct fbml_node *node, void *data) {
  node_callback_parameter *param = (node_callback_parameter*)data;

  Array params;
  params.append(param->user_data);
  params.append(Object(NEW(FBMLNode)(node, param->tree)));
  Variant ret = invoke(param->user_func_name.data(), params);

  return strdup(ret.toString().data());
}

static fbml_css_sanitizer *
get_css_sanitizer(CVarRef sanitizer, callback_parameter *param,
                  fbml_css_sanitizer *css_sanitizer) {
  if (sanitizer.isNull()) return NULL;

  Variant v = sanitizer["func"];
  if (!v.isNull()) {
    param->user_func_name = v.toString();
  } else {
    param->user_func_name = "fbml_css_translate_url";
  }
  v = sanitizer["data"];
  if (!v.isNull()) {
    param->user_data = v;
  }
  v = sanitizer["prefix"];
  if (!v.isNull()) {
    String s = v.toString();
    param->id_prefix = s;
    css_sanitizer->container_selector = (char*)s.data();
  } else {
    css_sanitizer->container_selector = NULL;
  }
  v = sanitizer["id_selector"];
  if (!v.isNull()) {
    String s = v.toString();
    param->id_selector = s;
    css_sanitizer->identifier_prefix = (char*)s.data();
  } else {
    css_sanitizer->identifier_prefix = NULL;
  }

  param->fallback = "\"\"";
  css_sanitizer->pfunc_url_translator = callback_user_function;
  css_sanitizer->url_translate_data = param;
  return css_sanitizer;
}

static fbml_js_sanitizer *
get_js_sanitizer(CVarRef sanitizer, callback_parameter *param,
                 fbml_js_sanitizer *js_sanitizer) {
  fbml_js_sanitizer_init(js_sanitizer);

  if (sanitizer.isNull() || sanitizer.toArray().empty()) return NULL;

  // non-array or empty array is treated as no sanitization
  Variant v = sanitizer["func"];
  if (!v.isNull()) {
    param->user_func_name = v.toString();
  } else {
    param->user_func_name = "fbml_js_translate_event_handler";
  }
  v = sanitizer["data"];
  if (!v.isNull()) {
    param->user_data = v;
  }
  v = sanitizer["prefix"];
  if (!v.isNull()) {
    String s = v.toString();
    param->id_prefix = s;
    js_sanitizer->identifier_prefix = (char*)s.data();
  }
  v = sanitizer["that"];
  if (!v.isNull()) {
    String s = v.toString();
    param->id_selector = s; // all we need is to hold the String
    js_sanitizer->this_replacement = (char*)s.data();
  }
  param->fallback = "";
  js_sanitizer->pfunc_eh_translator = callback_user_function;
  js_sanitizer->eh_translate_data = param;
  return js_sanitizer;
}

static fbml_attr_rewriter *
get_attr_rewriter(CVarRef rewriter, callback_parameter *param,
                  fbml_attr_rewriter *attr_rewriter) {
  if (rewriter.isNull()) return NULL;

  Variant v = rewriter["func"];
  if (!v.isNull()) {
    param->user_func_name = v.toString();
  } else {
    param->user_func_name = "fbml_rewrite_event_handler";
  }
  v = rewriter["data"];
  if (!v.isNull()) {
    param->user_data = v;
  }
  param->fallback = "";
  attr_rewriter->pfunc_rewriter = callback_attr_function;
  attr_rewriter->rewrite_data = param;
  return attr_rewriter;
}

///////////////////////////////////////////////////////////////////////////////
// fbml

bool f_fbml_tag_list_expanded_11() {
  return fbml_tag_list_expanded();
}

void f_fbml_complex_expand_tag_list_11(CArrRef new_tags, CArrRef new_attrs,
                                       CArrRef special_html, CArrRef precache,
                                       CArrRef style, CArrRef style_attrs,
                                       CArrRef script_attrs,
                                       CArrRef rewrite_attrs,
                                       CArrRef special_attrs,
                                       CArrRef schema) {
  vector<String> holder; // hold Strings so interal char* will live longer
  holder.reserve(1024);

  // Extension Tags
  char *tags[MAXTAGSIZE];
  copy_hash_to_strings(new_tags, tags, holder);

  // Extension Attrs
  char *attrs[MAXTAGSIZE];
  copy_hash_to_strings(new_attrs, attrs, holder);

  // special, precache, style, macros
  fbml_flaggable_tags *flagged_tags_pointers[5];
  fbml_flaggable_tags flagged_tags[4];
  char *tags_tags[4][MAXTAGSIZE];

#define MAKE_TAG_FLAGS(i, hash, TAG)                            \
  copy_hash_to_strings(hash, tags_tags[i], holder);             \
  flagged_tags[i].tags = tags_tags[i];                          \
  flagged_tags[i].flag = TAG;                                   \
  flagged_tags_pointers[i] = &flagged_tags[i];                  \

  MAKE_TAG_FLAGS(0, special_html,  FB_FLAG_SPECIAL_HTML);
  MAKE_TAG_FLAGS(1, precache,      FB_FLAG_PRECACHE);
  MAKE_TAG_FLAGS(2, new_tags,      FB_FLAG_FBNODE);
  MAKE_TAG_FLAGS(3, style,         FB_FLAG_STYLE);
  flagged_tags_pointers[4] = NULL;

  // style, script, special, rewrite
  fbml_flaggable_attrs *flagged_attrs_pointers[5];
  fbml_flaggable_attrs flagged_attrs[4];
  char *attrs_tags[4][MAXTAGSIZE];

#define MAKE_ATTR_FLAGS(i, hash, TAG)                            \
  copy_hash_to_strings(hash, attrs_tags[i], holder);             \
  flagged_attrs[i].attrs = attrs_tags[i];                        \
  flagged_attrs[i].flag = TAG;                                   \
  flagged_attrs_pointers[i] = &flagged_attrs[i];                 \

  MAKE_ATTR_FLAGS(0, special_attrs, FB_FLAG_ATTR_SPECIAL);
  MAKE_ATTR_FLAGS(1, rewrite_attrs, FB_FLAG_ATTR_REWRITE);
  MAKE_ATTR_FLAGS(2, script_attrs,  FB_FLAG_ATTR_SCRIPT);
  MAKE_ATTR_FLAGS(3, style_attrs,   FB_FLAG_ATTR_STYLE);
  flagged_attrs_pointers[4] = NULL;

  // schema - double loop {"context_tag" -> [tags]}
  fbml_context_schema *context_schema[MAXTAGSIZE];
  int i = 0;
  for (ArrayIter iter(schema); iter; ++iter) {
    fbml_schema *in_schema = make_fbml_schema();

    int n_children = 0;
    int n_attr = 0;

    Array arrTags = iter.second();
    for (ArrayIter iterTag(arrTags); iterTag; ++iterTag) {
      String tag = iterTag.second();
      holder.push_back(tag);
      const char *s = tag.data();
      if (s[0] != '_') {
        in_schema->illegal_children[n_children++] = (char*)s;
      } else {
        in_schema->illegal_children_attr[n_attr++] = (char*)(s + 1);
      }
    }
    in_schema->illegal_children[n_children] = NULL;
    in_schema->illegal_children_attr[n_attr] = NULL;

    String key = iter.first();
    holder.push_back(key);
    in_schema->ancestor_tag = (char*)key.data();
    context_schema[i] = make_fbml_context_schema((char*)key.data(), in_schema);
    i++;
  }
  context_schema[i] = NULL;

  // do it
  {
    WriteLock lock(s_mutex);
    fbml_expand_tag_list(tags, attrs,
                         flagged_tags_pointers, flagged_attrs_pointers,
                         context_schema);
  }

  // cleanup
  for (fbml_context_schema **p = context_schema; *p; ++p) {
    free_fbml_context_schema(*p);
  }
}

Array f_fbml_parse_opaque_11(CStrRef unsanitized_fbml, bool body_only,
                             bool preserve_comment,
                             bool internal_mode /* = false */,
                             CVarRef css_sanitizer /* = null_variant */,
                             CVarRef js_sanitizer /* = null_variant */,
                             CVarRef attr_rewriter /* = null_variant */) {
  callback_parameter css_param;
  fbml_css_sanitizer css_sanitizer_data;
  callback_parameter js_param;
  fbml_js_sanitizer js_sanitizer_data;
  callback_parameter attr_param;
  fbml_attr_rewriter attr_rewriter_data;
  fbml_node *tree;
  char *error = NULL;
  {
    ReadLock lock(s_mutex);
    fbml_parse((char*)unsanitized_fbml.data(), body_only, preserve_comment,
               internal_mode,
               get_css_sanitizer(css_sanitizer, &css_param,
                                 &css_sanitizer_data),
               get_js_sanitizer(js_sanitizer, &js_param,
                                &js_sanitizer_data),
               get_attr_rewriter(attr_rewriter, &attr_param,
                                 &attr_rewriter_data),
               NULL, NULL,
               &tree, &error);
  }

  fbml_node *root_node = tree;
  if (body_only && tree->children_count == 1) {
    root_node = tree->children[0];
  }

  Array ret;
  ret.set("root", Object(NEW(FBMLNode)(root_node,
                                       Object(NEW(FBMLTree)(tree)))));

  if (error) {
    if (*error) {
      ret.set("error", String(error, AttachString));
    } else {
      free(error);
    }
  }
  return ret;
}

Array f_fbml_sanitize_css_11(CStrRef unsanitized_css, bool decl_only,
                             int line_number, CArrRef css_sanitizer) {
  struct callback_parameter param;
  struct fbml_css_sanitizer sanitizer;
  char *sanitized_css = NULL;
  char *error = NULL;
  {
    ReadLock lock(s_mutex);
    fbml_sanitize_css((char*)unsanitized_css.data(), decl_only, line_number,
                      get_css_sanitizer(css_sanitizer, &param, &sanitizer),
                      &sanitized_css, &error);
  }

  Array ret;
  if (sanitized_css) {
    ret.set("sanitized", String(sanitized_css, AttachString));
  }
  if (error) {
    if (*error) {
      ret.set("error", String(error, AttachString));
    } else {
      free(error);
    }
  }
  return ret;
}

Array f_fbml_sanitize_js_11(CStrRef unsanitized_js, int line_number,
                            CArrRef js_sanitizer) {
  struct callback_parameter param;
  struct fbml_js_sanitizer sanitizer;
  char *sanitized_js = NULL;
  char *error = NULL;
  {
    ReadLock lock(s_mutex);
    fbml_sanitize_js((char*)unsanitized_js.data(), unsanitized_js.size(),
                     0, line_number,
                     get_js_sanitizer(js_sanitizer, &param, &sanitizer),
                     &sanitized_js, &error);
  }

  Array ret;
  if (sanitized_js) {
    ret.set("sanitized", String(sanitized_js, AttachString));
  }
  if (error) {
    if (*error) {
      ret.set("error", String(error, AttachString));
    } else {
      free(error);
    }
  }
  return ret;
}

String f_fbml_get_tag_name_11(CObjRef node) {
  return String(node.getTyped<FBMLNode>()->m_node->tag_name, CopyString);
}

Array f_fbml_get_children_11(CObjRef node) {
  FBMLNode *fnode = node.getTyped<FBMLNode>();
  fbml_node *p = fnode->m_node;

  Array ret;
  for (int i = 0 ; i < p->children_count; i++) {
    ret.append(Object(NEW(FBMLNode)(p->children[i], fnode->m_tree)));
  }
  return ret;
}

int f_fbml_get_children_count_11(CObjRef node) {
  return node.getTyped<FBMLNode>()->m_node->children_count;
}

Array f_fbml_get_children_by_name_11(CObjRef node, CStrRef tag) {
  unsigned short lookup = fbml_lookup_tag_by_name((char*)tag.data());
  FBMLNode *fnode = node.getTyped<FBMLNode>();
  fbml_node *p = fnode->m_node;

  Array ret;
  for (int i = 0 ; i < p->children_count; i++) {
    fbml_node *cur = p->children[i];
    if (cur->eHTMLTag == lookup) {
      ret.append(Object(NEW(FBMLNode)(cur, fnode->m_tree)));
    }
  }
  return ret;
}

Array f_fbml_get_attributes_11(CObjRef node) {
  FBMLNode *fnode = node.getTyped<FBMLNode>();
  fbml_node *p = fnode->m_node;

  Array ret;
  for (int i = 0 ; i < p->attribute_count; i++) {
    fbml_attribute *cur = p->attributes[i];
    ret.set(String(cur->name, CopyString), String(cur->value, CopyString));
  }
  return ret;
}

String f_fbml_get_attribute_11(CObjRef node, CStrRef name) {
  FBMLNode *fnode = node.getTyped<FBMLNode>();
  char *result = fbml_node_get_attribute(fnode->m_node, (char*)name.data());
  if (result) {
    return String(result, CopyString);
  }
  return String();
}

Variant f_fbml_attr_to_bool_11(CStrRef name) {
  int result;
  int state = fbml_node_attr_to_bool((char*)name.data(), &result);
  if (state == 1) {
    return (bool)result;
  }
  return null;
}

String f_fbml_attr_to_color_11(CStrRef name) {
  char *result;
  int state = fbml_node_attr_to_color((char*)name.data(), &result);
  if (state == 1) {
    return String(result, AttachString);
  }
  return String();
}

String f_fbml_get_text_11(CObjRef node) {
  FBMLNode *fnode = node.getTyped<FBMLNode>();
  fbml_node *p = fnode->m_node;
  if (p->text) {
    return String(p->text, CopyString);
  }
  return String();
}

void f_fbml_precache_11(CObjRef node, CVarRef data, CStrRef callback) {
  FBMLNode *fnode = node.getTyped<FBMLNode>();

  struct node_callback_parameter param;
  param.tree = fnode->m_tree;
  param.user_func_name = callback;
  param.user_data = data;
  param.fallback = "";

  struct fbml_node_precacher node_pre;
  node_pre.precache_node_data = &param;
  node_pre.pfunc_precacher = callback_node_function;

  fbml_node_precache(fnode->m_node, &node_pre);
}

Array f_fbml_batch_precache_11(CObjRef node) {
  FBMLNode *fnode = node.getTyped<FBMLNode>();
  fbml_precache_bunch **node_bunch = fbml_node_batch_precache(fnode->m_node);
  Array ret;
  for (fbml_precache_bunch **iter = node_bunch; *iter; iter++) {
    Array nodes;
    for (fbml_node **node_iter = (*iter)->nodes; *node_iter; node_iter++) {
      nodes.append(Object(NEW(FBMLNode)(*node_iter, fnode->m_tree)));
    }
    ret.set((*iter)->tag, nodes);
    fbml_node_bunch_free(*iter);
  }
  free(node_bunch);

  return ret;
}

String f_fbml_render_children_11(CObjRef node, CVarRef data,
                                 CStrRef html_callback, CStrRef fb_callback,
                                 int internal_mode /* = 0 */) {
  FBMLNode *fnode = node.getTyped<FBMLNode>();

  struct node_callback_parameter html;
  html.tree = fnode->m_tree;
  html.user_func_name = html_callback;
  html.user_data = data;
  html.fallback = "";

  struct node_callback_parameter fb;
  fb.tree = fnode->m_tree;
  fb.user_func_name = fb_callback;
  fb.user_data = data;
  fb.fallback = "";

  struct fbml_node_renderer node_rend;
  node_rend.fb_node_data = &fb;
  node_rend.html_node_data = &html;
  node_rend.pfunc_renderer = callback_node_function;
  node_rend.node = fnode->m_node;

  char *result = fbml_node_render_children(fnode->m_node, internal_mode,
                                           &node_rend);
  return String(result, AttachString);
}

String f_fbml_flatten_11(CObjRef node) {
  FBMLNode *fnode = node.getTyped<FBMLNode>();
  char *s = fbml_node_print(fnode->m_node);
  return String(s, AttachString);
}

///////////////////////////////////////////////////////////////////////////////
// html profiler

static string html_profile_read_file(const char *filename) {
  Array params;
  params.append(filename);
  Variant ret = invoke("html_profile_read_file", params);
  return ret.toString().data();
}

String f_html_profile(CStrRef html) {
  ReadLock lock(s_mutex);

  ostringstream out;
  int ret = html_profile(html.data(), out, html_profile_read_file);
  if (ret == 0) {
    return out.str();
  }
  return "";
}

///////////////////////////////////////////////////////////////////////////////

String f_fbjsparse(CStrRef input) {
  throw NotImplementedException(__func__);
}

///////////////////////////////////////////////////////////////////////////////
}
