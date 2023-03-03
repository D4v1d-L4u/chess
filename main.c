#include <stdio.h>
#include <stdlib.h>
#include "chessboard.h"

void print_board(struct Chessboard chessboard) {
    char i = 0;
    while (i<64){
        if(i%8 == 0){
            printf("\n");
        }
        if(*(chessboard.gameState+i) == 0){
            printf("0 ");
        }else{
            printf("%c ", *(chessboard.gameState+i));
        }
        i++;
    }
    printf("\n");
}

int main(void){
    struct Chessboard chessboard;
    char* fen;
    init_board(&chessboard);
    print_board(chessboard);
    fen  =  get_FEN(&chessboard);
    printf("%s\n",fen);
    free(fen);

    set_gameState(&chessboard, "8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50");
    print_board(chessboard);
    fen  =  get_FEN(&chessboard);
    printf("%s\n",fen);
    free(fen);

    set_gameState(&chessboard, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    print_board(chessboard);
    fen  =  get_FEN(&chessboard);
    printf("%s\n",fen);
    free(fen);



    set_gameState(&chessboard, "rnbqk2r/pp2p1bp/2p3p1/3p1pP1/3P1Q2/2N2N2/PPP1PPP1/2KR1B1R b kq - 1 9");
    print_board(chessboard);
    fen  =  get_FEN(&chessboard);
    printf("%s\n",fen);
    free(fen);

    // idea for printing out the chessboard
    printf("\033[0m\033[41m"); // White foreground / Red background
    printf("Something\n");
    printf("\033[0m"); //Resets the text to default color.
    printf("AAAAAA\n");
}

