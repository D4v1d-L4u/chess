#ifndef CHESS_CHESSBOARD_H
#define CHESS_CHESSBOARD_H

struct Chessboard{

    /*
     * Values in the array:
     * 80 := white Pawn (P)
     * 82 := white rook (R)
     * 78 := white knight (N)
     * 66 := white bishop (B)
     * 81 := white queen (Q)
     * 75 := white king (K)
     * 112 := black Pawn (p)
     * 114 := black rook (r)
     * 110 := black knight (n)
     * 98 := black bishop (b)
     * 113 := black queen (q)
     * 107 := black king (k)
     *
     *
     * Chess-field:
     * 00 01 02 03 04 05 06 07
     * 08 09 10 11 12 13 14 15
     * 16 17 18 19 20 21 22 23
     * 24 25 26 27 28 29 30 31
     * 32 33 34 35 36 37 38 39
     * 40 41 42 43 44 45 46 47
     * 48 49 50 51 52 53 54 55
     * 56 57 58 59 60 61 62 63
     */
    char gameState [64];
    int halfmove_clock; // for fifty-move rule
    int half_round_count; // starts at 0 (even -> whites turn/ odd -> blacks turn)
    int round_count;
    /*
     * 0 if not possible
     * else it is the index of the possible square
     */
    char en_passant;
    /*
     * Castling (0000):
     * 0001 bit for white king-side castling
     * 0010 bit for white queen-side castling
     * 0100 bit for black king-side castling
     * 1000 bit for black queen-side castling
     */
    char castling;

    // saves the positions of the kings on the chessboard
    char w_king_position;
    char b_king_position;
};

/*
 * Initialize the Chessboard to start the game.
 */
void init_board(struct Chessboard* board);

/*
 * Moves a chess-piece 'from' a position 'to' another position.
 * Returns 0 if it was possible.
 * Returns 1 if it was not possible.
 */
int make_move(struct Chessboard* board, char* from, char* to);


/*
 * Ends a chess game.
 */
void surrender(struct Chessboard* board);

/*
 * Prints out the Chessboard.
 */
void print_Chessboard(struct Chessboard* board);

/*
 * prints chessboard with highlighted legal moves
 * from a given position (in Algebraic-Notation)
 */
void highlight_legal_moves(struct Chessboard* board, char* position);

/*
 * Prints out the state of the game in Forsyth-Edwards Notation (FEN)
 */
char* get_FEN(struct Chessboard* board);

/*
 * Takes Forsyth-Edwards Notation (FEN) and sets the state of the chessboard according to it
 */
void set_gameState(struct Chessboard* board, char* fen);

//TODO only fot testing!!!!
char* get_possible_moves (struct Chessboard* board,  char index);
#endif //CHESS_CHESSBOARD_H
