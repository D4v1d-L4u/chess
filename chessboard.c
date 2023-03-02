#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "chessboard.h"
#include "stringbuilder.h"

char convert_AlgebraicNotation_in_index(char algebraicNotation[3]) {
    char output;
    algebraicNotation[0] = tolower(algebraicNotation[0]);
    switch (algebraicNotation[0]) {
        case 'a':
            output = 0;
            break;
        case 'b':
            output = 1;
            break;
        case 'c':
            output = 2;
            break;
        case 'd':
            output = 3;
            break;
        case 'e':
            output = 4;
            break;
        case 'f':
            output = 5;
            break;
        case 'g':
            output = 6;
            break;
        case 'h':
            output = 7;
            break;
        default:
            printf("Invalid algebraic notation\n");
            return -1;
    }
    switch (algebraicNotation[1]) {
        case '1':
            output += 56;
            break;
        case '2':
            output += 48;
            break;
        case '3':
            output += 40;
            break;
        case '4':
            output += 32;
            break;
        case '5':
            output += 24;
            break;
        case '6':
            output += 16;
            break;
        case '7':
            output += 8;
            break;
        case '8':
            output += 0;
            break;
        default:
            printf("Invalid algebraic notation\n");
            return -1;
    }
    return output;
}

// TODO assure that dest is size 3
void convert_index_in_AlgebraicNotation(char dest[3], char index){
    switch (index%8) {
        case 0:
            sprintf(dest, "a%d", 8 - (index / 8));
            break;
        case 1:
            sprintf(dest, "b%d", 8 - (index / 8));
            break;
        case 2:
            sprintf(dest, "c%d", 8 - (index / 8));
            break;
        case 3:
            sprintf(dest, "d%d", 8 - (index / 8));
            break;
        case 4:
            sprintf(dest, "e%d", 8 - (index / 8));
            break;
        case 5:
            sprintf(dest, "f%d", 8 - (index / 8));
            break;
        case 6:
            sprintf(dest, "g%d", 8 - (index / 8));
            break;
        case 7:
            sprintf(dest, "h%d", 8 - (index / 8));
            break;
        default:
            printf("Index is wrong!\n");
    }
}

// TODO make set_gameState check if chess rules are violated
void set_gameState(struct Chessboard* board, char* fen){
    memset(board->gameState, 0, 64);
    char* current = &fen[0];
    int i = 0;
    // set gameState
    while (*current && *current != ' '){
        if(i > 63){
            printf("FAL is invalid.\n");
            return;
        }
        switch (*current) { // TODO find a alternative for the switch here
            case 'r':
                board->gameState[i] = 'r';
                i++;
                break;
            case 'n':
                board->gameState[i] = 'n';
                i++;
                break;
            case 'b':
                board->gameState[i] = 'b';
                i++;
                break;
            case 'q':
                board->gameState[i] = 'q';
                i++;
                break;
            case 'k':
                board->gameState[i] = 'k';
                i++;
                break;
            case 'p':
                board->gameState[i] = 'p';
                i++;
                break;

            case 'R':
                board->gameState[i] = 'R';
                i++;
                break;
            case 'N':
                board->gameState[i] = 'N';
                i++;
                break;
            case 'B':
                board->gameState[i] = 'B';
                i++;
                break;
            case 'Q':
                board->gameState[i] = 'Q';
                i++;
                break;
            case 'K':
                board->gameState[i] = 'K';
                i++;
                break;
            case 'P':
                board->gameState[i] = 'P';
                i++;
                break;

            case '/':
                if(i%8 != 0){
                    i += 8 - i%8;
                }
                break;

            case '1':
                i += 1;
                break;
            case '2':
                i += 2;
                break;
            case '3':
                i += 3;
                break;
            case '4':
                i += 4;
                break;
            case '5':
                i += 5;
                break;
            case '6':
                i += 6;
                break;
            case '7':
                i += 7;
                break;
            case '8':
                i += 8;
                break;
            default:
                printf("FAL is invalid.\n");
                return;
        }
        current++;
    }
    current++;
    // get whose turn it is
    if(*current == 'w'){
        board->half_round_count = 0;
    }
    else if(*current == 'b'){
        board->half_round_count = 1;
    }
    else{
        printf("FAL is invalid.\n");
        return;
    }
    current += 2;

    // get castling state
    board->castling = 0;
    while (*current && *current != ' '){
        switch (*current) {
            case 'K':
                board->castling += 1;
                break;
            case 'Q':
                board->castling += 2;
                break;
            case 'k':
                board->castling +=4;
                break;
            case 'q':
                board->castling +=8;
                break;
            case '-':
                break;
            default:
                printf("FAL is invalid.\n");
                return;
        }
        current++;
    }
    if(board->castling > 15){
        printf("FAL is invalid.\n");
        return;
    }
    current++;

    // get information about En passant
    if(*current == '-'){
        board->en_passant = 0;
    }else{
        char tmp[3];
        memcpy(tmp, current, 2);
        board->en_passant = convert_AlgebraicNotation_in_index(tmp);
        if(board->en_passant == -1){
            board->en_passant = 0;
            printf("FAL is invalid.\n");
        }
    }
    current += 2;
    if(*current == ' '){
        current++;
    }

    // get halfmove_clock (for fifty-move rule)
    char* ptr;
    board->halfmove_clock = strtol(current, &ptr, 10);
    current = ptr;

    // get round count
    board->round_count = strtol(current, &ptr, 10);
    board->half_round_count += 2*board->round_count;
}

// TODO remove stringbuilder and replace it with string of size 87 + 1
char* get_FEN(struct Chessboard* board){ // max length  87
    struct Stringbuilder fen;
    init_stringbuilder(&fen);
    char emptySpaceCounter = 0;
    // write down the piece positions
    for(int i = 0; i < 64; i++){
        if(board->gameState[i] == 0){
            emptySpaceCounter++;
        }
        else{
            if(emptySpaceCounter){
                append_num(&fen,emptySpaceCounter);
                emptySpaceCounter = 0;
            }
            append(&fen, board->gameState[i]);
        }
        if((i+1)%8 == 0 && i != 0){
            if(emptySpaceCounter){
                append_num(&fen, emptySpaceCounter);
                emptySpaceCounter = 0;
            }
            if(i != 63){
                append(&fen, '/');
            }

        }
    }
    append(&fen, ' ');
    // write who's turn it is
    if(board->half_round_count%2 == 0){
        append_string(&fen, "w ");
    } else{
        append_string(&fen, "b ");
    }

    // write down the castling state
    // white king-side castling
    char check = 1;
    if((board->castling & 0x1) > 0){
        append(&fen, 'K');
        check = 0;

    }
    // white queen-side castling
    if((board->castling & 0x2) > 0){
        append(&fen, 'Q');
        check = 0;
    }
    // black king-side castling
    if((board->castling & 0x4) > 0){
        append(&fen, 'k');
        check = 0;
    }
    // black queen-side castling
    if((board->castling & 0x8) > 0){
        append(&fen, 'q');
    }else if(check){
        append(&fen, '-');
    }
    append(&fen, ' ');


    // write down en_passant
    if(board->en_passant != 0){
        char algebraicNotation[3];
        convert_index_in_AlgebraicNotation(algebraicNotation,board->en_passant);
        append_string(&fen, algebraicNotation);
    }
    else{
        append(&fen ,'-');
    }
    append(&fen, ' ');

    // write down fifty-move rule status
    append_num(&fen, board->halfmove_clock);
    append(&fen, ' ');
    // write round count
    append_num(&fen, board->round_count);
    return getString(&fen);
}

void init_board(struct Chessboard* board){
    set_gameState(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

char* get_possible_moves (struct Chessboard* board,  char index){
    char* possible_moves = calloc(27, 1);
    if(possible_moves == NULL){
        printf("calloc failed.\n");
        exit(EXIT_FAILURE);
    }
    if(index > 63){
        printf("%d is illegal index for chessboard.\n", index);
    }
    char chessPiece = *(board->gameState + index);
    switch (chessPiece) {

    }
    return possible_moves;
}
