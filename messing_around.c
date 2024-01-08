#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char** storage;
    int amount;
    int size;
} StringArray;

StringArray *string_array_create() {
    StringArray *string_array = malloc(sizeof(StringArray));
    string_array->storage = NULL;
    string_array->amount = 0;
    string_array->size = 0;
}

void string_array_preserve(StringArray *string_array, int size) {
    unsigned int v = size;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    string_array->size = v;
    string_array->storage = realloc(string_array->storage, (string_array->size) * sizeof(char*));
    printf("REALLOC!\n");
}

void string_array_push(StringArray *string_array, const char *string) {
    string_array->amount++;
    if (string_array->amount > string_array->size) {
        if (string_array->size == 0) {
            string_array->size = 1;
        }
        else {
            string_array->size *= 2;
        }
        string_array->storage = realloc(string_array->storage, (string_array->size) * sizeof(char*));
        printf("REALLOC!\n");
    }
    string_array->storage[string_array->amount - 1] = (char *)malloc(strlen(string));
    strcpy(string_array->storage[string_array->amount - 1], string);
}

void string_array_print(StringArray *string_array) {
    for (int i = 0; i < string_array->amount; ++i) {
        printf("%s\n", string_array->storage[i]);
    }
}

int starts_with(const char *str, const char *substr) {
    int length_str = strlen(str);
    int length_substr = strlen(substr);

    if (length_str < length_substr) {
        return 0;
    }

    for (int i = 0; i < length_substr; ++i) {
        if (str[i] != substr[i]) {
            return 0;
        }
    }

    return 1;
}

void lex(const char *code) {
    //StringArray tokens;
    

    //return strings;
}

int main() {
    StringArray *tokens = string_array_create();
    string_array_preserve(tokens, 16);
    
    string_array_push(tokens, "Hello");
    string_array_push(tokens, "My");
    string_array_push(tokens, "Cool");
    string_array_push(tokens, "Fellas");
    
    string_array_print(tokens);
    return 0;
}
