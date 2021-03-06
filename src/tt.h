/*
 * tt.h
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TT_H
#define TT_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "hlist.h"

#define DEBUG
#undef DEBUG

#ifdef DEBUG
    #define LOG(format, ...) \
        fprintf(stdout, format, __VA_ARGS__)
#else
    #define LOG(format, ...) \
        0
#endif

typedef struct __lex lex;
typedef enum __token TOKEN;

enum __token {
    SEMICOLON = 1, // ;
    EXCLAM, // !
    NEQ, // !=
    COLON, // :
    COMMA, // , // 5
    LPARAN, // (
    RPARAN, // ) 
    LBRACK, // [
    RBRACK, // ]
    LBRACE, // { 10
    RBRACE, // }
    EQUAL, // == 
    ASSIGN, // =
    GT, // >
    LT, // < 15
    GTE, // >=
    LTE, // <= 
    ADD, // +
    MINUS, // -
    MUL, // * 20
    DIV, // / 
    DOUBLEQUOTE, // " 22
    SINGLEQUOTE, // '
    NUMBER, // 0-9
    IDENTIFIER, // 25
    IF, // if 
    ELSE, // else 27
    WHILE, // while
    DEFINE, // define
    RETURN, // return 30
    TRUE, // true
    FALSE, // false
    OR, // ||
    AND, // &&
};

struct __lex {
    char *value;
    TOKEN token;
    int line;
};

typedef enum __ast_type ast_type;

enum __ast_type {
    NUMBERAST,
    VARIABLEAST,
    BINARYAST,
    CHARACTERAST,
    STRINGAST,
    IDENTIFIERAST,
    CALLAST,
    FUNCTIONAST,
    IFAST,
    WHILEAST,
    BOOLEANAST,
};

typedef struct environment_s environment;
typedef struct env_node_s env_node;
typedef struct ast_s ast_t;
typedef hlist_t expressions;
typedef struct expression_s expression;

// environment
struct environment_s {
    hlist_t hlist;
    struct environment_s *prev;
};

struct env_node_s {
    ast_t *variable;
    ast_t *value;
    hlist_node_t node;
};

// expression
struct expression_s {
    ast_t *ast;
    hlist_node_t node;
};

#define ast_object \
    ast_type type; \
    int line

struct ast_s {
    ast_object;
};

typedef struct _number_ast_s {
    ast_object;
    long value;
} number_ast_t;

#define new_number_ast(x, v, no) \
    x = malloc(sizeof(number_ast_t)); \
    x->line = no; \
    x->type = NUMBERAST; \
    x->value = v

typedef struct _variable_ast_s {
    ast_object;
    char *value;
} variable_ast_t;

#define new_variable_ast(x, v, no) \
    x = (variable_ast_t*)malloc(sizeof(variable_ast_t)); \
    x->line = no; \
    x->type = VARIABLEAST; \
    x->value = v

typedef struct _binary_ast_s {
    ast_object;
    ast_t *op;
    ast_t *lhs;
    ast_t *rhs;
} binary_ast_t;

#define new_binary_ast(x, o, l, r, no) \
    x = (binary_ast_t*)malloc(sizeof(binary_ast_t)); \
    x->type = BINARYAST; \
    x->line = no; \
    x->op = o; \
    x->lhs = l; \
    x->rhs = r

typedef struct _boolean_ast_s {
    ast_object;
    char value;
}boolean_ast_t;

#define new_boolean_ast(x, v, no) \
    x = (boolean_ast_t *) malloc(sizeof(boolean_ast_t)); \
    x->type = BOOLEANAST; \
    x->line = no; \
    x->value = v

typedef struct _character_ast_s {
    ast_object;
    char value;
} character_ast_t;

#define new_character_ast(x, v, no) \
    x = (character_ast_t*)malloc(sizeof(character_ast_t)); \
    x->line = no; \
    x->type = CHARACTERAST; \
    x->value = v

typedef struct _string_ast_s {
    ast_object;
    char *value;
} string_ast_t;

#define new_string_ast(x, v, no) \
    x = (string_ast_t*)malloc(sizeof(string_ast_t)); \
    x->line = no; \
    x->type = STRINGAST; \
    x->value = v

typedef struct _identifier_ast_s {
    ast_object;
    char *value;
} identifier_ast_t;


typedef struct _call_ast_s {
    ast_object;
    char *name;
    expressions *args;
} call_ast_t;

#define new_call_ast(x, no) \
    x = (call_ast_t *)malloc(sizeof(call_ast_t)); \
    x->type = CALLAST; \
    x->line = no

typedef struct _function_ast_s {
    ast_object;
    char *name;
    expressions *params;
    expressions *body;
    environment *env;
} function_ast_t;

#define new_function_ast(x, nam, no) \
    x = (function_ast_t*)malloc(sizeof(function_ast_t)); \
    x->type = FUNCTIONAST;\
    x->line = no; \
    x->name = nam

typedef struct _while_ast {
    ast_object;
    ast_t *condition;
    expressions *body;
} while_ast_t;

#define new_while_ast(x, cond, no) \
    x = (while_ast_t*)malloc(sizeof(while_ast_t)); \
    x->type = WHILEAST; \
    x->line = no; \
    x->condition = cond

typedef struct _if_ast {
    ast_object;
    ast_t *condition;
    expressions *then;
    expressions *els;
} if_ast_t;

#define new_if_ast(x, cond, no) \
    x = (if_ast_t*)malloc(sizeof(if_ast_t)); \
    x->type = IFAST; \
    x->line = no; \
    x->condition = cond

#define IS(ast, t) \
    (ast->type == t)

#define MAX_WORD_SIZE 40
#define MAX_STRING 10000
#define ASSIGN_LEX_LIST(s, t, l) \
    lex_list[lex_index].value = s; \
    lex_list[lex_index].token = t; \
    lex_list[lex_index++].line = l

#define ERRORF1(row, str) \
    fprintf(stderr, "in file %s line %d error : "#str"\n", current_file, row); \
    exit(-1);

static inline void ERRORF(const char *filename, int row, const char *strfmt, ...) {
    if (filename) {
        fprintf(stderr, "in file %s, ", filename);
    }
    fprintf(stderr, "in line %d Error : ", row);
    va_list ap;
    va_start(ap, strfmt);
    vfprintf(stderr, strfmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(-1);
}

#define PRINTF_ENUM(e) \
    printf("[print] ENUM %d"#e"\n", e)

// for environment
environment *init_env();
void print_env(environment*);
void define_variable(ast_t *, ast_t *, environment *);
environment *extend_environment(environment*);
environment *extend_environment_back(environment**);
ast_t **lookup_variable(ast_t *, environment *);
ast_t **lookup_variable_in_current_frame(ast_t *, environment *);
int set_variable_value(ast_t *, ast_t *, environment*);
// end for environment

// for lexer
extern lex lex_list[MAX_STRING];
extern int lex_index;
extern char *current_file;

void lexer(FILE *);
void print_lexer_result();
// end for lexer

// for parser
expressions *parser();
void print_ast(ast_t *, int);
// end for parser


// for expression
void init_expressions(expressions *);
void add_after_expression(expression *, expression *);

// for eval
ast_t *eval(ast_t *, environment **);
ast_t *eval_expressions(expressions *, environment **);
int is_variable(ast_t *);

// for binary_eval
ast_t *eval_equal(binary_ast_t*, environment *);
ast_t *eval_notequal(binary_ast_t*, environment *);
ast_t *eval_assignment(binary_ast_t*, environment *);
ast_t *eval_greater(binary_ast_t*, environment *);
ast_t *eval_ge(binary_ast_t*, environment *);
ast_t *eval_less(binary_ast_t*, environment *);
ast_t *eval_le(binary_ast_t*, environment *);
ast_t *eval_add(binary_ast_t*, environment *);
ast_t *eval_sub(binary_ast_t*, environment *);
ast_t *eval_mul(binary_ast_t*, environment *);
ast_t *eval_div(binary_ast_t*, environment *);
ast_t *eval_or(binary_ast_t*, environment *);
ast_t *eval_and(binary_ast_t*, environment *);

// for internal_func
ast_t *internal_printf(ast_t*, environment*);
ast_t *internal_require(ast_t*, environment**);

#endif /* !TT_H */
