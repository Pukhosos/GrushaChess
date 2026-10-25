#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "include/common/square.h"
#include "include/position/board.h"
#include "include/position/enums.h"

typedef struct {
    const char *start;
    size_t length;
} FenField;

/**
 * @brief Convert a FEN piece character to its `SquareKind`.
 *
 * @return The corresponding `SquareKind` (`EMPTY_SQUARE` if invalid).
 */
static SquareKind piece_from_char(char fen_piece_character) {
    switch (fen_piece_character) {
        case 'P': return WHITE_PAWN;
        case 'p': return BLACK_PAWN;
        case 'N': return WHITE_KNIGHT;
        case 'n': return BLACK_KNIGHT;
        case 'B': return WHITE_BISHOP;
        case 'b': return BLACK_BISHOP;
        case 'R': return WHITE_ROOK;
        case 'r': return BLACK_ROOK;
        case 'Q': return WHITE_QUEEN;
        case 'q': return BLACK_QUEEN;
        case 'K': return WHITE_KING;
        case 'k': return BLACK_KING;
        default: return EMPTY_SQUARE;
    }
}

/**
 * @brief Convert a FEN castling character to its castling right.
 *
 * @return The corresponding `CastelingRight`, or zero if invalid.
 */
static U8 castling_right_from_char(char fen_castling_character) {
    switch (fen_castling_character) {
        case 'K': return CAN_WHITE_CASTLE_SHORT;
        case 'k': return CAN_BLACK_CASTLE_SHORT;
        case 'Q': return CAN_WHITE_CASTLE_LONG;
        case 'q': return CAN_BLACK_CASTLE_LONG;
        default: return 0;
    }
}

/**
 * @brief Parse a 32-bit unsigned decimal integer.
 *
 * @param field FEN field containing the decimal integer to parse.
 * @param max Maximum permitted value of the parsed integer.
 * @param value Receives the parsed integer on success.
 *
 * @return true if valid and within the specified maximum, false otherwise.
 *
 * @note On failure, `value` remains unchanged.
 */
static bool parse_decimal(FenField field, U16 max, U16 *value) {
    if (field.length == 0) return false;
    U16 result = 0;
    for (size_t i = 0; i < field.length; ++i) {
        char character = field.start[i];
        if (character < '0' || character > '9') return false;
        U16 digit = (U16)(character - '0');
        if (digit > max || result > (max - digit) / 10) return false;
        result = result * 10 + digit;
    }
    *value = result;
    return true;
}

/** @brief Whether castling rights order is correct. */
static bool is_castling_rights_order_valid(FenField field) {
    if (field.length == 1 && field.start[0] == '-') return true;
    if (field.length == 0 || field.length > 4) return false;
    const char *order = "KQkq";
    size_t j = 0;
    for (size_t i = 0; i < field.length; ++i) {
        while (j < 4 && order[j] != field.start[i]) ++j;
        if (j == 4) return false;
        ++j;
    }
    return true;
}

FenParsingResult from_fen(BoardPosition *board_position, const char *fen) {
    if (board_position == NULL) return FEN_NULL_BOARD_POSITION;
    if (fen == NULL) return FEN_NULL_FEN;

    // Split the FEN into exactly six fields
    FenField fields[6];
    const char *fen_character = fen;
    for (size_t i = 0; i < 6; ++i) {
        fields[i].start = fen_character;
        while (*fen_character && *fen_character != ' ') {
            if (isspace((U8)*fen_character)) return FEN_INVALID_FORMAT;
            ++fen_character;
        }
        fields[i].length = (size_t)(fen_character - fields[i].start);
        if (fields[i].length == 0) return FEN_INVALID_FORMAT;
        if (i < 5) {
            if (*fen_character != ' ') return FEN_INVALID_FORMAT;
            ++fen_character;
        }
    }
    if (*fen_character != '\0') return FEN_INVALID_FORMAT;

    /*
     * Parse into a temporary position so that the original position remains
     * unchanged if any field is invalid.
     */
    BoardPosition temp = {0};
    temp.en_passant = NO_EN_PASSANT;

    // Field 1: Piece placement.
    int rank = 7;
    int file = 0;
    U32 white_kings = 0;
    U32 black_kings = 0;
    for (size_t i = 0; i < fields[0].length; ++i) {
        char fen_character = fields[0].start[i];
        if (fen_character == '/') {
            if (file != 8 || rank == 0) return FEN_INVALID_PIECE_PLACEMENT;
            --rank;
            file = 0;
            continue;
        }
        if (fen_character >= '1' && fen_character <= '8') {
            if (
                i > 0
                && fields[0].start[i - 1] >= '1'
                && fields[0].start[i - 1] <= '8'
            ) return FEN_INVALID_PIECE_PLACEMENT;
            file += fen_character - '0';
            if (file > 8) return FEN_INVALID_PIECE_PLACEMENT;
            continue;
        }
        SquareKind piece = piece_from_char(fen_character);
        if (piece == EMPTY_SQUARE || file >= 8) return FEN_INVALID_PIECE_PLACEMENT;

        // Pawns cannot occupy the first or eighth rank
        if (
            (piece == WHITE_PAWN || piece == BLACK_PAWN)
            && (rank == 0 || rank == 7)
        ) return FEN_INVALID_PIECE_PLACEMENT;
        U64 mask = make_square_mask_from_square((Square)(rank * 8 + file));

        // Piece bitboard
        temp.bitboards[piece] |= mask;

        /*
         * Color occupancy bitboard.
         *
         * White pieces have even indices.
         * Black pieces have odd indices.
         */
        OccupancyKind occupancy = (piece & 1) ? BLACK_OCCUPANCY : WHITE_OCCUPANCY;
        temp.bitboards[occupancy] |= mask;

        // Combined occupancy bitboard.
        temp.bitboards[COMBINED_OCCUPANCY] |= mask;
        if (piece == WHITE_KING) ++white_kings;
        else if (piece == BLACK_KING) ++black_kings;
        ++file;
    }
    if (
        rank != 0
        || file != 8
        || white_kings != 1
        || black_kings != 1
    ) return FEN_INVALID_PIECE_PLACEMENT;

    // Field 2: Side to move
    if (
        fields[1].length != 1
        || (fields[1].start[0] != 'w' && fields[1].start[0] != 'b')
    ) return FEN_INVALID_SIDE_TO_MOVE;
    bool black_to_move = (fields[1].start[0] == 'b');

    // Field 3: Castling rights
    if (!is_castling_rights_order_valid(fields[2])) return FEN_INVALID_CASTLING_RIGHTS;
    if (fields[2].length == 1 && fields[2].start[0] == '-') temp.castling_rights = 0;
    else {
        for (size_t i = 0; i < fields[2].length; ++i) {
            U8 right = castling_right_from_char(fields[2].start[i]);
            if (right == 0) return FEN_INVALID_CASTLING_RIGHTS;
            temp.castling_rights |= right;
        }
    }

    // Field 4: En passant target
    if (!(fields[3].length == 1 && fields[3].start[0] == '-')) {
        if (fields[3].length != 2) return FEN_INVALID_EN_PASSANT;
        char en_passant_file = fields[3].start[0];
        char en_passant_rank = fields[3].start[1];
        if (
            en_passant_file < 'a' || en_passant_file > 'h'
        ) return FEN_INVALID_EN_PASSANT;

        /*
         * If Black is to move, White just advanced a pawn: the target must be
         * on rank 3.
         *
         * If White is to move, Black just advanced a pawn: the target must be
         * on rank 6.
         */
        if (
            en_passant_rank != (black_to_move ? '3' : '6')
        ) return FEN_INVALID_EN_PASSANT;
        U8 square = (U8)(en_passant_rank - '1') * (U8)8 + (U8)(en_passant_file - 'a');
        U64 mask = make_square_mask_from_square((Square)square);

        // The target square must be empty
        if (temp.bitboards[COMBINED_OCCUPANCY] & mask) {
            return FEN_INVALID_EN_PASSANT;
        }

        // Verify the pawn that advanced two squares
        U8 pawn_square = black_to_move ? square + (U8)8 : square - (U8)8;
        SquareKind pawn = black_to_move ? WHITE_PAWN : BLACK_PAWN;
        if (
            !(
                temp.bitboards[pawn]
                & make_square_mask_from_square((Square)pawn_square)
            )
        ) return FEN_INVALID_EN_PASSANT;

        // The pawn's original square must be empty
        U8 original_square = black_to_move ? square - (U8)8 : square + (U8)8;
        if (
            temp.bitboards[COMBINED_OCCUPANCY]
            & make_square_mask_from_square((Square)original_square)
        ) return FEN_INVALID_EN_PASSANT;
        temp.en_passant = square;
    }

    // Field 5: Halfmove clock
    U16 halfmove_clock;
    if (
        !parse_decimal(fields[4], UINT16_MAX, &halfmove_clock)
        || temp.en_passant != NO_EN_PASSANT && halfmove_clock != 0
    ) return FEN_INVALID_HALFMOVE_CLOCK;
    temp.ply_clock = halfmove_clock;

    // Field 6: Fullmove number
    U16 fullmove_number;
    if (
        !parse_decimal(fields[5], MAX_PLY_COUNT / 2 + 1, &fullmove_number)
        || fullmove_number == 0
    ) return FEN_INVALID_FULLMOVE_NUMBER;

    // Derive the ply count from the fullmove number and the active color
    U16 ply_count = (U16)2 * (fullmove_number - (U16)1) + (U16)black_to_move;
    if (ply_count > MAX_PLY_COUNT) return FEN_INVALID_FULLMOVE_NUMBER;
    temp.ply_count = ply_count;

    // Commit the fully parsed position
    *board_position = temp;
    return FEN_OK;
}
