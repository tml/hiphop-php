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

#include <stdlib.h>
#include <string.h>
#include <sstream>
#include "fqlnode.h"

using namespace std;

// LEXER function prototypes
typedef void *yyscan_t;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
int yylex_init (yyscan_t* scanner);
int yylex_destroy (yyscan_t yyscanner );
YY_BUFFER_STATE yy_scan_string (const char *yy_str ,yyscan_t yyscanner );
int yylex(YYSTYPE *yylval, yyscan_t scanner);

// static data for the parser
static bool g_fql_static_data_set = false;
static set<string> fql_table_names;
static map<string, struct fql_function_data> fql_function_names;

FQLParserState fql_parse_query(const char *query, string name = "");

int fql_static_data_set() {
  return g_fql_static_data_set ? 1 : 0;
}

void fql_set_static_data(set<string> table_names,
                         map<string, struct fql_function_data> function_names) {
  fql_table_names = table_names;
  fql_function_names = function_names;
  g_fql_static_data_set = true;
}

bool fql_validate_table_name(string table_name, FQLParserState *state) {
  if (!strchr(table_name.c_str(), '.')) {
    set<string>::const_iterator it = fql_table_names.find(table_name);
    if (it == fql_table_names.end()) {
      throw FQLParserException("Unknown table: " + table_name,
                               FQL_EC_UNKNOWN_TABLE);
      return false;
    }
  }
  return true;
}

bool fql_validate_function_data(string fnName,
                                FQLExpressionListNode *args) {
  map<string, struct fql_function_data>::const_iterator it =
    fql_function_names.find(fnName);
  if (it == fql_function_names.end()) {
    throw FQLParserException(fnName + " is not a valid function name.",
                             FQL_EC_UNKNOWN_FUNCTION);
  }
  int num_given_params = 0;
  if (args) {
    num_given_params = args->length();
  }
  int num_required_params = (*it).second.num_params;
  if (num_required_params >= 0 &&
      num_required_params != num_given_params) {
    stringstream msg;
    msg << fnName << " function expects " << num_required_params <<
      " parameters; " << num_given_params << " given.";
    throw FQLParserException(msg.str(), FQL_EC_INVALID_PARAM);
  }
  return true;
}

void *ParseAlloc(void *(*mallocProc)(size_t));
void Parse(void *, int, FQLToken, FQLParserState *);
void ParseFree(void *, void (*freeProc)(void*));
void ParseTrace(FILE *, char *);

FQLStatementNode *fql_parse(const char *query) {
  FQLParserState state = fql_parse_query(query);
  if (!state.dependencies.empty()) {
    set<string>::iterator it = state.dependencies.begin();
    string table = *it;
    delete state.stmt;
    throw FQLParserException("Unknown table: #" + table, FQL_EC_UNKNOWN_TABLE);
  }
  return state.stmt;
}

FQLParserState fql_parse_query(const char *query, string name) {
  FQLParserState state(name);
  void *pParser = ParseAlloc(malloc);
  int token_type;
  FQLToken curr_token;
  curr_token.token = NULL;
  curr_token.pos = 0;
  yyscan_t scanner;
  yylex_init(&scanner);
  yy_scan_string(query, scanner);
  //ParseTrace(stdout, "<<");
  try {
    while ((token_type = yylex(&curr_token, scanner)) != 0) {
      switch (token_type) {
      case TOK_UNKNOWN: {
        // throw error
        stringstream ss;
        ss << "unexpected '" << curr_token.token << "' at position " <<
          curr_token.pos - strlen(curr_token.token) << ".";
        free(curr_token.token);
        throw FQLParserException(ss.str());
      }
      case TOK_WHITESPACE:
        // skip over whitespace
        break;
      default:
        Parse(pParser, token_type, curr_token, &state);
      }
      if (!state.errors.empty()) {
        throw (state.errors.front());
      }
    }
    Parse(pParser, 0, curr_token, &state);
    if (!state.errors.empty()) {
      throw (state.errors.front());
    }
  } catch (FQLParserException e) {
    if (state.stmt) {
      delete state.stmt;
    }
    yylex_destroy(scanner);
    ParseFree(pParser, free);
    throw;
  }
  yylex_destroy(scanner);
  ParseFree(pParser, free);
  return state;
}

