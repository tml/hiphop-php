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

#include <cpp/ext/ext_fbintern.h>
#include <cpp/ext/fql/fqlnode.h>
#include <xhp_preprocess.hpp>
#include <cpp/base/source_info.h>
#include <cpp/base/class_info.h>
#include <cpp/base/runtime_option.h>

using namespace std;

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

bool f_fb_debug_rlog(CVarRef err, int include_flags /* = -1 */,
                     int type /* = -1 */, CVarRef headers /* = null_variant */) {
  switch (type) {
  case (1<<3L) /* E_NOTICE */:
  case (1<<1L) /* E_WARNING */:
    Logger::Warning("DEBUG_RLOG: %s\n", err.toString().data());
    break;
  default:
    Logger::Error("DEBUG_RLOG: %s\n", err.toString().data());
    break;
  }
  return true;
}

struct RenderWrappedBreaks {
  int* breaks;
  int length;
  RenderWrappedBreaks() : breaks(NULL), length(0) {}
};

static ThreadLocal<RenderWrappedBreaks> renderwrapped_breaks;

String f_fb_render_wrapped(CStrRef text, int linelen /* = 26 */,
                           CStrRef head /* = "<span>" */,
                           CStrRef tail /* = "</span><wbr></wbr><span class=\"word_break\"></span>" */) {
  RenderWrappedBreaks &breaks = *renderwrapped_breaks.get();
  int* &renderwrapped_breaks = breaks.breaks;
  int &renderwrapped_length = breaks.length;

  int text_len = text.size();
  int head_len = head.size();
  int tail_len = tail.size();
  if (linelen <= 0 || text_len < linelen) {
    return text;
  }

  int num_breaks = 0,
      max_breaks = (text_len + (linelen-1))/(linelen);

  /* If we might have more breaks to keep track of in this string than we have
     room for, reallocate the break buffer. Also, check that we have a
     buffer -- since we use available RAM to limit the maximum wrappable string
     length, we might have exceeded that in the last call to this function
     and suffered a prealloc failure. */
  if (!renderwrapped_breaks || max_breaks > renderwrapped_length) {
    renderwrapped_length = max_breaks * 2;
    renderwrapped_breaks = (int*)realloc(renderwrapped_breaks,
                                         sizeof(int) * renderwrapped_length);
  }

  int charpos   = 0,  /* Characters (not bytes!) since last break. */
      expect    = 0,  /* Expected bytes in this character.         */
      ii,
      jj;
  unsigned char b;

  for (ii = 0; ii < text_len; ++ii) {

    if (charpos >= linelen) {
      renderwrapped_breaks[num_breaks++] = ii;
      charpos = 0;
    }

    ++charpos;

    b = (unsigned char)text.data()[ii];
    if (b == '&') {

      /* HTML entity -- scan forward until a semicolon. Note that this will
         not respect multibyte characters, but that's probably OK because they
         would be (a) invalid unicode and (b) invalid HTML. */
      while (++ii < text_len && text.data()[ii] != ';')
        ;

    } else if (b == ' ' || b == '\t' || b == '\n' || b == '\r') {
      charpos = 0;
    } else if (b < 192 || b > 247) {
      expect = 0;
    } else if (b < 224) {
      expect = 1;
    } else if (b < 240) {
      expect = 2;
    } else if (b < 248) {
      expect = 3;
    }

    for (jj = 0; jj < expect; ++jj) {
      b = (unsigned char)text.data()[++ii];
      if (b < 128 || b > 223) {
        /* This is an invalid non-start byte, so recover by ungetting it so we
           can start the next character here. */
        --ii;
        break;
      }
    }

    expect = 0;
  }

  char *rbuf = (char*)malloc(sizeof(char) * (1 + text_len +
                                             ((head_len + tail_len) *
                                              num_breaks)));
  char *rpos = rbuf;
  int  spos  = 0;

  for (ii = 0; ii < num_breaks; ++ii) {
    memcpy(rpos, head, head_len);
    rpos += head_len;
    memcpy(rpos, text.data() + spos, renderwrapped_breaks[ii] - spos);
    rpos += renderwrapped_breaks[ii] - spos;
    spos  = renderwrapped_breaks[ii];
    memcpy(rpos, tail, tail_len);
    rpos += tail_len;
  }

  memcpy(rpos, text.data() + spos, text_len - spos);
  rpos += text_len - spos;
  *rpos = 0;

  /* If we ended up with a buffer larger than 1MB, realloc it back down to
     a reasonable size. */
  if (renderwrapped_length * sizeof(int) > (1024 * 1024)) {
    renderwrapped_length = (1024 * 1024) / sizeof(int);
    renderwrapped_breaks = (int*)realloc(renderwrapped_breaks, sizeof(int) *
                                         renderwrapped_length);
  }
  return String(rbuf, rpos - rbuf, AttachString);
}

Array f_fb_get_derived_classes(CStrRef filename, CStrRef basecls) {
  Array ret = Array::Create();

  if (!filename.empty()) {
    const char *relativePath = filename.data();
    if (*relativePath == '/' &&
        strncmp(RuntimeOption::SourceRoot.c_str(), relativePath,
                RuntimeOption::SourceRoot.size()) == 0) {
      relativePath += RuntimeOption::SourceRoot.size();
      if (*relativePath == '/') {
        relativePath++;
      }
    }

    if (*relativePath != '/') {
      std::vector<const char *> classes;
      SourceInfo::TheSourceInfo.getDeclaredClasses(relativePath, classes);
      for (unsigned int i = 0; i < classes.size(); i++) {
        if (strcasecmp(classes[i], basecls) == 0) {
          ret.append(classes[i]);
        } else {
          const ClassInfo *cls = ClassInfo::FindClass(classes[i]);
          if (cls && cls->derivesFrom(basecls.data(), true)) {
            ret.append(classes[i]);
          }
        }
      }
    }
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
// FQL parser

static Variant create(FQLNode *node);

static Object create(FQLConstantNode *node) {
  return create_object
    ("FQLConstantExpression",
     CREATE_VECTOR1(StringUtil::SqlDecode(String(node->getValue()))));
}

static Object create(FQLArithmeticExpressionNode *node) {
  return create_object
    ("FQLArithmeticExpression",
     CREATE_VECTOR3(String(node->getOp()),
                    create(node->getLeft()),
                    create(node->getRight())));
}

static Object create(FQLComparisonExpressionNode *node) {
  return create_object
    ("FQLComparisonExpression",
     CREATE_VECTOR3(String(node->getOp()),
                    create(node->getLeft()),
                    create(node->getRight())));
}

static Object create(FQLFieldExpressionNode *node) {
  char *long_field_name = const_cast<char *>(node->getField().c_str());
  char *curr = strtok(long_field_name, ".");
  char *curr0 = curr;

  Array other_fields = Array::Create();
  while (curr = strtok(NULL, ".")) {
    other_fields.append(String(curr, CopyString));
  }

  Object obj = create_object("FQLFieldExpression", Array::Create(), false);
  obj->o_set("field_name", -1, String(curr0, CopyString));
  obj->o_set("other_fields", -1, other_fields);
  return obj;
}

static Object create(FQLNotExpressionNode *node) {
  return create_object
    ("FQLNotExpression",
     CREATE_VECTOR1(create(node->getExpr())));
}

template<typename T>
static Array create(const std::vector<T*> &items) {
  Array expr_list = Array::Create();
  for (unsigned int i = 0; i < items.size(); i++) {
    expr_list.append(create(items[i]));
  }
  return expr_list;
}

static Object create(FQLDisjunctionNode *node) {
  Object obj = create_object("FQLDisjunction", Array::Create(), false);
  obj->o_set("operands", -1, create(node->getOperands()));
  return obj;
}

static Object create(FQLConjunctionNode *node) {
  Object obj = create_object("FQLConjunction", Array::Create(), false);
  obj->o_set("operands", -1, create(node->getOperands()));
  return obj;
}

static Object create(FQLFunctionExpressionNode *node) {
  return create_object
    ("FQLFunction",
     CREATE_VECTOR2(String(node->getFnName()),
                    create(node->getArgs())));
}

static Object create(FQLContainsExpressionNode *node) {
  Object obj = create_object("FQLContainsExpression", Array::Create(), false);
  obj->o_set("searchCols", -1, create(node->getSearchCols()));
  obj->o_set("queryStr", -1, String(node->getQuery()));
  return obj;
}

static Object create(FQLInArrayNode *node) {
  return create_object
    ("FQLInArray",
     CREATE_VECTOR2(create(node->getExpr()),
                    create(node->getList())));
}

static Object create(FQLStatementNode *node) {
  Object ret;
  if (node->isNative()) {
    ret = create_object("FQLStatement", Array::Create(), false);
  } else {
    ret = create_object("FQLVirtualStatement", Array::Create(), false);
  }
  ret->o_set("select", -1, create(node->getSelect()));
  ret->o_set("from", -1, String(node->getFrom()));
  if (node->getWhere()) {
    ret->o_set("where", -1, create(node->getWhere()));
  }

  FQLOrderByNode *orderby_node = node->getOrderBy();
  if (orderby_node) {
    ret->o_set("orderby", -1, create(orderby_node->getOrderby()));
    ret->o_set("desc", -1, orderby_node->getDesc());
  }

  FQLLimitNode *limit_node = node->getLimit();
  if (limit_node) {
    ret->o_set("limit", -1, limit_node->getLimit());
    ret->o_set("offset", -1, limit_node->getOffset());
  }

  return ret;
}

static Object create(FQLInStatementNode *node) {
  return create_object
    ("FQLInStatement",
     CREATE_VECTOR2(create(node->getExpr()),
                    create(node->getStatement())));
}

static Variant create(FQLNode *node) {
  switch (node->nodeType()) {
  case FQL_CONSTANT_EXPRESSION:
    return create((FQLConstantNode *)node);
  case FQL_ARITHMETIC_EXPRESSION:
    return create((FQLArithmeticExpressionNode *)node);
  case FQL_EXPRESSION_LIST:
    return create(((FQLExpressionListNode *)node)->getList());
  case FQL_COMPARISON_EXPRESSION:
    return create((FQLComparisonExpressionNode *)node);
  case FQL_FUNCTION_EXPRESSION:
    return create((FQLFunctionExpressionNode *)node);
  case FQL_CONJUNCTION:
    return create((FQLConjunctionNode *)node);
  case FQL_DISJUNCTION:
    return create((FQLDisjunctionNode *)node);
  case FQL_IN_ARRAY:
    return create((FQLInArrayNode *)node);
  case FQL_FIELD_EXPRESSION:
    return create((FQLFieldExpressionNode *)node);
  case FQL_CONTAINS_EXPRESSION:
    return create((FQLContainsExpressionNode *)node);
  case FQL_NOT_EXPRESSION:
    return create((FQLNotExpressionNode *)node);
  case FQL_STATEMENT:
    return create((FQLStatementNode *)node);
  case FQL_IN_STATEMENT:
    return create((FQLInStatementNode *)node);
  default:
    break;
  }
  return null;
}

static ReadWriteMutex s_fql_mutex;

void f_fql_set_static_data_10(CArrRef tables, CArrRef functions) {
  set<string> table_names;
  for (ArrayIter iter(tables); iter; ++iter) {
    table_names.insert(iter.second().toString().data());
  }

  std::map<string, struct fql_function_data> function_names;
  for (ArrayIter iter(functions); iter; ++iter) {
    struct fql_function_data param_data;
    param_data.num_params = iter.second().rvalAt("params").toInt64();
    function_names[iter.first().toString().data()] = param_data;
  }

  WriteLock lock(s_fql_mutex);
  fql_set_static_data(table_names, function_names);
}

bool f_fql_static_data_set_10() {
  ReadLock lock(s_fql_mutex);
  return fql_static_data_set();
}

Array f_fql_parse_10(CStrRef query) {
  ReadLock lock(s_fql_mutex);

  FQLStatementNode *stmt = NULL;
  Array ret = Array::Create();
  try {
    stmt = fql_parse(query.data());
    ret.set("stmt", create(stmt));
  } catch (FQLParserException &e) {
    ret.set("error", String(e.what(), CopyString));
    ret.set("error_code", e.getErrorCode());
  } catch (std::exception &e) {
    ret.set("error", String(e.what(), CopyString));
  }
  if (stmt) {
    delete stmt;
  }
  return ret;
}

Array f_fql_multiparse_10(CArrRef query) {
  map<string, string> query_set;
  for (ArrayIter iter(query); iter; ++iter) {
    query_set[iter.first().toString().data()] =
      iter.second().toString().data();
  }

  Array ret = Array::Create();
  try {
    Array all_results_array = Array::Create();
    vector<vector<FQLParserState > > all_results = fql_multiparse(query_set);
    int num_rounds = all_results.size();
    for (int i = 0; i < num_rounds; i++) {
      Array round_arr = Array::Create();
      vector<FQLParserState> &round = all_results[i];
      for (unsigned int j = 0; j < round.size(); j++) {
        FQLParserState &curr = round[j];
        Object stmt = create(curr.stmt);
        delete curr.stmt;
        stmt->o_set("reused", -1, curr.reused);
        round_arr.set(String(curr.name), stmt);
      }
      all_results_array.append(round_arr);
    }
    ret.set("results", all_results_array);
  } catch (FQLParserException &e) {
    ret.set("error", String(e.what(), CopyString));
    ret.set("error_code", e.getErrorCode());
  } catch (std::exception &e) {
    ret.set("error", String(e.what(), CopyString));
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
// xhp functions

Array f_xhp_preprocess_code(CStrRef code) {
  string rewrit, error;
  uint32_t error_line;

  // Mangle code
  string code_str(code.data(), code.size());
  XHPResult result = xhp_preprocess(code_str, rewrit, false,
                                    error, error_line);

  // Build return code
  Array ret = Array::Create();
  switch (result) {
  case XHPErred:
    ret.set("error", String(error));
    ret.set("error_line", (int64)error_line);
    break;
  case XHPRewrote:
    ret.set("new_code", String(rewrit));
    break;
  default:
    // do nothing
    break;
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
}
