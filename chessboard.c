#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "chessboard.h"
#include "stringbuilder.h"

char convert_AlgebraicNotation_in_index(char algebraicNotation[3]) {
    char output;
    algebraicNotation[0] = tolower(algebraicNotation[0]);
    if(algebraicNotation[0] < 'a' || algebraicNotation[0] > 'h'){
        printf("%s is an invalid AlgebraicNotation.", algebraicNotation);
        return  -1;
    }
    output = algebraicNotation[0] - 97;
    if(algebraicNotation[1] < '1' || algebraicNotation[1] > '8'){
        printf("%s is an invalid AlgebraicNotation.", algebraicNotation);
        return  -1;
    }
    output +=  (8 - (algebraicNotation[1]-48))*8;
    return output;
}

// TODO assure that dest is size 3
void convert_index_in_AlgebraicNotation(char dest[3], char index){
    if (index < 0 || index > 63){
        printf("%d is an invalid index", index);
        exit(EXIT_FAILURE);
    }
    sprintf(dest, "%c%d", 97 + index%8, 8 - (index / 8));
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
            exit(EXIT_FAILURE);
        }
        switch (*current) {
            case 'r':
            case 'n':
            case 'b':
            case 'q':
            case 'k':
            case 'p':
            case 'R':
            case 'N':
            case 'B':
            case 'Q':
            case 'K':
            case 'P':
                board->gameState[i] = *current;
                i++;
                break;
            case '/':
                if(i%8 != 0){
                    i += 8 - i%8;
                }
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                i += *current-48;
                break;
            default:
                printf("FAL is invalid.\n");
                exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
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
                exit(EXIT_FAILURE);
        }
        current++;
    }
    if(board->castling > 15){
        printf("FAL is invalid.\n");
        exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
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

char is_white_piece(char piece){
    switch (piece) {
        case 'R':
        case 'N':
        case 'B':
        case 'Q':
        case 'K':
        case 'P':
            return 1;
        default:
            return 0;
    }
}

char is_black_piece(char piece){
    switch (piece) {
        case 'r':
        case 'n':
        case 'b':
        case 'q':
        case 'k':
        case 'p':
            return 1;
        default:
            return 0;
    }
}

// checks not if through the move the own king can be taken FIX
char* get_possible_moves (struct Chessboard* board,  char index){
    char* possible_moves = calloc(27, 1);
    if(possible_moves == NULL){
        printf("calloc failed.\n");
        exit(EXIT_FAILURE);
    }
    if(index > 63){
        printf("%d is illegal index for chessboard.\n", index);
        return NULL;
    }
    char chessPiece = *(board->gameState + index);


    char current_row = index/8;
    int i = 0;
    switch (chessPiece) {
        case 'r':
            break;
        case 'n':
            break;
        case 'b':
            // normal move
            if(*(board->gameState+index +  8) == 0){
                possible_moves[i] = index + 8;
                i++;
            }
            // two squares at once (only if piece is at its starting point)
            if(*(board->gameState+index +  16) == 0 &&index > 7 && index < 16){
                possible_moves[i] = index + 16;
                i++;
            }
            // take a white piece on the left (whites perspective)
            if(is_black_piece(*(board->gameState + (index+7))) && ((index+9)/8)-1 == current_row){
                possible_moves[i] = index + 7;
                i++;
            }
            // take a white piece on the right (whites perspective)
            if(is_black_piece(*(board->gameState + (index+9))) && ((index+9)/8)-1 == current_row){
                possible_moves[i] = index + 9;
                i++;
            }
            // en passant check
            if((index+1 == board->en_passant && ((index+1)/8) == current_row) || (index-1 == board->en_passant && ((index-1)/8) == current_row)){
                possible_moves[i] = board->en_passant+8;
            }
            break;
        case 'q':
            break;
        case 'k':
            break;
        case 'p':
            break;

        case 'R':
            break;
        case 'N':
            break;
        case 'B':
            break;
        case 'Q':
            break;
        case 'K':
            break;
        case 'P':
            break;
        case '\0':
            printf("Selected an empty square.\n");
            return possible_moves;
        default:
            printf("%c is an invalid piece on board.", chessPiece);
            exit(EXIT_FAILURE);
    }
    return possible_moves;
}
