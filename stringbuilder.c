#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stringbuilder.h"

void init_stringbuilder(struct Stringbuilder *stringbuilder) {
    stringbuilder->string = calloc(10, 1);
    if(stringbuilder->string == NULL){
        printf("calloc error\n");
        exit(EXIT_FAILURE);
    }
    stringbuilder->size = 21;
    stringbuilder->current_length = 0;
}

void append(struct Stringbuilder *stringbuilder, char src){
    if(stringbuilder->current_length >= stringbuilder->size - 1){
        int newSize = stringbuilder->size/2 + stringbuilder->size;
        char*  tmp = calloc(newSize, 1);
        if(tmp == NULL){
            printf("calloc error\n");
            exit(EXIT_FAILURE);
        }
        memcpy(tmp, stringbuilder->string, stringbuilder->current_length);
        free(stringbuilder->string);
        stringbuilder->string = tmp;
        stringbuilder->size = newSize;
    }
    int i = 0;
    while(*(stringbuilder->string + i)){
        i++;
    }
    if (stringbuilder->current_length != i){
        printf("\n|||str(%d) und i(%d) sind ungleich\n", stringbuilder->current_length, i);
    }
    *(stringbuilder->string + stringbuilder->current_length) = src;
    stringbuilder->current_length++;
}

void append_string(struct Stringbuilder *stringbuilder, char* src){
    while (*src){
        append(stringbuilder, *src);
        src++;
    }
}

void append_num(struct Stringbuilder *stringbuilder, long number){
    char* number_as_string = calloc(20, 1); // TODO find way tu reduce the 20 etc.
    if(number_as_string == NULL){
        printf("calloc error\n");
        exit(EXIT_FAILURE);
    }
    sprintf(number_as_string, "%ld", number);
    append_string(stringbuilder, number_as_string);
    free(number_as_string);
}

//TODO make it that the returned string is smaller etc.
char* getString(struct  Stringbuilder *stringbuilder){
    return stringbuilder->string;
}
