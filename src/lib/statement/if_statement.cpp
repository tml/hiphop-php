/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 Facebook, Inc. (http://www.facebook.com)          |
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

#include <lib/statement/if_statement.h>
#include <lib/statement/if_branch_statement.h>
#include <lib/statement/statement_list.h>
#include <lib/expression/constant_expression.h>
#include <lib/statement/block_statement.h>

using namespace HPHP;
using namespace std;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////
// constructors/destructors

IfStatement::IfStatement
(STATEMENT_CONSTRUCTOR_PARAMETERS, StatementListPtr stmts)
  : Statement(STATEMENT_CONSTRUCTOR_PARAMETER_VALUES), m_stmts(stmts) {
}

StatementPtr IfStatement::clone() {
  IfStatementPtr stmt(new IfStatement(*this));
  stmt->m_stmts = Clone(m_stmts);
  return stmt;
}

int IfStatement::getRecursiveCount() const {
  return m_stmts->getRecursiveCount();
}

///////////////////////////////////////////////////////////////////////////////
// parser functions

///////////////////////////////////////////////////////////////////////////////
// static analysis functions

void IfStatement::analyzeProgram(AnalysisResultPtr ar) {
  if (m_stmts) m_stmts->analyzeProgram(ar);
}

bool IfStatement::hasDecl() const {
  return m_stmts && m_stmts->hasDecl();
}

bool IfStatement::hasRetExp() const {
  return m_stmts && m_stmts->hasRetExp();
}

ConstructPtr IfStatement::getNthKid(int n) const {
  switch (n) {
    case 0:
      return m_stmts;
    default:
      return ConstructPtr();
  }
  ASSERT(0);
}

int IfStatement::getKidCount() const {
  return 1;
}

int IfStatement::setNthKid(int n, ConstructPtr cp) {
  switch (n) {
    case 0:
      m_stmts = boost::dynamic_pointer_cast<StatementList>(cp);
      return 1;
    default:
      return 0;
  }
  ASSERT(0);
}

StatementPtr IfStatement::preOptimize(AnalysisResultPtr ar) {
  ar->preOptimize(m_stmts);
  if (ar->getPhase() != AnalysisResult::SecondPreOptimize) {
    return StatementPtr();
  }

  int i;
  int j;

  for (i = 0; i < m_stmts->getCount(); i++) {
    IfBranchStatementPtr branch =
      dynamic_pointer_cast<IfBranchStatement>((*m_stmts)[i]);
    ExpressionPtr condition = branch->getCondition();
    if (!condition) break; // else branch
    if (condition->is(Expression::KindOfConstantExpression)) {
      ConstantExpressionPtr exp =
        dynamic_pointer_cast<ConstantExpression>(condition);
      if (exp->isBoolean()) {
        if (exp->getBooleanValue()) {
          // if (true) branch
          for (j = i + 1; j < m_stmts->getCount(); j++) {
            if ((*m_stmts)[j]->hasDecl()) break;
          }
          // no declarations after if (true)
          if (j == m_stmts->getCount()) break;
        } else {
          // if (false) branch
          if (!branch->hasDecl()) {
            m_stmts->removeElement(i);
            i--;
          }
        }
      }
    }
  }

  if (i == m_stmts->getCount()) return StatementPtr();

  // either else branch or if (true) branch without further declarations

  i++;
  while (i < m_stmts->getCount()) m_stmts->removeElement(i);

  // if there is only one branch left, return stmt.
  if (m_stmts->getCount() == 1) {
    return dynamic_pointer_cast<IfBranchStatement>((*m_stmts)[0])->getStmt();
  } else if (m_stmts->getCount() == 0) {
    return NULL_STATEMENT();
  } else {
    return StatementPtr();
  }
}

StatementPtr IfStatement::postOptimize(AnalysisResultPtr ar) {
  ar->postOptimize(m_stmts);
  for (int i = 0; i < m_stmts->getCount(); i++) {
    IfBranchStatementPtr branch =
      dynamic_pointer_cast<IfBranchStatement>((*m_stmts)[i]);
    ExpressionPtr condition = branch->getCondition();
    if (!condition) {
      if (!branch->getStmt()->hasImpl()) {
        // remove else branch without C++ implementation.
        ar->incOptCounter();
        m_stmts->removeElement(i);
      }
      break;
    }
    if (condition->is(Expression::KindOfConstantExpression)) {
      ConstantExpressionPtr exp =
        dynamic_pointer_cast<ConstantExpression>(condition);
      // Remove if (false) branch without C++ implementation.
      // if (true) branch without C++ implementation is kept unless
      // it is the last branch. In general we cannot let a if (true)
      // branch short-circuit the rest branches which if removed may
      // cause g++ to complain unreferenced variables.
      if (exp->isBoolean() && !branch->getStmt()->hasImpl()) {
        if (!exp->getBooleanValue() ||
            (exp->getBooleanValue() && i == m_stmts->getCount() - 1)) {
          ar->incOptCounter();
          m_stmts->removeElement(i);
          i--;
        }
      }
    }
  }
  if (m_stmts->getCount() == 0) {
    return NULL_STATEMENT();
  } else {
    return StatementPtr();
  }
}

void IfStatement::inferTypes(AnalysisResultPtr ar) {
  if (m_stmts) m_stmts->inferTypes(ar);
}

///////////////////////////////////////////////////////////////////////////////
// code generation functions

void IfStatement::outputPHP(CodeGenerator &cg, AnalysisResultPtr ar) {
  for (int i = 0; i < m_stmts->getCount(); i++) {
    if (i > 0) cg.printf("else");
    (*m_stmts)[i]->outputPHP(cg, ar);
  }
}

void IfStatement::outputCPP(CodeGenerator &cg, AnalysisResultPtr ar) {
  for (int i = 0; i < m_stmts->getCount(); i++) {
    if (i > 0) cg.printf("else ");
    (*m_stmts)[i]->outputCPP(cg, ar);
  }
}
