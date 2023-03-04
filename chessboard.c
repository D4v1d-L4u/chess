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

// TODO Make testcases for this methode && checks not if through the move the own king can be taken FIX
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
    int k; // Iterator for iterating through the game-board
    switch (chessPiece) {
        case 'K':
        case 'k':
            // move one right
            if(index + 1 < 64  && (index+1)/8 == current_row){
                if(*(board->gameState+1) == 0){
                    possible_moves[i] = index + 1;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState+1))){
                    possible_moves[i] = index + 1;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState+1))){
                    possible_moves[i] = index + 1;
                    i++;
                }
            }
            // move one left
            if(index - 1 > 0  && (index-1)/8 == current_row){
                if(*(board->gameState-1) == 0){
                    possible_moves[i] = index - 1;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState-1))){
                    possible_moves[i] = index - 1;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState-1))){
                    possible_moves[i] = index - 1;
                    i++;
                }
            }
            // move one down
            if(index + 8 < 64){
                if(*(board->gameState+8) == 0){
                    possible_moves[i] = index + 8;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState+8))){
                    possible_moves[i] = index + 8;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState+8))){
                    possible_moves[i] = index + 8;
                    i++;
                }
            }
            // move one up
            if(index - 8 > 0){
                if(*(board->gameState-8) == 0){
                    possible_moves[i] = index - 8;
                } else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState-8))){
                    possible_moves[i] = index - 8;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState-8))){
                    possible_moves[i] = index - 8;
                }
            }
            break;

        case 'Q': // is a mix of rook and bishop
        case 'q':
        case 'R':
        case 'r':
            // moving to the right (whites perspective)
            k = 1;
            while(index + k < 64  && *(board->gameState+k) == 0 && (index+k)/8 == current_row){
                possible_moves[i] = index+k;
                i++;
                k++;
            }
            // check for taking a piece
            if(is_white_piece(chessPiece)){
                if(index + k < 64  && is_black_piece(*(board->gameState+k)) && (index+k)/8 == current_row){
                    possible_moves[i] = index+k;
                    i++;
                }
            }
            else if(index + k < 64  && is_white_piece(*(board->gameState+k)) && (index+k)/8 == current_row){
                possible_moves[i] = index+k;
                i++;
            }

            // moving to the left (whites perspective)
            k = 1;
            while(index - k > 0 && *(board->gameState-k) == 0 && (index-k)/8 == current_row){
                possible_moves[i] = index-k;
                i++;
                k--;
            }
            // check for taking a piece
            if(is_white_piece(chessPiece)){
                if(index - k > 0 && is_black_piece(*(board->gameState-k)) && (index-k)/8 == current_row){
                    possible_moves[i] = index-k;
                    i++;
                }
            }
            else if(index - k > 0 && is_white_piece(*(board->gameState-k)) && (index-k)/8 == current_row){
                possible_moves[i] = index-k;
                i++;
            }

            // moving down (whites perspective)
            k = 8;
            while(index + k < 64  && *(board->gameState+k) == 0){
                possible_moves[i] = index+k;
                i++;
                k +=8;
            }
            // check for taking a piece
            if(is_white_piece(chessPiece)){
                if(index + k < 64  && is_black_piece(*(board->gameState+k))){
                    possible_moves[i] = index+k;
                    i++;
                }
            }
            else if(index + k < 64  && is_white_piece(*(board->gameState+k))){
                possible_moves[i] = index+k;
                i++;
            }
            // moving up (whites perspective)
            k = 8;
            while(index - k > 0  && *(board->gameState-k) == 0){
                possible_moves[i] = index-k;
                i++;
                k -=8;
            }
            // check for taking a piece
            if(is_white_piece(chessPiece)){
                if(index - k > 0  && is_black_piece(*(board->gameState-k))){
                    possible_moves[i] = index-k;
                    i++;
                }
            }
            else if(index - k > 0  && is_white_piece(*(board->gameState-k))){
                    possible_moves[i] = index-k;
                    i++;
            }
            // check for fallthrough for queen
            if(chessPiece == 'r' || chessPiece == 'R'){ // causes some warnings witch prevent compilation
                break;
            }
        case 'B':
        case 'b':
            // moving to the lower right (whites perspective)
            k = 9;
            while(index + k < 64  && *(board->gameState+k) == 0 && ((index+k)/8)-(k/9) == current_row){
                possible_moves[i] = index+k;
                i++;
                k += 9;
            }
            // check for taking a piece
            if(is_white_piece(chessPiece)){
                if(index + k < 64  && is_black_piece(*(board->gameState+k)) && ((index+k)/8)-(k/9) == current_row){
                    possible_moves[i] = index+k;
                    i++;
                }
            }
            else if(index + k < 64  && is_white_piece(*(board->gameState+k)) && ((index+k)/8)-(k/9) == current_row){
                possible_moves[i] = index+k;
                i++;
            }

            // moving to the upper left (whites perspective)
            k = 9;
            while(index - k > 0 && *(board->gameState-k) == 0 && ((index-k)/8)+(k/9) == current_row){
                possible_moves[i] = index-k;
                i++;
                k -= 9;
            }
            // check for taking a piece
            if(is_white_piece(chessPiece)){
                if(index - k > 0 && is_black_piece(*(board->gameState-k)) && ((index-k)/8)+(k/9) == current_row){
                    possible_moves[i] = index-k;
                    i++;
                }
            }
            else if(index - k > 0 && is_white_piece(*(board->gameState-k)) && ((index-k)/8)+(k/9) == current_row){
                possible_moves[i] = index-k;
                i++;
            }

            // moving lower left (whites perspective)
            k = 7;
            while(index + k < 64  && *(board->gameState+k) == 0 && ((index+k)/8)-(k/9) == current_row){
                possible_moves[i] = index+k;
                i++;
                k += 7;
            }
            // check for taking a piece
            if(is_white_piece(chessPiece)){
                if(index + k < 64  && is_black_piece(*(board->gameState+k)) && ((index+k)/8)-(k/9) == current_row){
                    possible_moves[i] = index+k;
                    i++;
                }
            }
            else if(index + k < 64  && is_white_piece(*(board->gameState+k)) && ((index+k)/8)-(k/9) == current_row){
                possible_moves[i] = index+k;
                i++;
            }
            // moving upper right (whites perspective)
            k = 7;
            while(index - k > 0  && *(board->gameState-k) == 0 && ((index-k)/8)+(k/9) == current_row){
                possible_moves[i] = index-k;
                i++;
                k -=7;
            }
            // check for taking a piece
            if(is_white_piece(chessPiece)){
                if(index - k > 0  && is_black_piece(*(board->gameState-k)) && ((index-k)/8)+(k/9) == current_row){
                    possible_moves[i] = index-k;
                }
            }
            else if(index - k > 0  && is_white_piece(*(board->gameState-k)) && ((index-k)/8)+(k/9) == current_row){
                possible_moves[i] = index-k;
             }
            break;
        case 'N':
        case 'n':
            // lower right (+17) (+10)
            if(index + 17 < 64  && ((index+17)/8)-2 == current_row){
                if(*(board->gameState+17) == 0){
                    possible_moves[i] = index + 17;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState+17))){
                    possible_moves[i] = index + 17;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState+17))){
                    possible_moves[i] = index + 17;
                    i++;
                }
            }
            if(index + 10 < 64  && ((index+10)/8)-1 == current_row){
                if(*(board->gameState+10) == 0){
                    possible_moves[i] = index + 10;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState+10))){
                    possible_moves[i] = index + 10;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState+10))){
                    possible_moves[i] = index + 10;
                    i++;
                }
            }
            // lower left (+15) (+6)
            if(index + 15 < 64  && ((index+15)/8)-2 == current_row){
                if(*(board->gameState+15) == 0){
                    possible_moves[i] = index + 15;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState+15))){
                    possible_moves[i] = index + 15;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState+15))){
                    possible_moves[i] = index + 15;
                    i++;
                }
            }
            if(index + 6 < 64  && ((index+6)/8)-1 == current_row){
                if(*(board->gameState+6) == 0){
                    possible_moves[i] = index + 6;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState+6))){
                    possible_moves[i] = index + 6;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState+6))){
                    possible_moves[i] = index + 6;
                    i++;
                }
            }
            // upper left (-17) (-10)
            if(index + 17 > 0 && ((index-17)/8)+2 == current_row){
                if(*(board->gameState-17) == 0){
                    possible_moves[i] = index - 17;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState-17))){
                    possible_moves[i] = index - 17;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState-17))){
                    possible_moves[i] = index - 17;
                    i++;
                }
            }
            if(index - 10 > 0  && ((index-10)/8)+1 == current_row){
                if(*(board->gameState-10) == 0){
                    possible_moves[i] = index - 10;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState-10))){
                    possible_moves[i] = index - 10;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState-10))){
                    possible_moves[i] = index - 10;
                    i++;
                }
            }
            // upper right (-15) (-6)
            if(index - 15 > 0  && ((index-15)/8)+2 == current_row){
                if(*(board->gameState-15) == 0){
                    possible_moves[i] = index - 15;
                    i++;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState-15))){
                    possible_moves[i] = index - 15;
                    i++;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState-15))){
                    possible_moves[i] = index - 15;
                    i++;
                }
            }
            if(index - 6 > 0  && ((index-6)/8)+1 == current_row){
                if(*(board->gameState-6) == 0){
                    possible_moves[i] = index - 6;
                }
                else if(is_white_piece(chessPiece) && is_black_piece(*(board->gameState-6))){
                    possible_moves[i] = index - 6;
                }
                else if(is_black_piece(chessPiece) && is_white_piece(*(board->gameState-6))){
                    possible_moves[i] = index - 6;
                }
            }
            break;
        case 'p':
            // normal move
            if(*(board->gameState+index +  8) == 0){
                possible_moves[i] = index + 8;
                i++;
                // two squares at once (only if piece is at its starting point)
                if(*(board->gameState+index +  16) == 0 && index > 7 && index < 16){
                    possible_moves[i] = index + 16;
                    i++;
                }
            }

            // take a white piece on the left (whites perspective)
            if(is_white_piece(*(board->gameState + (index+7))) && ((index+7)/8)-1 == current_row){
                possible_moves[i] = index + 7;
                i++;
            }
            // take a white piece on the right (whites perspective)
            if(is_white_piece(*(board->gameState + (index+9))) && ((index+9)/8)-1 == current_row){
                possible_moves[i] = index + 9;
                i++;
            }

            // en passant check
            if((index+1 == board->en_passant && ((index+1)/8) == current_row) || (index-1 == board->en_passant && ((index-1)/8) == current_row)){
                possible_moves[i] = board->en_passant+8;
            }
            break;
        case 'P':
            // normal move
            if(*(board->gameState+index -  8) == 0){
                possible_moves[i] = index - 8;
                i++;
                // two squares at once (only if piece is at its starting point)
                if(*(board->gameState+index -  16) == 0 && index > 47 && index < 56){
                    possible_moves[i] = index - 16;
                    i++;
                }
            }
            // take a black piece on the right (whites perspective)
            if(is_black_piece(*(board->gameState + (index-7))) && ((index-7)/8)+1 == current_row){
                possible_moves[i] = index - 7;
                i++;
            }
            // take a black piece on the left (whites perspective)
            if(is_black_piece(*(board->gameState + (index-9))) && ((index-9)/8)+1 == current_row){
                possible_moves[i] = index - 9;
                i++;
            }
            // en passant check
            if((index+1 == board->en_passant && ((index+1)/8) == current_row) || (index-1 == board->en_passant && ((index-1)/8) == current_row)){
                possible_moves[i] = board->en_passant-8;
            }
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
