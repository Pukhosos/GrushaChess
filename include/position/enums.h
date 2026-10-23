#ifndef POSITION_ENUMS_H
#define POSITION_ENUMS_H

#include "include/common/uints.h"

/**
 * @brief Identifies the piece kind occupying a square on a chess board.
 *
 * Encodes the six chess piece types for each color, plus an empty-square
 * sentinel. Piece values range from `0x00` to `0x0B` and correspond directly to
 * their indices in `Position::bitboards`. White pieces occupy even-indexed
 * positions, while black pieces occupy odd-indexed positions.
 *
 * @note `EMPTY_SQUARE` does not have a corresponding bitboard.
 */
typedef enum : U8 {
    WHITE_PAWN      = 0x00,
    BLACK_PAWN      = 0x01,
    WHITE_KNIGHT    = 0x02,
    BLACK_KNIGHT    = 0x03,
    WHITE_BISHOP    = 0x04,
    BLACK_BISHOP    = 0x05,
    WHITE_ROOK      = 0x06,
    BLACK_ROOK      = 0x07,
    WHITE_QUEEN     = 0x08,
    BLACK_QUEEN     = 0x09,
    WHITE_KING      = 0x0A,
    BLACK_KING      = 0x0B,
    EMPTY_SQUARE    = 0x0F, // Not a bitboard
} SquareKind;

/**
 * @brief Identifies an aggregate occupancy bitboard index.
 *
 * Values correspond directly to indices in `Position::bitboards`, following the
 * 12 individual piece bitboards.
 *
 * `WHITE_OCCUPANCY` and `BLACK_OCCUPANCY` represent all squares occupied by the
 * respective color, `COMBINED_OCCUPANCY` represents all occupied squares
 * regardless of color.
 */
typedef enum : U8 {
    WHITE_OCCUPANCY     = 0x0C,
    BLACK_OCCUPANCY     = 0x0D,
    COMBINED_OCCUPANCY  = 0x0E,
} OccupancyKind;

#endif
