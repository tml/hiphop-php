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
// Copyright 2004-2009 Facebook. All Rights Reserved.

#ifndef __FQLNODE_H__
#define __FQLNODE_H__

#include <vector>
#include <iostream>
#include <string>
#include <exception>
#include "fql.h"
#include <map>
#include <set>

#define YYSTYPE FQLToken
#define FQL_EC_PARSER_ERROR          601
#define FQL_EC_UNKNOWN_TABLE         603
#define FQL_EC_UNKNOWN_FUNCTION      605
#define FQL_EC_INVALID_PARAM         606
#define FQL_EC_UNRESOLVED_DEPENDENCY 614

#define TOK_WHITESPACE 1000
#define TOK_UNKNOWN 1001

#define FQL_EXPRESSION_LIST       1
#define FQL_CONSTANT_EXPRESSION   2
#define FQL_ARITHMETIC_EXPRESSION 3
#define FQL_COMPARISON_EXPRESSION 4
#define FQL_FUNCTION_EXPRESSION   5
#define FQL_CONJUNCTION           6
#define FQL_DISJUNCTION           7
#define FQL_IN_ARRAY              8
#define FQL_FIELD_EXPRESSION      9
#define FQL_NOT_EXPRESSION        10
#define FQL_ORDER_BY              11
#define FQL_LIMIT                 12
#define FQL_STATEMENT             13
#define FQL_IN_STATEMENT          14
#define FQL_CONTAINS_EXPRESSION   15

struct fql_function_data {
  int num_params;
};

int fql_static_data_set();
void fql_set_static_data(std::set<std::string> table_names,
                         std::map<std::string, struct fql_function_data> function_names);

class FQLNode {
 public:
  FQLNode() {}
  virtual ~FQLNode() { }
  virtual unsigned int nodeType() const = 0;
};

class FQLExpressionNode : public FQLNode {
 public:
  virtual bool operator==(const FQLExpressionNode &rhs) const {
    return false;
  }
  virtual ~FQLExpressionNode() {}
  bool checkType(const FQLExpressionNode &rhs) const {
    return (nodeType() == rhs.nodeType());
  }
};

class FQLExpressionListNode : public FQLNode {
 public:
  FQLExpressionListNode(FQLExpressionNode *first = NULL) {
    if (first) {
      list.push_back(first);
    }
  }
  ~FQLExpressionListNode() {
    for (unsigned int i = 0; i < list.size(); i++) {
      if (list[i]) {
        delete list[i];
      }
    }
  }
  bool operator==(const FQLExpressionListNode &rhs) const;
  unsigned int nodeType() const { return FQL_EXPRESSION_LIST; };

  int length() const { return list.size(); }
  void append(FQLExpressionNode *next) {
    list.push_back(next);
  }
  std::vector<FQLExpressionNode *>&getList() { return list; }

 protected:
  std::vector<FQLExpressionNode *> list;
};

class FQLConstantNode : public FQLExpressionNode {
 public:
  FQLConstantNode(std::string value) :
    value(value) {  }
    bool operator==(const FQLExpressionNode &rhs) const;
    unsigned int nodeType() const { return FQL_CONSTANT_EXPRESSION; };
    std::string getValue() { return value; }
 private:
  std::string value;
};

class FQLBinOpExpressionNode : public FQLExpressionNode {
 public:
  FQLBinOpExpressionNode(std::string op, FQLExpressionNode *left,
                         FQLExpressionNode *right) :
    op(op),
    left(left),
    right(right) { }
    ~FQLBinOpExpressionNode() {
      delete left;
      delete right;
    }
    bool operator==(const FQLExpressionNode &rhs) const;
    std::string getOp() { return op; }
    FQLExpressionNode *getLeft() { return left; }
    FQLExpressionNode *getRight() { return right; }
 protected:
  std::string op;
  FQLExpressionNode *left;
  FQLExpressionNode *right;
};

class FQLArithmeticExpressionNode : public FQLBinOpExpressionNode {
 public:
  FQLArithmeticExpressionNode(std::string op, FQLExpressionNode *left,
                              FQLExpressionNode *right) :
    FQLBinOpExpressionNode(op, left, right) { }

    unsigned int nodeType() const { return FQL_ARITHMETIC_EXPRESSION; };
};

class FQLComparisonExpressionNode : public FQLBinOpExpressionNode {
 public:
  FQLComparisonExpressionNode(std::string op, FQLExpressionNode *left,
                              FQLExpressionNode *right) :
    FQLBinOpExpressionNode(op, left, right) { }

    unsigned int nodeType() const { return FQL_COMPARISON_EXPRESSION; };
};

class FQLFunctionExpressionNode : public FQLExpressionNode {
 public:
  FQLFunctionExpressionNode(std::string fn_name,
                            FQLExpressionListNode *args);
  ~FQLFunctionExpressionNode() {
    if (args) {
      delete args;
    }
  }
  bool operator==(const FQLExpressionNode &rhs) const;
  unsigned int nodeType() const { return FQL_FUNCTION_EXPRESSION; };
  std::string getFnName() { return fnName; }
  FQLExpressionListNode *getArgs() { return args; }
 protected:
  std::string fnName;
  FQLExpressionListNode *args;
};

class FQLMultiOpExpressionNode : public FQLExpressionNode {
 public:
  void append(FQLExpressionNode *node) {
    operands.push_back(node);
  }
  ~FQLMultiOpExpressionNode() {
    for (unsigned int i = 0; i < operands.size(); i++) {
      delete operands[i];
    }
  }
  bool operator==(const FQLExpressionNode &rhs) const;
  std::vector<FQLExpressionNode *>& getOperands() { return operands; }
 protected:
  std::vector<FQLExpressionNode *> operands;
};

class FQLConjunctionNode : public FQLMultiOpExpressionNode {
 public:
  FQLConjunctionNode() {}
  FQLConjunctionNode(FQLExpressionNode *left,
                     FQLExpressionNode *right) {
    operands.push_back(left);
    operands.push_back(right);
  }
  unsigned int nodeType() const { return FQL_CONJUNCTION; };
};

class FQLDisjunctionNode : public FQLMultiOpExpressionNode {
 public:
  FQLDisjunctionNode() {}
  FQLDisjunctionNode(FQLExpressionNode *left,
                     FQLExpressionNode *right) {
    operands.push_back(left);
    operands.push_back(right);
  }
  unsigned int nodeType() const { return FQL_DISJUNCTION; };
};

class FQLInArrayNode : public FQLExpressionNode {
 public:
  FQLInArrayNode(FQLExpressionNode *expr, FQLExpressionListNode *list) :
    expr(expr),
    list(list) { }
    ~FQLInArrayNode() {
      delete expr;
      delete list;
    }
  unsigned int nodeType() const { return FQL_IN_ARRAY; };
  bool operator==(const FQLExpressionNode &rhs) const;
  FQLExpressionNode *getExpr() { return expr; }
  FQLExpressionListNode *getList() { return list; }
 protected:
  FQLExpressionNode *expr;
  FQLExpressionListNode *list;
};

class FQLFieldExpressionNode : public FQLExpressionNode {
 public:
  FQLFieldExpressionNode(std::string field) : field(field) { }
    ~FQLFieldExpressionNode() {  }
  unsigned int nodeType() const { return FQL_FIELD_EXPRESSION; };
  bool operator==(const FQLExpressionNode &rhs) const;
  std::string getField() { return field; }

  std::string field;
};

class FQLNotExpressionNode : public FQLExpressionNode {
 public:
  FQLNotExpressionNode(FQLExpressionNode *expr) :
    expr(expr) { }
    ~FQLNotExpressionNode() { delete expr; }
  unsigned int nodeType() const { return FQL_NOT_EXPRESSION; }
  bool operator==(const FQLExpressionNode &rhs) const;
  FQLExpressionNode *getExpr() { return expr; }
 protected:
  FQLExpressionNode *expr;
};

class FQLContainsExpressionNode : public FQLExpressionNode {
 public:
  FQLContainsExpressionNode(std::string query, FQLExpressionListNode *search_cols) :
    query(query),
    searchCols(search_cols) { }
    ~FQLContainsExpressionNode() {
      if (searchCols) {
        delete searchCols;
      }
    }
  unsigned int nodeType() const { return FQL_CONTAINS_EXPRESSION; }
  bool operator==(const FQLExpressionNode &rhs) const;
  std::string getQuery() { return query; }
  FQLExpressionListNode *getSearchCols() { return searchCols; }

 protected:
  std::string query;
  FQLExpressionListNode *searchCols;
};

class FQLOrderByNode : public FQLNode {
 public:
  FQLOrderByNode(FQLExpressionNode *orderby, bool desc) :
    orderby(orderby),
    desc(desc) { }
    ~FQLOrderByNode() { delete orderby; }

  unsigned int nodeType() const { return FQL_ORDER_BY; };
  bool operator==(const FQLOrderByNode &rhs) const;
  FQLExpressionNode *getOrderby() { return orderby; }
  bool getDesc() { return desc; }

 protected:
  FQLExpressionNode *orderby;
  bool desc;
};

class FQLLimitNode : public FQLNode {
 public:
  FQLLimitNode(int limit, int offset) :
    limit(limit),
    offset(offset) { }

  unsigned int nodeType() const { return FQL_LIMIT; };
  bool operator==(const FQLLimitNode &rhs) const;
  int getLimit() { return limit; }
  int getOffset() { return offset; }

 protected:
  int limit;
  int offset;
};

class FQLStatementNode : public FQLNode {
 public:
  FQLStatementNode(FQLExpressionListNode *select,
                   std::string from,
                   FQLExpressionNode *where,
                   FQLOrderByNode *orderby = NULL,
                   FQLLimitNode *limit = NULL,
                   bool native = true);
  ~FQLStatementNode() {
    if (select) {
      delete select;
    }
    if (where) {
      delete where;
    }
    if (orderby) {
      delete orderby;
    }
    if (limit) {
      delete limit;
    }
  }
  bool validate() const;
    unsigned int nodeType() const { return FQL_STATEMENT; }
    bool operator==(const FQLStatementNode &rhs) const;

    void setSelect(FQLExpressionListNode *select) {
      if (this->select) {
        delete this->select;
      }
      this->select = select;
    }
    void setFrom(std::string from) {
      this->from = from;
    }
    void setWhere(FQLExpressionNode *where) {
      if (this->where) {
        delete this->where;
      }
      this->where = where;
    }
    void setOrderBy(FQLOrderByNode *orderby) {
      if (this->orderby) {
        delete this->orderby;
      }
      this->orderby = orderby;
    }
    void setLimit(FQLLimitNode *limit) {
      if (this->limit) {
        delete this->limit;
      }
      this->limit = limit;
    }
    FQLExpressionListNode *getSelect() { return select; }
    std::string getFrom() { return from; }
    FQLExpressionNode *getWhere() { return where; }
    FQLOrderByNode *getOrderBy() { return orderby; }
    FQLLimitNode *getLimit() { return limit; }
    bool isNative() { return native; }
 private:
  FQLExpressionListNode *select;
  std::string from;
  FQLExpressionNode *where;
  FQLOrderByNode *orderby;
  FQLLimitNode *limit;
  bool native;
};

class FQLInStatementNode : public FQLExpressionNode {
 public:
  FQLInStatementNode(FQLExpressionNode *expr, FQLStatementNode *statement) :
    expr(expr),
    statement(statement) { }
    ~FQLInStatementNode() {
      if (expr) {
        delete expr;
      }
      if (statement) {
        delete statement;
      }
    }
  unsigned int nodeType() const { return FQL_IN_STATEMENT; };
  bool operator==(const FQLExpressionNode &rhs) const;
  FQLExpressionNode *getExpr() { return expr; }
  FQLStatementNode *getStatement() { return statement; }
 protected:
  FQLExpressionNode *expr;
  FQLStatementNode *statement;
};

typedef struct {
  char *token;
  unsigned int pos;
} FQLToken;

class FQLParserException : public std::exception {
 public:
 FQLParserException(std::string msg, int code=FQL_EC_PARSER_ERROR) :
  error(msg),
    errorCode(code) { }
    ~FQLParserException() throw() { }
  virtual const char *what() const throw() {
    return error.c_str();
  }

  int getErrorCode() {
    return errorCode;
  }

  static std::string getTokenText(int tokenType, FQLToken token) {
    switch (tokenType) {
    case TOK_SEMICOLON:
      return ";";
    case TOK_SELECT:
      return "SELECT";
    case TOK_CONTAINS:
      return "CONTAINS";
    case TOK_FROM:
      return "FROM";
    case TOK_WHERE:
      return "WHERE";
    case TOK_ORDER:
      return "ORDER";
    case TOK_BY:
      return "BY";
    case TOK_DESC:
      return "ASC";
    case TOK_LIMIT:
      return "LIMIT";
    case TOK_COMMA:
      return ",";
    case TOK_OFFSET:
      return "OFFSET";
    case TOK_OR:
      return "OR";
    case TOK_AND:
      return "AND";
    case TOK_IN:
      return "IN";
    case TOK_OPEN_PAREN:
      return "(";
    case TOK_CLOSE_PAREN:
      return ")";
    case TOK_STAR:
      return "*";
    case TOK_SLASH:
      return "/";
    case TOK_NOT:
      return "NOT";
    default:
      return token.token;
    }
  }

 private:
  std::string error;
  int errorCode;
};

class FQLParserState {
 public:
  FQLParserState(std::string name) {
    stmt = NULL;
    this->name = name;
    reused = false;
  }
  static void deleteStmtList(std::vector<FQLParserState> states);
  std::set<std::string> dependencies;

  bool reused;
  std::string name;
  FQLStatementNode *stmt;
  std::vector<FQLParserException> errors;
};

std::vector<std::vector<FQLParserState > >
fql_multiparse(const std::map<std::string, std::string> &query_set);
FQLStatementNode *fql_parse(const char *query);
FQLParserState fql_parse_query(const char *query, std::string name);
bool fql_validate_table_name(std::string table_name, FQLParserState *state);
bool fql_validate_function_data(std::string fnName, FQLExpressionListNode *args);

#endif
