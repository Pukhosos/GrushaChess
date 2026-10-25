// depends-on: src/position/board.c

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "include/common/square.h"
#include "include/position/board.h"
#include "include/position/enums.h"

#ifndef TEST_DISPATCH
#define TEST_DISPATCH
#endif

static void test_initial_position() {
    // Arrange
    BoardPosition board = {0};
    board.bitboards[WHITE_PAWN]         = UINT64_C(0x000000000000FF00);
    board.bitboards[WHITE_KNIGHT]       = B1_MASK | G1_MASK;
    board.bitboards[WHITE_BISHOP]       = C1_MASK | F1_MASK;
    board.bitboards[WHITE_ROOK]         = A1_MASK | H1_MASK;
    board.bitboards[WHITE_QUEEN]        = D1_MASK;
    board.bitboards[WHITE_KING]         = E1_MASK;
    board.bitboards[WHITE_OCCUPANCY]    = UINT64_C(0x000000000000FFFF);
    board.bitboards[BLACK_PAWN]         = UINT64_C(0x00FF000000000000);
    board.bitboards[BLACK_KNIGHT]       = B8_MASK | G8_MASK;
    board.bitboards[BLACK_BISHOP]       = C8_MASK | F8_MASK;
    board.bitboards[BLACK_ROOK]         = A8_MASK | H8_MASK;
    board.bitboards[BLACK_QUEEN]        = D8_MASK;
    board.bitboards[BLACK_KING]         = E8_MASK;
    board.bitboards[BLACK_OCCUPANCY]    = UINT64_C(0xFFFF000000000000);
    board.bitboards[COMBINED_OCCUPANCY] = UINT64_C(0xFFFF00000000FFFF);
    board.ply_count = 0;
    board.ply_clock = 0;
    board.castling_rights = (
        CAN_WHITE_CASTLE_SHORT
        | CAN_WHITE_CASTLE_LONG
        | CAN_BLACK_CASTLE_SHORT
        | CAN_BLACK_CASTLE_LONG
    );
    board.en_passant = NO_EN_PASSANT;
    
    // Act
    char fen[FEN_BUFFER_SIZE];
    bool result = to_fen(&board, fen);

    // Assert
    assert(result == true);
    assert(
        strcmp(
            fen,
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
        ) == 0
    );
}

TEST_DISPATCH
