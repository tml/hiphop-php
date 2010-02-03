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
#include "fqlnode.h"
#include <algorithm>
#include <map>
#include <set>
#include <vector>

using namespace std;

vector<vector<FQLParserState > >
fql_multiparse(const map<string, string> &query_set) {
  set<string> query_keys;
  set<string> reused;
  vector<FQLParserState> query_results;
  map<string, string>::const_iterator it = query_set.begin();
  vector<vector<FQLParserState > > all_results;
  while (it != query_set.end()) {
    string key = (*it).first;
    string query = (*it).second;
    try {
      FQLParserState state = fql_parse_query(query.c_str(), key);
      reused.insert(state.dependencies.begin(), state.dependencies.end());
      query_results.push_back(state);
      it++;
    } catch (FQLParserException &e) {
      FQLParserState::deleteStmtList(query_results);
      throw;
    }
  }
  while (!query_results.empty()) {
    vector<FQLParserState> round_results;
    vector<FQLParserState>::iterator results_it = query_results.begin();
    set<string> new_query_keys;
    while (results_it != query_results.end()) {
      FQLParserState state = *results_it;
      state.reused = (reused.find(state.name) != reused.end());
      if (includes(query_keys.begin(),
                   query_keys.end(),
                   state.dependencies.begin(),
                   state.dependencies.end())) {
        new_query_keys.insert(state.name);
        round_results.push_back(state);
        results_it = query_results.erase(results_it);
      } else {
        results_it++;
      }
    }
    if (round_results.empty() && !query_results.empty()) {
      FQLParserState state = query_results.front();
      FQLParserState::deleteStmtList(query_results);
      for (unsigned int i = 0; i < all_results.size(); i++) {
        FQLParserState::deleteStmtList(all_results[i]);
      }
      throw FQLParserException("Unresolved dependency detected for query named: '"
                               +state.name + "'",
                               FQL_EC_UNRESOLVED_DEPENDENCY);
    }
    query_keys.insert(new_query_keys.begin(), new_query_keys.end());
    all_results.push_back(round_results);
  }
  return all_results;
}
