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
/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#line 1 "fql.y"

#include <sstream>
#include <assert.h>
#include "fqlnode.h"
#include "fql.h"

#line 15 "fql.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/*
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands.
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 49
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE FQLToken
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  FQLOrderByNode * yy7;
  int yy28;
  FQLLimitNode * yy43;
  FQLStatementNode * yy45;
  FQLConstantNode * yy46;
  FQLExpressionListNode * yy72;
  FQLExpressionNode * yy88;
  bool yy89;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  FQLParserState * __attribute__((unused)) state ;
#define ParseARG_PDECL , FQLParserState * __attribute__((unused)) state
#define ParseARG_FETCH  FQLParserState * __attribute__((unused)) state  = yypParser->state
#define ParseARG_STORE yypParser->state  = state
#define YYNSTATE 87
#define YYNRULE 51
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    32,   75,   39,   40,   50,   58,   59,   75,   47,   43,
 /*    10 */    49,   31,   33,   73,   74,   27,    2,   75,   29,   73,
 /*    20 */    74,   15,   16,   75,   56,   43,   49,   31,   33,   73,
 /*    30 */    74,   34,   75,   42,   33,   73,   74,   75,   28,   47,
 /*    40 */    43,   49,   31,   33,   73,   74,   35,   75,    5,   73,
 /*    50 */    67,   22,   58,   59,   47,   43,   49,   31,   33,   73,
 /*    60 */    74,    2,  139,   75,   52,   62,   23,   54,   64,   75,
 /*    70 */    57,   78,   19,   31,   33,   73,   74,   45,    6,   79,
 /*    80 */    80,   73,   71,   65,   66,   17,   26,   75,   37,   75,
 /*    90 */    41,   13,   24,   75,   55,   43,   49,   31,   33,   73,
 /*   100 */    74,   73,   72,   10,   75,   81,   18,   14,    8,   83,
 /*   110 */    75,   36,   43,   49,   31,   33,   73,   74,   75,   44,
 /*   120 */    31,   33,   73,   74,   25,   51,   43,   49,   31,   33,
 /*   130 */    73,   74,   75,   38,   60,   87,   84,   20,   61,   53,
 /*   140 */    43,   49,   31,   33,   73,   74,   75,   11,   63,   12,
 /*   150 */    14,   13,   30,   21,   43,   49,   31,   33,   73,   74,
 /*   160 */    52,   41,   46,   52,   10,   69,    3,   78,    9,   10,
 /*   170 */    78,  140,  140,   45,    6,   79,   45,    6,   79,   86,
 /*   180 */    52,   17,   85,   52,   17,  140,  140,   78,  140,  140,
 /*   190 */    78,  140,  140,   45,    6,   79,   77,    6,   79,  140,
 /*   200 */    75,   17,  140,  140,   17,  140,  140,   52,   48,   49,
 /*   210 */    31,   33,   73,   74,   78,  140,   52,  140,    4,  140,
 /*   220 */    45,    6,   79,   78,  140,    7,   68,    7,   17,  140,
 /*   230 */     1,   79,    7,   70,  140,   76,  140,  140,  140,  140,
 /*   240 */    82,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    31,   32,   13,   14,   35,   36,   37,   32,   39,   40,
 /*    10 */    41,   42,   43,   44,   45,   30,    2,   32,   43,   44,
 /*    20 */    45,   24,   25,   32,   39,   40,   41,   42,   43,   44,
 /*    30 */    45,   31,   32,   42,   43,   44,   45,   32,    3,   39,
 /*    40 */    40,   41,   42,   43,   44,   45,   31,   32,   13,   44,
 /*    50 */    45,   35,   36,   37,   39,   40,   41,   42,   43,   44,
 /*    60 */    45,    2,   46,   32,    5,   29,    4,    5,   28,   32,
 /*    70 */     1,   12,   41,   42,   43,   44,   45,   18,   19,   20,
 /*    80 */    21,   44,   45,    9,   10,   26,   34,   32,   47,   32,
 /*    90 */     7,   17,   38,   32,   39,   40,   41,   42,   43,   44,
 /*   100 */    45,   44,   45,   15,   32,   44,   22,   23,    6,   21,
 /*   110 */    32,   39,   40,   41,   42,   43,   44,   45,   32,   41,
 /*   120 */    42,   43,   44,   45,   34,   39,   40,   41,   42,   43,
 /*   130 */    44,   45,   32,   11,   33,    0,   33,   12,   12,   39,
 /*   140 */    40,   41,   42,   43,   44,   45,   32,    8,   12,   16,
 /*   150 */    23,   17,   20,   39,   40,   41,   42,   43,   44,   45,
 /*   160 */     5,    7,   19,    5,   15,   21,   19,   12,    6,   15,
 /*   170 */    12,   48,   48,   18,   19,   20,   18,   19,   20,   24,
 /*   180 */     5,   26,   24,    5,   26,   48,   48,   12,   48,   48,
 /*   190 */    12,   48,   48,   18,   19,   20,   21,   19,   20,   48,
 /*   200 */    32,   26,   48,   48,   26,   48,   48,    5,   40,   41,
 /*   210 */    42,   43,   44,   45,   12,   48,    5,   48,   13,   48,
 /*   220 */    18,   19,   20,   12,   48,   13,   21,   13,   26,   48,
 /*   230 */    19,   20,   13,   21,   48,   21,   48,   48,   48,   48,
 /*   240 */    21,
};
#define YY_SHIFT_USE_DFLT (-12)
#define YY_SHIFT_MAX 56
static const short yy_shift_ofst[] = {
 /*     0 */    14,   59,  155,  175,  202,  158,  202,  202,  202,  202,
 /*    10 */   202,  202,  202,  178,  178,  178,  178,  178,  211,   74,
 /*    20 */   -11,  154,   69,  102,   83,  122,  122,   35,   62,   -3,
 /*    30 */   205,   84,  212,   -3,  214,  219,   88,  135,  125,  126,
 /*    40 */   136,  139,  127,  133,  134,  143,  132,  149,  133,  134,
 /*    50 */   144,  149,  147,  149,  162,  149,  149,
};
#define YY_REDUCE_USE_DFLT (-32)
#define YY_REDUCE_MAX 26
static const short yy_reduce_ofst[] = {
 /*     0 */    16,  -31,  -15,    0,   15,   55,   72,   86,  100,  114,
 /*    10 */   168,   31,   78,   -9,  -25,    5,   37,   57,   61,   40,
 /*    20 */    36,   52,   41,   54,   90,  101,  103,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   138,  138,  138,  138,  138,  138,  138,  138,  138,  138,
 /*    10 */   138,  138,  138,  138,  138,  138,  138,  138,  138,  100,
 /*    20 */   105,   97,   91,   94,   97,  103,  103,  138,  138,  118,
 /*    30 */   138,  117,  138,  119,  138,  138,  138,  138,  138,  138,
 /*    40 */   138,  138,  110,  106,  109,  138,  138,  132,  107,  108,
 /*    50 */   138,  133,  126,   95,  138,  137,  136,   90,   88,   89,
 /*    60 */    92,  101,  102,  104,   96,   98,   99,  120,  111,  113,
 /*    70 */   114,  121,  124,  125,  122,  127,  128,  129,  130,  131,
 /*    80 */   115,  116,  112,  123,   93,  135,  134,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
**
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/*
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = {
  "$",             "SEMICOLON",     "SELECT",        "FROM",
  "VIRT_TABLE",    "IDENT",         "WHERE",         "ORDER",
  "BY",            "DESC",          "ASC",           "LIMIT",
  "INT",           "COMMA",         "OFFSET",        "OR",
  "AND",           "CMP",           "CONTAINS",      "OPEN_PAREN",
  "STRING",        "CLOSE_PAREN",   "IN",            "ADD_OP",
  "STAR",          "SLASH",         "NOT",           "error",
  "opt_desc",      "opt_offset",    "star_catching_expr_list",  "expression_list",
  "constant",      "opt_limit",     "opt_order",     "statement",
  "virt_statement",  "native_statement",  "opt_where",     "disjunction",
  "conjunction",   "expression",    "sum",           "prod",
  "term",          "big_term",      "start",         "opt_semicolon",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "start ::= statement opt_semicolon",
 /*   1 */ "statement ::= virt_statement",
 /*   2 */ "statement ::= native_statement",
 /*   3 */ "opt_semicolon ::= SEMICOLON",
 /*   4 */ "opt_semicolon ::=",
 /*   5 */ "virt_statement ::= SELECT star_catching_expr_list FROM VIRT_TABLE opt_where opt_order opt_limit",
 /*   6 */ "native_statement ::= SELECT star_catching_expr_list FROM IDENT WHERE disjunction opt_order opt_limit",
 /*   7 */ "opt_where ::=",
 /*   8 */ "opt_where ::= WHERE disjunction",
 /*   9 */ "opt_order ::= ORDER BY expression opt_desc",
 /*  10 */ "opt_order ::=",
 /*  11 */ "opt_desc ::= DESC",
 /*  12 */ "opt_desc ::= ASC",
 /*  13 */ "opt_desc ::=",
 /*  14 */ "opt_limit ::= LIMIT INT COMMA INT",
 /*  15 */ "opt_limit ::= LIMIT INT opt_offset",
 /*  16 */ "opt_limit ::=",
 /*  17 */ "opt_offset ::= OFFSET INT",
 /*  18 */ "opt_offset ::=",
 /*  19 */ "disjunction ::= conjunction",
 /*  20 */ "disjunction ::= disjunction OR conjunction",
 /*  21 */ "conjunction ::= expression",
 /*  22 */ "conjunction ::= conjunction AND expression",
 /*  23 */ "expression ::= expression CMP sum",
 /*  24 */ "expression ::= CONTAINS OPEN_PAREN STRING CLOSE_PAREN",
 /*  25 */ "expression ::= CONTAINS OPEN_PAREN STRING COMMA expression_list CLOSE_PAREN",
 /*  26 */ "expression ::= sum IN OPEN_PAREN statement CLOSE_PAREN",
 /*  27 */ "expression ::= sum IN OPEN_PAREN expression_list CLOSE_PAREN",
 /*  28 */ "expression ::= sum IN OPEN_PAREN CLOSE_PAREN",
 /*  29 */ "expression ::= sum IN term",
 /*  30 */ "expression ::= sum",
 /*  31 */ "sum ::= sum ADD_OP prod",
 /*  32 */ "sum ::= prod",
 /*  33 */ "prod ::= prod STAR big_term",
 /*  34 */ "prod ::= prod SLASH big_term",
 /*  35 */ "prod ::= big_term",
 /*  36 */ "big_term ::= OPEN_PAREN disjunction CLOSE_PAREN",
 /*  37 */ "big_term ::= NOT big_term",
 /*  38 */ "big_term ::= term",
 /*  39 */ "term ::= IDENT",
 /*  40 */ "term ::= constant",
 /*  41 */ "term ::= IDENT OPEN_PAREN expression_list CLOSE_PAREN",
 /*  42 */ "term ::= IDENT OPEN_PAREN CLOSE_PAREN",
 /*  43 */ "constant ::= INT",
 /*  44 */ "constant ::= STRING",
 /*  45 */ "expression_list ::= disjunction",
 /*  46 */ "expression_list ::= expression_list COMMA disjunction",
 /*  47 */ "star_catching_expr_list ::= STAR",
 /*  48 */ "star_catching_expr_list ::= star_catching_expr_list COMMA STAR",
 /*  49 */ "star_catching_expr_list ::= disjunction",
 /*  50 */ "star_catching_expr_list ::= star_catching_expr_list COMMA disjunction",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/*
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
      /* TERMINAL Destructor */
    case 1: /* SEMICOLON */
    case 2: /* SELECT */
    case 3: /* FROM */
    case 4: /* VIRT_TABLE */
    case 5: /* IDENT */
    case 6: /* WHERE */
    case 7: /* ORDER */
    case 8: /* BY */
    case 9: /* DESC */
    case 10: /* ASC */
    case 11: /* LIMIT */
    case 12: /* INT */
    case 13: /* COMMA */
    case 14: /* OFFSET */
    case 15: /* OR */
    case 16: /* AND */
    case 17: /* CMP */
    case 18: /* CONTAINS */
    case 19: /* OPEN_PAREN */
    case 20: /* STRING */
    case 21: /* CLOSE_PAREN */
    case 22: /* IN */
    case 23: /* ADD_OP */
    case 24: /* STAR */
    case 25: /* SLASH */
    case 26: /* NOT */
{
#line 29 "fql.y"

  if ((yypminor->yy0).token) {
    free((yypminor->yy0).token);
    (yypminor->yy0).token = NULL;
  }

#line 503 "fql.c"
}
      break;
    case 30: /* star_catching_expr_list */
    case 31: /* expression_list */
{
#line 36 "fql.y"

  if ((yypminor->yy72)) {
    delete (yypminor->yy72);
  }

#line 515 "fql.c"
}
      break;
    case 32: /* constant */
{
#line 46 "fql.y"

  if ((yypminor->yy46)) {
    delete (yypminor->yy46);
  }

#line 526 "fql.c"
}
      break;
    case 33: /* opt_limit */
{
#line 51 "fql.y"

  if ((yypminor->yy43)) {
    delete (yypminor->yy43);
  }

#line 537 "fql.c"
}
      break;
    case 34: /* opt_order */
{
#line 56 "fql.y"

  if ((yypminor->yy7)) {
    delete (yypminor->yy7);
  }

#line 548 "fql.c"
}
      break;
    case 35: /* statement */
{
#line 66 "fql.y"

  if ((yypminor->yy45)) {
    delete (yypminor->yy45);
  }

#line 559 "fql.c"
}
      break;
    case 38: /* opt_where */
    case 39: /* disjunction */
    case 40: /* conjunction */
    case 41: /* expression */
    case 42: /* sum */
    case 43: /* prod */
    case 44: /* term */
    case 45: /* big_term */
{
#line 61 "fql.y"

  if ((yypminor->yy88)) {
    delete (yypminor->yy88);
  }

#line 577 "fql.c"
}
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/*
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;

  if( stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_MAX ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_MAX );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_SZ_ACTTAB );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 46, 2 },
  { 35, 1 },
  { 35, 1 },
  { 47, 1 },
  { 47, 0 },
  { 36, 7 },
  { 37, 8 },
  { 38, 0 },
  { 38, 2 },
  { 34, 4 },
  { 34, 0 },
  { 28, 1 },
  { 28, 1 },
  { 28, 0 },
  { 33, 4 },
  { 33, 3 },
  { 33, 0 },
  { 29, 2 },
  { 29, 0 },
  { 39, 1 },
  { 39, 3 },
  { 40, 1 },
  { 40, 3 },
  { 41, 3 },
  { 41, 4 },
  { 41, 6 },
  { 41, 5 },
  { 41, 5 },
  { 41, 4 },
  { 41, 3 },
  { 41, 1 },
  { 42, 3 },
  { 42, 1 },
  { 43, 3 },
  { 43, 3 },
  { 43, 1 },
  { 45, 3 },
  { 45, 2 },
  { 45, 1 },
  { 44, 1 },
  { 44, 1 },
  { 44, 4 },
  { 44, 3 },
  { 32, 1 },
  { 32, 1 },
  { 31, 1 },
  { 31, 3 },
  { 30, 1 },
  { 30, 3 },
  { 30, 1 },
  { 30, 3 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0: /* start ::= statement opt_semicolon */
#line 120 "fql.y"
{
  state->stmt = yymsp[-1].minor.yy45;
}
#line 916 "fql.c"
        break;
      case 1: /* statement ::= virt_statement */
      case 2: /* statement ::= native_statement */
#line 124 "fql.y"
{
  yygotominor.yy45 = yymsp[0].minor.yy45;
}
#line 924 "fql.c"
        break;
      case 3: /* opt_semicolon ::= SEMICOLON */
#line 130 "fql.y"
{
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
#line 931 "fql.c"
        break;
      case 4: /* opt_semicolon ::= */
      case 7: /* opt_where ::= */
      case 10: /* opt_order ::= */
      case 16: /* opt_limit ::= */
#line 131 "fql.y"
{
}
#line 940 "fql.c"
        break;
      case 5: /* virt_statement ::= SELECT star_catching_expr_list FROM VIRT_TABLE opt_where opt_order opt_limit */
#line 132 "fql.y"
{
  state->dependencies.insert(yymsp[-3].minor.yy0.token+1);
  yygotominor.yy45 = new FQLStatementNode(yymsp[-5].minor.yy72, yymsp[-3].minor.yy0.token+1, yymsp[-2].minor.yy88, yymsp[-1].minor.yy7, yymsp[0].minor.yy43, false);
  free(yymsp[-3].minor.yy0.token);
  yymsp[-3].minor.yy0.token = NULL;
  yy_destructor(yypParser,2,&yymsp[-6].minor);
  yy_destructor(yypParser,3,&yymsp[-4].minor);
}
#line 952 "fql.c"
        break;
      case 6: /* native_statement ::= SELECT star_catching_expr_list FROM IDENT WHERE disjunction opt_order opt_limit */
#line 138 "fql.y"
{
  fql_validate_table_name(yymsp[-4].minor.yy0.token, state);
  yygotominor.yy45 = new FQLStatementNode(yymsp[-6].minor.yy72, yymsp[-4].minor.yy0.token, yymsp[-2].minor.yy88, yymsp[-1].minor.yy7, yymsp[0].minor.yy43, true);
  free(yymsp[-4].minor.yy0.token);
  yymsp[-4].minor.yy0.token = NULL;
  yy_destructor(yypParser,2,&yymsp[-7].minor);
  yy_destructor(yypParser,3,&yymsp[-5].minor);
  yy_destructor(yypParser,6,&yymsp[-3].minor);
}
#line 965 "fql.c"
        break;
      case 8: /* opt_where ::= WHERE disjunction */
#line 147 "fql.y"
{
    yygotominor.yy88 = yymsp[0].minor.yy88;
  yy_destructor(yypParser,6,&yymsp[-1].minor);
}
#line 973 "fql.c"
        break;
      case 9: /* opt_order ::= ORDER BY expression opt_desc */
#line 151 "fql.y"
{
  yygotominor.yy7 = new FQLOrderByNode(yymsp[-1].minor.yy88, yymsp[0].minor.yy89);
  yy_destructor(yypParser,7,&yymsp[-3].minor);
  yy_destructor(yypParser,8,&yymsp[-2].minor);
}
#line 982 "fql.c"
        break;
      case 11: /* opt_desc ::= DESC */
#line 156 "fql.y"
{
  yygotominor.yy89 = true;
  yy_destructor(yypParser,9,&yymsp[0].minor);
}
#line 990 "fql.c"
        break;
      case 12: /* opt_desc ::= ASC */
#line 159 "fql.y"
{ yygotominor.yy89 = false;   yy_destructor(yypParser,10,&yymsp[0].minor);
}
#line 996 "fql.c"
        break;
      case 13: /* opt_desc ::= */
#line 160 "fql.y"
{ yygotominor.yy89 = false; }
#line 1001 "fql.c"
        break;
      case 14: /* opt_limit ::= LIMIT INT COMMA INT */
#line 162 "fql.y"
{
  yygotominor.yy43 = new FQLLimitNode(atoi(yymsp[0].minor.yy0.token), atoi(yymsp[-2].minor.yy0.token));
  free(yymsp[0].minor.yy0.token);
  free(yymsp[-2].minor.yy0.token);
  yymsp[0].minor.yy0.token = NULL;
  yymsp[-2].minor.yy0.token = NULL;
  yy_destructor(yypParser,11,&yymsp[-3].minor);
  yy_destructor(yypParser,13,&yymsp[-1].minor);
}
#line 1014 "fql.c"
        break;
      case 15: /* opt_limit ::= LIMIT INT opt_offset */
#line 169 "fql.y"
{
  yygotominor.yy43 = new FQLLimitNode(atoi(yymsp[-1].minor.yy0.token), yymsp[0].minor.yy28);
  free(yymsp[-1].minor.yy0.token);
  yymsp[-1].minor.yy0.token = NULL;
  yy_destructor(yypParser,11,&yymsp[-2].minor);
}
#line 1024 "fql.c"
        break;
      case 17: /* opt_offset ::= OFFSET INT */
#line 175 "fql.y"
{
  yygotominor.yy28 = atoi(yymsp[0].minor.yy0.token);
  free(yymsp[0].minor.yy0.token);
  yymsp[0].minor.yy0.token = NULL;
  yy_destructor(yypParser,14,&yymsp[-1].minor);
}
#line 1034 "fql.c"
        break;
      case 18: /* opt_offset ::= */
#line 180 "fql.y"
{ yygotominor.yy28 = 0; }
#line 1039 "fql.c"
        break;
      case 19: /* disjunction ::= conjunction */
      case 21: /* conjunction ::= expression */
      case 30: /* expression ::= sum */
      case 32: /* sum ::= prod */
      case 35: /* prod ::= big_term */
      case 38: /* big_term ::= term */
#line 182 "fql.y"
{
  yygotominor.yy88 = yymsp[0].minor.yy88;
}
#line 1051 "fql.c"
        break;
      case 20: /* disjunction ::= disjunction OR conjunction */
#line 185 "fql.y"
{
  if (yymsp[-2].minor.yy88->nodeType() == FQL_DISJUNCTION) {
    ((FQLDisjunctionNode *)yymsp[-2].minor.yy88)->append(yymsp[0].minor.yy88);
    yygotominor.yy88 = yymsp[-2].minor.yy88;
  } else {
    FQLDisjunctionNode *node = new FQLDisjunctionNode();
    node->append(yymsp[-2].minor.yy88);
    node->append(yymsp[0].minor.yy88);
    yygotominor.yy88 = node;
  }
  yy_destructor(yypParser,15,&yymsp[-1].minor);
}
#line 1067 "fql.c"
        break;
      case 22: /* conjunction ::= conjunction AND expression */
#line 200 "fql.y"
{
  if (yymsp[-2].minor.yy88->nodeType() == FQL_CONJUNCTION) {
    ((FQLConjunctionNode *)yymsp[-2].minor.yy88)->append(yymsp[0].minor.yy88);
    yygotominor.yy88 = yymsp[-2].minor.yy88;
  } else {
    FQLConjunctionNode *node = new FQLConjunctionNode();
    node->append(yymsp[-2].minor.yy88);
    node->append(yymsp[0].minor.yy88);
    yygotominor.yy88 = node;
  }
  yy_destructor(yypParser,16,&yymsp[-1].minor);
}
#line 1083 "fql.c"
        break;
      case 23: /* expression ::= expression CMP sum */
#line 212 "fql.y"
{
  yygotominor.yy88 = new FQLComparisonExpressionNode(yymsp[-1].minor.yy0.token, yymsp[-2].minor.yy88, yymsp[0].minor.yy88);
  free(yymsp[-1].minor.yy0.token);
  yymsp[-1].minor.yy0.token = NULL;
}
#line 1092 "fql.c"
        break;
      case 24: /* expression ::= CONTAINS OPEN_PAREN STRING CLOSE_PAREN */
#line 218 "fql.y"
{
  FQLExpressionListNode *empty = new FQLExpressionListNode();
  yygotominor.yy88 = new FQLContainsExpressionNode(std::string(yymsp[-1].minor.yy0.token+1, strlen(yymsp[-1].minor.yy0.token)-2),
                                    empty);
  free(yymsp[-1].minor.yy0.token);
  yymsp[-1].minor.yy0.token = NULL;
  yy_destructor(yypParser,18,&yymsp[-3].minor);
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,21,&yymsp[0].minor);
}
#line 1106 "fql.c"
        break;
      case 25: /* expression ::= CONTAINS OPEN_PAREN STRING COMMA expression_list CLOSE_PAREN */
#line 226 "fql.y"
{
  yygotominor.yy88 = new FQLContainsExpressionNode(std::string(yymsp[-3].minor.yy0.token+1, strlen(yymsp[-3].minor.yy0.token)-2), yymsp[-1].minor.yy72);
  free(yymsp[-3].minor.yy0.token);
  yymsp[-3].minor.yy0.token = NULL;
  yy_destructor(yypParser,18,&yymsp[-5].minor);
  yy_destructor(yypParser,19,&yymsp[-4].minor);
  yy_destructor(yypParser,13,&yymsp[-2].minor);
  yy_destructor(yypParser,21,&yymsp[0].minor);
}
#line 1119 "fql.c"
        break;
      case 26: /* expression ::= sum IN OPEN_PAREN statement CLOSE_PAREN */
#line 232 "fql.y"
{
  yygotominor.yy88 = new FQLInStatementNode(yymsp[-4].minor.yy88, yymsp[-1].minor.yy45);
  yy_destructor(yypParser,22,&yymsp[-3].minor);
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,21,&yymsp[0].minor);
}
#line 1129 "fql.c"
        break;
      case 27: /* expression ::= sum IN OPEN_PAREN expression_list CLOSE_PAREN */
#line 235 "fql.y"
{
  yygotominor.yy88 = new FQLInArrayNode(yymsp[-4].minor.yy88, yymsp[-1].minor.yy72);
  yy_destructor(yypParser,22,&yymsp[-3].minor);
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,21,&yymsp[0].minor);
}
#line 1139 "fql.c"
        break;
      case 28: /* expression ::= sum IN OPEN_PAREN CLOSE_PAREN */
#line 238 "fql.y"
{
  yygotominor.yy88 = new FQLInArrayNode(yymsp[-3].minor.yy88, new FQLExpressionListNode());
  yy_destructor(yypParser,22,&yymsp[-2].minor);
  yy_destructor(yypParser,19,&yymsp[-1].minor);
  yy_destructor(yypParser,21,&yymsp[0].minor);
}
#line 1149 "fql.c"
        break;
      case 29: /* expression ::= sum IN term */
#line 241 "fql.y"
{
  // make a list
  FQLExpressionListNode *node = new FQLExpressionListNode(yymsp[0].minor.yy88);
  yygotominor.yy88 = new FQLInArrayNode(yymsp[-2].minor.yy88, node);
  yy_destructor(yypParser,22,&yymsp[-1].minor);
}
#line 1159 "fql.c"
        break;
      case 31: /* sum ::= sum ADD_OP prod */
#line 250 "fql.y"
{
  yygotominor.yy88 = new FQLArithmeticExpressionNode(yymsp[-1].minor.yy0.token, yymsp[-2].minor.yy88, yymsp[0].minor.yy88);
  free(yymsp[-1].minor.yy0.token);
  yymsp[-1].minor.yy0.token = NULL;
}
#line 1168 "fql.c"
        break;
      case 33: /* prod ::= prod STAR big_term */
#line 258 "fql.y"
{
  yygotominor.yy88 = new FQLArithmeticExpressionNode("*", yymsp[-2].minor.yy88, yymsp[0].minor.yy88);
  yy_destructor(yypParser,24,&yymsp[-1].minor);
}
#line 1176 "fql.c"
        break;
      case 34: /* prod ::= prod SLASH big_term */
#line 261 "fql.y"
{
  yygotominor.yy88 = new FQLArithmeticExpressionNode("/", yymsp[-2].minor.yy88, yymsp[0].minor.yy88);
  yy_destructor(yypParser,25,&yymsp[-1].minor);
}
#line 1184 "fql.c"
        break;
      case 36: /* big_term ::= OPEN_PAREN disjunction CLOSE_PAREN */
#line 268 "fql.y"
{
  yygotominor.yy88 = yymsp[-1].minor.yy88;
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,21,&yymsp[0].minor);
}
#line 1193 "fql.c"
        break;
      case 37: /* big_term ::= NOT big_term */
#line 271 "fql.y"
{
  yygotominor.yy88 = new FQLNotExpressionNode(yymsp[0].minor.yy88);
  yy_destructor(yypParser,26,&yymsp[-1].minor);
}
#line 1201 "fql.c"
        break;
      case 39: /* term ::= IDENT */
#line 278 "fql.y"
{
  yygotominor.yy88 = new FQLFieldExpressionNode(yymsp[0].minor.yy0.token);
  free(yymsp[0].minor.yy0.token);
  yymsp[0].minor.yy0.token = NULL;
}
#line 1210 "fql.c"
        break;
      case 40: /* term ::= constant */
#line 283 "fql.y"
{
  yygotominor.yy88 = yymsp[0].minor.yy46;
}
#line 1217 "fql.c"
        break;
      case 41: /* term ::= IDENT OPEN_PAREN expression_list CLOSE_PAREN */
#line 286 "fql.y"
{
  try {
    fql_validate_function_data(yymsp[-3].minor.yy0.token, yymsp[-1].minor.yy72);
  } catch (FQLParserException e) {
    state->errors.push_back(e);
  }
  yygotominor.yy88 = new FQLFunctionExpressionNode(yymsp[-3].minor.yy0.token, yymsp[-1].minor.yy72);
  free(yymsp[-3].minor.yy0.token);
  yymsp[-3].minor.yy0.token = NULL;
  yy_destructor(yypParser,19,&yymsp[-2].minor);
  yy_destructor(yypParser,21,&yymsp[0].minor);
}
#line 1233 "fql.c"
        break;
      case 42: /* term ::= IDENT OPEN_PAREN CLOSE_PAREN */
#line 297 "fql.y"
{
  try {
    fql_validate_function_data(yymsp[-2].minor.yy0.token, NULL);
  } catch (FQLParserException e) {
    state->errors.push_back(e);
  }
  FQLExpressionListNode *empty = new FQLExpressionListNode();
  yygotominor.yy88 = new FQLFunctionExpressionNode(yymsp[-2].minor.yy0.token, empty);
  free(yymsp[-2].minor.yy0.token);
  yymsp[-2].minor.yy0.token = NULL;
  yy_destructor(yypParser,19,&yymsp[-1].minor);
  yy_destructor(yypParser,21,&yymsp[0].minor);
}
#line 1250 "fql.c"
        break;
      case 43: /* constant ::= INT */
#line 309 "fql.y"
{
  yygotominor.yy46 = new FQLConstantNode(yymsp[0].minor.yy0.token);
  free(yymsp[0].minor.yy0.token);
  yymsp[0].minor.yy0.token = NULL;
}
#line 1259 "fql.c"
        break;
      case 44: /* constant ::= STRING */
#line 314 "fql.y"
{
  yygotominor.yy46 = new FQLConstantNode(std::string(yymsp[0].minor.yy0.token+1, strlen(yymsp[0].minor.yy0.token)-2));
  free(yymsp[0].minor.yy0.token);
  yymsp[0].minor.yy0.token = NULL;
}
#line 1268 "fql.c"
        break;
      case 45: /* expression_list ::= disjunction */
      case 49: /* star_catching_expr_list ::= disjunction */
#line 320 "fql.y"
{
  yygotominor.yy72 = new FQLExpressionListNode();
  yygotominor.yy72->append(yymsp[0].minor.yy88);
}
#line 1277 "fql.c"
        break;
      case 46: /* expression_list ::= expression_list COMMA disjunction */
      case 50: /* star_catching_expr_list ::= star_catching_expr_list COMMA disjunction */
#line 324 "fql.y"
{
  yygotominor.yy72 = yymsp[-2].minor.yy72;
  yygotominor.yy72->append(yymsp[0].minor.yy88);
  yy_destructor(yypParser,13,&yymsp[-1].minor);
}
#line 1287 "fql.c"
        break;
      case 47: /* star_catching_expr_list ::= STAR */
#line 329 "fql.y"
{
  yygotominor.yy72 = NULL;
  throw FQLParserException("SELECT * is not supported.  Please manually list the columns you are interested in.");
  yy_destructor(yypParser,24,&yymsp[0].minor);
}
#line 1296 "fql.c"
        break;
      case 48: /* star_catching_expr_list ::= star_catching_expr_list COMMA STAR */
#line 333 "fql.y"
{
  yygotominor.yy72 = NULL;
  throw FQLParserException("SELECT * is not supported.  Please manually list the columns you are interested in.");
  yy_destructor(yypParser,30,&yymsp[-2].minor);
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yy_destructor(yypParser,24,&yymsp[0].minor);
}
#line 1307 "fql.c"
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 107 "fql.y"

  if (yymajor == 0) {
    throw FQLParserException("unexpected end of query.");
  } else {
    std::stringstream ss;
    std::string token_text;
    token_text = FQLParserException::getTokenText(yymajor, TOKEN);
    ss << "unexpected '" << token_text <<
      "' at position " << TOKEN.pos - token_text.length() << ".";
    state->errors.push_back(FQLParserException(ss.str()));
  }

#line 1378 "fql.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
#line 26 "fql.y"


#line 1400 "fql.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
