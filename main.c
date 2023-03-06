#include <stdio.h>
#include <stdlib.h>
#include "chessboard.h"

void print_board(struct Chessboard chessboard, const char* highlithe) {
    char i = 0;
    while (i<64){
        if(i%8 == 0){
            printf("\n");
        }
        if(highlithe!=0){
            int k=0;
            while(*(highlithe+k)){
                if(*(highlithe+k) == i){
                    printf("\033[0m\033[41m"); // White foreground / Red background
                }
                k++;
            }
        }
        if(*(chessboard.gameState+i) == 0){
            printf("0");
            printf("\033[0m ");
        }else{
            printf("%c", *(chessboard.gameState+i));
            printf("\033[0m ");
        }
        printf("\033[0m"); //Resets the text to default color.
        i++;
    }
    printf("\n");
}

int main(void){
    struct Chessboard chessboard;
    char* fen;
    init_board(&chessboard);
    print_board(chessboard, 0);
    fen  =  get_FEN(&chessboard);
    printf("%s\n",fen);
    free(fen);
    set_gameState(&chessboard, "8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50");
    print_board(chessboard, 0);
    fen  =  get_FEN(&chessboard);
    printf("%s\n",fen);
    free(fen);

    set_gameState(&chessboard, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    print_board(chessboard, 0);
    fen  =  get_FEN(&chessboard);
    printf("%s\n",fen);
    free(fen);



    set_gameState(&chessboard, "rnbqk2r/pp2p1bp/2p3p1/3p1pP1/3P1Q2/2N2N2/PPP1PPP1/2KR1B1R b kq - 1 9");
    print_board(chessboard, 0);
    fen  =  get_FEN(&chessboard);
    printf("%s\n",fen);
    free(fen);


    set_gameState(&chessboard, "rnbqk2r/pp2p1bp/2p3p1/3p1pP1/2BP1Q2/2N2N2/PPP1PPP1/2KR1B1R b kq - 1 9");
    print_board(chessboard, 0);
    fen  =  get_FEN(&chessboard);
    printf("%s\n",fen);
    free(fen);

    // testing possible moves
    printf("\n\nPossible moves test:\n");
    char* highlight = get_possible_moves(&chessboard, 1);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->knight on index 1\n");


    highlight = get_possible_moves(&chessboard, 3);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->queen on index 3\n");

    highlight = get_possible_moves(&chessboard, 14);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->bishop on index 14\n");

    highlight = get_possible_moves(&chessboard, 63);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->Rook on index 63\n");

    highlight = get_possible_moves(&chessboard, 35);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->Pawn on index 35\n");

    highlight = get_possible_moves(&chessboard, 42);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->Knight on index 42\n");

    highlight = get_possible_moves(&chessboard, 59);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->Rook on index 59\n");

    highlight = get_possible_moves(&chessboard, 34);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->Bishop on index 34\n");

    highlight = get_possible_moves(&chessboard, 18);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->pawn on index 18\n");

    highlight = get_possible_moves(&chessboard, 48);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->Pawn on index 48\n");

    highlight = get_possible_moves(&chessboard, 58);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->King on index 58\n");

    highlight = get_possible_moves(&chessboard, 4);
    print_board(chessboard, highlight);
    free(highlight);
    printf("->king on index 4\n");

    quick_move(&chessboard, "e8", "g8");
    print_board(chessboard, 0);
    printf("->king(4) king-side castling\n");



}

