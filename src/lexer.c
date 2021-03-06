/*
 * lexer.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

lex lex_list[MAX_STRING];
int lex_index = 0;
int line = 1;

int peek(FILE* fp) {
    int c = getc(fp);
    ungetc(c, fp);
    return c;
}

void consume_char(FILE* fp) {
    getc(fp);
}

void pass_word(FILE *fp) {
    char c;
    while((c = getc(fp)) != EOF && c != '\n');
    if (c == '\n'){
        line++;
    }
}

static char get_backslash_char(char a) {
    switch(a) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'a':
            return '\a';
        default:
            return a;
    }
}

void scan_word(FILE *input, int c, int indicator) {
    lex_list[lex_index].value = malloc(MAX_WORD_SIZE);
    int i = 0;
    if (c != '\\') {
        lex_list[lex_index].value[i++] = c;
    } else {
        lex_list[lex_index].value[i++] = get_backslash_char(getc(input));
    }

    if (indicator == 0 && isdigit(c)) {
        while (isdigit(c = getc(input))) {
            lex_list[lex_index].value[i++] = c;
        }
        if (isalpha(c)) {
            ERRORF(current_file, line, "identifier should not starts with digit");
        }
        ungetc(c, input);
        lex_list[lex_index].token = NUMBER;
    } else {
        char last = ' ';
        if (indicator == 1) {
            while((c = getc(input)) != EOF && ((c != '"') || (c == '"' && last == '\\'))) {
                if (last == '\\') {
                    last = c;
                    lex_list[lex_index].value[i++] = get_backslash_char(c);
                    continue;
                }
                last = c;
                if (c == '\n')
                    line++;
                if (c == '\\')
                    continue;
                lex_list[lex_index].value[i++] = c;
            }
            if (c == EOF) {
                ERRORF(current_file, line, "unmatched double quote");
            }
        } else if (indicator == 0) {
            while(isalnum(c = getc(input)) || (c == '_')) {
                lex_list[lex_index].value[i++] = c;
            }
        } else if (indicator == 2) {
            if ((c = getc(input)) != '\'') {
                ERRORF(current_file, line, "unmatched single quote");
            }
        }
        ungetc(c, input);

        lex_list[lex_index].value[i] = '\0';
        char *value = lex_list[lex_index].value;
        if(strcmp(value, "if") == 0) {
            lex_list[lex_index].token = IF;
        } else if (strcmp(value, "else") == 0) {
            lex_list[lex_index].token = ELSE;
        } else if (strcmp(value, "while") == 0) {
            lex_list[lex_index].token = WHILE;
        } else if (strcmp(value, "define") == 0) {
            lex_list[lex_index].token = DEFINE;
        } else if (strcmp(value, "true") == 0) {
            lex_list[lex_index].token = TRUE;
        } else if (strcmp(value, "false") == 0) {
            lex_list[lex_index].token = FALSE;
        } else {
            lex_list[lex_index].token = IDENTIFIER;
        }
    }
    lex_list[lex_index].value[i] = '\0';
    lex_list[lex_index++].line = line;
    LOG("%s scanned; token = %d\n", lex_list[lex_index-1].value, lex_list[lex_index-1].token);
}

void lexer(FILE *input) {
    //int c = getc(input);
    line = 1;
    int c = ' ';

    /* if pair == 0, there's no pair;
     * if pair == 1, this is a string pair;
     * if pair == 2, this is a character pair;
     */
    int pair = 0;
    while (c != EOF) {
        c = getc(input);
        if (c == '\n') {
            LOG("%s\n", "'\\n' scanned");
            line++;
        }
        if (c == EOF) {
            LOG("%s\n", "'EOF' scanned");
            break;
        }
        if (c == ';') {
            LOG("%s\n", "';' scanned");
            ASSIGN_LEX_LIST(";", SEMICOLON, line);
            continue;
        }
        if (c == ':') {
            LOG("%s\n", "':' scanned");
            ASSIGN_LEX_LIST(":", COLON, line);
            continue;
        }
        if (c == ',') {
            LOG("%s\n", "',' scanned");
            ASSIGN_LEX_LIST(",", COMMA, line);
            continue;
        }
        if (c == '(') {
            LOG("%s\n", "'(' scanned");
            ASSIGN_LEX_LIST("(", LPARAN, line);
            continue;
        }
        if (c == ')') {
            LOG("%s\n", "')' scanned");
            ASSIGN_LEX_LIST(")", RPARAN, line);
            continue;
        }
        if (c == '[') {
            LOG("%s\n", "'[' scanned");
            ASSIGN_LEX_LIST("[", LBRACK, line);
            continue;
        }
        if (c == ']') {
            LOG("%s\n", "']' scanned");
            ASSIGN_LEX_LIST("]", RBRACK, line);
            continue;
        }
        if (c == '{') {
            LOG("%s\n", "'{' scanned");
            ASSIGN_LEX_LIST("{", LBRACE, line);
            continue;
        }
        if (c == '}') {
            LOG("%s\n", "'}' scanned");
            ASSIGN_LEX_LIST("}", RBRACE, line);
            continue;
        }
        if (c == '=') {
            if (peek(input) == '=') {
                LOG("%s\n", "'==' scanned");
                consume_char(input);
                ASSIGN_LEX_LIST("==", EQUAL, line);
            } else {
                LOG("%s\n", "'=' scanned");
                ASSIGN_LEX_LIST("=", ASSIGN, line);
            }
            continue;
        }
        if (c == '!') {
            if (peek(input) == '=') {
                LOG("%s\n", "'!=' scanned");
                consume_char(input);
                ASSIGN_LEX_LIST("!=", NEQ, line);
            } else {
                LOG("%s\n", "'!' scanned");
                ASSIGN_LEX_LIST("!", EXCLAM, line);
            }
            continue;
        }
        if (c == '>') {
            if(peek(input) == '=') {
                LOG("%s\n", "'>=' scanned");
                consume_char(input);
                ASSIGN_LEX_LIST(">=", GTE, line);
            } else {
                LOG("%s\n", "'>' scanned");
                ASSIGN_LEX_LIST(">", GT, line);
            }
            continue;
        }
        if (c == '|') {
            if(peek(input) == '|') {
                LOG("%s\n", "'||' scanned");
                consume_char(input);
                ASSIGN_LEX_LIST("||", OR, line);
            } else {
                ERRORF(current_file, line, "OR needs two slash (||)");
            }
            continue;
        }
        if (c == '&') {
            if(peek(input) == '&') {
                LOG("%s\n", "'&&' scanned");
                consume_char(input);
                ASSIGN_LEX_LIST("&&", AND, line);
            } else {
                ERRORF(current_file, line, "AND needs two and (&&)");
            }
            continue;
        }
        if (c == '<') {
            if (peek(input) == '=') {
                LOG("%s\n", "'<=' scanned");
                consume_char(input);
                ASSIGN_LEX_LIST("<=", LTE, line);
            } else {
                LOG("%s\n", "'<' scanned");
                ASSIGN_LEX_LIST("<", LT, line);
            }
            continue;
        }
        if (c == '+') {
            LOG("%s\n", "'+' scanned");
            ASSIGN_LEX_LIST("+", ADD, line);
            continue;
        }
        if (c == '-') {
            LOG("%s\n", "'-' scanned");
            ASSIGN_LEX_LIST("-", MINUS, line);
            continue;
        }
        if (c == '*') {
            LOG("%s\n", "'*' scanned");
            ASSIGN_LEX_LIST("*", MUL, line);
            continue;
        }
        if (c == '/') {
            LOG("%s\n", "'/' scanned");
            ASSIGN_LEX_LIST("/", DIV, line);
            continue;
        }
        if (c == '"') {
            LOG("%s\n", "'\"' scanned");
            if (pair == 1 || pair == 0)
                pair = 1 - pair;
            ASSIGN_LEX_LIST("\"", DOUBLEQUOTE, line);
            continue;
        }
        if (c == '\'') {
            LOG("%s\n", "\"'\" scanned");
            if (pair == 2 || pair == 0)
                pair = 2 - pair;
            ASSIGN_LEX_LIST("'", SINGLEQUOTE, line);
            continue;
        }
        if (pair != 0) {
            scan_word(input, c, pair);
            continue;
        }
        if (c == '#') {
            pass_word(input);
            continue;
        }
        if (isalnum(c) || c == '_'){
            scan_word(input, c, 0);
            continue;
        }
        if (isspace(c)) {
            continue;
        }
        ERRORF(current_file, -1, "unknown character %c", c);
    }
}


void print_lexer_result() {
    for (int i = 0; i < lex_index; i++) {
        LOG("===== %d =====\n", i);
        LOG("%s\n", lex_list[i].value);
        LOG("%d\n", lex_list[i].token);
        LOG("%d\n", lex_list[i].line);
        LOG("%s\n", "\n");
    }
}
