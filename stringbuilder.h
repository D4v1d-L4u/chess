#ifndef CHESS_STRINGBUILDER_H
#define CHESS_STRINGBUILDER_H

#include <stddef.h>

struct Stringbuilder{
    char* string;
    int current_length;
    int size;
};
/*
 * Initialize the Stringbuilder.
 * the size is at the beginning at 21
 */
void init_stringbuilder(struct Stringbuilder *stringbuilder);

/*
 * Appends a given char to the Stringbuilder
 */
void append(struct Stringbuilder *stringbuilder, char src);

/*
 * Appends a given char to the Stringbuilder
 */
void append_string(struct Stringbuilder *stringbuilder, char* src);

/*
 * Appends a given number (long) to the Stringbuilder
 */
void append_num(struct Stringbuilder *stringbuilder, long number);

/*
 * Returns the string created by the Stringbuilder
 */
char* getString(struct  Stringbuilder *stringbuilder);

#endif //CHESS_STRINGBUILDER_H
