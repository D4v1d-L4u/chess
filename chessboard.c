#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chessboard.h"
#include "stringbuilder.h"

char convert_AlgebraicNotation_in_index(char algebraicNotation[3]) {
    char output;
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
void set_gameState(struct Chessboard *board, char *fen){
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
            case 'K':
                board->w_king_position = i;
                board->gameState[i] = *current;
                i++;
                break;
            case 'k':
                board->b_king_position = i;
                board->gameState[i] = *current;
                i++;
                break;
            case 'Q':
            case 'q':
            case 'R':
            case 'r':
            case 'B':
            case 'b':
            case 'N':
            case 'n':
            case 'P':
            case 'p':
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
char* get_FEN(struct Chessboard *board){ // max length  87
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

void init_board(struct Chessboard *board){
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


// 1 if white is in check 0 if not
char w_is_in_check(struct Chessboard *board){
    char king_position = board->w_king_position;
    int current_row = king_position/8;
    // checking for pawns
    if((*(board->gameState + (king_position-7))) == 'p' && ((king_position-7)/8)+1 == current_row){
        return 1;
    }
    if((*(board->gameState + (king_position-9))) == 'p' && ((king_position-9)/8)+1 == current_row){
        return 1;
    }

    // checking for knights
    // lower right (+17) (+10)
    if(king_position + 17 < 64  && ((king_position+17)/8)-2 == current_row && (*(board->gameState+king_position+17)) == 'n'){
        return 1;
    }
    if(king_position + 10 < 64  && ((king_position+10)/8)-1 == current_row && (*(board->gameState+king_position+10)) == 'n'){
        return 1;
    }
    // lower left (+15) (+6)
    if(king_position + 15 < 64  && ((king_position+15)/8)-2 == current_row && (*(board->gameState+king_position+15)) == 'n'){
        return 1;
    }
    if(king_position + 6 < 64  && ((king_position+6)/8)-1 == current_row && (*(board->gameState+king_position+6)) == 'n'){
        return 1;
    }
    // upper left (-17) (-10)
    if(king_position + 17 > 0 && ((king_position-17)/8)+2 == current_row &&  (*(board->gameState+king_position-17)) == 'n'){
        return 1;
    }
    if(king_position - 10 > 0  && ((king_position-10)/8)+1 == current_row && (*(board->gameState+king_position-10)) == 'n'){
        return 1;
    }
    // upper right (-15) (-6)
    if(king_position - 15 > 0  && ((king_position-15)/8)+2 == current_row && (*(board->gameState+king_position-15)) == 'n'){
        return 1;
    }
    if(king_position - 6 > 0  && ((king_position-6)/8)+1 == current_row && (*(board->gameState+king_position-6)) == 'n'){
        return 1;
    }

    // checking for rooks (and queens)
    char k;
    // moving to the right (whites perspective)
    k = 1;
    while(king_position + k < 64  && *(board->gameState+king_position+k) == 0 && (king_position+k)/8 == current_row){
        k++;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'r' && (king_position+k)/8 == current_row){
        return 1;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'q' && (king_position+k)/8 == current_row){
        return 1;
    }

    // moving to the left (whites perspective)
    k = 1;
    while(king_position - k > 0 && *(board->gameState+king_position-k) == 0 && (king_position-k)/8 == current_row){
        k++;
    }
    if(king_position - k > 0 && (*(board->gameState+king_position-k)) == 'r' && (king_position-k)/8 == current_row){
        return 1;
    }
    if(king_position - k > 0 && (*(board->gameState+king_position-k)) == 'q' && (king_position-k)/8 == current_row){
        return 1;
    }

    // moving down (whites perspective)
    k = 8;
    while(king_position + k < 64  && *(board->gameState+king_position+k) == 0){
        k +=8;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'r'){
        return 1;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'q'){
        return 1;
    }
    // moving up (whites perspective)
    k = 8;
    while(king_position - k > 0  && *(board->gameState+king_position-k) == 0){

        k +=8;
    }
    if(king_position - k > 0  && (*(board->gameState+king_position-k)) == 'r'){
        return 1;
    }
    if(king_position - k > 0  && (*(board->gameState+king_position-k)) == 'q'){
        return 1;
    }

    // checking for bishops (and queens)
    // moving to the lower right (whites perspective)
    k = 9;
    while(king_position + k < 64  && *(board->gameState+king_position+k) == 0 && ((king_position+k)/8)-(k/9) == current_row){
        k += 9;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'b' && ((king_position+k)/8)-(k/9) == current_row){
        return 1;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'q' && ((king_position+k)/8)-(k/9) == current_row){
        return 1;
    }

    // moving to the upper left (whites perspective)
    k = 9;
    while(king_position - k > 0 && *(board->gameState+king_position-k) == 0 && ((king_position-k)/8)+(k/9) == current_row){
        k += 9;
    }
    if(king_position - k > 0 && (*(board->gameState+king_position-k)) == 'b' && ((king_position-k)/8)+(k/9) == current_row){
        return 1;
    }
    if(king_position - k > 0 && (*(board->gameState+king_position-k)) == 'q' && ((king_position-k)/8)+(k/9) == current_row){
        return 1;
    }

    // moving lower left (whites perspective)
    k = 7;
    while(king_position + k < 64  && *(board->gameState+king_position+k) == 0 && ((king_position+k)/8)-(k/7) == current_row){
        k += 7;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'b' && ((king_position+k)/8)-(k/7) == current_row){
        return 1;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'q' && ((king_position+k)/8)-(k/7) == current_row){
        return 1;
    }

    // moving upper right (whites perspective)
    k = 7;
    while(king_position - k > 0  && *(board->gameState+king_position-k) == 0 && ((king_position-k)/8)+(k/7) == current_row){
        k +=7;
    }
    if(king_position - k > 0  && (*(board->gameState+king_position-k)) == 'b' && ((king_position-k)/8)+(k/7) == current_row){
        return 1;
    }
    if(king_position - k > 0  && (*(board->gameState+king_position-k)) == 'q' && ((king_position-k)/8)+(k/7) == current_row){
        return 1;
    }

    // checking for king
    // move one right
    if(king_position + 1 < 64  && (king_position+1)/8 == current_row && (*(board->gameState+king_position+1)) == 'k'){
        return 1;
    }
    // move one left
    if(king_position - 1 > 0  && (king_position-1)/8 == current_row && (*(board->gameState+king_position-1)) == 'k'){
        return 1;
    }
    // move one down
    if(king_position + 8 < 64 && (*(board->gameState+king_position+8)) == 'k'){
        return 1;
    }
    // move one up
    if(king_position - 8 > 0 && (*(board->gameState+king_position-8)) == 'k'){
        return 1;
    }
    // move upper left
    if(king_position - 9 > 0 && ((king_position-9)/8)+1 == current_row && (*(board->gameState+king_position-9)) == 'k'){
        return 1;
    }
    // move upper right
    if(king_position - 7 > 0 && ((king_position-7)/8)+1 == current_row && (*(board->gameState+king_position-7)) == 'k'){
        return 1;
    }
    // move lower left
    if(king_position + 7 < 64 && ((king_position+7)/8)-1 == current_row && (*(board->gameState+king_position+7)) == 'k'){
        return 1;
    }
    // move lower right
    if(king_position + 9 < 64 && ((king_position+9)/8)-1 == current_row && (*(board->gameState+king_position+9)) == 'k'){
        return 1;
    }
    return 0;
}
char b_is_in_check(struct Chessboard *board){
    char king_position = board->b_king_position;
    int current_row = king_position/8;
    // checking for pawns
    // take a white piece on the left (whites perspective)
    if((*(board->gameState + (king_position+7)) == 'P') && ((king_position+7)/8)-1 == current_row){
        return 1;
    }
    // take a white piece on the right (whites perspective)
    if((*(board->gameState + (king_position+9)) == 'P') && ((king_position+9)/8)-1 == current_row){
        return 1;
    }

    // checking for knights
    // lower right (+17) (+10)
    if(king_position + 17 < 64  && ((king_position+17)/8)-2 == current_row && (*(board->gameState+king_position+17)) == 'N'){
        return 1;
    }
    if(king_position + 10 < 64  && ((king_position+10)/8)-1 == current_row && (*(board->gameState+king_position+10)) == 'N'){
        return 1;
    }
    // lower left (+15) (+6)
    if(king_position + 15 < 64  && ((king_position+15)/8)-2 == current_row && (*(board->gameState+king_position+15)) == 'N'){
        return 1;
    }
    if(king_position + 6 < 64  && ((king_position+6)/8)-1 == current_row && (*(board->gameState+king_position+6)) == 'N'){
        return 1;
    }
    // upper left (-17) (-10)
    if(king_position + 17 > 0 && ((king_position-17)/8)+2 == current_row &&  (*(board->gameState+king_position-17)) == 'N'){
        return 1;
    }
    if(king_position - 10 > 0  && ((king_position-10)/8)+1 == current_row && (*(board->gameState+king_position-10)) == 'N'){
        return 1;
    }
    // upper right (-15) (-6)
    if(king_position - 15 > 0  && ((king_position-15)/8)+2 == current_row && (*(board->gameState+king_position-15)) == 'N'){
        return 1;
    }
    if(king_position - 6 > 0  && ((king_position-6)/8)+1 == current_row && (*(board->gameState+king_position-6)) == 'N'){
        return 1;
    }

    // checking for rooks (and queens)
    char k;
    // moving to the right (whites perspective)
    k = 1;
    while(king_position + k < 64  && *(board->gameState+king_position+k) == 0 && (king_position+k)/8 == current_row){
        k++;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'R' && (king_position+k)/8 == current_row){
        return 1;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'Q' && (king_position+k)/8 == current_row){
        return 1;
    }

    // moving to the left (whites perspective)
    k = 1;
    while(king_position - k > 0 && *(board->gameState+king_position-k) == 0 && (king_position-k)/8 == current_row){
        k++;
    }
    if(king_position - k > 0 && (*(board->gameState+king_position-k)) == 'R' && (king_position-k)/8 == current_row){
        return 1;
    }
    if(king_position - k > 0 && (*(board->gameState+king_position-k)) == 'Q' && (king_position-k)/8 == current_row){
        return 1;
    }

    // moving down (whites perspective)
    k = 8;
    while(king_position + k < 64  && *(board->gameState+king_position+k) == 0){
        k +=8;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'R'){
        return 1;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'Q'){
        return 1;
    }
    // moving up (whites perspective)
    k = 8;
    while(king_position - k > 0  && *(board->gameState+king_position-k) == 0){

        k +=8;
    }
    if(king_position - k > 0  && (*(board->gameState+king_position-k)) == 'R'){
        return 1;
    }
    if(king_position - k > 0  && (*(board->gameState+king_position-k)) == 'Q'){
        return 1;
    }

    // checking for bishops (and queens)
    // moving to the lower right (whites perspective)
    k = 9;
    while(king_position + k < 64  && *(board->gameState+king_position+k) == 0 && ((king_position+k)/8)-(k/9) == current_row){
        k += 9;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'B' && ((king_position+k)/8)-(k/9) == current_row){
        return 1;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'Q' && ((king_position+k)/8)-(k/9) == current_row){
        return 1;
    }

    // moving to the upper left (whites perspective)
    k = 9;
    while(king_position - k > 0 && *(board->gameState+king_position-k) == 0 && ((king_position-k)/8)+(k/9) == current_row){
        k += 9;
    }
    if(king_position - k > 0 && (*(board->gameState+king_position-k)) == 'B' && ((king_position-k)/8)+(k/9) == current_row){
        return 1;
    }
    if(king_position - k > 0 && (*(board->gameState+king_position-k)) == 'Q' && ((king_position-k)/8)+(k/9) == current_row){
        return 1;
    }

    // moving lower left (whites perspective)
    k = 7;
    while(king_position + k < 64  && *(board->gameState+king_position+k) == 0 && ((king_position+k)/8)-(k/7) == current_row){
        k += 7;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'B' && ((king_position+k)/8)-(k/7) == current_row){
        return 1;
    }
    if(king_position + k < 64  && (*(board->gameState+king_position+k)) == 'Q' && ((king_position+k)/8)-(k/7) == current_row){
        return 1;
    }

    // moving upper right (whites perspective)
    k = 7;
    while(king_position - k > 0  && *(board->gameState+king_position-k) == 0 && ((king_position-k)/8)+(k/7) == current_row){
        k +=7;
    }
    if(king_position - k > 0  && (*(board->gameState+king_position-k)) == 'B' && ((king_position-k)/8)+(k/7) == current_row){
        return 1;
    }
    if(king_position - k > 0  && (*(board->gameState+king_position-k)) == 'Q' && ((king_position-k)/8)+(k/7) == current_row){
        return 1;
    }

    // checking for king
    // move one right
    if(king_position + 1 < 64  && (king_position+1)/8 == current_row && (*(board->gameState+king_position+1)) == 'K'){
        return 1;
    }
    // move one left
    if(king_position - 1 > 0  && (king_position-1)/8 == current_row && (*(board->gameState+king_position-1)) == 'K'){
        return 1;
    }
    // move one down
    if(king_position + 8 < 64 && (*(board->gameState+king_position+8)) == 'K'){
        return 1;
    }
    // move one up
    if(king_position - 8 > 0 && (*(board->gameState+king_position-8)) == 'K'){
        return 1;
    }
    // move upper left
    if(king_position - 9 > 0 && ((king_position-9)/8)+1 == current_row && (*(board->gameState+king_position-9)) == 'K'){
        return 1;
    }
    // move upper right
    if(king_position - 7 > 0 && ((king_position-7)/8)+1 == current_row && (*(board->gameState+king_position-7)) == 'K'){
        return 1;
    }
    // move lower left
    if(king_position + 7 < 64 && ((king_position+7)/8)-1 == current_row && (*(board->gameState+king_position+7)) == 'K'){
        return 1;
    }
    // move lower right
    if(king_position + 9 < 64 && ((king_position+9)/8)-1 == current_row && (*(board->gameState+king_position+9)) == 'K'){
        return 1;
    }
    return 0;
}

// will return 1 if the move will put yourself in check
char will_be_in_check(struct Chessboard *board, char from, char to){
    if(*(board->gameState+from) == 0){
        return 0;
    }
    // handle movement of the king
    if(*(board->gameState+from) == 'K'){
        board->w_king_position = to;
    }
    if(*(board->gameState+from) == 'k'){
        board->b_king_position = to;
    }
    // handle en passant
    char en_passant_pawn_type = *(board->gameState+board->en_passant);
    if(*(board->gameState+from) == 'P' && to+8 == board->en_passant){
        *(board->gameState+board->en_passant) = 0;
    }
    if(*(board->gameState+from) == 'p' && to-8 == board->en_passant){
        *(board->gameState+board->en_passant) = 0;
    }
    char tmp = *(board->gameState+to);
    char output;
    *(board->gameState+to) = *(board->gameState+from);
    *(board->gameState+from) = 0;
    if(is_white_piece(*(board->gameState+to))){
        output = w_is_in_check(board);
    } else{
        output = b_is_in_check(board);
    }
    if(*(board->gameState+to) == 'K'){
        board->w_king_position = from;
    }
    if(*(board->gameState+to) == 'k'){
        board->b_king_position = from;
    }
    *(board->gameState+board->en_passant) = en_passant_pawn_type;

    *(board->gameState+from) = *(board->gameState+to);
    *(board->gameState+to) = tmp;
    return output;

    return 0;
}

// checks if white is in checkmate (returns 1 if it is checkmate else 0)
char w_is_checkmate(struct Chessboard *board){
    for(char i = 0; i < 63; i++){
        if(is_white_piece(*(board->gameState+i))){
            char* possible_moves;
            possible_moves = get_possible_moves(board, i);
            if(possible_moves[0] != 0){
                free(possible_moves);
                return 0;
            }
            free(possible_moves);
        }
    }
    return 1;
}
// checks if black is in checkmate (returns 1 if it is checkmate else 0)
char b_is_checkmate(struct Chessboard *board){
    for(char i = 0; i < 63; i++){
        if(is_black_piece(*(board->gameState+i))){
            char* possible_moves;
            possible_moves = get_possible_moves(board, i);
            if(possible_moves[0] != 0){
                free(possible_moves);
                return 0;
            }
            free(possible_moves);
        }
    }
    return 1;
}

void get_w_pawn_moves(char *possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;
    // normal move
    if(*(board->gameState+index -  8) == 0){
        if(!will_be_in_check(board, index, index-8)){
            possible_moves[i] = index - 8;
            i++;
        }
        // two squares at once (only if piece is at its starting point)
        if(*(board->gameState+index -  16) == 0 && index > 47 && index < 56){
            if(!will_be_in_check(board, index, index-16)){
                possible_moves[i] = index - 16;
                i++;
            }
        }
    }
    // take a black piece on the right (whites perspective)
    if(is_black_piece(*(board->gameState + (index-7))) && ((index-7)/8)+1 == current_row){
        if(!will_be_in_check(board, index, index - 7)){
            possible_moves[i] = index - 7;
            i++;
        }
    }
    // take a black piece on the left (whites perspective)
    if(is_black_piece(*(board->gameState + (index-9))) && ((index-9)/8)+1 == current_row){
        if(!will_be_in_check(board, index, index-9)){
            possible_moves[i] = index - 9;
            i++;
        }
    }
    // en passant check
    if((index+1 == board->en_passant && ((index+1)/8) == current_row) || (index-1 == board->en_passant && ((index-1)/8) == current_row)){
        if(!will_be_in_check(board, index, index-8)){
            possible_moves[i] = board->en_passant-8;
        }
    }
}
void get_b_pawn_moves(char *possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;
    // normal move
    if(*(board->gameState+index +  8) == 0){
        if(!will_be_in_check(board, index, index+8)){
            possible_moves[i] = index + 8;
            i++;
        }
        // two squares at once (only if piece is at its starting point)
        if(*(board->gameState+index +  16) == 0 && index > 7 && index < 16){
            if(!will_be_in_check(board, index, index+16)){
                possible_moves[i] = index + 16;
                i++;
            }
        }
    }

    // take a white piece on the left (whites perspective)
    if(is_white_piece(*(board->gameState + (index+7))) && ((index+7)/8)-1 == current_row){
        if(!will_be_in_check(board, index, index+7)){
            possible_moves[i] = index + 7;
            i++;
        }
    }
    // take a white piece on the right (whites perspective)
    if(is_white_piece(*(board->gameState + (index+9))) && ((index+9)/8)-1 == current_row){
        if(!will_be_in_check(board, index, index+9)){
            possible_moves[i] = index + 9;
            i++;
        }
    }

    // en passant check
    if((index+1 == board->en_passant && ((index+1)/8) == current_row) || (index-1 == board->en_passant && ((index-1)/8) == current_row)){
        if(!will_be_in_check(board, index, board->en_passant+8)){
            possible_moves[i] = board->en_passant+8;
        }
    }
}

void get_w_knight_moves(char *possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;
    // lower right (+17) (+10)
    if(index + 17 < 64  && ((index+17)/8)-2 == current_row && !will_be_in_check(board, index, index+17)){
        if(*(board->gameState+index+17) == 0){
            possible_moves[i] = index + 17;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index+17))){
            possible_moves[i] = index + 17;
            i++;
        }
    }
    if(index + 10 < 64  && ((index+10)/8)-1 == current_row && !will_be_in_check(board, index, index+10)){
        if(*(board->gameState+index+10) == 0){
            possible_moves[i] = index + 10;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index+10))){
            possible_moves[i] = index + 10;
            i++;
        }
    }
    // lower left (+15) (+6)
    if(index + 15 < 64  && ((index+15)/8)-2 == current_row && !will_be_in_check(board, index, index+15)){
        if(*(board->gameState+index+15) == 0){
            possible_moves[i] = index + 15;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index+15))){
            possible_moves[i] = index + 15;
            i++;
        }
    }
    if(index + 6 < 64  && ((index+6)/8)-1 == current_row && !will_be_in_check(board, index, index+6)){
        if(*(board->gameState+index+6) == 0){
            possible_moves[i] = index + 6;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index+6))){
            possible_moves[i] = index + 6;
            i++;
        }
    }
    // upper left (-17) (-10)
    if(index + 17 > 0 && ((index-17)/8)+2 == current_row && !will_be_in_check(board, index, index-17)){
        if(*(board->gameState+index-17) == 0){
            possible_moves[i] = index - 17;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index-17))){
            possible_moves[i] = index - 17;
            i++;
        }
    }
    if(index - 10 > 0  && ((index-10)/8)+1 == current_row && !will_be_in_check(board, index, index-10)){
        if(*(board->gameState+index-10) == 0){
            possible_moves[i] = index - 10;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index-10))){
            possible_moves[i] = index - 10;
            i++;
        }
    }
    // upper right (-15) (-6)
    if(index - 15 > 0  && ((index-15)/8)+2 == current_row && !will_be_in_check(board, index, index-15)){
        if(*(board->gameState+index-15) == 0){
            possible_moves[i] = index - 15;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index-15))){
            possible_moves[i] = index - 15;
            i++;
        }
    }
    if(index - 6 > 0  && ((index-6)/8)+1 == current_row && !will_be_in_check(board, index, index)-6){
        if(*(board->gameState+index-6) == 0){
            possible_moves[i] = index - 6;
        }
        else if(is_black_piece(*(board->gameState+index-6))){
            possible_moves[i] = index - 6;
        }
    }
}
void get_b_knight_moves(char *possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;
    // lower right (+17) (+10)
    if(index + 17 < 64  && ((index+17)/8)-2 == current_row && !will_be_in_check(board, index, index+17)){
        if(*(board->gameState+index+17) == 0){
            possible_moves[i] = index + 17;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index+17))){
            possible_moves[i] = index + 17;
            i++;
        }
    }
    if(index + 10 < 64  && ((index+10)/8)-1 == current_row && !will_be_in_check(board, index, index+10)){
        if(*(board->gameState+index+10) == 0){
            possible_moves[i] = index + 10;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index+10))){
            possible_moves[i] = index + 10;
            i++;
        }
    }
    // lower left (+15) (+6)
    if(index + 15 < 64  && ((index+15)/8)-2 == current_row && !will_be_in_check(board, index, index+15)){
        if(*(board->gameState+index+15) == 0){
            possible_moves[i] = index + 15;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index+15))){
            possible_moves[i] = index + 15;
            i++;
        }
    }
    if(index + 6 < 64  && ((index+6)/8)-1 == current_row && !will_be_in_check(board, index, index+6)){
        if(*(board->gameState+index+6) == 0){
            possible_moves[i] = index + 6;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index+6))){
            possible_moves[i] = index + 6;
            i++;
        }
    }
    // upper left (-17) (-10)
    if(index + 17 > 0 && ((index-17)/8)+2 == current_row && !will_be_in_check(board, index, index-17)){
        if(*(board->gameState+index-17) == 0){
            possible_moves[i] = index - 17;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index-17))){
            possible_moves[i] = index - 17;
            i++;
        }
    }
    if(index - 10 > 0  && ((index-10)/8)+1 == current_row && !will_be_in_check(board, index, index-10)){
        if(*(board->gameState+index-10) == 0){
            possible_moves[i] = index - 10;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index-10))){
            possible_moves[i] = index - 10;
            i++;
        }
    }
    // upper right (-15) (-6)
    if(index - 15 > 0  && ((index-15)/8)+2 == current_row && !will_be_in_check(board, index, index-15)){
        if(*(board->gameState+index-15) == 0){
            possible_moves[i] = index - 15;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index-15))){
            possible_moves[i] = index - 15;
            i++;
        }
    }
    if(index - 6 > 0  && ((index-6)/8)+1 == current_row && !will_be_in_check(board, index, index-6)){
        if(*(board->gameState+index-6) == 0){
            possible_moves[i] = index - 6;
        }
        else if(is_white_piece(*(board->gameState+index-6))){
            possible_moves[i] = index - 6;
        }
    }
}

void get_w_rook_moves(char *possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;
    char k;
    // moving to the right (whites perspective)
    k = 1;
    while(index + k < 64  && *(board->gameState+index+k) == 0 && (index+k)/8 == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
        k++;
    }
    // check for taking a piece
    if(index + k < 64  && is_black_piece(*(board->gameState+index+k)) && (index+k)/8 == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
    }

    // moving to the left (whites perspective)
    k = 1;
    while(index - k > 0 && *(board->gameState+index-k) == 0 && (index-k)/8 == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
        k++;
    }
    // check for taking a piece
    if(index - k > 0 && is_black_piece(*(board->gameState+index-k)) && (index-k)/8 == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
    }


    // moving down (whites perspective)
    k = 8;
    while(index + k < 64  && *(board->gameState+index+k) == 0){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
        k +=8;
    }
    // check for taking a piece
    if(index + k < 64  && is_black_piece(*(board->gameState+index+k))){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
    }

    // moving up (whites perspective)
    k = 8;
    while(index - k > 0  && *(board->gameState+index-k) == 0){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
        k +=8;
    }
    // check for taking a piece
    if(index - k > 0  && is_black_piece(*(board->gameState+index-k))){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
        }
    }
}
void get_b_rook_moves(char* possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;
    char k;
    // moving to the right (whites perspective)
    k = 1;
    while(index + k < 64  && *(board->gameState+index+k) == 0 && (index+k)/8 == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
        k++;
    }
    // check for taking a piece
    if(index + k < 64  && is_white_piece(*(board->gameState+index+k)) && (index+k)/8 == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
    }

    // moving to the left (whites perspective)
    k = 1;
    while(index - k > 0 && *(board->gameState+index-k) == 0 && (index-k)/8 == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
        k++;
    }
    // check for taking a piece

    if(index - k > 0 && is_white_piece(*(board->gameState+index-k)) && (index-k)/8 == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
    }

    // moving down (whites perspective)
    k = 8;
    while(index + k < 64  && *(board->gameState+index+k) == 0){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
        k +=8;
    }
    // check for taking a piece
    if(index + k < 64  && is_white_piece(*(board->gameState+index+k))){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
    }
    // moving up (whites perspective)
    k = 8;
    while(index - k > 0  && *(board->gameState+index-k) == 0){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
        k +=8;
    }
    // check for taking a piece
    if(index - k > 0  && is_white_piece(*(board->gameState+index-k))){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
        }
    }
}

void get_w_bishop_moves(char *possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;
    char k;

    // moving to the lower right (whites perspective)
    k = 9;
    while(index + k < 64  && *(board->gameState+index+k) == 0 && ((index+k)/8)-(k/9) == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
        k += 9;
    }
    // check for taking a piece
    if(index + k < 64  && is_black_piece(*(board->gameState+index+k)) && ((index+k)/8)-(k/9) == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
    }


    // moving to the upper left (whites perspective)
    k = 9;
    while(index - k > 0 && *(board->gameState+index-k) == 0 && ((index-k)/8)+(k/9) == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
        k += 9;
    }
    // check for taking a piece
    if(index - k > 0 && is_black_piece(*(board->gameState+index-k)) && ((index-k)/8)+(k/9) == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
    }

    // moving lower left (whites perspective)
    k = 7;
    while(index + k < 64  && *(board->gameState+index+k) == 0 && ((index+k)/8)-(k/7) == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
        k += 7;
    }
    // check for taking a piece
    if(index + k < 64  && is_black_piece(*(board->gameState+index+k)) && ((index+k)/8)-(k/7) == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
    }
    // moving upper right (whites perspective)
    k = 7;
    while(index - k > 0  && *(board->gameState+index-k) == 0 && ((index-k)/8)+(k/7) == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
        k +=7;
    }
    // check for taking a piece
    if(index - k > 0  && is_black_piece(*(board->gameState+index-k)) && ((index-k)/8)+(k/7) == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
        }
    }
}
void get_b_bishop_moves(char *possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;
    char k;
    // moving to the lower right (whites perspective)
    k = 9;
    while(index + k < 64  && *(board->gameState+index+k) == 0 && ((index+k)/8)-(k/9) == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
        k += 9;
    }
    // check for taking a piece
    if(index + k < 64  && is_white_piece(*(board->gameState+index+k)) && ((index+k)/8)-(k/9) == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
    }

    // moving to the upper left (whites perspective)
    k = 9;
    while(index - k > 0 && *(board->gameState+index-k) == 0 && ((index-k)/8)+(k/9) == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
        k += 9;
    }
    // check for taking a piece
    if(index - k > 0 && is_white_piece(*(board->gameState+index-k)) && ((index-k)/8)+(k/9) == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
    }

    // moving lower left (whites perspective)
    k = 7;
    while(index + k < 64  && *(board->gameState+index+k) == 0 && ((index+k)/8)-(k/7) == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
        k += 7;
    }
    // check for taking a piece
    if(index + k < 64  && is_white_piece(*(board->gameState+index+k)) && ((index+k)/8)-(k/7) == current_row){
        if (!will_be_in_check(board, index, index+k)){
            possible_moves[i] = index+k;
            i++;
        }
    }
    // moving upper right (whites perspective)
    k = 7;
    while(index - k > 0  && *(board->gameState+index-k) == 0 && ((index-k)/8)+(k/7) == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
            i++;
        }
        k +=7;
    }
    // check for taking a piece
    if(index - k > 0  && is_white_piece(*(board->gameState+index-k)) && ((index-k)/8)+(k/7) == current_row){
        if (!will_be_in_check(board, index, index-k)){
            possible_moves[i] = index-k;
        }
    }
}

void get_w_queen_moves(char *possible_moves, struct Chessboard *board, char index){
    get_w_rook_moves(possible_moves, board, index);
    char tmp[15];
    get_w_bishop_moves((char *) &tmp, board, index);
    strcat(possible_moves, tmp);
}
void get_b_queen_moves(char *possible_moves, struct Chessboard *board, char index){
    get_b_rook_moves(possible_moves, board, index);
    char tmp[15];
    get_b_bishop_moves((char *) &tmp, board, index);
    strcat(possible_moves, tmp);
}

void get_w_king_moves(char* possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;

    // move one right
    if(index + 1 < 64  && (index+1)/8 == current_row && !will_be_in_check(board, index, index+1)){
        if(*(board->gameState+index+1) == 0){
            possible_moves[i] = index + 1;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index+1))){
            possible_moves[i] = index + 1;
            i++;
        }
    }
    // move one left
    if(index - 1 > 0  && (index-1)/8 == current_row && !will_be_in_check(board, index, index-1)){
        if(*(board->gameState+index-1) == 0){
            possible_moves[i] = index - 1;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index-1))){
            possible_moves[i] = index - 1;
            i++;
        }
    }
    // move one down
    if(index + 8 < 64 && !will_be_in_check(board, index, index+8)){
        if(*(board->gameState+index+8) == 0){
            possible_moves[i] = index + 8;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index+8))){
            possible_moves[i] = index + 8;
            i++;
        }
    }
    // move one up
    if(index - 8 > 0 && !will_be_in_check(board, index, index-8)){
        if(*(board->gameState+index-8) == 0){
            possible_moves[i] = index - 8;
            i++;
        } else if(is_black_piece(*(board->gameState+index-8))){
            possible_moves[i] = index - 8;
            i++;
        }
    }
    // move upper left
    if(index - 9 > 0 && ((index-9)/8)+1 == current_row && !will_be_in_check(board, index, index-9)){
        if(*(board->gameState+index-9) == 0){
            possible_moves[i] = index - 9;
            i++;
        } else if(is_black_piece(*(board->gameState+index-9))){
            possible_moves[i] = index - 9;
            i++;
        }
    }
    // move upper right
    if(index - 7 > 0 && ((index-7)/8)+1 == current_row && !will_be_in_check(board, index, index-7)){
        if(*(board->gameState+index-7) == 0){
            possible_moves[i] = index - 7;
            i++;
        } else if(is_black_piece(*(board->gameState+index-7))){
            possible_moves[i] = index - 7;
            i++;
        }
    }
    // move lower left
    if(index + 7 < 64 && ((index+7)/8)-1 == current_row && !will_be_in_check(board, index, index+7)){
        if(*(board->gameState+index+7) == 0){
            possible_moves[i] = index + 7;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index+7))){
            possible_moves[i] = index + 7;
            i++;
        }
    }
    // move lower right
    if(index + 9 < 64 && ((index+9)/8)-1 == current_row && !will_be_in_check(board, index, index+9)){
        if(*(board->gameState+index+9) == 0){
            possible_moves[i] = index + 9;
            i++;
        }
        else if(is_black_piece(*(board->gameState+index+9))){
            possible_moves[i] = index + 9;
            i++;
        }
    }
    // castling
    char k;
    // king-side castling
    if((board->castling & 0x1) && !w_is_in_check(board)){
        // moving to the right (whites perspective)
        k = 1;
        while(index + k < 64  && *(board->gameState+index+k) == 0 && (index+k)/8 == current_row){
            k++;
        }
        if(index + k < 64  && (*(board->gameState+index+k)) == 'R' && (index+k)/8 == current_row){
            if(!will_be_in_check(board, index, 62)){
                possible_moves[i] = 62;
                i++;
            }
        }
    }
    // queen-side castling
    if((board->castling & 0x2) && !w_is_in_check(board)){
        // moving to the left (whites perspective)
        k = 1;
        while(index - k > 0 && *(board->gameState+index-k) == 0 && (index-k)/8 == current_row){
            k++;
        }
        if(index - k > 0 && (*(board->gameState+index-k)) == 'R' && (index-k)/8 == current_row){
            if(!will_be_in_check(board, index, 58)){
                possible_moves[i] = 58;
            }
        }
    }


}
void get_b_king_moves(char* possible_moves, struct Chessboard *board, char index){
    int i = 0;
    char current_row = index/8;

    // move one right
    if(index + 1 < 64  && (index+1)/8 == current_row && !will_be_in_check(board, index, index+1)){
        if(*(board->gameState+index+1) == 0){
            possible_moves[i] = index + 1;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index+1))){
            possible_moves[i] = index + 1;
            i++;
        }
    }
    // move one left
    if(index - 1 > 0  && (index-1)/8 == current_row && !will_be_in_check(board, index, index-1)){
        if(*(board->gameState+index-1) == 0){
            possible_moves[i] = index - 1;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index-1))){
            possible_moves[i] = index - 1;
            i++;
        }
    }
    // move one down
    if(index + 8 < 64 && !will_be_in_check(board, index, index+8)){
        if(*(board->gameState+index+8) == 0){
            possible_moves[i] = index + 8;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index+8))){
            possible_moves[i] = index + 8;
            i++;
        }
    }
    // move one up
    if(index - 8 > 0 && !will_be_in_check(board, index, index-8)){
        if(*(board->gameState+index-8) == 0){
            possible_moves[i] = index - 8;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index-8))){
            possible_moves[i] = index - 8;
            i++;
        }
    }
    // move upper left
    if(index - 9 > 0 && ((index-9)/8)+1 == current_row && !will_be_in_check(board, index, index-9)){
        if(*(board->gameState+index-9) == 0){
            possible_moves[i] = index - 9;
            i++;
        } else if(is_white_piece(*(board->gameState+index-9))){
            possible_moves[i] = index - 9;
            i++;
        }
    }
    // move upper right
    if(index - 7 > 0 && ((index-7)/8)+1 == current_row && !will_be_in_check(board, index, index-7)){
        if(*(board->gameState+index-7) == 0){
            possible_moves[i] = index - 7;
            i++;
        } else if(is_white_piece(*(board->gameState+index-7))){
            possible_moves[i] = index - 7;
            i++;
        }
    }
    // move lower left
    if(index + 7 < 64 && ((index+7)/8)-1 == current_row && !will_be_in_check(board, index, index+7)){
        if(*(board->gameState+index+7) == 0){
            possible_moves[i] = index + 7;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index+7))){
            possible_moves[i] = index + 7;
            i++;
        }
    }
    // move lower right
    if(index + 9 < 64 && ((index+9)/8)-1 == current_row && !will_be_in_check(board, index, index+9)){
        if(*(board->gameState+index+9) == 0){
            possible_moves[i] = index + 9;
            i++;
        }
        else if(is_white_piece(*(board->gameState+index+9))){
            possible_moves[i] = index + 9;
            i++;
        }
    }
    // castling
    char k;
    // king-side castling
    if((board->castling & 0x4) && !b_is_in_check(board)){
        // moving to the right (whites perspective)
        k = 1;
        while(index + k < 64  && *(board->gameState+index+k) == 0 && (index+k)/8 == current_row){
            k++;
        }
        if(index + k < 64  && (*(board->gameState+index+k)) == 'r' && (index+k)/8 == current_row){
            if(!will_be_in_check(board, index, 6)){
                possible_moves[i] = 6;
                i++;
            }
        }
    }
    // queen-side castling
    if((board->castling & 0x8) && !b_is_in_check(board)){
        // moving to the left (whites perspective)
        k = 1;
        while(index - k > 0 && *(board->gameState+index-k) == 0 && (index-k)/8 == current_row){
            k++;
        }
        if(index - k > 0 && (*(board->gameState+index-k)) == 'r' && (index-k)/8 == current_row){
            if(!will_be_in_check(board, index, 2)){
                possible_moves[i] = 2;
            }
        }
    }
}

// TODO clean up
char* get_possible_moves (struct Chessboard *board,  char index){
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
    switch (chessPiece) {
        case 'K':
            get_w_king_moves(possible_moves, board, index);
            break;
        case 'k':
            get_b_king_moves(possible_moves, board, index);
            break;
        case 'Q':
            get_w_queen_moves(possible_moves, board, index);
            break;
        case 'q':
            get_b_queen_moves(possible_moves, board, index);
            break;
        case 'R':
            get_w_rook_moves(possible_moves, board, index);
            break;
        case 'r':
            get_b_rook_moves(possible_moves, board, index);
            break;
        case 'B':
            get_w_bishop_moves(possible_moves, board, index);
            break;
        case 'b':
            get_b_bishop_moves(possible_moves, board, index);
            break;
        case 'N':
            get_w_knight_moves(possible_moves, board, index);
            break;
        case 'n':
            get_b_knight_moves(possible_moves, board, index);
            break;
        case 'P':
            get_w_pawn_moves(possible_moves, board, index);
            break;
        case 'p':
            get_b_pawn_moves(possible_moves, board, index);
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

// TODO increase counter etc.
int quick_move(struct Chessboard *board, char *from, char *to){
    char from_index = convert_AlgebraicNotation_in_index(from);
    char to_index = convert_AlgebraicNotation_in_index(to);
    if(from_index == -1 || to_index == -1){
        return 1;
    }
    char *possible_moves = get_possible_moves(board, from_index);
    if(strchr(possible_moves, to_index) == NULL){
        return 1;
    }
    free(possible_moves);

    // handle movement of the king
    if(*(board->gameState+from_index) == 'K'){
        board->w_king_position = to_index;
        // king-side castling
        if(to_index == 62 && (board->castling & 0x1)){
            board->gameState[63] = 0;
            board->gameState[61] = 'R';
        }
        // queen-side castling
        else if(to_index == 58 && (board->castling & 0x2)){
            board->gameState[56] = 0;
            board->gameState[59] = 'R';
        }
    }
    if(*(board->gameState+from_index) == 'k'){
        board->b_king_position = to_index;
        // king-side castling
        if(to_index == 6 && (board->castling & 0x4)){
            board->gameState[7] = 0;
            board->gameState[5] = 'r';
        }
        // queen-side castling
        else if(to_index == 2 && (board->castling & 0x8)){
            board->gameState[0] = 0;
            board->gameState[3] = 'r';
        }
    }
    // set enpassant
    if(*(board->gameState+from_index) == 'P' && (from_index - to_index == 16)){
        board->en_passant = (char) (to_index + 8);
    }
    if(*(board->gameState+from_index) == 'p' && (to_index - from_index == 16)){
        board->en_passant = (char) (to_index - 8);
    }
        // handle en passant
    if(*(board->gameState+from_index) == 'P' && to_index+8 == board->en_passant){
        *(board->gameState+board->en_passant) = 0;
        board->en_passant = 0;
    }
    if(*(board->gameState+from_index) == 'p' && to_index-8 == board->en_passant){
        *(board->gameState+board->en_passant) = 0;
        board->en_passant = 0;
    }

    // the actual move
    *(board->gameState+to_index) = *(board->gameState+from_index);
    *(board->gameState+from_index) = 0;
    return 0;
}
