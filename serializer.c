#include "tokenizer.h"
#include <string.h>

typedef struct Parser{
    size_t tokens_size;
    size_t out_size;
    size_t out_index;
    size_t curr;
    Token *tokens;
    const char *data;
    char *out;
}Parser;

int parser_add_to_out(Parser *parser, const char *data, size_t data_size){
    if (data_size >= (parser->out_size - parser->out_index)){
        parser->out_size = (parser->out_size + data_size) * 2;
        parser->out = realloc(parser->out, sizeof(char) * parser->out_size);
        if (parser->out == NULL){
            fprintf(stderr, "unable to raelloc out\n");
            return 0;
        }
    }

    memcpy(parser->out + parser->out_index, data, sizeof(char) * data_size);
    parser->out_index += data_size;

    return 1;
}

void parser_move(Parser *parser){
    if (parser->curr >= parser->tokens_size){
        return;
    }

    parser->curr++;
}

int parser_comsume(Parser *parser, TOKEN_TYPE type){
    if (parser->tokens[parser->curr].type != type){
        return 0;
    }

    parser->curr++;
    return 1;
}

int parser_parse_object(Parser *parser);
int parser_parse_array(Parser *parser);

int parser_parse_value(Parser *parser){
    switch (parser->tokens[parser->curr].type) {
        case TYPE_OPEN_BRACE:{
            if (!parser_add_to_out(parser, " os", strlen(" os"))){
                return 0;
            }
            if (!parser_parse_object(parser)){
                return 0;
            }
            if (!parser_add_to_out(parser, " oe", strlen(" oe"))){
                return 0;
            }
            break;
        }
        case TYPE_OPEN_BRACKET:{
            if (!parser_add_to_out(parser, " as", strlen(" as"))){
                return 0;
            }
            if (!parser_parse_array(parser)){
                return 0;
            }
            if (!parser_add_to_out(parser, " ae", strlen(" ae"))){
                return 0;
            }
            break;
        }
        case TYPE_STRING:{
            if (!parser_add_to_out(parser, " s", strlen(" s"))){
                return 0;
            }
            parser_move(parser);
            break;
        }
        case TYPE_NUMBER:{
            if (!parser_add_to_out(parser, " f", strlen(" f"))){
                return 0;
            }
            parser_move(parser);
            break;
        }
        case TYPE_FALSE:
        case TYPE_TRUE:{
            if (!parser_add_to_out(parser, " b", strlen(" b"))){
                return 0;
            }
            parser_move(parser);
            break;
        }
        case TYPE_NULL:{
            if (!parser_add_to_out(parser, " 0", strlen(" 0"))){
                return 0;
            }
            parser_move(parser);
            break;
        }
        default:{
            fprintf(stderr, "invalid type %d\n", parser->tokens[parser->curr].type);
            return 0;
        }
    }

    return 1;
}

int is_space_(int c){
    return (c == '\x20' || c == '\x09' || c == '\x0A' || c == '\x0D');
}

int parser_parse_key_value(Parser *parser){
    if (!parser_comsume(parser, TYPE_STRING)){
        fprintf(stderr, "expceted string");
        return 0;
    }


    size_t start = parser->tokens[parser->curr - 1].start;
    size_t length = parser->tokens[parser->curr - 1].length;
    while (is_space_(parser->data[start]) || parser->data[start] == '"') {
        start++;
        length--;
    }
    length--;

    if (!parser_add_to_out(parser, " ", strlen(" "))){
        return 0;
    }

    if (!parser_add_to_out(parser, parser->data + start, length)){
        return 0;
    }

    if (!parser_comsume(parser, TYPE_COLON)){
        fprintf(stderr, "expceted colon");
        return 0;
    }

    switch (parser->tokens[parser->curr].type) {
        case TYPE_OPEN_BRACE:{
            if (!parser_add_to_out(parser, ": os", strlen(": os"))){
                return 0;
            }
            if (!parser_parse_object(parser)){
                return 0;
            }
            if (!parser_add_to_out(parser, " oe", strlen(" oe"))){
                return 0;
            }
            break;
        }
        case TYPE_OPEN_BRACKET:{
            if (!parser_add_to_out(parser, ": as", strlen(": as"))){
                return 0;
            }
            if (!parser_parse_array(parser)){
                return 0;
            }
            if (!parser_add_to_out(parser, " ae", strlen(" ae"))){
                return 0;
            }
            break;
        }
        case TYPE_STRING:{
            if (!parser_add_to_out(parser, ": s", strlen(": s"))){
                return 0;
            }
            parser_move(parser);
            break;
        }
        case TYPE_NUMBER:{
            if (!parser_add_to_out(parser, ": f", strlen(": f"))){
                return 0;
            }
            parser_move(parser);
            break;
        }
        case TYPE_FALSE:
        case TYPE_TRUE:{
            if (!parser_add_to_out(parser, ": b", strlen(": b"))){
                return 0;
            }
            parser_move(parser);
            break;
        }
        case TYPE_NULL:{
            if (!parser_add_to_out(parser, ": 0", strlen(": 0"))){
                return 0;
            }
            parser_move(parser);
            break;
        }
        default:{
            fprintf(stderr, "invalid type %d\n", parser->tokens[parser->curr].type);
            return 0;
        }
    }

    return 1;
}

int parser_parse_array(Parser *parser){
    if (!parser_comsume(parser, TYPE_OPEN_BRACKET)){
        fprintf(stderr, "expceted open bracket\n");
        return 0;
    }

    while (1) {
        if (parser->tokens[parser->curr].type == TYPE_CLOSED_BRACKET){
            break;
        }
        if (!parser_parse_value(parser)){
            fprintf(stderr, "invalind value\n");
            return 0;
        }

        int res = parser_comsume(parser, TYPE_COMMA);
        if (!res){
            if (!parser_comsume(parser, TYPE_CLOSED_BRACKET)){
                fprintf(stderr, "untermintaed array\n");
                return 0;
            }

            return 1;;
        }
        if (!parser_add_to_out(parser, ",", strlen(","))){
            return 0;
        }
        if (parser->curr >= parser->tokens_size || parser->tokens[parser->curr].type == TYPE_CLOSED_BRACKET){
            fprintf(stderr, "trailing comma\n");
            return 0;
        }
    }

    if (!parser_comsume(parser, TYPE_CLOSED_BRACKET)){
        fprintf(stderr, "expected cloased bracket\n");
        return 0;
    }

    return 1;
}

int parser_parse_object(Parser *parser){
    if (!parser_comsume(parser, TYPE_OPEN_BRACE)){
        fprintf(stderr, "expceted open brace\n");
        return 0;
    }

    while (1) {
        if (parser->tokens[parser->curr].type == TYPE_CLOSED_BRACE){
            break;
        }
        if (!parser_parse_key_value(parser)){
            fprintf(stderr, "invalind key value\n");
            return 0;
        }
        int res = parser_comsume(parser, TYPE_COMMA);
        if (!res){
            if (!parser_comsume(parser, TYPE_CLOSED_BRACE)){
                fprintf(stderr, "untermintaed object\n");
                return 0;
            }

            return 1;
        }
        if (!parser_add_to_out(parser, ",", strlen(","))){
            return 0;
        }
        if (parser->curr >= parser->tokens_size || parser->tokens[parser->curr].type == TYPE_CLOSED_BRACE){
            fprintf(stderr, "trailing comma\n");
            return 0;
        }
    }

    if (!parser_comsume(parser, TYPE_CLOSED_BRACE)){
        fprintf(stderr, "expected cloased brace\n");
        return 0;
    }

    return 1;
}

int parser_parse_token(Parser *parser){
    switch (parser->tokens[parser->curr].type) {
        case TYPE_OPEN_BRACE:{
            if (!parser_add_to_out(parser, " os", strlen(" os"))){
                return 0;
            }
            if (!parser_parse_object(parser)){
                return 0;
            }
            if (!parser_add_to_out(parser, " oe", strlen(" oe"))){
                return 0;
            }
            break;
        }
        case TYPE_OPEN_BRACKET:{
            if (!parser_add_to_out(parser, " as", strlen(" as"))){
                return 0;
            }
            if (!parser_parse_array(parser)){
                return 0;
            }
            if (!parser_add_to_out(parser, " ae", strlen(" ae"))){
                return 0;
            }
            break;
        }
        default:{
            fprintf(stderr, "unknown token type %d\n", parser->tokens[parser->curr].type);
            return 0;
        }
    }

    return parser->curr >= parser->tokens_size ? -1 : 1;
}

char *serialize(const char *data, size_t data_size, size_t *out_size){
    Tokenizer *tokenizer = tokenizer_new(data, data_size);
    if (tokenizer == NULL){
        return NULL;
    }

    size_t tokens_size = 0;
    Token *tokens = tokenizer_parse(tokenizer, &tokens_size);
    if (tokenizer->error != ERROR_NONE && tokenizer->error != ERROR_EOF){
        fprintf(stderr, "tokneize should be EOF vs %d\n", tokenizer->error);
        return NULL;
    }

    Parser *parser = malloc(sizeof(Parser));
    if (parser == NULL){
        fprintf(stderr, "unable to allocate memory for parser\n");
        return NULL;
    }

    parser->data = data;
    parser->tokens = tokens;
    parser->tokens_size = tokens_size;
    parser->out_size = 1;
    parser->out_index = 0;
    parser->curr = 0;
    parser->out = malloc(sizeof(char) * parser->out_size);
    if (parser == NULL){
        fprintf(stderr, "unable to allocate memory for out\n");
        return NULL;
    }

    int res;
    while (1) {
        res = parser_parse_token(parser);
        if (!res){
            fprintf(stderr, "error\n");
            return NULL;
        }
        if (res == -1){
            break;
        }
    }

    return parser->out;
}

int main (int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "usage:%s <JSON_STRING>\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t out_size = 0;
    char *out = serialize(argv[1], strlen(argv[1]), &out_size);
    if (out == NULL){
        fprintf(stderr, "%s:ERROR: \n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("%*s\n", (int)out_size, out);
    free(out);
    return EXIT_SUCCESS;
}
