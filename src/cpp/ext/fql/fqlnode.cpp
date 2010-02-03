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
#include <iostream>
#include "fqlnode.h"
#include "fql.h"

using namespace std;

FQLStatementNode::FQLStatementNode(FQLExpressionListNode *select,
                                   string from,
                                   FQLExpressionNode *where,
                                   FQLOrderByNode *orderby,
                                   FQLLimitNode *limit,
                                   bool native) :
  select(select),
  from(from),
  where(where),
  orderby(orderby),
  limit(limit),
  native(native) { }

bool FQLConstantNode::operator==(const FQLExpressionNode &rhs) const {
  if (!checkType(rhs)) {
    return false;
  }
  const FQLConstantNode *rhsPtr =
    static_cast<const FQLConstantNode *>(&rhs);
  return (value == rhsPtr->value);
}

bool FQLBinOpExpressionNode::operator==(const FQLExpressionNode &rhs) const {
  if (!checkType(rhs)) {
    return false;
  }
  const FQLBinOpExpressionNode *rhsPtr =
    static_cast<const FQLBinOpExpressionNode *>(&rhs);
  return ((op == rhsPtr->op) &&
          (*left == *(rhsPtr->left)) &&
          (*right == *(rhsPtr->right)));
}

bool FQLFunctionExpressionNode::operator==(const FQLExpressionNode &rhs) const {
  if (!checkType(rhs)) {
    return false;
  }
  const FQLFunctionExpressionNode *rhsPtr =
    static_cast<const FQLFunctionExpressionNode *>(&rhs);
  return ((fnName == rhsPtr->fnName) &&
          ((args == NULL && rhsPtr->args == NULL) ||
           (*args == *(rhsPtr->args))));
}

bool FQLContainsExpressionNode::operator==(const FQLExpressionNode &rhs) const {
  if (!checkType(rhs)) {
    return false;
  }
  const FQLContainsExpressionNode *rhsPtr =
    static_cast<const FQLContainsExpressionNode *>(&rhs);
  return ((query == rhsPtr->query) &&
          ((searchCols == NULL && rhsPtr->searchCols == NULL) ||
           (*searchCols == *(rhsPtr->searchCols))));
}

FQLFunctionExpressionNode::FQLFunctionExpressionNode(string fn_name,
                                                     FQLExpressionListNode *args) :
  fnName(fn_name),
  args(args) {
}

bool FQLExpressionListNode::operator==(const FQLExpressionListNode &rhs) const {
  if (list.size() != rhs.list.size()) {
    return false;
  }
  for (unsigned int i = 0; i < list.size(); i++) {
    if (!(*(list[i]) == *(rhs.list[i]))) {
      return false;
    }
  }
  return true;
}

bool FQLMultiOpExpressionNode::operator==(const FQLExpressionNode &rhs) const {
  if (!checkType(rhs)) {
    return false;
  }
  const FQLMultiOpExpressionNode *rhsPtr =
    static_cast<const FQLMultiOpExpressionNode *>(&rhs);
  if (operands.size() != rhsPtr->operands.size()) {
    return false;
  }
  for (unsigned int i = 0; i < operands.size(); i++) {
    if (!(*(operands[i]) == *(rhsPtr->operands[i]))) {
      return false;
    }
  }
  return true;
}

bool FQLInArrayNode::operator==(const FQLExpressionNode &rhs) const {
  if (!checkType(rhs)) {
    return false;
  }
  const FQLInArrayNode *rhsPtr =
    static_cast<const FQLInArrayNode *>(&rhs);
  return ((*expr == *(rhsPtr->expr)) && (*list == *(rhsPtr->list)));
}

bool FQLFieldExpressionNode::operator==(const FQLExpressionNode &rhs) const {
  if (!checkType(rhs)) {
    return false;
  }
  const FQLFieldExpressionNode *rhsPtr =
    static_cast<const FQLFieldExpressionNode *>(&rhs);
  return (field == rhsPtr->field);
}

bool FQLNotExpressionNode::operator==(const FQLExpressionNode &rhs) const {
  if (!checkType(rhs)) {
    return false;
  }
  const FQLNotExpressionNode *rhsPtr =
    static_cast<const FQLNotExpressionNode *>(&rhs);
  return (*expr == *(rhsPtr->expr));
}

bool FQLOrderByNode::operator==(const FQLOrderByNode &rhs) const {
  return (*orderby == *(rhs.orderby) && desc == rhs.desc);
}

bool FQLLimitNode::operator==(const FQLLimitNode &rhs) const {
  return (limit == rhs.limit && offset == rhs.offset);
}

bool FQLStatementNode::operator==(const FQLStatementNode &rhs) const {
  return ((*select == *(rhs.select)) &&
          (from == rhs.from) &&
          (*where == *(rhs.where)) &&
          ((orderby == NULL && rhs.orderby == NULL) ||
           (*orderby == *(rhs.orderby))) &&
          ((limit == NULL && rhs.limit == NULL) ||
          (*limit == *(rhs.limit))));
}

bool FQLInStatementNode::operator==(const FQLExpressionNode &rhs) const {
  if (!checkType(rhs)) {
    return false;
  }
  const FQLInStatementNode *rhsPtr =
    static_cast<const FQLInStatementNode *>(&rhs);
  return (*expr == *(rhsPtr->expr) && *statement == *(rhsPtr->statement));
}

void FQLParserState::deleteStmtList(vector<FQLParserState> states) {
  for (unsigned int i = 0; i < states.size(); i++) {
    if (states[i].stmt) {
      delete states[i].stmt;
    }
  }
}
