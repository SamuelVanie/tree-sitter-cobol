#include <tree_sitter/parser.h>
#include <tree_sitter/alloc.h>
#include <wctype.h>

enum TokenType {
    WHITE_SPACES,
    LINE_PREFIX_COMMENT,
    LINE_SUFFIX_COMMENT,
    LINE_COMMENT,
    SOURCE_FORMAT_DIRECTIVE,
    COMMENT_ENTRY,
    multiline_string,
};

typedef struct {
    bool free_format_mode;
} Scanner;

void *tree_sitter_COBOL_external_scanner_create() {
    Scanner *scanner = ts_calloc(1, sizeof(Scanner));
    scanner->free_format_mode = false;
    return scanner;
}

static bool is_white_space(int c) {
    return iswspace(c) || c == ';' || c == ',';
}

const int number_of_comment_entry_keywords = 9;
char* any_content_keyword[] = {
    "author",
    "installlation",
    "date-written",
    "date-compiled",
    "security",
    "identification division",
    "environment division",
    "data division",
    "procedure division",
};

static bool start_with_word( TSLexer *lexer, char *words[], int number_of_words) {
    while(lexer->lookahead == ' ' || lexer->lookahead == '\t') {
        lexer->advance(lexer, true);
    }

    char *keyword_pointer[number_of_words];
    bool continue_check[number_of_words];
    for(int i=0; i<number_of_words; ++i) {
        keyword_pointer[i] = words[i];
        continue_check[i] = true;
    }

    while(true) {
        // At the end of the line
        if(lexer->get_column(lexer) > 71 || lexer->lookahead == '\n' || lexer->lookahead == 0) {
            return false;
        }

        // If all keyword matching fails, move to the end of the line
        bool all_match_failed = true;
        for(int i=0; i<number_of_words; ++i) {
            if(continue_check[i]) {
                all_match_failed = false;
            }
        }

        if(all_match_failed) {
            for(; lexer->get_column(lexer) < 71 && lexer->lookahead != '\n' && lexer->lookahead != 0;
            lexer->advance(lexer, true)) {
            }
            return false;
        }

        // If the head of the line matches any of specified keywords, return true;
        char c = lexer->lookahead;
        for(int i=0; i<number_of_words; ++i) {
            if(*(keyword_pointer[i]) == 0 && continue_check[i]) {
                return true;
            }
        }

        // matching keywords
        for(int i=0; i<number_of_words; ++i) {
            char k = *(keyword_pointer[i]);
            if(continue_check[i]) {
                continue_check[i] = c == towupper(k) || c == towlower(k);
            }
            (keyword_pointer[i])++;
        }

        // next character
        lexer->advance(lexer, true);
    }

    return false;
}

static void skip_whitespace(TSLexer *lexer) {
    while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
        lexer->advance(lexer, false);
    }
}

static bool scan_format_directive(Scanner *scanner, TSLexer *lexer) {
    skip_whitespace(lexer);
    
    if (lexer->lookahead != '>') return false;
    lexer->advance(lexer, false);
    if (lexer->lookahead != '>') return false;
    lexer->advance(lexer, false);
    
    skip_whitespace(lexer);
    
    const char *source = "SOURCE";
    for (int i = 0; i < 6; i++) {
        if (towupper(lexer->lookahead) != source[i]) return false;
        lexer->advance(lexer, false);
    }
    
    skip_whitespace(lexer);
    
    const char *format = "FORMAT";
    for (int i = 0; i < 6; i++) {
        if (towupper(lexer->lookahead) != format[i]) return false;
        lexer->advance(lexer, false);
    }
    
    skip_whitespace(lexer);
    
    if (towupper(lexer->lookahead) == 'I') {
        lexer->advance(lexer, false);
        if (towupper(lexer->lookahead) == 'S') {
            lexer->advance(lexer, false);
            skip_whitespace(lexer);
        }
    }
    
    if (towupper(lexer->lookahead) == 'F') {
        lexer->advance(lexer, false);
        if (towupper(lexer->lookahead) == 'R') {
            lexer->advance(lexer, false);
            if (towupper(lexer->lookahead) != 'E') return false;
            lexer->advance(lexer, false);
            if (towupper(lexer->lookahead) != 'E') return false;
            lexer->advance(lexer, false);
            scanner->free_format_mode = true;
        } else if (towupper(lexer->lookahead) == 'I') {
            lexer->advance(lexer, false);
            if (towupper(lexer->lookahead) != 'X') return false;
            lexer->advance(lexer, false);
            if (towupper(lexer->lookahead) != 'E') return false;
            lexer->advance(lexer, false);
            if (towupper(lexer->lookahead) != 'D') return false;
            lexer->advance(lexer, false);
            scanner->free_format_mode = false;
        } else {
            return false;
        }
    } else {
        return false;
    }
    
    while (lexer->lookahead != '\n' && lexer->lookahead != 0) {
        lexer->advance(lexer, false);
    }
    
    lexer->result_symbol = SOURCE_FORMAT_DIRECTIVE;
    lexer->mark_end(lexer);
    return true;
}

bool tree_sitter_COBOL_external_scanner_scan(void *payload, TSLexer *lexer,
                                            const bool *valid_symbols) {
    Scanner *scanner = (Scanner *)payload;
    
    if (lexer->lookahead == 0) {
        return false;
    }

    if (valid_symbols[SOURCE_FORMAT_DIRECTIVE]) {
        if (scan_format_directive(scanner, lexer)) {
            return true;
        }
    }

    if (valid_symbols[WHITE_SPACES]) {
        if (is_white_space(lexer->lookahead)) {
            while (is_white_space(lexer->lookahead)) {
                lexer->advance(lexer, true);
            }
            lexer->result_symbol = WHITE_SPACES;
            lexer->mark_end(lexer);
            return true;
        }
    }

    if (valid_symbols[LINE_PREFIX_COMMENT] && !scanner->free_format_mode) {
        if (lexer->get_column(lexer) <= 5) {
            if (lexer->get_column(lexer) == 0 && lexer->lookahead != 0 && lexer->lookahead != '\n') {
                while (lexer->get_column(lexer) <= 5 && lexer->lookahead != 0 && lexer->lookahead != '\n') {
                    lexer->advance(lexer, true);
                }
                lexer->result_symbol = LINE_PREFIX_COMMENT;
                lexer->mark_end(lexer);
                return true;
            }
        }
    }

    if (valid_symbols[LINE_COMMENT]) {
	if (scanner->free_format_mode) {
	    return false;
        } else {
            if (lexer->get_column(lexer) == 6) {
                if (lexer->lookahead == '*' || lexer->lookahead == '/') {
                    while (lexer->lookahead != '\n' && lexer->lookahead != 0) {
                        lexer->advance(lexer, true);
                    }
                    lexer->result_symbol = LINE_COMMENT;
                    lexer->mark_end(lexer);
                    return true;
                } else {
                    lexer->advance(lexer, true);
                    lexer->mark_end(lexer);
                    return false;
                }
            }
        }
    }

    if (valid_symbols[LINE_SUFFIX_COMMENT] && !scanner->free_format_mode) {
        if (lexer->get_column(lexer) >= 72) {
            while (lexer->lookahead != '\n' && lexer->lookahead != 0) {
                lexer->advance(lexer, true);
            }
            lexer->result_symbol = LINE_SUFFIX_COMMENT;
            lexer->mark_end(lexer);
            return true;
        }
    }

    if (valid_symbols[COMMENT_ENTRY]) {
        if (!start_with_word(lexer, any_content_keyword, number_of_comment_entry_keywords)) {
            lexer->mark_end(lexer);
            lexer->result_symbol = COMMENT_ENTRY;
            return true;
        } else {
            return false;
        }
    }

    if (valid_symbols[multiline_string]) {
        if (scanner->free_format_mode) {
            return false;
        }
        
        while(true) {
            if(lexer->lookahead != '"') {
                return false;
            }
            lexer->advance(lexer, false);
            while(lexer->lookahead != '"' && lexer->lookahead != 0 && lexer->get_column(lexer) < 72) {
                lexer->advance(lexer, false);
            }
            if(lexer->lookahead == '"') {
                lexer->result_symbol = multiline_string;
                lexer->advance(lexer, false);
                lexer->mark_end(lexer);
                return true;
            }
            while(lexer->lookahead != 0 && lexer->lookahead != '\n') {
                lexer->advance(lexer, true);
            }
            if(lexer->lookahead == 0) {
                return false;
            }
            lexer->advance(lexer, true);
            int i;
            for(i=0; i<=5; ++i) {
                if(lexer->lookahead == 0 || lexer->lookahead == '\n') {
                    return false;
                }
                lexer->advance(lexer, true);
            }

            if(lexer->lookahead != '-') {
                return false;
            }

            lexer->advance(lexer, true);
            while(lexer->lookahead == ' ' && lexer->get_column(lexer) < 72) {
                lexer->advance(lexer, true);
            }
        }
    }

    return false;
}

unsigned tree_sitter_COBOL_external_scanner_serialize(void *payload, char *buffer) {
    Scanner *scanner = (Scanner *)payload;
    if (scanner == NULL) return 0;
    
    buffer[0] = scanner->free_format_mode ? 1 : 0;
    return 1;
}

void tree_sitter_COBOL_external_scanner_deserialize(void *payload, const char *buffer, unsigned length) {
    Scanner *scanner = (Scanner *)payload;
    if (scanner == NULL || length == 0) return;
    
    scanner->free_format_mode = (buffer[0] != 0);
}

void tree_sitter_COBOL_external_scanner_destroy(void *payload) {
    Scanner *scanner = (Scanner *)payload;
    if (scanner != NULL) {
        ts_free(scanner);
    }
}
