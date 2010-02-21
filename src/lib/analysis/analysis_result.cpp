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

#include <lib/analysis/analysis_result.h>
#include <lib/analysis/file_scope.h>
#include <lib/analysis/class_scope.h>
#include <lib/analysis/dependency_graph.h>
#include <lib/analysis/code_error.h>
#include <lib/statement/statement_list.h>
#include <lib/analysis/symbol_table.h>
#include <util/logger.h>
#include <lib/package.h>
#include <lib/parser/parser.h>
#include <util/util.h>
#include <util/hash.h>
#include <lib/option.h>
#include <lib/analysis/function_scope.h>
#include <lib/system/builtin_symbols.h>
#include <lib/analysis/constant_table.h>
#include <lib/analysis/variable_table.h>
#include <lib/expression/scalar_expression.h>
#include <lib/expression/constant_expression.h>
#include <lib/expression/expression_list.h>
#include <lib/expression/array_pair_expression.h>
#include <util/process.h>
#include <cpp/base/rtti_info.h>
#include <cpp/ext/ext_json.h>

using namespace HPHP;
using namespace std;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////
// initialization

AnalysisResult::AnalysisResult()
  : BlockScope("Root", StatementPtr(), BlockScope::ProgramScope),
    m_package(NULL), m_parseOnDemand(false), m_phase(AnalyzeInclude),
    m_newlyInferred(0), m_dynamicClass(false), m_dynamicFunction(false),
    m_classForcedVariants(false), m_optCounter(0),
    m_scalarArraysCounter(0), m_paramRTTICounter(0),
    m_scalarArraySortedAvgLen(0), m_scalarArraySortedIndex(0),
    m_scalarArraySortedSumLen(0), m_scalarArrayCompressedTextSize(0) {
  m_dependencyGraph = DependencyGraphPtr(new DependencyGraph());
  m_insideScalarArray = false;
}

void AnalysisResult::appendExtraCode(const std::string &code) {
  if (m_extraCode.empty()) {
    m_extraCode = "<?php\n";
  }
  m_extraCode += code + "\n";
}

///////////////////////////////////////////////////////////////////////////////
// general functions

void AnalysisResult::setFileScope(FileScopePtr fileScope) {
  ASSERT(fileScope);
  m_file = fileScope;

  StringToFileScopePtrMap::const_iterator iter =
    m_files.find(fileScope->getName());
  ASSERT(iter == m_files.end());
  m_files[fileScope->getName()] = fileScope;
  vertex_descriptor vertex = add_vertex(m_depGraph);
  fileScope->setVertex(vertex);
  m_fileVertMap[vertex] = fileScope;
  m_fileScopes.push_back(fileScope);
}

FileScopePtr AnalysisResult::findFileScope(const std::string &name,
                                           bool parseOnDemand) {
  StringToFileScopePtrMap::const_iterator iter = m_files.find(name);
  if (iter != m_files.end()) return iter->second;

  if (parseOnDemand &&
      Option::PackageExcludeFiles.find(name) !=
      Option::PackageExcludeFiles.end()) {
    return FileScopePtr();
  }

  FileScopePtr curr = m_file;
  if (parseOnDemand && m_parseOnDemand &&
      m_package && m_package->parse(name.c_str())) {
    m_file = curr;
    iter = m_files.find(name);
    ASSERT(iter != m_files.end());
    return iter->second;
  }
  return FileScopePtr();
}

void AnalysisResult::pushScope(BlockScopePtr scope) {
  m_scope = scope;
  m_scopes.push_back(scope);
  if (scope->is(BlockScope::FileScope)) {
    m_file = dynamic_pointer_cast<HPHP::FileScope>(scope);
  }
}

void AnalysisResult::popScope() {
  ASSERT(!m_scopes.empty());
  m_scopes.pop_back();
  if (m_scopes.empty()) {
    m_scope.reset();
  } else {
    m_scope = m_scopes.back();
  }
}

ClassScopePtr AnalysisResult::getClassScope() const {
  for (int i = m_scopes.size() - 1; i >= 0; i--) {
    ClassScopePtr classScope =
      dynamic_pointer_cast<HPHP::ClassScope>(m_scopes[i]);
    if (classScope) return classScope;
  }
  return ClassScopePtr();
}

FunctionScopePtr AnalysisResult::getFunctionScope() const {
  return dynamic_pointer_cast<HPHP::FunctionScope>(getScope());
}

ClassScopePtr AnalysisResult::resolveClass(std::string &className) {
  // resolving self and parent
  if (className == "self") {
    ClassScopePtr cls = getClassScope();
    if (cls) className = cls->getName();
  } else if (className == "parent") {
    ClassScopePtr cls = getClassScope();
    if (cls && !cls->getParent().empty()) {
      className = cls->getParent();
    }
  }
  return findClass(className);
}

FunctionScopePtr AnalysisResult::findFunction(const std::string &funcName) {
  StringToFunctionScopePtrVecMap::const_iterator bit =
    m_functions.find(funcName);
  if (bit != m_functions.end()) {
    return bit->second[0];
  }
  StringToFunctionScopePtrVecMap::const_iterator iter =
    m_functionDecs.find(funcName);
  if (iter != m_functionDecs.end()) {
    return iter->second.back();
  }
  return FunctionScopePtr();
}

FunctionScopePtr AnalysisResult::findHelperFunction(const std::string &funcName) {
  StringToFunctionScopePtrVecMap::const_iterator bit =
    m_helperFunctions.find(funcName);
  if (bit != m_helperFunctions.end()) {
    return bit->second[0];
  }
  return FunctionScopePtr();
}

BlockScopePtr AnalysisResult::findConstantDeclarer(const std::string &name) {
  if (getConstants()->isPresent(name)) return shared_from_this();
  StringToFileScopePtrMap::const_iterator iter = m_constDecs.find(name);
  if (iter != m_constDecs.end()) return iter->second;
  return BlockScopePtr();
}

ClassScopePtr AnalysisResult::findClass(const std::string &name,
                                        FindClassBy by /* = ClassName */) {
  AnalysisResultPtr ar = shared_from_this();
  if (by == PropertyName) return ClassScopePtr();

  if (by == MethodName) {
    StringToClassScopePtrVecMap::iterator iter =
      m_methodToClassDecs.find(name);
    if (iter != m_methodToClassDecs.end()) {
      if (iter->second.size() == 1) {
        iter->second[0]->findFunction(ar, name, true)->setDynamic();
        return ClassScopePtr();
      } else {
        // The call to findClass by method name means all these
        // same-named methods should be dynamic since there will
        // be an invoke to call one of them.
        BOOST_FOREACH(ClassScopePtr cls, iter->second) {
          FunctionScopePtr func = cls->findFunction(ar, name, true);
          // Something fishy here
          if (func) {
            func->setDynamic();
          }
        }
        iter->second.clear();
      }
    }
  } else {
    StringToClassScopePtrMap::const_iterator sysIter =
      m_systemClasses.find(name);
    if (sysIter != m_systemClasses.end()) return sysIter->second;

    StringToClassScopePtrVecMap::const_iterator iter = m_classDecs.find(name);
    if (iter != m_classDecs.end()) {
      return iter->second.back();
    }
  }
  return ClassScopePtr();
}

const ClassScopePtrVec &AnalysisResult::findClasses(const std::string &name) {
  return m_classDecs[name];
}

bool AnalysisResult::classMemberExists(const std::string &name,
                                       FindClassBy by) {
  if (by == MethodName) {
    return m_methodToClassDecs.find(name) != m_methodToClassDecs.end();
  }
  return  m_classDecs.find(name) != m_classDecs.end();
}

FunctionContainerPtr AnalysisResult::getFunctionContainer() {
  ClassScopePtr cls = getClassScope();
  if (cls) return cls;
  return getFileScope();
}

int AnalysisResult::getFunctionCount() const {
  int total = 0;
  for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
       iter != m_files.end(); ++iter) {
    total += iter->second->getFunctionCount();
  }
  return total;
}

int AnalysisResult::getClassCount() const {
  int total = 0;
  for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
       iter != m_files.end(); ++iter) {
    total += iter->second->getClassCount();
  }
  return total;
}

void AnalysisResult::countReturnTypes(std::map<std::string, int> &counts) {
  for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
       iter != m_files.end(); ++iter) {
    iter->second->countReturnTypes(counts);
  }
}

///////////////////////////////////////////////////////////////////////////////
// static analysis functions

bool AnalysisResult::declareFunction(FunctionScopePtr funcScope) {
  string fname = funcScope->getName();
  // System functions override
  if (m_functions.find(fname) != m_functions.end()) {
    return false;
  }

  FunctionScopePtrVec &funcs = m_functionDecs[fname];
  if (funcs.size() == 1) {
    funcs[0]->setRedeclaring(0);
    getCodeError()->record(CodeError::DeclaredFunctionTwice,
                           funcs[0]->getStmt(),
                           funcScope->getStmt());
  }
  if (funcs.size() > 0 ||
      Option::DynamicInvokeFunctions.find(fname) !=
      Option::DynamicInvokeFunctions.end()) {
    funcScope->setRedeclaring(funcs.size());
  }
  funcs.push_back(funcScope);
  return true;
}

bool AnalysisResult::declareClass(ClassScopePtr classScope) {
  string cname = classScope->getName();
  // System classes override
  if (m_systemClasses.find(cname) != m_systemClasses.end()) {
    return false;
  }
  AnalysisResultPtr ar = shared_from_this();
  ClassScopePtrVec &classes = m_classDecs[cname];
  if (classes.size() == 1) {
    classes[0]->setRedeclaring(ar, 0);
    getCodeError()->record(CodeError::DeclaredClassTwice,
                           classScope->getStmt(),
                           classes[0]->getStmt(),
                           classScope->getName().c_str());
  }
  if (classes.size() > 0) {
    classScope->setRedeclaring(ar, classes.size());
  }
  if (m_classForcedVariants) {
    classScope->getVariables()->forceVariants(ar);
  }
  classes.push_back(classScope);
  return true;
}

bool AnalysisResult::declareConst(FileScopePtr fs, const string& name) {
  if (getConstants()->isPresent(name) ||
      m_constDecs.find(name) != m_constDecs.end()) {
    m_constRedeclared.insert(name);
    return false;
  } else {
    m_constDecs[name] = fs;
    return true;
  }
}

////// Dependencies /////////////////////////////////////

void AnalysisResult::link(FileScopePtr user, FileScopePtr provider) {
  if (user != provider) {
    add_edge(user->vertex(), provider->vertex(), m_depGraph);
  }
}

bool AnalysisResult::addClassDependency(FileScopePtr usingFile,
                                        const std::string &className) {
  if (BuiltinSymbols::s_classes.find(className) !=
      BuiltinSymbols::s_classes.end())
    return true;

  StringToClassScopePtrVecMap::const_iterator iter =
    m_classDecs.find(className);
  if (iter == m_classDecs.end() || iter->second.size() != 1) return false;
  ClassScopePtr classScope = iter->second[0];
  FileScopePtr fileScope = classScope->getFileScope();
  link(usingFile, fileScope);
  return true;
}
bool AnalysisResult::addFunctionDependency(FileScopePtr usingFile,
                                           const std::string &functionName) {
  if (BuiltinSymbols::s_functions.find(functionName) !=
      BuiltinSymbols::s_functions.end())
    return true;
  StringToFunctionScopePtrVecMap::const_iterator iter =
    m_functionDecs.find(functionName);
  if (iter == m_functionDecs.end() ||
      iter->second.size() != 1) return false;
  FunctionScopePtr functionScope = iter->second[0];
  FileScopePtr fileScope = functionScope->getFileScope();
  link(usingFile, fileScope);
  return true;
}
bool AnalysisResult::addIncludeDependency(FileScopePtr usingFile,
                                          const std::string &includeFilename) {
  ASSERT(!includeFilename.empty());
  FileScopePtr fileScope = findFileScope(includeFilename, true);
  if (fileScope) {
    link(usingFile, fileScope);
    return true;
  } else {
    return false;
  }
}
bool AnalysisResult::addConstantDependency(FileScopePtr usingFile,
                                           const std::string &constantName) {
  if (m_constants->isPresent(constantName))
    return true;

  StringToFileScopePtrMap::const_iterator iter = m_constDecs.find(constantName);
  if (iter == m_constDecs.end()) return false;
  FileScopePtr fileScope = iter->second;
  link(usingFile, fileScope);
  return true;
}

bool AnalysisResult::isConstantDeclared(const std::string &constName) {
  if (m_constants->isPresent(constName)) return true;
  StringToFileScopePtrMap::const_iterator iter = m_constDecs.find(constName);
  if (iter == m_constDecs.end()) return false;
  FileScopePtr fileScope = iter->second;
  ConstantTablePtr constants = fileScope->getConstants();
  ConstructPtr decl = constants->getValue(constName);
  if (decl) return true;
  return false;
}

bool AnalysisResult::isBaseSysRsrcClass(const std::string &className) {
  return m_baseSysRsrcClasses.find(className) != m_baseSysRsrcClasses.end();
}

bool AnalysisResult::isConstantRedeclared(const std::string &constName) {
  return m_constRedeclared.find(constName) != m_constRedeclared.end();
}

void AnalysisResult::addCallee(StatementPtr stmt) {
  if (m_calleesAdded.find(stmt) == m_calleesAdded.end()) {
    m_callees.push_back(stmt);
    m_calleesAdded.insert(stmt);
  }
}

///// Program ////////////////////////////

void AnalysisResult::loadBuiltins() {
  AnalysisResultPtr ar = shared_from_this();
  BuiltinSymbols::loadFunctions(ar, m_functions);
  BuiltinSymbols::loadHelperFunctions(ar, m_helperFunctions);
  BuiltinSymbols::loadClasses(ar, m_systemClasses);
  BuiltinSymbols::loadVariables(ar, m_variables);
  BuiltinSymbols::loadConstants(ar, m_constants);
  BuiltinSymbols::loadBaseSysRsrcClasses(ar, m_baseSysRsrcClasses);
}

void AnalysisResult::analyzeProgram() {
  AnalysisResultPtr ar = shared_from_this();

  getVariables()->forceVariants(ar);
  getVariables()->setAttribute(VariableTable::ContainsLDynamicVariable);
  getVariables()->setAttribute(VariableTable::ContainsExtract);
  pushScope(ar);

  // Analyze Includes
  Logger::Verbose("Analyzing Includes");
  setPhase(AnalysisResult::AnalyzeInclude);
  unsigned int i;
  for (i = 0; i < m_fileScopes.size(); i++) {
    m_fileScopes[i]->analyzeProgram(ar);
  }
  if (!m_extraCode.empty()) {
    string filename = Option::LambdaPrefix + "lambda";
    Parser::parseString(m_extraCode.c_str(), ar, filename.c_str());
  }
  for (; i < m_fileScopes.size(); i++) {
    m_fileScopes[i]->analyzeProgram(ar);
  }

  // Analyze All
  Logger::Verbose("Analyzing All");
  setPhase(AnalysisResult::AnalyzeAll);
  for (i = 0; i < m_fileScopes.size(); i++) {
    m_fileScopes[i]->analyzeProgram(ar);
  }

  // Collect methods
  string cname;
  ClassScopePtr cls;
  for (StringToClassScopePtrVecMap::const_iterator iter = m_classDecs.begin();
       iter != m_classDecs.end(); ++iter) {
    BOOST_FOREACH(cls, iter->second) {
      if (cls->isRedeclaring()) {
        cls->setStaticDynamic(ar);
      }
      StringToFunctionScopePtrMap methods;
      cls->collectMethods(ar, methods);
      bool needAbstractMethodImpl =
        (!cls->isAbstract() && !cls->isInterface());
      for (StringToFunctionScopePtrMap::const_iterator iterMethod =
           methods.begin(); iterMethod != methods.end(); ++iterMethod) {
        FunctionScopePtr func = iterMethod->second;
        if (!func->hasImpl() && needAbstractMethodImpl) {
          FunctionScopePtr tmpFunc =
            cls->findFunction(ar, func->getName(), true, true);
          if (!tmpFunc || !tmpFunc->hasImpl()) {
              getCodeError()->record(CodeError::MissingAbstractMethodImpl,
                                     cls->getStmt(), func->getStmt());
          }
        }
        m_methodToClassDecs[iterMethod->first].push_back(cls);
      }
    }
  }
  BOOST_FOREACH(tie(cname, cls), m_systemClasses) {
    StringToFunctionScopePtrMap methods;
    cls->collectMethods(ar, methods);
    for (StringToFunctionScopePtrMap::const_iterator iterMethod =
           methods.begin(); iterMethod != methods.end(); ++iterMethod) {
      m_methodToClassDecs[iterMethod->first].push_back(cls);
    }
  }
}


void AnalysisResult::analyzeProgramFinal() {
  AnalysisResultPtr ar = shared_from_this();
  setPhase(AnalysisResult::AnalyzeFinal);
  for (uint i = 0; i < m_fileScopes.size(); i++) {
    m_fileScopes[i]->analyzeProgram(ar);
  }
}

void AnalysisResult::inferTypes(int maxPass /* = 100 */) {
  AnalysisResultPtr ar = shared_from_this();
  setPhase(FirstInference);
  int lastInferred = 0;
  bool lastInference = false;
  for (int i = 0; i < maxPass; i++) {
    m_newlyInferred = 0;
    for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
         iter != m_files.end(); ++iter) {
      FileScopePtr file = iter->second;
      pushScope(file);
      file->inferTypes(ar);
      popScope();
    }
    if (lastInference) {
      return;
    }
    if (i > 1 && m_newlyInferred == 0 /* lastInferred */) {
      lastInference = true;
    }
    lastInferred = m_newlyInferred;
    Logger::Verbose("newly inferred types: %d", m_newlyInferred);
    if (lastInference) {
      setPhase(LastInference);
    } else {
      setPhase(i == 0 ? SecondInference : MoreInference);
    }
  }
  ASSERT(false);
}

static void dumpVisitor(AnalysisResultPtr ar, StatementPtr s, void *data)
{
  s->dump(0, ar);
}

void AnalysisResult::dump() {
  visitFiles(dumpVisitor, 0);
}

void AnalysisResult::visitFiles(void (*cb)(AnalysisResultPtr, StatementPtr, void*), void *data) {
  AnalysisResultPtr ar = shared_from_this();
  for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
       iter != m_files.end(); ++iter) {
    FileScopePtr file = iter->second;

    file->visit(ar, cb, data);
  }
}

///////////////////////////////////////////////////////////////////////////////
// optimization functions

void AnalysisResult::preOptimize(int maxPass /* = 100 */) {
  AnalysisResultPtr ar = shared_from_this();
  int lastOptCounter;
  int i;
  setPhase(FirstPreOptimize);
  while (true) {
    for (i = 0; i < maxPass; i++) {
      lastOptCounter = m_optCounter;
      for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
           iter != m_files.end(); ++iter) {
        FileScopePtr file = iter->second;
        pushScope(file);
        file->preOptimize(ar);
        popScope();
      }
      if (lastOptCounter == m_optCounter) break;
    }
    ASSERT(i <= 2);
    if (m_phase == FirstPreOptimize) {
      setPhase(SecondPreOptimize);
    } else {
      break;
    }
  }
}

void AnalysisResult::postOptimize(int maxPass /* = 100 */) {
  AnalysisResultPtr ar = shared_from_this();
  setPhase(AnalysisResult::PostOptimize);
  int lastOptCounter;
  int i;
  for (i = 0; i < maxPass; i++) {
    lastOptCounter = m_optCounter;
    for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
         iter != m_files.end(); ++iter) {
      FileScopePtr file = iter->second;
      pushScope(file);
      file->postOptimize(ar);
      popScope();
    }
    if (lastOptCounter == m_optCounter) break;
  }
  ASSERT(i <= 100);
}

///////////////////////////////////////////////////////////////////////////////
// code generation functions

int AnalysisResult::registerScalarArray(ExpressionPtr pairs) {
  if (!Option::ScalarArrayOptimization || m_insideScalarArray) {
    return -1;
  }

  string text;
  if (pairs) {
    // Normal picked PHP wouldn't work here, because predefined constants,
    // e.g., __CLASS__, need to be translated.
    text = pairs->getText(false, true);
  }
  std::map<std::string, int>::const_iterator iter = m_scalarArrays.find(text);
  if (iter != m_scalarArrays.end()) return iter->second;
  int id = m_scalarArraysCounter++;
  m_scalarArrays[text] = id;
  m_scalarArrayIds.push_back(pairs);
  return id;
}

bool AnalysisResult::getInsideScalarArray() {
  return m_insideScalarArray;
}

void AnalysisResult::setInsideScalarArray(bool flag) {
  m_insideScalarArray = flag;
}

string AnalysisResult::prepareFile(const char *root, const string &fileName,
                                   bool chop) {
  string fullPath = root;
  if (!fullPath.empty() && fullPath[fullPath.size() - 1] != '/') {
    fullPath += "/";
  }

  string file = fileName;
  size_t npos = file.rfind('/');
  if (npos != string::npos) {
    file = file.substr(npos + 1);
  }

  if (chop && file.size() > 4 && file.substr(file.length() - 4) == ".php") {
    fullPath += file.substr(0, file.length() - 4);
  } else {
    fullPath += file;
  }
  for (int pos = strlen(root); pos < (int)fullPath.size(); pos++) {
    if (fullPath[pos] == '/') {
      mkdir(fullPath.substr(0, pos).c_str(), 0777);
    }
  }
  return fullPath;
}

void AnalysisResult::forceClassVariants() {
  if (m_classForcedVariants) {
    return;
  }
  m_classForcedVariants = true;

  AnalysisResultPtr ar = shared_from_this();
  for (StringToClassScopePtrVecMap::const_iterator iter = m_classDecs.begin();
       iter != m_classDecs.end(); ++iter) {
    BOOST_FOREACH(ClassScopePtr cls, iter->second) {
      cls->getVariables()->forceVariants(ar);
    }
  }
}

void AnalysisResult::forceClassVariants(const std::string &name) {
  if (m_classForcedVariants) {
    return;
  }

  AnalysisResultPtr ar = shared_from_this();
  for (StringToClassScopePtrVecMap::const_iterator iter = m_classDecs.begin();
       iter != m_classDecs.end(); ++iter) {
    BOOST_FOREACH(ClassScopePtr cls, iter->second) {
      cls->getVariables()->forceVariant(ar, name);
    }
  }
}

bool AnalysisResult::outputAllPHP(CodeGenerator::Output output) {
  AnalysisResultPtr ar = shared_from_this();
  switch (output) {
  case CodeGenerator::PickledPHP:
    for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
         iter != m_files.end(); ++iter) {
      string fullPath = prepareFile(m_outputPath.c_str(), iter->first, false);
      ofstream f(fullPath.c_str());
      if (f) {
        CodeGenerator cg(&f, output);
        cg.printf("<?php\n");
        Logger::Info("Generating %s...", fullPath.c_str());
        iter->second->getStmt()->outputPHP(cg, ar);
        f.close();
      } else {
        Logger::Error("Unable to open %s for write", fullPath.c_str());
      }
    }
    return true; // we are done
  default:
    ASSERT(false);
  }

  CodeErrorPtr codeError = getCodeError();
  if (codeError->exists(CodeError::DeclaredFunctionTwice)) {
    Logger::Error("Same function was declared twice."
                  "\nCheck CodeError.js under DeclaredFunctionTwice.");
    return false;
  }
  if (codeError->exists(CodeError::DeclaredClassTwice)) {
    Logger::Error("Same class was declared twice."
                  "\nCheck CodeError.js under DeclaredClassTwice.");
    return false;
  }
  if (codeError->exists(CodeError::BadPHPIncludeFile)) {
    Logger::Error("There are some bad include_once or require_once statements."
                  "\nCheck CodeError.js under BadPHPIncludeFile.");
    return false;
  }

  return true;
}

void AnalysisResult::getTrueDeps(FileScopePtr f,
                                 map<string, FileScopePtr> &trueDeps) {
  adjacency_iterator adj, adjEnd;
  for (tie(adj, adjEnd) = adjacent_vertices(f->vertex(), m_depGraph);
       adj != adjEnd; adj++) {
    VertexToFileScopePtrMap::const_iterator iter = m_fileVertMap.find(*adj);
    ASSERT(iter != m_fileVertMap.end());
    trueDeps[iter->second->getName()] = iter->second;
  }
}

void AnalysisResult::clusterByFileSizes(StringToFileScopePtrVecMap &clusters,
                                        int clusterCount) {
  ASSERT(clusterCount > 0);

  std::map<std::string, FileScopePtr> sortedFiles;
  long totalSize = 0;
  BOOST_FOREACH(FileScopePtr f, m_fileScopes) {
    totalSize += f->getSize();
    sortedFiles[f->getName()] = f;
  }

  const int FUZZYNESS = 1024; // 1kB

  int clusterSize = totalSize / clusterCount;
  int size = 0;
  int count = 1;
  string clusterName = Option::FormatClusterFile(count);
  FileScopePtrVec largeFiles;
  for (std::map<std::string, FileScopePtr>::const_iterator iter =
         sortedFiles.begin(); iter != sortedFiles.end(); ++iter) {
    FileScopePtr f = iter->second;
    if (f->getSize() > clusterSize) {
      largeFiles.push_back(f);
    } else {
      size += f->getSize();
      if ((size / FUZZYNESS) > (clusterSize / FUZZYNESS)) {
        clusterName = Option::FormatClusterFile(++count);
        size = f->getSize();
      }
      clusters[clusterName].push_back(f);
    }
  }
  for (unsigned int i = 0; i < largeFiles.size(); i++) {
    clusters[Option::FormatClusterFile(++count)].push_back(largeFiles[i]);
  }
}

void AnalysisResult::repartitionCPP(const string &filename, int64 targetSize,
                                    bool insideHPHP) {
  struct stat results;
  if (stat(filename.c_str(), &results)) {
    // error, shouldn't happen
    return;
  }
  int64 size = (int64)results.st_size;
  if (size <= targetSize * 2) {
    // tolerable size
    return;
  }

  vector <string> includes;
  vector <string> preface;
  bool inPreface = false;
  string line;
  int64 current = 0;
  int seq = 0;
  ifstream fin(filename.c_str());
  string base = filename.substr(0, filename.length() - 4);
  char foutName[PATH_MAX];
  snprintf(foutName, sizeof(foutName), "%s-%d.cpp", base.c_str(), seq);
  ofstream fout(foutName);
  while (getline(fin, line)) {
    fout << line << endl;
    current += line.length() + 1;
    if (inPreface) preface.push_back(line);

    if (line[0] == '#') {
      includes.push_back(line);
    } else if (line == "/* preface starts */") {
      inPreface = true;
      preface.clear();
      preface.push_back(line);
    } else if (line == "/* preface finishes */") {
      inPreface = false;
    } else if (line == "} /* function */") {
      // a possible cut point
      if (current > targetSize) {
        if (insideHPHP) {
          // namespace HPHP
          fout << "}" << endl;
        }

        fout.close();
        snprintf(foutName, sizeof(foutName),
                 "%s-%d.cpp", base.c_str(), ++seq);
        fout.open(foutName);
        current = 0;
        for (unsigned int j = 0; j < includes.size(); j++) {
          fout << includes[j] << endl;
        }
        if (insideHPHP) {
          fout << "namespace HPHP {" << endl;
        }
        for (unsigned int j = 0; j < preface.size(); j++) {
          fout << preface[j] << endl;
        }
      }
    }
  }
  fout.close();

  fin.close();
  remove(filename.c_str());
}

void AnalysisResult::repartitionLargeCPP(const vector<string> &filenames,
                                         const vector<string> &additionals) {
  int64 totalSize = 0;
  int count = 0;
  for (unsigned int i = 0; i < filenames.size(); i++) {
    struct stat results;
    if (stat(filenames[i].c_str(), &results) == 0) {
      totalSize += results.st_size;
      count++;
    }
  }
  int64 averageSize = totalSize / count;
  for (unsigned int i = 0; i < filenames.size(); i++) {
    repartitionCPP(filenames[i], averageSize, true);
  }
  for (unsigned int i = 0; i < additionals.size(); i++) {
    repartitionCPP(additionals[i], averageSize, false);
  }
}

void AnalysisResult::outputAllCPP(CodeGenerator::Output output,
                                  int clusterCount,
                                  const std::string *compileDir) {
  if (output == CodeGenerator::SystemCPP) {
    Option::GenerateCPPMain = false;
    outputCPPSystem();
  }

  FileScopePtrVec trueDeps;
  StringToFileScopePtrVecMap clusters;
  if (clusterCount > 0) {
    clusterByFileSizes(clusters, clusterCount);
  } else {
    BOOST_FOREACH(FileScopePtr f, m_fileScopes) {
      clusters[f->outputFilebase()].push_back(f);
    }
  }

  if (Option::GenerateCPPMacros && output != CodeGenerator::SystemCPP) {
    // system functions are currently unchanged
    createGlobalFuncTable();
  }

  vector <string> filenames;

  AnalysisResultPtr ar = shared_from_this();
  string root = getOutputPath() + "/";
  for (StringToFileScopePtrVecMap::const_iterator iter = clusters.begin();
       iter != clusters.end(); ++iter) {
    // for each cluster, generate one implementation file
    Util::mkdir(root + iter->first);
    string filename = root + iter->first + ".cpp";
    filenames.push_back(filename);
    ofstream f(filename.c_str());
    if (compileDir) {
      // this is the file that will be compiled, so we need to use this
      // for source info:
      filename = *compileDir + "/" + iter->first + ".cpp";
    }
    CodeGenerator cg(&f, output, &filename);
    outputCPPClusterImpl(cg, iter->second);
    f.close();

    // for each file, generate one header and a list of class headers
    BOOST_FOREACH(FileScopePtr fs, iter->second) {
      string fileBase = fs->outputFilebase();
      Util::mkdir(root + fileBase);
      string header = fileBase + ".h";
      string fwheader = fileBase + ".fw.h";
      string fileHeader = root + header;
      string fwFileHeader = root + fwheader;
      {
        ofstream f(fwFileHeader.c_str());
        CodeGenerator cg(&f, output);
        fs->outputCPPForwardDeclHeader(cg, ar);
        f.close();
      }
      {
        ofstream f(fileHeader.c_str());
        CodeGenerator cg(&f, output);
        fs->outputCPPDeclHeader(cg, ar);
        f.close();
      }
    }

    // for each file, generate one list of class headers
    BOOST_FOREACH(FileScopePtr fs, iter->second) {
      fs->outputCPPClassHeaders(ar, output);
    }
  }

  if (Option::GenerateCPPMacros) {
    outputCPPDynamicTables(output);
  }
  if (Option::GenerateCPPMain) {
    outputCPPGlobalDeclarations();
    outputCPPMain();
    outputCPPScalarArrays(false);
    outputCPPGlobalVariablesMethods(1);
    outputCPPGlobalVariablesMethods(2);
    outputCPPGlobalVariablesMethods(3);
    outputCPPGlobalVariablesMethods(4);
    if (Option::PrecomputeLiteralStrings && m_stringLiterals.size() > 0) {
      outputCPPLiteralStringPrecomputation();
    }
    outputCPPGlobalState();
  }
  if (Option::GenerateCPPMacros && output != CodeGenerator::SystemCPP) {
    outputCPPClassMapFile();
    outputCPPSourceInfos();
    outputCPPNameMaps();
  }

  vector<string> additionalCPPs;
  if (Option::GenerateFFI) {
    outputCPPFFIStubs();
    additionalCPPs.push_back(m_outputPath + "/" + Option::FFIFilePrefix +
                             "stubs.cpp");
    outputHSFFIStubs();
    outputJavaFFIStubs();
    outputJavaFFICppImpl();
    additionalCPPs.push_back(m_outputPath + "/" + Option::FFIFilePrefix +
                             "java_stubs.cpp");
    outputJavaFFICppDecl();
    outputSwigFFIStubs();
  }

  if (clusterCount > 0) repartitionLargeCPP(filenames, additionalCPPs);

  if (Option::GenRTTIProfileData) {
    outputRTTIMetaData(Option::RTTIOutputFile.c_str());
  }
}

void AnalysisResult::outputAllCPP(CodeGenerator &cg) {
  AnalysisResultPtr ar = shared_from_this();
  cg.setOutput(CodeGenerator::MonoCPP);
  cg.setContext(CodeGenerator::CppImplementation);

  if (Option::GenerateCPPMain) {
    cg.printf("\n");
    cg.printInclude("<cpp/base/hphp.h>");
    cg.printInclude(string(Option::SystemFilePrefix) + "global_variables.h");
    cg.printf("\n");
  }

  cg.namespaceBegin();
  getVariables()->setAttribute(VariableTable::ForceGlobal);
  getVariables()->outputCPP(cg, ar);
  getVariables()->clearAttribute(VariableTable::ForceGlobal);

  cg.setContext(CodeGenerator::CppForwardDeclaration);
  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputCPPForwardDeclarations(cg, ar);
    popScope();
  }
  cg.setContext(CodeGenerator::CppDeclaration);
  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputCPPDeclarations(cg, ar);
    popScope();
  }
  cg.setContext(CodeGenerator::CppImplementation);
  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputCPPImpl(cg, ar);
    popScope();
  }
  cg.setContext(CodeGenerator::CppPseudoMain);
  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputCPPPseudoMain(cg, ar);
    popScope();
  }
  cg.namespaceEnd();

  if (Option::GenerateCPPMain) {
    cg.printf("#ifndef HPHP_BUILD_LIBRARY\n");
    cg.indentBegin("int main(int argc, char** argv) {\n");
    cg.printf("return HPHP::execute_program(argc, argv);\n");
    cg.indentEnd("}\n");
    cg.printf("#endif\n");
  }
}

void AnalysisResult::outputCPPExtClassImpl(CodeGenerator &cg) {
  AnalysisResultPtr ar = shared_from_this();

  // output create proxies
  vector<const char *> classes;
  StringToClassScopePtrVecMap merged(m_classDecs);
  for (StringToClassScopePtrMap::const_iterator iter = m_systemClasses.begin();
       iter != m_systemClasses.end(); ++iter) {
    ClassScopePtr cls = iter->second;
    bool extension = cls->getAttribute(ClassScope::Extension);
    if (cls->isInterface())
      continue;
    const char *clsName = cls->getName().c_str();
    classes.push_back(clsName);
    merged[cls->getName()].push_back(cls);
    cls->outputCPPDynamicClassImpl(cg, ar);
    if (extension) {
      pushScope(cls);
      cls->outputCPPSupportMethodsImpl(cg, ar);
      popScope();
    }
  }
  for (StringToClassScopePtrVecMap::const_iterator iter =
         m_classDecs.begin();
       iter != m_classDecs.end();
       ++iter) {
    ClassScopePtr cls = iter->second[0];
    if (cls->isInterface())
      continue;
    classes.push_back(cls->getName().c_str());
  }
  ClassScope::outputCPPDynamicClassCreateImpl(cg, merged, classes);
  ClassScope::outputCPPInvokeStaticMethodImpl(cg, merged, classes);
  ClassScope::outputCPPGetStaticPropertyImpl(cg, merged, classes);
  ClassScope::outputCPPGetClassConstantImpl(cg, merged, classes);
}

void AnalysisResult::outputCPPClassMapFile() {
  string filename = m_outputPath + "/" + Option::SystemFilePrefix +
    "class_map.cpp";
  Util::mkdir(filename);
  ofstream f(filename.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);
  cg.printf("\n");
  cg.printInclude("<cpp/base/hphp.h>");
  cg.printInclude(string(Option::SystemFilePrefix) + "global_variables.h");
  cg.printf("\n");
  cg.printf("using namespace std;\n");

  cg.namespaceBegin();
  outputCPPClassMap(cg);
  cg.namespaceEnd();
  f.close();
}

void AnalysisResult::recordSourceInfo(const std::string &fileline,
                                      LocationPtr loc) {
  // we only need one to one mapping, and there doesn't seem to be a need
  // to display multiple PHP file locations for one C++ frame
  m_sourceInfos[fileline] = loc;
}

void AnalysisResult::recordClassSource(const std::string &clsname,
                                       const std::string &filename) {
  m_clsNameMap[clsname].insert(filename);
}

void AnalysisResult::recordFunctionSource(const std::string &funcname,
                                          const std::string &filename) {
  m_funcNameMap[funcname].insert(filename);
}

void AnalysisResult::outputCPPSourceInfos() {
  string filename = m_outputPath + "/" + Option::SystemFilePrefix +
    "source_info.cpp";
  Util::mkdir(filename);
  ofstream f(filename.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);
  cg.printf("\n");
  cg.printInclude("<cpp/base/hphp.h>");

  cg.namespaceBegin();
  cg.printf("const char *g_source_root = \"%s\";\n",
            (Process::GetCurrentDirectory() + '/').c_str());
  cg.indentBegin("const char *g_source_info[] = {\n");

  for (map<string, LocationPtr>::const_iterator
         iter = m_sourceInfos.begin(); iter != m_sourceInfos.end(); ++iter) {
    LocationPtr loc = iter->second;
    cg.printf("\"%s\", \"%s\", (const char *)%d,\n",
              iter->first.c_str(), loc->file, loc->line1);
  }

  cg.printf("NULL\n");
  cg.indentEnd("};\n");
  cg.namespaceEnd();
  f.close();
}

void AnalysisResult::outputCPPNameMaps() {
  string filename = m_outputPath + "/" + Option::SystemFilePrefix +
    "name_maps.cpp";
  Util::mkdir(filename);
  ofstream f(filename.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);
  cg.printf("\n");
  cg.printInclude("<cpp/base/hphp.h>");

  cg.namespaceBegin();

  cg.printSection("Class -> File");
  cg.indentBegin("const char *g_source_cls2file[] = {\n");
  for (map<string, set<string> >::const_iterator
         iter = m_clsNameMap.begin(); iter != m_clsNameMap.end(); ++iter) {
    for (set<string>::const_iterator iterInner = iter->second.begin();
         iterInner != iter->second.end(); ++iterInner) {
      cg.printf("\"%s\", \"%s\",\n", iter->first.c_str(), iterInner->c_str());
    }
  }
  cg.printf("NULL\n");
  cg.indentEnd("};\n");


  cg.printSection("Function -> File");
  cg.indentBegin("const char *g_source_func2file[] = {\n");
  for (map<string, set<string> >::const_iterator
         iter = m_funcNameMap.begin(); iter != m_funcNameMap.end(); ++iter) {
    for (set<string>::const_iterator iterInner = iter->second.begin();
         iterInner != iter->second.end(); ++iterInner) {
      cg.printf("\"%s\", \"%s\",\n", iter->first.c_str(), iterInner->c_str());
    }
  }
  cg.printf("NULL\n");
  cg.indentEnd("};\n");

  cg.printSection("Param RTTI Id -> Name");
  cg.indentBegin("const char *g_paramrtti_map[] = {\n");
  if (Option::GenRTTIProfileData) {
    char **params = (char **)calloc(m_paramRTTICounter, sizeof(char*));
    for (map<string, int>::const_iterator
         iter = m_paramRTTIs.begin(); iter != m_paramRTTIs.end(); ++iter) {
      ASSERT(params[iter->second] == NULL);
      params[iter->second] = (char *)iter->first.c_str();
    }
    for (int i = 0; i < m_paramRTTICounter; i++) {
      ASSERT(params[i]);
      cg.printf("\"%s\", // %d\n", params[i], i);
    }
    free(params);
  }
  cg.printf("NULL\n");
  cg.indentEnd("};\n");

  cg.namespaceEnd();
  f.close();
}

void AnalysisResult::outputRTTIMetaData(const char *filename) {
  ASSERT(filename && *filename);
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    throw Exception("Unable to open %s: %s", filename,
                    Util::safe_strerror(errno).c_str());
  }
  fprintf(f, "%d\n", m_paramRTTICounter);
  for (set<string>::const_iterator
       iter = m_rttiFuncs.begin(); iter != m_rttiFuncs.end(); ++iter) {
    fprintf(f, "%s\n", iter->c_str());
  }
  fclose(f);
}

void AnalysisResult::outputCPPDynamicTablesHeader
    (CodeGenerator &cg,
    bool includeGlobalVars /* = true*/, bool includes /* = true */) {
  bool system = cg.getOutput() == CodeGenerator::SystemCPP;
  if (system) {
    cg.printf("\n");
    cg.printInclude("<cpp/base/hphp_system.h>");
    cg.printInclude("<cpp/ext/ext.h>");
    cg.printInclude("<cpp/eval/eval.h>");
    cg.printf("\n");
  } else {
    cg.printf("\n");
    cg.printInclude("<cpp/base/hphp.h>");
    if (includeGlobalVars) {
      cg.printInclude(string(Option::SystemFilePrefix) + "global_variables.h");
    }
    if (Option::EnableEval >= Option::LimitedEval) {
      cg.printInclude("<cpp/eval/eval.h>");
    }
    cg.printf("\n");
  }
  if (includes) {
    string n;
    FileScopePtr f;
    BOOST_FOREACH(tie(n, f), m_files) {
      cg.printInclude(f->outputFilebase());
    }
  }
  cg.printf("\n");
  cg.printf("using namespace std;\n");

  cg.namespaceBegin();

}

void AnalysisResult::createGlobalFuncTable() {
  vector<const char *> funcs;
  for (StringToFunctionScopePtrVecMap::const_iterator iter =
         m_functionDecs.begin(); iter != m_functionDecs.end(); ++iter) {
    FunctionScopePtr func = iter->second[0];
    if (func->isDynamic() || func->isRedeclaring()) {
      funcs.push_back(iter->second[0]->name().c_str());
    }
  }
  if (funcs.size() > 0) {
    m_funcTableSize = Util::roundUpToPowerOfTwo(funcs.size() * 2);
    CodeGenerator::BuildJumpTable(funcs, m_funcTable, m_funcTableSize, true);
  } else {
    m_funcTableSize = 0;
  }
}

vector<const char *> &AnalysisResult::getFuncTableBucket(FunctionScopePtr func)
{
  string name = Util::toLower(func->getOriginalName());
  int64 hash = hash_string_i(name.c_str());
  int64 index = hash % m_funcTableSize;
  return m_funcTable[index];
}

void AnalysisResult::outputCPPDynamicTables(CodeGenerator::Output output) {
  AnalysisResultPtr ar = shared_from_this();
  bool system = output == CodeGenerator::SystemCPP;
  {
    string tablePath = m_outputPath + "/" + Option::SystemFilePrefix +
      "dynamic_table_func.no.cpp";
    Util::mkdir(tablePath);
    ofstream fTable(tablePath.c_str());
    CodeGenerator cg(&fTable, output);

    outputCPPDynamicTablesHeader(cg, false, false);

    cg.printSection("Function Invoke Proxies", false);
    for (CodeGenerator::MapIntToStringVec::const_iterator it =
           m_funcTable.begin(); it != m_funcTable.end(); it++) {
      if (it->second.size() == 1 &&
          !findFunction(it->second[0])->isRedeclaring()) {
        // no conflict
        cg.printf("Variant %s%s(const char *s, CArrRef params, int64 hash, "
                  "bool fatal);\n",
                  Option::InvokePrefix, it->second.front());
      } else {
        for (unsigned int i = 0; i < it->second.size(); i++) {
          const char *name = it->second.at(i);
          StringToFunctionScopePtrVecMap::const_iterator iterFunc =
            m_functionDecs.find(name);
          ASSERT(iterFunc != m_functionDecs.end());
          cg.printf("Variant %s%s(CArrRef params);\n", Option::InvokePrefix,
                    name);
        }
        cg.indentBegin("static Variant invoke_case_%d(const char *s, "
                       "CArrRef params, int64 hash, bool fatal) {\n",
                       it->first);
        for (unsigned int i = 0; i < it->second.size(); i++) {
          const char *name = it->second.at(i);
          StringToFunctionScopePtrVecMap::const_iterator iterFuncs =
            m_functionDecs.find(name);
          cg.printf("HASH_INVOKE(0x%016llXLL, %s);\n",
                    hash_string_i(name), name);
        }
        cg.printf("return invoke_builtin(s, params, hash, fatal);\n");
        cg.indentEnd("}\n");
      }
    }

    cg.printSection("Function Invoke Table");
    if (system) {
      outputCPPJumpTable(cg, ar);
      outputCPPEvalInvokeTable(cg, ar);
    } else {
      if (m_funcTableSize > 0) {
        // initializes the function pointer array
        cg.printf("static Variant (*funcTable[%d])"
                  "(const char *, CArrRef, int64, bool);\n", m_funcTableSize);
        cg.indentBegin("static class FuncTableInitializer {\n");
        cg.indentBegin("public: FuncTableInitializer() {\n");
        cg.printf("for (int i = 0; i < %d; i++) "
                    "funcTable[i] = &invoke_builtin;\n", m_funcTableSize);
        for (CodeGenerator::MapIntToStringVec::const_iterator it =
               m_funcTable.begin(); it != m_funcTable.end(); it++) {
          if (it->second.size() == 1 &&
              !findFunction(it->second[0])->isRedeclaring()) {
            cg.printf("funcTable[%d] = &%s%s;\n", it->first,
                      Option::InvokePrefix, it->second.front());
          } else {
            cg.printf("funcTable[%d] = &invoke_case_%d;\n",
                      it->first, it->first);
          }
        }
        cg.indentEnd("}\n");
        cg.indentEnd("} func_table_initializer;\n");
      }

      cg.indentBegin("Variant invoke(const char *s, CArrRef params,"
                     " int64 hash, bool tryInterp /* = true */, "
                     "bool fatal /* = true */) {\n");

      if (!Option::DynamicInvokeFunctions.empty() ||
          Option::EnableEval == Option::FullEval) {
        cg.printf("hphp_const_char_map<const char*> &funcs = "
                  "get_renamed_functions();\n");
        cg.printf("hphp_const_char_map<const char*>::const_iterator iter ="
                  " funcs.find(s);\n");
        cg.indentBegin("if (iter != funcs.end()) {\n");
        cg.printf("s = iter->second;\n");
        cg.printf("hash = -1;\n");
        cg.indentEnd("}\n");
      }
      // Eval invoke hook
      if (!system && Option::EnableEval == Option::FullEval) {
        // See if there's an eval'd version
        cg.indentBegin("if (tryInterp) {\n");
        cg.printf("Variant r;\n");
        cg.printf("if (eval_invoke_hook(r, s, params, hash)) "
                  "return r;\n");
        cg.indentEnd("}\n");
      }

      if (m_funcTableSize > 0) {
        cg.printf("if (hash < 0) hash = hash_string_i(s);\n");
        cg.printf("return funcTable[hash & %d](s, params, hash, fatal);\n",
                  m_funcTableSize - 1);
      } else {
        cg.printf("return invoke_builtin(s, params, hash, fatal);\n");
      }
      cg.indentEnd("}\n");

      outputCPPEvalInvokeTable(cg, ar);
    }
    cg.namespaceEnd();
    fTable.close();
  }
  {
    string n;
    string tablePath = m_outputPath + "/" + Option::SystemFilePrefix +
      "dynamic_table_class.no.cpp";
    Util::mkdir(tablePath);
    ofstream fTable(tablePath.c_str());
    CodeGenerator cg(&fTable, output);

    outputCPPDynamicTablesHeader(cg, true, false);
    cg.printSection("Class Invoke Tables");
    vector<const char*> classes;
    ClassScopePtr cls;
    for (StringToClassScopePtrVecMap::const_iterator iter =
           m_classDecs.begin();
         iter != m_classDecs.end();
         ++iter) {
      cls = iter->second[0];
      if (!cls->isUserClass() || cls->isInterface()) continue;
      classes.push_back(cls->getName().c_str());
      if (!system) {
        cls->outputCPPDynamicClassDecl(cg);
      }
      cls->outputCPPGlobalTableWrappersDecl(cg, ar);
    }
    if (system) {
      BOOST_FOREACH(tie(n, cls), m_systemClasses) {
        if (!cls->isInterface()) {
          classes.push_back(cls->getName().c_str());
        }
      }
      outputCPPExtClassImpl(cg);
    } else {
      ClassScope::outputCPPDynamicClassCreateImpl(cg, m_classDecs, classes);
      ClassScope::outputCPPInvokeStaticMethodImpl(cg, m_classDecs, classes);
      ClassScope::outputCPPGetStaticPropertyImpl(cg, m_classDecs, classes);
      ClassScope::outputCPPGetClassConstantImpl(cg, m_classDecs, classes);
    }
    cg.namespaceEnd();
    fTable.close();
  }
  {
    string tablePath = m_outputPath + "/" + Option::SystemFilePrefix +
      "dynamic_table_constant.no.cpp";
    Util::mkdir(tablePath);
    ofstream fTable(tablePath.c_str());
    CodeGenerator cg(&fTable, output);

    outputCPPDynamicTablesHeader(cg, true, false);

    vector<const char *> strings;
    hphp_const_char_map<bool> dyns;
    if (system) {
      ConstantTablePtr ct = getConstants();
      vector<string> syms;
      ct->getSymbols(syms);
      BOOST_FOREACH(string sym, syms) {
        strings.push_back(sym.c_str());
      }
    }
    BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
      ConstantTablePtr ct = fs->getConstants();
      vector<string> syms;
      ct->getSymbols(syms);
      BOOST_FOREACH(string sym, syms) {
        if (ct->isSystem(sym) && !system) continue;
        strings.push_back(sym.c_str());
        dyns[sym.c_str()] = ct->isDynamic(sym);
      }
      ct->outputCPP(cg, ar);
    }

    cg.printSection("Get Constant Table");
    cg.indentBegin("Variant get_%sconstant(CStrRef name) {\n",
                   system ? "builtin_" : "");
    cg.printDeclareGlobals();

    if (!system && Option::EnableEval == Option::FullEval) {
      // See if there's an eval'd version
      cg.indentBegin("{\n");
      cg.printf("Variant r;\n");
      cg.printf("if (eval_constant_hook(r, name)) return r;\n");
      cg.indentEnd("}\n");
    }

    if (strings.size() > 0) {
      cg.printf("const char* s = name.data();\n");
      for (JumpTable jt(cg, strings, false, false, false); jt.ready();
           jt.next()) {
        const char *name = jt.key();
        string varName = string(Option::ConstantPrefix) + name;
        hphp_const_char_map<bool>::const_iterator it = dyns.find(name);
        bool dyn = it != dyns.end() && it->second;
        if (dyn) {
          cg.printf("HASH_RETURN(0x%016llXLL, g->%s, %s);\n",
                      hash_string(name), varName.c_str(), name);
        } else {
          cg.printf("HASH_RETURN(0x%016llXLL, %s, %s);\n",
                      hash_string(name), varName.c_str(), name);
        }
      }
    }

    if (system) {
      cg.printf("return name;\n");
    } else {
      cg.printf("return get_builtin_constant(name);\n");
    }
    cg.indentEnd("}\n");
    cg.namespaceEnd();
    fTable.close();
  }
  {
    string tablePath = m_outputPath + "/" + Option::SystemFilePrefix +
      "dynamic_table_file.no.cpp";
    Util::mkdir(tablePath);
    ofstream fTable(tablePath.c_str());
    CodeGenerator cg(&fTable, output);

    outputCPPDynamicTablesHeader(cg, false, false);
    if (output != CodeGenerator::SystemCPP) {
      cg.printSection("File Invoke Table");
      vector<const char*> entries;
      BOOST_FOREACH(FileScopePtr f, m_fileScopes) {
        entries.push_back(f->getName().c_str());
        cg.printf("Variant %s%s(bool incOnce = false, LVariableTable* variables"
                  " = NULL);\n", Option::PseudoMainPrefix,
                  Option::mangleFilename(f->getName(), true).c_str());
      }

      cg.printf("\n");
      cg.indentBegin("Variant invoke_file(CStrRef path, "
                     "bool once /* = false */, "
                     "LVariableTable* variables /* = NULL */,"
                     "const char *currentDir /* = NULL */) {\n");
      if (!system && Option::EnableEval == Option::FullEval) {
        // See if there's an eval'd version
        cg.indentBegin("{\n");
        cg.printf("Variant r;\n");
        cg.printf("if (eval_invoke_file_hook(r, path, once, variables, "
                  "currentDir)) "
                  "return r;\n");
        cg.indentEnd("}\n");
      }

      string root;
      cg.printf("String s = canonicalize_path(path, \"%s\", %d);\n",
                root.c_str(), root.size());

      for (JumpTable jt(cg, entries, false, false, true); jt.ready();
           jt.next()) {
        const char *file = jt.key();
        cg.printf("HASH_INCLUDE(0x%016llXLL, \"%s\", %s);\n",
                  hash_string(file), file,
                  Option::mangleFilename(file, true).c_str());
      }


      // when we only have one file, we default to running the file
      if (entries.size() == 1) {
        cg.printf("if (s.empty()) return %s%s(once, variables);\n",
                  Option::PseudoMainPrefix,
                  Option::mangleFilename(entries[0], true).c_str());
      }
      cg.printf("Logger::Verbose(\"Tried to invoke %%s but file not found.\", "
                "s.data());\n");
      cg.printf("return throw_missing_file(s.data());\n");
      cg.indentEnd("}\n");
    }

    cg.namespaceEnd();
    fTable.close();
  }
}

void AnalysisResult::outputCPPSystem() {
  string filename = string(Option::SystemFilePrefix) + "system_globals.h";

  string headerPath = m_outputPath + "/" + filename;
  Util::mkdir(headerPath);
  ofstream fSystem(headerPath.c_str());
  CodeGenerator cg(&fSystem, CodeGenerator::SystemCPP);

  string implPath = m_outputPath + "/" + Option::SystemFilePrefix +
    "system_globals.cpp";
  ofstream fSystemImpl(implPath.c_str());
  cg.setStream(CodeGenerator::ImplFile, &fSystemImpl);

  cg.headerBegin(filename.c_str());
  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    cg.printInclude(fs->outputFilebase());
  }

  cg.namespaceBegin();
  AnalysisResultPtr ar = shared_from_this();
  getVariables()->setAttribute(VariableTable::ForceGlobal);
  getVariables()->outputCPP(cg, ar);
  getVariables()->clearAttribute(VariableTable::ForceGlobal);
  getConstants()->outputCPP(cg, ar);
  cg.namespaceEnd();

  cg.headerEnd(filename.c_str());

  cg.setContext(CodeGenerator::CppImplementation);
  cg.useStream(CodeGenerator::ImplFile);
  cg.printf("\n");
  cg.printInclude(filename.c_str());
  cg.namespaceBegin();
  outputCPPGlobalImplementations(cg);
  cg.namespaceEnd();

  fSystemImpl.close();
  fSystem.close();

  outputCPPScalarArrays(true);
}

void AnalysisResult::outputCPPRedeclaredFunctionDecl(CodeGenerator &cg) {
  for (StringToFunctionScopePtrVecMap::const_iterator iter =
         m_functionDecs.begin(); iter != m_functionDecs.end(); ++iter) {
    const char *name = iter->first.c_str();
    if (iter->second[0]->isRedeclaring()) {
      cg.printf("Variant (*%s%s)(CArrRef params);\n",
                Option::InvokePrefix, name);
    }
  }
}

void AnalysisResult::outputCPPRedeclaredFunctionImpl(CodeGenerator &cg) {
  for (StringToFunctionScopePtrVecMap::const_iterator iter =
         m_functionDecs.begin(); iter != m_functionDecs.end(); ++iter) {
    if (iter->second[0]->isRedeclaring()) {
      const char *name = iter->first.c_str();
      cg.printf("%s%s = invoke_failed_%s;\n", Option::InvokePrefix,
                name, name);
    }
  }
}

void AnalysisResult::outputCPPRedeclaredClassDecl(CodeGenerator &cg) {
  for (StringToClassScopePtrVecMap::const_iterator iter =
         m_classDecs.begin(); iter != m_classDecs.end(); ++iter) {
    const char *name = iter->first.c_str();
    if (iter->second[0]->isRedeclaring()) {
      cg.printf("ClassStaticsPtr %s%s;\n", Option::ClassStaticsObjectPrefix,
                name);
    }
  }
}

void AnalysisResult::outputCPPRedeclaredClassImpl(CodeGenerator &cg) {
  for (StringToClassScopePtrVecMap::const_iterator iter =
         m_classDecs.begin(); iter != m_classDecs.end(); ++iter) {
    if (iter->second[0]->isRedeclaring()) {
      const char *name = iter->first.c_str();
      cg.printf("%s%s = ClassStaticsPtr(NEW(ClassStatics)(\"%s\"));\n",
                Option::ClassStaticsObjectPrefix, name, name);
    }
  }
}

void AnalysisResult::outputCPPDynamicConstantDecl(CodeGenerator &cg) {
  AnalysisResultPtr ar = shared_from_this();
  getConstants()->outputCPPDynamicDecl(cg, ar);
  for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
       iter != m_files.end(); ++iter) {
    iter->second->getConstants()->outputCPPDynamicDecl(cg, ar);
  }
  for (StringToClassScopePtrVecMap::const_iterator iter = m_classDecs.begin();
       iter != m_classDecs.end(); ++iter) {
    for (vector<ClassScopePtr>::const_iterator viter = iter->second.begin();
         viter != iter->second.end(); ++viter) {
      const ClassScopePtr &cls = *viter;
      pushScope(cls);
      cls->getConstants()->outputCPPDynamicDecl(cg, ar);
      popScope();
    }
  }
}

void AnalysisResult::outputCPPDynamicConstantImpl(CodeGenerator &cg) {
  AnalysisResultPtr ar = shared_from_this();
  getConstants()->outputCPPDynamicImpl(cg, ar);
  for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
       iter != m_files.end(); ++iter) {
    iter->second->getConstants()->outputCPPDynamicImpl(cg, ar);
  }
}

void AnalysisResult::outputCPPScalarArrayDecl(CodeGenerator &cg) {
  bool system = cg.getOutput() == CodeGenerator::SystemCPP;
  const char *prefix = system ? Option::SystemScalarArrayName :
    Option::ScalarArrayName;
  if (m_scalarArrayIds.size() > 0) {
    cg.printf("static StaticArray %s[%d];\n", prefix, m_scalarArrayIds.size());
  }
}

void AnalysisResult::outputHexBuffer(CodeGenerator &cg, const char *name,
                                     const char *buf, int len) {
  cg.printf("static const char %s[%d] = {\n", name, len);
  for (int i = 0; i < len; i++) {
    if (i % 10 == 0) cg.printf("  ");
    cg.printf("0x%02x, ", (unsigned char)buf[i]);
    if (i % 10 == 9) cg.printf("\n");
  }
  cg.printf("};\n\n");
}

string AnalysisResult::getScalarArrayCompressedText() {
  Array arrs;
  for (unsigned int i = 0; i < m_scalarArrayIds.size(); i++) {
    ExpressionPtr exp = m_scalarArrayIds[i];
    if (exp) {
      Variant value;
      bool ret = exp->getScalarValue(value);
      if (!ret) ASSERT(false);
      if (!value.isArray()) ASSERT(false);
      arrs.append(value);
    } else {
      arrs.append(StaticArray((ArrayElement*)NULL));
    }
  }
  String s = f_serialize(arrs);
  int len = s.size();
  char *zd = gzencode(s.data(), len, 9, CODING_GZIP);
  m_scalarArrayCompressedTextSize = len;
  return string(zd, len);
}

void AnalysisResult::getLiteralStringCompressed(std::string &zsdata,
                                                std::string &zldata) {
  int nstrings = m_stringLiterals.size();
  ASSERT(nstrings > 0);
  string *sortedById = new string[nstrings];
  for (map<string, pair<int, ScalarExpressionPtr> >::const_iterator it =
       m_stringLiterals.begin(); it != m_stringLiterals.end(); ++it) {
    int index = it->second.first;
    ASSERT(0 <= index && index < nstrings);
    sortedById[index] = it->first;
  }
  string sdata;
  string ldata;
  for (int i = 0; i < nstrings; i++) {
    sdata += sortedById[i];
    sdata += string("\0", 1);

    int size = sortedById[i].size();
    char buf[sizeof(size)];
    memcpy(buf, &size, sizeof(size));
    ldata += string(buf, sizeof(size));
  }
  int sdlen = sdata.size();
  char *zsd = gzencode(sdata.data(), sdlen, 9, CODING_GZIP);
  int ldlen = ldata.size();
  char *zld = gzencode(ldata.data(), ldlen, 9, CODING_GZIP);
  zsdata = string(zsd, sdlen);
  zldata = string(zld, ldlen);
}

void AnalysisResult::outputCPPScalarArrayImpl(CodeGenerator &cg) {
  if (m_scalarArrayIds.size() == 0) return;
  bool system = cg.getOutput() == CodeGenerator::SystemCPP;
  AnalysisResultPtr ar = shared_from_this();
  if (Option::ScalarArrayCompression && !system) {
    string s = getScalarArrayCompressedText();
    outputHexBuffer(cg, "sa_cdata", s.data(), s.size());
  }
  const char *clsname = system ? "SystemScalarArrays" : "ScalarArrays";
  const char *prefix = system ? Option::SystemScalarArrayName :
    Option::ScalarArrayName;

  cg.printf("StaticArray %s::%s[%d];\n",
            clsname, prefix, m_scalarArrayIds.size());
}

static bool SortByExpressionLength(const AnalysisResult::ScalarArrayExp &p1,
                                   const AnalysisResult::ScalarArrayExp &p2) {
  return p1.len < p2.len;
}

void AnalysisResult::outputCPPScalarArrayInit(CodeGenerator &cg, int fileCount,
                                              int part) {
  if (part == 0) {
    int sumLen = 0;
    m_scalarArraySorted.clear();
    if (m_scalarArrayIds.size() > 0) {
      m_scalarArraySorted.resize(m_scalarArrayIds.size());
      for (unsigned int i = 0; i < m_scalarArrayIds.size(); i++) {
        ScalarArrayExp &exp = m_scalarArraySorted[i];
        exp.id = i;
        exp.len = m_scalarArrayIds[i] ?
                  m_scalarArrayIds[i]->getText(true).size() : 0;
        exp.exp = m_scalarArrayIds[i];
        sumLen += exp.len;
      }
      sort(m_scalarArraySorted.begin(), m_scalarArraySorted.end(),
           SortByExpressionLength);
    }
    m_scalarArraySortedAvgLen = sumLen / fileCount;
  }

  if (m_scalarArraySorted.empty()) return;
  AnalysisResultPtr ar = shared_from_this();
  bool system = cg.getOutput() == CodeGenerator::SystemCPP;
  const char *prefix = system ? Option::SystemScalarArrayName :
    Option::ScalarArrayName;

  bool nonEmpty = false;
  while (m_scalarArraySortedIndex < (int)m_scalarArraySorted.size()) {
    ScalarArrayExp &exp = m_scalarArraySorted[m_scalarArraySortedIndex++];
    m_scalarArraySortedSumLen += exp.len;
    // In case we hit a huge scalar array, leave it for the next file unless
    // the current file is still empty.
    if ((m_scalarArraySortedSumLen > (part + 1) * m_scalarArraySortedAvgLen)
         && (part < fileCount - 1) && nonEmpty) {
      m_scalarArraySortedIndex--;
      break;
    }
    nonEmpty = true;
    cg.printf("%s[%d] = StaticArray(", prefix, exp.id);
    if (exp.exp) {
      ExpressionListPtr expList =
        dynamic_pointer_cast<ExpressionList>(exp.exp);
      int numElems = expList->getCount();
      if (numElems <= Option::ScalarArrayOverflowLimit) {
        expList->outputCPP(cg, ar);
        cg.printf(", NULL);\n");
      } else {
        ExpressionListPtr subExpList =
          dynamic_pointer_cast<ExpressionList>(expList->clone());
        for (int i = Option::ScalarArrayOverflowLimit; i < numElems; i++) {
          subExpList->removeElement(Option::ScalarArrayOverflowLimit);
        }
        ar->setInsideScalarArray(true);
        subExpList->outputCPP(cg, ar);
        ar->setInsideScalarArray(false);
        cg.printf(", NULL);\n");
        for (int i = Option::ScalarArrayOverflowLimit; i < numElems; i++) {
          ExpressionPtr elemExp = (*expList)[i];
          ArrayPairExpressionPtr pair =
            dynamic_pointer_cast<ArrayPairExpression>(elemExp);
          ExpressionPtr name = pair->getName();
          ExpressionPtr value = pair->getValue();
          if (name) {
            cg.printf("%s[%d].set(", prefix, exp.id);
            name->outputCPP(cg, ar);
            cg.printf(", ");
            value->outputCPP(cg, ar);
            ScalarExpressionPtr sc =
              dynamic_pointer_cast<ScalarExpression>(name);
            if (sc) {
              int64 hash = sc->getHash();
              if (hash >= 0) {
                cg.printf(", 0x%016llXLL", hash);
              }
            }
            cg.printf(");");
          } else {
            cg.printf("%s[%d].append(", prefix, exp.id);
            value->outputCPP(cg, ar);
            cg.printf(");");
          }
          cg.printf("\n");
        }
        cg.printf("%s[%d].setStatic();\n", prefix, exp.id);
      }
    } else {
      cg.printf("(ArrayElement*)NULL);\n");
    }
  }
}

void AnalysisResult::outputCPPGlobalDeclarations() {
  string filename = string(Option::SystemFilePrefix) + "global_variables.h";

  string headerPath = m_outputPath + "/" + filename;
  Util::mkdir(headerPath);
  ofstream f(headerPath.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);

  cg.headerBegin(filename.c_str());
  cg.printInclude("<cpp/base/hphp.h>");
  cg.namespaceBegin();

  AnalysisResultPtr ar = shared_from_this();
  getVariables()->setAttribute(VariableTable::ForceGlobal);
  getVariables()->outputCPP(cg, ar);
  getVariables()->clearAttribute(VariableTable::ForceGlobal);

  cg.printf("\n");
  if (getVariables()->getAttribute(VariableTable::ContainsLDynamicVariable)) {
    cg.printf("LVariableTable *get_variable_table();\n");
  } else if (getVariables()->
             getAttribute(VariableTable::ContainsDynamicVariable)) {
    cg.printf("RVariableTable *get_variable_table();\n");
  }

  for (StringToFunctionScopePtrVecMap::const_iterator iter =
         m_functionDecs.begin(); iter != m_functionDecs.end(); ++iter) {
    const char *name = iter->first.c_str();
    if (iter->second[0]->isRedeclaring()) {
      cg.printf("extern Variant invoke_failed_%s(CArrRef params);\n",
                name);
    }
  }
  for (StringToClassScopePtrVecMap::const_iterator iter =
         m_classDecs.begin(); iter != m_classDecs.end(); ++iter) {
    if (iter->second[0]->isRedeclaring()) {
      cg.printf("int %s%s();\n", Option::ClassStaticsIdGetterPrefix,
                     iter->first.c_str());
    }
  }
  cg.namespaceEnd();
  cg.headerEnd(filename.c_str());
  f.close();
}

void AnalysisResult::outputCPPGlobalImplementations(CodeGenerator &cg) {
  AnalysisResultPtr ar = shared_from_this();
  CodeGenerator::Context con = cg.getContext();
  cg.setContext(CodeGenerator::CppImplementation);
  getVariables()->setAttribute(VariableTable::ForceGlobal);
  getVariables()->outputCPP(cg, ar);
  getVariables()->clearAttribute(VariableTable::ForceGlobal);

  // Function pointers for redeclared funcs
  for (StringToFunctionScopePtrVecMap::const_iterator iter =
         m_functionDecs.begin(); iter != m_functionDecs.end(); ++iter) {
    if (iter->second[0]->isRedeclaring()) {
      const char *name = iter->first.c_str();
      cg.indentBegin("Variant invoke_failed_%s(CArrRef params) {\n",
                     name);
      cg.printf("return invoke_failed(\"%s\", params, -1);\n", name);
      cg.indentEnd("}\n");
    }
  }

  for (StringToClassScopePtrVecMap::const_iterator iter =
         m_classDecs.begin(); iter != m_classDecs.end(); ++iter) {
    if (iter->second[0]->isRedeclaring()) {
      cg.indentBegin("int %s%s() {\n", Option::ClassStaticsIdGetterPrefix,
                     iter->first.c_str());
      cg.printDeclareGlobals();
      cg.printf("return g->%s%s->getRedeclaringId();\n",
                Option::ClassStaticsObjectPrefix, iter->first.c_str());
      cg.indentEnd("}\n");
    }
  }
  cg.setContext(con);
}

void AnalysisResult::outputCPPSystemImplementations(CodeGenerator &cg) {
  BOOST_FOREACH(FileScopePtr f, m_fileScopes) {
    if (f->hasImpl(shared_from_this())) {
      cg.printf("%s%s(false);\n", Option::PseudoMainPrefix,
                f->pseudoMainName().c_str());
    }
  }
}

void AnalysisResult::outputCPPFileRunDecls(CodeGenerator &cg) {
  BOOST_FOREACH(FileScopePtr f, m_fileScopes) {
    cg.printf("bool run_%s%s;\n", Option::PseudoMainPrefix,
              f->pseudoMainName().c_str());
  }
}

void AnalysisResult::outputCPPFileRunImpls(CodeGenerator &cg) {
  BOOST_FOREACH(FileScopePtr f, m_fileScopes) {
    cg.printf(",\n  run_%s%s(false)", Option::PseudoMainPrefix,
              f->pseudoMainName().c_str());
  }
}

void AnalysisResult::outputCPPClassStaticInitializerDecls(CodeGenerator &cg) {
  for (StringToClassScopePtrVecMap::const_iterator iter = m_classDecs.begin();
       iter != m_classDecs.end(); ++iter) {
    BOOST_FOREACH(ClassScopePtr cls, iter->second) {
      cls->outputCPPStaticInitializerDecl(cg);
    }
  }
}

void AnalysisResult::outputCPPClassStaticInitializerFlags(CodeGenerator &cg,
                                                          bool constructor) {
  const char *fmt = constructor ? ",\n %s%s(false)" : "bool %s%s;\n";
  for (StringToClassScopePtrVecMap::const_iterator iter = m_classDecs.begin();
       iter != m_classDecs.end(); ++iter) {
    BOOST_FOREACH(ClassScopePtr cls, iter->second) {
      if (cls->needLazyStaticInitializer()) {
        cg.printf(fmt, Option::ClassStaticInitializerFlagPrefix,
                  cls->getId().c_str());
      }
    }
  }
}

void AnalysisResult::outputCPPScalarArrays(bool system) {
  setInsideScalarArray(true);

  int fileCount = system ? 1 : Option::ScalarArrayFileCount;
  if (system) {
    fileCount = 1;
  } else if ((m_scalarArrayIds.size() / Option::ScalarArrayFileCount) == 0) {
    fileCount = 1;
  } else {
    fileCount = Option::ScalarArrayFileCount;
  }
  if (Option::ScalarArrayCompression && !system) fileCount = 1;
  for (int i = 0; i < fileCount; i++) {
    string filename = m_outputPath + "/" + Option::SystemFilePrefix +
      "scalar_arrays_" + lexical_cast<string>(i) + ".no.cpp";
    Util::mkdir(filename);
    ofstream f(filename.c_str());
    CodeGenerator cg(&f, system ? CodeGenerator::SystemCPP :
                     CodeGenerator::ClusterCPP);

    cg.printf("\n");
    cg.printInclude("<cpp/base/hphp.h>");
    cg.printInclude(string(Option::SystemFilePrefix) +
                    (system ? "system_globals.h" : "global_variables.h"));

    cg.printf("\n");
    cg.namespaceBegin();

    AnalysisResultPtr ar = shared_from_this();
    CodeGenerator::Context con = cg.getContext();
    cg.setContext(CodeGenerator::CppImplementation);
    getVariables()->setAttribute(VariableTable::ForceGlobal);
    outputCPPScalarArrays(cg, fileCount, i);
    getVariables()->clearAttribute(VariableTable::ForceGlobal);
    cg.setContext(con);

    cg.namespaceEnd();
    f.close();
  }

  setInsideScalarArray(false);
}

void AnalysisResult::outputCPPScalarArrays(CodeGenerator &cg, int fileCount,
                                           int part) {
  bool system = cg.getOutput() == CodeGenerator::SystemCPP;
  const char *clsname = system ? "SystemScalarArrays" : "ScalarArrays";
  const char *prefix = system ? Option::SystemScalarArrayName :
    Option::ScalarArrayName;

  if (fileCount == 1) {
    outputCPPScalarArrayImpl(cg);
    cg.printf("\n");
    cg.indentBegin("void %s::initialize() {\n", clsname);
    if (!system) {
      cg.printf("SystemScalarArrays::initialize();\n");
    }
    if (m_scalarArrayIds.size() > 0) {
      if (Option::ScalarArrayCompression && !system) {
        ASSERT(m_scalarArrayCompressedTextSize > 0);
        cg.printf("ArrayUtil::InitScalarArrays(%s, %d, sa_cdata, %d);\n",
                  prefix, m_scalarArrayIds.size(),
                  m_scalarArrayCompressedTextSize);
      } else {
        outputCPPScalarArrayInit(cg, 1, 0);
      }
    }
    cg.indentEnd("}\n");
    return;
  }

  if (part == 0) {
    ASSERT(!(Option::ScalarArrayCompression && !system));
    outputCPPScalarArrayImpl(cg);
    cg.printf("\n");
    cg.indentBegin("void %s::initialize() {\n", clsname);
    if (!system) {
      cg.printf("SystemScalarArrays::initialize();\n");
    }
    for (int i = 0; i < fileCount; i++) {
      cg.printf("initialize_%d();\n", i);
    }
    cg.indentEnd("}\n");
  }

  cg.printf("\n");
  cg.indentBegin("void %s::initialize_%d() {\n", clsname, part);
  outputCPPScalarArrayInit(cg, fileCount, part);
  cg.indentEnd("}\n");
}

void AnalysisResult::outputCPPGlobalVariablesMethods(int part) {
  string filename = m_outputPath + "/" + Option::SystemFilePrefix +
    "global_variables_" + lexical_cast<string>(part) + ".no.cpp";
  Util::mkdir(filename);
  ofstream f(filename.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);
  AnalysisResultPtr ar = shared_from_this();

  cg.printf("\n");
  cg.printInclude("<cpp/base/hphp.h>");
  cg.printInclude(string(Option::SystemFilePrefix) + "global_variables.h");
  if (part == 1) {
    getVariables()->outputCPPGlobalVariablesDtorIncludes(cg, ar);
  }
  cg.printf("\n");
  cg.printf("using namespace std;\n");
  cg.namespaceBegin();

  CodeGenerator::Context con = cg.getContext();
  cg.setContext(CodeGenerator::CppImplementation);
  getVariables()->setAttribute(VariableTable::ForceGlobal);
  switch (part) {
  case 1:
    getVariables()->outputCPPGlobalVariablesDtor(cg);
    getVariables()->outputCPPGlobalVariablesGetImpl (cg, ar);
    break;
  case 2: getVariables()->outputCPPGlobalVariablesExists  (cg, ar); break;
  case 3: getVariables()->outputCPPGlobalVariablesGetIndex(cg, ar); break;
  case 4: getVariables()->outputCPPGlobalVariablesMethods (cg, ar); break;
  default: ASSERT(false);
  }
  getVariables()->clearAttribute(VariableTable::ForceGlobal);
  cg.setContext(con);

  cg.namespaceEnd();

  f.close();
}

void AnalysisResult::outputCPPGlobalStateBegin(CodeGenerator &cg,
                                               const char *section) {
  cg.indentBegin("static void output_%s(FILE *fp) {\n", section);
  cg.printf("DECLARE_GLOBAL_VARIABLES(g);\n");
  cg.printf("print(fp, \"\\n$%s = json_decode('\");\n", section);
  cg.printf("Array %s;\n", section);
}

void AnalysisResult::outputCPPGlobalStateEnd(CodeGenerator &cg,
                                             const char *section) {
  cg.printf("String s = f_json_encode(%s);\n", section);
  cg.printf("s = StringUtil::CEncode(s, \"\\\\\\\'\");\n");
  cg.printf("print(fp, s);\n");
  cg.printf("print(fp, \"', true);\\n\");\n");
  cg.indentEnd("}\n\n");
}

void AnalysisResult::outputCPPGlobalStateDynamicConstants
(CodeGenerator &cg) {
  AnalysisResultPtr ar = shared_from_this();
  const char *section = "dynamic_constants";
  outputCPPGlobalStateBegin(cg, section);
  getConstants()->outputCPPGlobalState(cg, ar);
  for (StringToFileScopePtrMap::const_iterator iter = m_files.begin();
       iter != m_files.end(); ++iter) {
    iter->second->getConstants()->outputCPPGlobalState(cg, ar);
  }
  outputCPPGlobalStateEnd(cg, section);
}

void AnalysisResult::outputCPPGlobalStatePseudoMainVariables
(CodeGenerator &cg) {
  const char *section = "pseudomain_variables";
  outputCPPGlobalStateBegin(cg, section);
  BOOST_FOREACH(FileScopePtr f, m_fileScopes) {
    cg.printf("%s.set(\"run_%s%s\", g->run_%s%s);\n", section,
              Option::PseudoMainPrefix, f->pseudoMainName().c_str(),
              Option::PseudoMainPrefix, f->pseudoMainName().c_str());
  }
  outputCPPGlobalStateEnd(cg, section);
}

void AnalysisResult::outputCPPGlobalStateRedeclaredFunctions
(CodeGenerator &cg) {
  const char *section = "redeclared_functions";
  outputCPPGlobalStateBegin(cg, section);
  for (StringToFunctionScopePtrVecMap::const_iterator iter =
         m_functionDecs.begin(); iter != m_functionDecs.end(); ++iter) {
    const char *name = iter->first.c_str();
    if (iter->second[0]->isRedeclaring()) {
      cg.printf("%s.set(\"%s%s\", (int64)g->%s%s);\n", section,
                Option::InvokePrefix, name, Option::InvokePrefix, name);
    }
  }
  outputCPPGlobalStateEnd(cg, section);
}

void AnalysisResult::outputCPPGlobalStateRedeclaredClasses
(CodeGenerator &cg) {
  const char *section = "redeclared_classes";
  outputCPPGlobalStateBegin(cg, section);
  for (StringToClassScopePtrVecMap::const_iterator iter =
         m_classDecs.begin(); iter != m_classDecs.end(); ++iter) {
    const char *name = iter->first.c_str();
    if (iter->second[0]->isRedeclaring()) {
      cg.printf("%s.set(\"%s%s\", g->%s%s->getRedeclaringId());\n",
                section,
                Option::ClassStaticsObjectPrefix, name,
                Option::ClassStaticsObjectPrefix, name);
    }
  }
  outputCPPGlobalStateEnd(cg, section);
}

void AnalysisResult::outputCPPGlobalState() {
  string filename = m_outputPath + "/" + Option::SystemFilePrefix +
    "global_state.no.cpp";
  Util::mkdir(filename);
  ofstream f(filename.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);
  AnalysisResultPtr ar = shared_from_this();

  cg.printf("\n");
  cg.printInclude("<cpp/base/hphp.h>");
  cg.printInclude(string(Option::SystemFilePrefix) + "global_variables.h");
  cg.printf("\n");
  cg.printf("using namespace std;\n");
  cg.namespaceBegin();

  cg.indentBegin("static void print(FILE *fp, String s) {\n");
  cg.indentBegin("if (fp) {\n");
  cg.printf("fwrite(s.c_str(), 1, s.size(), fp);\n");
  cg.printf("return;\n");
  cg.indentEnd("}\n");
  cg.printf("echo(s);\n");
  cg.indentEnd("}\n");

  outputCPPGlobalStateDynamicConstants(cg);
  getVariables()->outputCPPGlobalState(cg, ar);
  outputCPPGlobalStatePseudoMainVariables(cg);
  outputCPPGlobalStateRedeclaredFunctions(cg);
  outputCPPGlobalStateRedeclaredClasses(cg);

  cg.indentBegin("void output_global_state(FILE *fp) {\n");
  cg.printf("output_static_global_variables(fp);\n");
  cg.printf("output_dynamic_global_variables(fp);\n");
  cg.printf("output_dynamic_constants(fp);\n");
  cg.printf("output_method_static_variables(fp);\n");
  cg.printf("output_method_static_inited(fp);\n");
  cg.printf("output_class_static_variables(fp);\n");
  cg.printf("output_pseudomain_variables(fp);\n");
  cg.printf("output_redeclared_functions(fp);\n");
  cg.printf("output_redeclared_classes(fp);\n");
  cg.indentEnd("}\n\n");

  cg.namespaceEnd();
  f.close();
}

void AnalysisResult::outputCPPMain() {
  string mainPath = m_outputPath + "/" + Option::SystemFilePrefix +
    "main.no.cpp";
  Util::mkdir(mainPath);
  ofstream fMain(mainPath.c_str());
  CodeGenerator cg(&fMain, CodeGenerator::ClusterCPP);

  cg.printf("\n");
  cg.printInclude("<cpp/base/hphp.h>");
  cg.printInclude("<cpp/base/array/zend_array.h>");
  cg.printInclude(string(Option::SystemFilePrefix) + "global_variables.h");

  cg.printf("\n");
  cg.printf("using namespace std;\n");
  cg.namespaceBegin();
  outputCPPClassStaticInitializerDecls(cg);
  cg.printf("\n");
  outputCPPGlobalImplementations(cg);
  cg.namespaceEnd();

  cg.printf("\n");
  cg.printf("#ifndef HPHP_BUILD_LIBRARY\n");
  cg.indentBegin("int main(int argc, char** argv) {\n");
  cg.printf("return HPHP::execute_program(argc, argv);\n");
  cg.indentEnd("}\n");
  cg.printf("#endif\n");

  fMain.close();
}

void AnalysisResult::outputCPPClassMap(CodeGenerator &cg) {
  AnalysisResultPtr ar = shared_from_this();

  if (!Option::GenerateCPPMetaInfo) return;
  cg.indentBegin("const char *g_class_map[] = {\n");

  // system functions
  cg.printf("(const char *)ClassInfo::IsSystem, NULL, \"\",\n");
  cg.printf("NULL,\n"); // interfaces
  for (StringToFunctionScopePtrVecMap::const_iterator iter =
         m_functions.begin(); iter != m_functions.end(); ++iter) {
    FunctionScopePtr func = iter->second[0];
    ASSERT(!func->isUserFunction());
    func->outputCPPClassMap(cg, ar);
  }
  cg.printf("NULL,\n"); // methods
  cg.printf("NULL,\n"); // properties
  // system constants
  m_constants->outputCPPClassMap(cg, ar);

  // user functions
  cg.printf("(const char *)ClassInfo::IsNothing, NULL, \"\",\n");
  cg.printf("NULL,\n"); // interfaces
  for (StringToFunctionScopePtrVecMap::const_iterator iter =
         m_functionDecs.begin(); iter != m_functionDecs.end(); ++iter) {
    FunctionScopePtr func = iter->second[0];
    if (func->isUserFunction()) {
      func->outputCPPClassMap(cg, ar);
    }
  }
  cg.printf("NULL,\n"); // methods
  cg.printf("NULL,\n"); // properties

  // user defined constants
  for (int i = 0; i < (int)m_fileScopes.size(); i++) {
    ConstantTablePtr constants = m_fileScopes[i]->getConstants();
    constants->outputCPPClassMap(cg, ar, (i == (int)m_fileScopes.size() - 1));
  }

  // system classes
  for (StringToClassScopePtrMap::const_iterator iter = m_systemClasses.begin();
       iter != m_systemClasses.end(); ++iter) {
    iter->second->outputCPPClassMap(cg, ar);
  }

  // user classes
  for (StringToClassScopePtrVecMap::const_iterator iter = m_classDecs.begin();
       iter != m_classDecs.end(); ++iter) {
    bool redec = iter->second[0]->isRedeclaring();
    if (redec) {
      cg.printf("(const char *)ClassInfo::IsRedeclared, \"%s\", "
                "(const char *)%s%s,\n", iter->first.c_str(),
                Option::ClassStaticsIdGetterPrefix, iter->first.c_str());
    }
    BOOST_FOREACH(ClassScopePtr cls, iter->second) {
      cls->outputCPPClassMap(cg, ar);
    }
    if (redec) {
      cg.printf("NULL,\n");
    }
  }

  cg.printf("NULL\n");
  cg.indentEnd("};\n");
}

void AnalysisResult::outputCPPClusterImpl(CodeGenerator &cg,
                                          const FileScopePtrVec &files) {
  AnalysisResultPtr ar = shared_from_this();
  cg.printf("\n");

  // includes
  map<string, FileScopePtr> toInclude;
  BOOST_FOREACH(FileScopePtr fs, files) {
    getTrueDeps(fs, toInclude);
    toInclude[fs->getName()] = fs;
  }
  for (map<string, FileScopePtr>::const_iterator iter = toInclude.begin();
       iter != toInclude.end(); ++iter) {
    FileScopePtr fs = iter->second;
    cg.printInclude(fs->outputFilebase());
  }
  cg.printInclude("<cpp/ext/ext.h>");
  if (Option::EnableEval >= Option::LimitedEval ||
      cg.getOutput() == CodeGenerator::SystemCPP) {
    cg.printInclude("<cpp/eval/eval.h>");
  }

  // implementations
  cg.namespaceBegin();
  BOOST_FOREACH(FileScopePtr fs, files) {
    pushScope(fs);
    cg.setContext(CodeGenerator::CppImplementation);
    fs->outputCPPImpl(cg, ar);
    cg.setContext(CodeGenerator::CppPseudoMain);
    fs->outputCPPPseudoMain(cg, ar);
    popScope();
  }
  cg.namespaceEnd();
}


void AnalysisResult::outputCPPFFIStubs() {
  AnalysisResultPtr ar = shared_from_this();
  string iPath = m_outputPath + "/" + Option::FFIFilePrefix +
    "stubs.cpp";
  string hPath = m_outputPath + "/" + Option::FFIFilePrefix +
    "stubs.h";
  Util::mkdir(iPath);
  ofstream fi(iPath.c_str());
  ofstream fh(hPath.c_str());
  CodeGenerator cg(&fh, CodeGenerator::ClusterCPP);
  cg.printInclude("<cpp/base/hphp_ffi.h>");
  cg.printf("using namespace HPHP;\n");
  cg.printf("extern \"C\" {\n");

  cg.setStream(CodeGenerator::ImplFile, &fi);
  cg.useStream(CodeGenerator::ImplFile);
  cg.printInclude("\"stubs.h\"");
  cg.printf("/* preface starts */\n");
  cg.printf("using namespace HPHP;\n\n");
  cg.printf("/* preface finishes */\n");

  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputCPPFFI(cg, ar);
    popScope();
  }

  cg.useStream(CodeGenerator::PrimaryStream);
  cg.printf("}\n");
  fi.close();
  fh.close();
}
void AnalysisResult::outputHSFFIStubs() {
  AnalysisResultPtr ar = shared_from_this();
  string path = m_outputPath + "/" + Option::FFIFilePrefix +
    "HphpStubs.hs";
  Util::mkdir(path);
  ofstream f(path.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);
  cg.printf("{-# INCLUDE \"stubs.h\" #-}\n");
  cg.printf("{-# LANGUAGE ForeignFunctionInterface #-}\n");
  cg.printf("module HphpStubs (");
  bool first = true;
  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    const StringToFunctionScopePtrVecMap &fns = fs->getFunctions();
    for (StringToFunctionScopePtrVecMap::const_iterator it = fns.begin();
         it != fns.end(); ++it) {
      BOOST_FOREACH(FunctionScopePtr func, it->second) {
        if (func->inPseudoMain()) continue;
        if (first) {
          first = false;
        } else {
          cg.printf(", ");
        }
        cg.printf("f_%s", func->getId().c_str());
      }
    }
  }
  cg.printf(") where\n");
  cg.printf("import HphpFFI\n");
  cg.printf("import Foreign.C\n");
  cg.printf("import Foreign.Ptr\n");
  cg.printf("import Foreign.Marshal.Alloc (alloca)\n");
  cg.printf("import Foreign.Storable (peek)\n");
  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputHSFFI(cg, ar);
    popScope();
  }
  f.close();
}

void AnalysisResult::outputJavaFFIStubs() {
  AnalysisResultPtr ar = shared_from_this();

  string packageName = Option::JavaFFIRootPackage;
  string packageDir = packageName;
  Util::replaceAll(packageDir, ".", "/");

  string outputDir = ar->getOutputPath() + "/" + Option::FFIFilePrefix +
    packageDir + "/";
  Util::mkdir(outputDir);

  string mainFile = outputDir + "HphpMain.java";
  ofstream fmain(mainFile.c_str());
  CodeGenerator cg(&fmain, CodeGenerator::FileCPP);
  cg.setContext(CodeGenerator::JavaFFI);

  cg.printf("package %s;\n\n", packageName.c_str());
  cg.printf("import hphp.*;\n\n");

  cg.indentBegin("public class HphpMain extends Hphp {\n");

  // generate an "identify" method that identifies the exact class type
  // of an HphpObject.
  cg.indentBegin("public static HphpVariant identify(HphpVariant v) {\n");
  cg.indentBegin("try {if (v instanceof HphpObject) {\n");
  cg.printf("return identify(v.getVariantPtr());\n");
  cg.indentEnd("} } catch (NoClassDefFoundError e) { }\n");
  cg.printf("return v;\n");
  cg.indentEnd("}\n\n");

  cg.printf("public static native HphpVariant identify(long ptr);\n\n");

  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputJavaFFI(cg, ar);
    popScope();
  }

  cg.indentEnd("}\n");
  fmain.close();
}

void AnalysisResult::outputJavaFFICppDecl() {
  AnalysisResultPtr ar = shared_from_this();
  string path = m_outputPath + "/" + Option::FFIFilePrefix +
    "java_stubs.h";
  Util::mkdir(path);
  ofstream f(path.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);
  cg.setContext(CodeGenerator::JavaFFICppDecl);

  cg.printf("extern \"C\" {\n\n");

  // the native signature of "identify"
  string packageName = Option::JavaFFIRootPackage;
  string mangledName = "Java_" + packageName + "_HphpMain_identify";
  Util::replaceAll(mangledName, ".", "_");
  cg.printf("JNIEXPORT jobject JNICALL\n");
  cg.printf("%s(JNIEnv *env, jclass main, jlong ptr);\n\n",
            mangledName.c_str());

  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputJavaFFICPPStub(cg, ar);
    popScope();
  }

  cg.printf("}\n");

  f.close();
}

void AnalysisResult::outputJavaFFICppImpl() {
  AnalysisResultPtr ar = shared_from_this();
  string path = m_outputPath + "/" + Option::FFIFilePrefix +
    "java_stubs.cpp";
  Util::mkdir(path);
  ofstream f(path.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);
  cg.setContext(CodeGenerator::JavaFFICppImpl);

  cg.printInclude("<jni.h>");
  cg.printInclude("<cpp/base/hphp_ffi.h>");
  cg.printInclude("<cpp/base/object_data.h>");
  cg.printInclude("<cpp/base/builtin_functions.h>");
  cg.printInclude("<cpp/ext/ext.h>");
  cg.printInclude("\"stubs.h\"");
  cg.printInclude("\"java_stubs.h\"");

  cg.printf("/* preface starts */\n");

  cg.printf("\nusing namespace HPHP;\n\n");

  // forward declaration of exportVariantToJava
  cg.printf("jobject exportVariantToJava(JNIEnv *, jclass, void *, int);\n\n");

  cg.printf("/* preface finishes */\n");

  // the native implementation of "identify"
  string packageName = Option::JavaFFIRootPackage;
  string mangledName = "Java_" + packageName + "_HphpMain_identify";
  Util::replaceAll(mangledName, ".", "_");
  cg.printf("JNIEXPORT jobject JNICALL\n");
  cg.indentBegin("%s(JNIEnv *env, jclass main, jlong ptr) {\n",
                 mangledName.c_str());
  Util::replaceAll(packageName, ".", "/");
  cg.printf("String clsName = concat(\"%s/\", "
            "toString(x_get_class((Variant *)ptr)));\n",
            packageName.c_str());
  cg.printf("jclass cls = env->FindClass(clsName.c_str());\n");
  cg.printf("jmethodID init = env->GetMethodID(cls, \"<init>\", \"(J)V\");\n");
  cg.printf("return env->NewObject(cls, init, ptr);\n");
  cg.indentEnd("}\n\n");

  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputJavaFFICPPStub(cg, ar);
    popScope();
  }

  f.close();
}

void AnalysisResult::outputSwigFFIStubs() {
  AnalysisResultPtr ar = shared_from_this();
  string path = m_outputPath + "/" + Option::FFIFilePrefix +
    Option::ProgramName + ".i";
  Util::mkdir(path);
  ofstream f(path.c_str());
  CodeGenerator cg(&f, CodeGenerator::ClusterCPP);

  cg.printf("%%module %s\n%%{\n", Option::ProgramName.c_str());
  cg.printInclude("<ffi/swig/swig.h>");
  cg.printInclude("\"stubs.h\"");
  cg.printf("using namespace HPHP;\n");

  cg.setContext(CodeGenerator::SwigFFIImpl);

  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputSwigFFIStubs(cg, ar);
    popScope();
  }

  cg.printf("%%}\n\n");

  cg.printf("/////////////////////\n");
  cg.printf("// system definitions\n");
  cg.printf("/////////////////////\n\n");

  cg.printf("%%include swig.i\n");

  cg.printf("\n/////////////////\n");
  cg.printf(  "// user functions\n");
  cg.printf(  "/////////////////\n\n");

  cg.setContext(CodeGenerator::SwigFFIDecl);

  BOOST_FOREACH(FileScopePtr fs, m_fileScopes) {
    pushScope(fs);
    fs->outputSwigFFIStubs(cg, ar);
    popScope();
  }

  f.close();
}

/**
 * Literal string to String precomputation
 */
void AnalysisResult::addLiteralString(const std::string s,
                                      ScalarExpressionPtr sc) {
  map<string, pair<int, ScalarExpressionPtr> >::const_iterator it =
    m_stringLiterals.find(s);
  if (it == m_stringLiterals.end()) {
    int ct = m_stringLiterals.size();
    m_stringLiterals[s] = pair<int, ScalarExpressionPtr>(ct, sc);
  }
}
int AnalysisResult::getLiteralStringId(const std::string &s) {
  map<string, pair<int, ScalarExpressionPtr> >::const_iterator it =
    m_stringLiterals.find(s);
  if (it != m_stringLiterals.end()) {
    return it->second.first;
  }
  return -1;
}

string AnalysisResult::getFuncId(ClassScopePtr cls, FunctionScopePtr func) {
  if (cls) {
    return cls->getId() + "::" + func->getId();
  }
  return func->getId();
}

string AnalysisResult::getParamRTTIEntryKey(ClassScopePtr cls,
                                            FunctionScopePtr func,
                                            const std::string &paramName) {
  return getFuncId(cls, func) + "::" + paramName;
}

void AnalysisResult::addParamRTTIEntry(ClassScopePtr cls,
                                       FunctionScopePtr func,
                                       const std::string &paramName) {
  string key = getParamRTTIEntryKey(cls, func, paramName);
  m_paramRTTIs[key] = m_paramRTTICounter++;
}

int AnalysisResult::getParamRTTIEntryId(ClassScopePtr cls,
                                        FunctionScopePtr func,
                                        const std::string &paramName) {
  string key = getParamRTTIEntryKey(cls, func, paramName);
  map<string, int>::const_iterator it = m_paramRTTIs.find(key);
  if (it == m_paramRTTIs.end()) return -1;
  return it->second;
}

void AnalysisResult::addRTTIFunction(const std::string &id) {
  m_rttiFuncs.insert(id);
}

void AnalysisResult::cloneRTTIFuncs
(ClassScopePtr cls, const StringToFunctionScopePtrVecMap &functions) {
  for (StringToFunctionScopePtrVecMap::const_iterator iter =
       functions.begin(); iter != functions.end(); ++iter) {
    for (unsigned int j = 0; j < iter->second.size(); j++) {
      FunctionScopePtr func = iter->second[j];
      const string funcId = getFuncId(cls, func);
      if (RTTIInfo::TheRTTIInfo.exists(funcId.c_str())) {
        StatementPtr stmt = func->getStmt();
        func->setStmtCloned(stmt->clone());
      }
    }
  }
}

void AnalysisResult::cloneRTTIFuncs(const char *RTTIDirectory) {
  RTTIInfo::TheRTTIInfo.loadMetaData(Option::RTTIOutputFile.c_str());
  RTTIInfo::TheRTTIInfo.loadProfData(RTTIDirectory);

  for (unsigned int i = 0; i < m_fileScopes.size(); i++) {
    // standalone rtti functions
    cloneRTTIFuncs(ClassScopePtr(), m_fileScopes[i]->getFunctions());

    // class rtti methods
    for (StringToClassScopePtrVecMap::const_iterator iter =
         m_fileScopes[i]->getClasses().begin();
         iter != m_fileScopes[i]->getClasses().end(); ++iter) {
      for (unsigned int j = 0; j < iter->second.size(); j++) {
        ClassScopePtr cls = iter->second[j];
        cloneRTTIFuncs(cls, cls->getFunctions());
      }
    }
  }
}

void AnalysisResult::outputCPPLiteralStringPrecomputation() {

  ASSERT(m_stringLiterals.size() > 0);
  AnalysisResultPtr self = shared_from_this();
  int bucketSize = m_stringLiterals.size() / Option::LiteralStringFileCount;
  if (bucketSize < 500) bucketSize = 500;
  uint bucketCount = m_stringLiterals.size() / bucketSize;
  if (bucketCount * bucketSize != m_stringLiterals.size()) {
    bucketCount++;
  }
  if (Option::LiteralStringCompression) bucketCount = 1;

  {
    string filename = m_outputPath + "/" + Option::SystemFilePrefix +
      "literal_strings.h";
    Util::mkdir(filename);
    ofstream f(filename.c_str());
    CodeGenerator cg(&f, CodeGenerator::ClusterCPP);

    cg.printf("\n");
    cg.printInclude("<cpp/base/hphp.h>");
    cg.printf("\n");
    cg.namespaceBegin();
    cg.indentBegin("class LiteralStringInitializer {\n");
    cg.printf("public:\n");
    cg.indentBegin("LiteralStringInitializer() {\n");
    for (uint i = 0; i < bucketCount; i++) {
      cg.printf("init_%d();\n", i);
    }
    cg.indentEnd("}\n");
    map<string, pair<int, ScalarExpressionPtr> >::const_iterator it =
      m_stringLiterals.begin();
    for (uint i = 0; i < bucketCount; i++) {
      cg.printf("void init_%d();\n", i);
    }
    cg.indentEnd("};\n");
    cg.namespaceEnd();
    f.close();
  }
  map<string, pair<int, ScalarExpressionPtr> >::const_iterator it =
    m_stringLiterals.begin();
  const char *lsname = "literalStrings";
  for (uint i = 0; i < bucketCount; i++) {
    stringstream filenames;
    filenames << m_outputPath << "/" << Option::SystemFilePrefix <<
      "literal_strings_" << i << ".cpp";
    string filename = filenames.str();
    Util::mkdir(filename);
    ofstream f(filename.c_str());
    CodeGenerator cg(&f, CodeGenerator::ClusterCPP);
    cg.printf("\n");
    cg.printInclude("\"literal_strings.h\"");
    cg.namespaceBegin();
    int sdataLen = 0;
    int ldataLen = 0;
    if (i == 0) {
      if (Option::LiteralStringCompression) {
        string zsdata;
        string zldata;
        getLiteralStringCompressed(zsdata, zldata);
        const char *sdata = zsdata.data();
        sdataLen = zsdata.size();
        const char *ldata = zldata.data();
        ldataLen = zldata.size();
        ASSERT(sdataLen > 0 && ldataLen > 0);
        outputHexBuffer(cg, "ls_csdata", sdata, sdataLen);
        outputHexBuffer(cg, "ls_cldata", ldata, ldataLen);
      }
      cg.printf("StaticString %s[%d];\n", lsname, m_stringLiterals.size());
      cg.printf("static LiteralStringInitializer literalStringInitializer;\n");
    }

    cg.indentBegin("void LiteralStringInitializer::init_%d() {\n", i);
    if (Option::LiteralStringCompression) {
      cg.printf("StringUtil::InitLiteralStrings"
                "(%s, %d, ls_csdata, %d, ls_cldata, %d);\n",
                lsname, m_stringLiterals.size(), sdataLen, ldataLen);
    } else {
      for (int j = 0; j < bucketSize &&
           it != m_stringLiterals.end(); ++it, ++j) {
        cg.printf("%s[%d] = ", lsname, it->second.first);
        it->second.second->outputCPPString(cg, self);
        cg.printf(";\n");
      }
    }
    cg.indentEnd("}\n");
    cg.namespaceEnd();
    f.close();
  }
}
