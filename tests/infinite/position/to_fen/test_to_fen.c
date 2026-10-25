// depends-on: src/position/board.c

#include <stdio.h>
#include <string.h>

#include "include/position/board.h"

int main(int argc, const char** argv) {
    if (argc != 2) return 1;
    BoardPosition board_position;
    FenParsingResult from_fen_result = from_fen(&board_position, argv[1]);
    if (from_fen_result != FEN_OK) {
        fprintf(stderr, "FEN parsing failed: %d\n", from_fen_result);
        return 1;
    }
    char fen[FEN_BUFFER_SIZE];
    bool to_fen_result = to_fen(&board_position, fen);
    if (!to_fen_result) {
        fprintf(stderr, "FEN construction failed\n");
        return 1;
    }
    return strcmp(fen, argv[1]);
}
