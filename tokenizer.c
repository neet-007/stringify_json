#include "tokenizer.h"

static Token tokenizer_token_new(Tokenizer *tokenizer, TOKEN_TYPE type){
    Token token;

    token.type = type;
    token.start = tokenizer->start;
    token.length = tokenizer->curr - tokenizer->start;

    return token;
}

Tokenizer *tokenizer_new(const char *data, size_t data_size){
    Tokenizer *tokenizer = malloc(sizeof(Tokenizer));

    tokenizer->error = ERROR_NONE;
    tokenizer->data = data;
    tokenizer->data_size = data_size;
    tokenizer->start = tokenizer->curr = 0;

    return tokenizer;
}

int tokenizer_peak(Tokenizer *tokenizer){
    if (tokenizer->curr >= tokenizer->data_size){
        return -1;
    }

    return tokenizer->data[tokenizer->curr + 1];
}

void tokenizer_move(Tokenizer *tokenizer){
    if (tokenizer->curr >= tokenizer->data_size){
        tokenizer->error = ERROR_EOF;
        return;
    }

    tokenizer->curr++;
}

int is_space(int c){
    return (c == '\x20' || c == '\x09' || c == '\x0A' || c == '\x0D');
}

void tokenizer_skip_white_space(Tokenizer *tokenizer){
    const char *data = tokenizer->data;
    while (is_space(data[tokenizer->curr])) {
        tokenizer_move(tokenizer);
        if (tokenizer->error != ERROR_NONE){
            break;
        }
    }
}

void tokenizer_parse_char(Tokenizer *tokenizer, int c){
    tokenizer_skip_white_space(tokenizer);
    if (tokenizer->error != ERROR_NONE){
        return;
    }

    if (tokenizer->data[tokenizer->curr] != c){
        tokenizer->error = ERROR_ILLEGAL_CHARACTER;
    }
}

Token tokenizer_parse_string(Tokenizer *tokenizer){
    int c;
    int skip = 0;
    Token token;
    const char *data = tokenizer->data;
    while (data[tokenizer->curr] != '"') {
        if (data[tokenizer->curr] == '\\'){
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                break;
            }
            c = data[tokenizer->curr];

            if (c != '"' & c != '\\' & c != '/' & c != 'b' & c != 'n' & c != 'r' & c != 't' & c != 'f'){
                tokenizer->error = ERROR_ILLEGAL_CHARACTER;
                return token;
            }
            if (c == '"' || c == '\\' || c == '/'){
                skip++;
            }
        }
        tokenizer_move(tokenizer);
        if (tokenizer->error != ERROR_NONE){
            break;
        }
    }

    if (tokenizer->error != ERROR_NONE){
        return token;
    }

    tokenizer_move(tokenizer);
    token = tokenizer_token_new(tokenizer, TYPE_STRING);
    //token.length -= skip;

    return token;
}

Token tokenizer_parse_literal(Tokenizer *tokenizer, LITERAL_TYPE type){
    Token token;
    const char *data = tokenizer->data;
    switch (type) {
        case TYPE_LITERAL_TRUE:{
            if (data[tokenizer->curr] != 'r'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                return token;
            }

            if (data[tokenizer->curr] != 'u'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                return token;
            }

            if (data[tokenizer->curr] != 'e'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);

            token = tokenizer_token_new(tokenizer, TYPE_TRUE);
            break;
        }

        case TYPE_LITERAL_FALSE:{
            if (data[tokenizer->curr] != 'a'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                return token;
            }

            if (data[tokenizer->curr] != 'l'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                return token;
            }

            if (data[tokenizer->curr] != 's'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                return token;
            }

            if (data[tokenizer->curr] != 'e'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);

            token = tokenizer_token_new(tokenizer, TYPE_FALSE);
            break;
        }

        case TYPE_LITERAL_NULL:{
            if (data[tokenizer->curr] != 'u'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                return token;
            }

            if (data[tokenizer->curr] != 'l'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                return token;
            }

            if (data[tokenizer->curr] != 'l'){
                tokenizer->error = ERROR_INVALID_LITERAL;
                return token;
            }
            tokenizer_move(tokenizer);

            token = tokenizer_token_new(tokenizer, TYPE_NULL);
            break;
        }

        default:{
            tokenizer->error = ERROR_INVALID_LITERAL;
            return token;
        }
    }

    return token;
}

Token tokenizer_parse_number(Tokenizer *tokenizer){
    Token token;
    const char *data = tokenizer->data;

    if (data[tokenizer->curr] == '-' || data[tokenizer->curr] == '+'){
        tokenizer_move(tokenizer);
        if (tokenizer->error != ERROR_NONE){
            return token;
        }
    }

    while ('0' <= data[tokenizer->curr] && data[tokenizer->curr] <= '9'){
        tokenizer_move(tokenizer);
        if (tokenizer->error != ERROR_NONE){
            break;
        }
    }

    if (tokenizer->error != ERROR_NONE){
        return token;
    }

    if (data[tokenizer->curr] == '.'){
        tokenizer_move(tokenizer);
        while ('0' <= data[tokenizer->curr] && data[tokenizer->curr] <= '9'){
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                break;
            }
        }

        if (tokenizer->error != ERROR_NONE){
            return token;
        }
    }

    if (data[tokenizer->curr] == 'e' || data[tokenizer->curr] == 'E'){
        tokenizer_move(tokenizer);
        if (data[tokenizer->curr] == '+' || data[tokenizer->curr] == '-'){
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                return token;;
            }
        }

        while ('0' <= data[tokenizer->curr] && data[tokenizer->curr] <= '9'){
            tokenizer_move(tokenizer);
            if (tokenizer->error != ERROR_NONE){
                break;
            }
        }

        if (tokenizer->error != ERROR_NONE){
            return token;
        }
    }

    token = tokenizer_token_new(tokenizer, TYPE_NUMBER);

    return token;
}

Token *tokenizer_parse(Tokenizer *tokenizer, size_t *tokens_size){
    Token token;
    size_t tokens_len = 0;
    *tokens_size = 1;
    Token *tokens = malloc(sizeof(Token) * *tokens_size);
    if (tokens == NULL){
        tokenizer->error = ERROR_MEMORY;
        return NULL;
    }

    int c;
    int loop = 1;
    while (loop) {
        tokenizer_skip_white_space(tokenizer);
        if (tokenizer->error != ERROR_NONE){
            if (tokenizer->error == ERROR_EOF){
                break;
            }
            return NULL;
        }

        c = tokenizer->data[tokenizer->curr];
        switch (c) {
            case '\0':{
                loop = 0;
                break;
            }
            case '{':{
                token = tokenizer_token_new(tokenizer, TYPE_OPEN_BRACE);
                tokenizer_move(tokenizer);
                break;
            }
            case '}':{
                token = tokenizer_token_new(tokenizer, TYPE_CLOSED_BRACE);
                tokenizer_move(tokenizer);
                break;
            }
            case '[':{
                token = tokenizer_token_new(tokenizer, TYPE_OPEN_BRACKET);
                tokenizer_move(tokenizer);
                break;
            }
            case ']':{
                token = tokenizer_token_new(tokenizer, TYPE_CLOSED_BRACKET);
                tokenizer_move(tokenizer);
                break;
            }
            case ':':{
                token = tokenizer_token_new(tokenizer, TYPE_COLON);
                tokenizer_move(tokenizer);
                break;
            }
            case ',':{
                token = tokenizer_token_new(tokenizer, TYPE_COMMA);
                tokenizer_move(tokenizer);
                break;
            }
            case '"':{
                tokenizer_move(tokenizer);
                token = tokenizer_parse_string(tokenizer);
                break;
            }
            default:{
                if (c == '-' || c == '+' || ('0' <= c && c <= '9')){
                    token = tokenizer_parse_number(tokenizer);
                }else if (c == 't'){
                    tokenizer_move(tokenizer);
                    token = tokenizer_parse_literal(tokenizer, TYPE_LITERAL_TRUE);
                }else if (c == 'f'){
                    tokenizer_move(tokenizer);
                    token = tokenizer_parse_literal(tokenizer, TYPE_LITERAL_FALSE);
                }else if (c == 'n'){
                    tokenizer_move(tokenizer);
                    token = tokenizer_parse_literal(tokenizer, TYPE_LITERAL_NULL);
                }else{
                    tokenizer->error = ERROR_ILLEGAL_CHARACTER;
                    return NULL;
                }
            }
        }
        if (loop == 0){
            break;
        }
        if (tokenizer->error != ERROR_NONE){
            if (tokenizer->error == ERROR_EOF){
                break;
            }
            return NULL;
        }

        if (tokens_len >= *tokens_size){
            *tokens_size *= 2;
            tokens = realloc(tokens, sizeof(Token) * (*tokens_size));
            if (tokens == NULL){
                tokenizer->error = ERROR_MEMORY;
                return NULL;
            }
        }

        tokens[tokens_len++] = token;
        tokenizer->start = tokenizer->curr;
    }

    *tokens_size = tokens_len;
    tokens = realloc(tokens, sizeof(Token) * (*tokens_size));
    if (tokens == NULL){
        tokenizer->error = ERROR_MEMORY;
        return NULL;
    }

    return tokens;
}
