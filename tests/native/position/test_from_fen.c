// depends-on: src/position/board.c

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "include/common/square.h"
#include "include/position/board.h"
#include "include/position/enums.h"

#ifndef TEST_DISPATCH
#define TEST_DISPATCH
#endif

const char *const INITIAL_POSITION = (
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
);

/**
 * @brief Assert that parsing fails without modifying the destination.
 */
static void assert_fen_error(const char *fen, FenParsingResult expected_result) {
    BoardPosition board;
    memset(&board, 0xA5, sizeof(board));
    BoardPosition original = board;
    FenParsingResult result = from_fen(&board, fen);
    assert(result == expected_result);
    assert(memcmp(&board, &original, sizeof(board)) == 0);
}

// 0. FEN_OK
static void test_initial_position() {
    // Arrange
    BoardPosition board;

    // Act
    FenParsingResult result = from_fen(&board, INITIAL_POSITION);

    // Assert
    assert(result           == FEN_OK);
    assert(board.ply_count  == 0);
    assert(board.ply_clock  == 0);
    assert(board.en_passant == NO_EN_PASSANT);
    assert(
        board.castling_rights
        == (
            CAN_WHITE_CASTLE_SHORT
            | CAN_WHITE_CASTLE_LONG
            | CAN_BLACK_CASTLE_SHORT
            | CAN_BLACK_CASTLE_LONG
        )
    );
    assert(board.bitboards[WHITE_PAWN]          == UINT64_C(0x000000000000FF00));
    assert(board.bitboards[WHITE_KNIGHT]        == (B1_MASK | G1_MASK));
    assert(board.bitboards[WHITE_BISHOP]        == (C1_MASK | F1_MASK));
    assert(board.bitboards[WHITE_ROOK]          == (A1_MASK | H1_MASK));
    assert(board.bitboards[WHITE_QUEEN]         == D1_MASK);
    assert(board.bitboards[WHITE_KING]          == E1_MASK);
    assert(board.bitboards[WHITE_OCCUPANCY]     == UINT64_C(0x000000000000FFFF));
    assert(board.bitboards[BLACK_PAWN]          == UINT64_C(0x00FF000000000000));
    assert(board.bitboards[BLACK_KNIGHT]        == (B8_MASK | G8_MASK));
    assert(board.bitboards[BLACK_BISHOP]        == (C8_MASK | F8_MASK));
    assert(board.bitboards[BLACK_ROOK]          == (A8_MASK | H8_MASK));
    assert(board.bitboards[BLACK_QUEEN]         == D8_MASK);
    assert(board.bitboards[BLACK_KING]          == E8_MASK);
    assert(board.bitboards[BLACK_OCCUPANCY]     == UINT64_C(0xFFFF000000000000));
    assert(board.bitboards[COMBINED_OCCUPANCY]  == UINT64_C(0xFFFF00000000FFFF));
}
static void test_maximum_ply_count() {
    BoardPosition board;
    FenParsingResult result = from_fen(&board, "4k3/8/8/8/8/8/8/4K3 b - - 0 8849");
    assert(result == FEN_OK);
    assert(board.ply_count == MAX_PLY_COUNT);
}
static void test_maximum_halfmove_clock() {
    BoardPosition board;
    FenParsingResult result = from_fen(&board, "4k3/8/8/8/8/8/8/4K3 b - - 150 76");
    assert(result == FEN_OK);
    assert(board.ply_clock == 150);
}
static void test_valid_en_passant_white() {
    BoardPosition board;
    FenParsingResult result = from_fen(&board, "4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
    assert(result == FEN_OK);
    assert(board.en_passant == D6);
}
static void test_valid_en_passant_black() {
    BoardPosition board;
    FenParsingResult result = from_fen(&board, "4k3/8/8/8/3Pp3/8/8/4K3 b - d3 0 1");
    assert(result == FEN_OK);
    assert(board.en_passant == D3);
}

// 1. FEN_INVALID_FORMAT
static void test_empty_fen() {
    assert_fen_error("", FEN_INVALID_FORMAT);
}
static void test_whitespace_only_fen() {
    assert_fen_error("    ", FEN_INVALID_FORMAT);
}
static void test_missing_side_to_move() {
    assert_fen_error("8/8/8/8/8/8/4K3/4k3", FEN_INVALID_FORMAT);
}
static void test_missing_castling_rights() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w", FEN_INVALID_FORMAT);
}
static void test_missing_en_passant() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w -", FEN_INVALID_FORMAT);
}
static void test_missing_halfmove_clock() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - -", FEN_INVALID_FORMAT);
}
static void test_missing_fullmove_number() {
    assert_fen_error("8/8/8/8/8/8/4K3/4k3 w - - 0", FEN_INVALID_FORMAT);
}
static void test_fen_with_extra_fiels() {
    assert_fen_error("8/8/8/8/8/8/4K3/4k3 w - - 0 1 extra", FEN_INVALID_FORMAT);
}
static void test_extra_fen_field() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 0 1 extra", FEN_INVALID_FORMAT);
}
static void test_extra_space() {
    assert_fen_error("4k3/8/8/8/8/8/4K3  w - - 0 1", FEN_INVALID_FORMAT);
}

// 2. FEN_INVALID_PIECE_PLACEMENT
static void test_too_few_ranks() {
    assert_fen_error("4k3/8/8/8/8/8/4K3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_too_many_ranks() {
    assert_fen_error("4k3/8/8/8/8/8/8/8/4K3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_rank_too_short() {
    assert_fen_error("4k3/8/8/8/8/8/8/3K3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_rank_too_long() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K4 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_too_few_empty_square_count() {
    assert_fen_error("4k3/8/8/8/8/8/8/3K04 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_too_many_empty_squares() {
    assert_fen_error("4k3/8/8/8/8/8/8/9 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_invalid_piece_character() {
    assert_fen_error("4k3/8/8/8/8/8/8/3XK3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_leading_rank_separator() {
    assert_fen_error("/4k3/8/8/8/8/8/8/4K3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_trailing_rank_separator() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3/ w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_double_rank_separator() {
    assert_fen_error("4k3/8//8/8/8/8/4K3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_consecutive_empty_square_digits() {
    assert_fen_error("44/8/8/8/8/8/8/4K2k w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_white_pawn_on_first_rank() {
    assert_fen_error("4k3/8/8/8/8/8/8/P3K3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_black_pawn_on_eighth_rank() {
    assert_fen_error("p3k3/8/8/8/8/8/8/4K3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_missing_white_king() {
    assert_fen_error("4k3/8/8/8/8/8/8/8 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_missing_black_king() {
    assert_fen_error("8/8/8/8/8/8/8/4K3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_multiple_white_kings() {
    assert_fen_error("4k3/8/8/8/8/8/8/3KK3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}
static void test_multiple_black_kings() {
    assert_fen_error("3kk3/8/8/8/8/8/8/4K3 w - - 0 1", FEN_INVALID_PIECE_PLACEMENT);
}

// 3. FEN_INVALID_SIDE_TO_MOVE
static void test_invalid_side_to_move_character() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 x - - 0 1", FEN_INVALID_SIDE_TO_MOVE);
}
static void test_uppercase_side_to_move() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 W - - 0 1", FEN_INVALID_SIDE_TO_MOVE);
}
static void test_multiple_side_to_move_characters() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 wb - - 0 1", FEN_INVALID_SIDE_TO_MOVE);
}
static void test_side_to_move_placeholder() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 - - - 0 1", FEN_INVALID_SIDE_TO_MOVE);
}

// 4. FEN_INVALID_CASTLING_RIGHTS
static void test_invalid_castling_character() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w X - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}
static void test_duplicate_white_short_castling() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w KK - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}
static void test_duplicate_white_long_castling() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w QQ - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}
static void test_duplicate_black_short_castling() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w kk - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}
static void test_duplicate_black_long_castling() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w qq - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}
static void test_hyphen_after_castling_right() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w K- - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}
static void test_hyphen_before_castling_right() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w -K - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}
static void test_double_castling_hyphen() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w -- - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}
static void test_invalid_castling_rights_order() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w qkQK - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}
static void test_invalid_castling_rights_partial_order() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w kQ - 0 1", FEN_INVALID_CASTLING_RIGHTS);
}

// 5. FEN_INVALID_EN_PASSANT
static void test_invalid_en_passant_rank() {
    assert_fen_error("4k3/8/8/3pP3/8/8/8/4K3 w - d9 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_invalid_en_passant_file() {
    assert_fen_error("4k3/8/8/3pP3/8/8/8/4K3 w - i6 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_wrong_en_passant_rank_white() {
    assert_fen_error("4k3/8/8/3pP3/8/8/8/4K3 w - d3 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_wrong_en_passant_rank_black() {
    assert_fen_error("4k3/8/8/8/3Pp3/8/8/4K3 b - d6 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_long_en_passant_field() {
    assert_fen_error("4k3/8/8/3pP3/8/8/8/4K3 w - d66 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_short_en_passant_field() {
    assert_fen_error("4k3/8/8/3pP3/8/8/8/4K3 w - d 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_occupied_en_passant_target_white() {
    assert_fen_error("4k3/8/3N4/3pP3/8/8/8/4K3 w - d6 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_occupied_en_passant_target_black() {
    assert_fen_error("4k3/8/8/8/3Pp3/3N4/8/4K3 b - d3 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_missing_en_passant_pawn_white() {
    assert_fen_error("4k3/8/8/4P3/8/8/8/4K3 w - d6 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_missing_en_passant_pawn_black() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 b - d3 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_occupied_en_passant_origin_white() {
    assert_fen_error("4k3/3n4/8/3pP3/8/8/8/4K3 w - d6 0 1", FEN_INVALID_EN_PASSANT);
}
static void test_occupied_en_passant_origin_black() {
    assert_fen_error("4k3/8/8/8/3P4/8/3N4/4K3 b - d3 0 1", FEN_INVALID_EN_PASSANT);
}

// 6. FEN_INVALID_HALFMOVE_CLOCK
static void test_negative_halfmove_clock() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - -1 1", FEN_INVALID_HALFMOVE_CLOCK);
}
static void test_non_numeric_halfmove_clock() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - abc 1", FEN_INVALID_HALFMOVE_CLOCK);
}
static void test_fractional_halfmove_clock() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 1.5 1", FEN_INVALID_HALFMOVE_CLOCK);
}
static void test_signed_halfmove_clock() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - +1 1", FEN_INVALID_HALFMOVE_CLOCK);
}
static void test_halfmove_clock_overflow() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 65536 1", FEN_INVALID_HALFMOVE_CLOCK);
}

// 7. FEN_INVALID_FULLMOVE_NUMBER
static void test_zero_fullmove_number() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 0 0", FEN_INVALID_FULLMOVE_NUMBER);
}
static void test_negative_fullmove_number() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 0 -1", FEN_INVALID_FULLMOVE_NUMBER);
}
static void test_non_numeric_fullmove_number() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 0 abc", FEN_INVALID_FULLMOVE_NUMBER);
}
static void test_fractional_fullmove_number() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 0 1.5", FEN_INVALID_FULLMOVE_NUMBER);
}
static void test_signed_fullmove_number() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 0 +1", FEN_INVALID_FULLMOVE_NUMBER);
}
static void test_fullmove_above_maximum_white() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 0 8850", FEN_INVALID_FULLMOVE_NUMBER);
}
static void test_fullmove_above_maximum_black() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 b - - 0 8850", FEN_INVALID_FULLMOVE_NUMBER);
}
static void test_fullmove_overflow() {
    assert_fen_error("4k3/8/8/8/8/8/8/4K3 w - - 0 65536", FEN_INVALID_FULLMOVE_NUMBER);
}

// 8. FEN_NULL_BOARD_POSITION
static void test_null_board_position() {
    FenParsingResult result = from_fen(NULL, INITIAL_POSITION);
    assert(result == FEN_NULL_BOARD_POSITION);
}

// 9. FEN_NULL_FEN
static void test_null_fen() {
    assert_fen_error(NULL, FEN_NULL_FEN);
}

TEST_DISPATCH
