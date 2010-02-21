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
#include <cpp/eval/runtime/eval_frame_injection.h>
#include <cpp/eval/runtime/variable_environment.h>
#include <cpp/eval/ast/construct.h>
#include <cpp/eval/parser/parser.h>

namespace HPHP {
namespace Eval {
///////////////////////////////////////////////////////////////////////////////

String EvalFrameInjection::getFileName() {
  return m_file;
}

Array EvalFrameInjection::getArgs() {
  return m_env.getParams();
}

void EvalFrameInjection::SetLine(const Construct *c) {
  ThreadInfo::s_threadInfo->m_top->line = c->loc()->line1;
}

///////////////////////////////////////////////////////////////////////////////
}
}

