#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum TOKENIZER_ERROR{
    ERROR_NONE,
    ERROR_EOF,
    ERROR_ILLEGAL_CHARACTER,
    ERROR_MISSING_COLON,
    ERROR_MISSING_COMMA,
    ERROR_INVALID_NUMBER,
    ERROR_INVALID_LITERAL,
    ERROR_UNTERMINATED_STRING,
    ERROR_UNTERMINATED_ARRAY,
    ERROR_UNTERMINATED_OBJECT,
    ERROR_MEMORY,
} TOKENZIER_ERROR;

typedef enum TOKEN_TYPE{
    TYPE_OPEN_BRACE,
    TYPE_CLOSED_BRACE,
    TYPE_OPEN_BRACKET,
    TYPE_CLOSED_BRACKET,
    TYPE_COLON,
    TYPE_COMMA,
    TYPE_STRING,
    TYPE_NUMBER,
    TYPE_FALSE,
    TYPE_TRUE,
    TYPE_NULL,
} TOKEN_TYPE;

typedef enum LITERAL_TYPE{
    TYPE_LITERAL_NULL,
    TYPE_LITERAL_TRUE,
    TYPE_LITERAL_FALSE,
} LITERAL_TYPE;

typedef struct Token{
    TOKEN_TYPE type;
    size_t start;
    size_t length;
} Token;

typedef struct Tokenizer{
    TOKENZIER_ERROR error;
    size_t start;
    size_t curr;
    size_t data_size;
    const char *data;
} Tokenizer;

Tokenizer *tokenizer_new(const char *data, size_t data_size);
Token *tokenizer_parse(Tokenizer *tokenizer, size_t *tokens_size);


