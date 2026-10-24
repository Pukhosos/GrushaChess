#ifndef POSITION_BOARD_H
#define POSITION_BOARD_H

#include "include/common/uints.h"

enum {
    /**
     * @brief Number of bitboards used to represent pieces in a chess position.
     *
     * Includes 12 piece-specific bitboards, two color-specific occupancy
     * bitboards, and one combined occupancy bitboard.
     */
    BITBOARD_COUNT = 15,
};

enum {
    /**
     * @brief Maximum number of half-moves in a chess game.
     *
     * Defines the upper bound for the ply count stored in a `BoardPosition`.
     */
    MAX_PLY_COUNT = 17697,
};

enum {
    /** @brief Sentinel value indicating that no en passant target exists. */
    NO_EN_PASSANT = 0xFF,
};

/**
 * @brief Represents a chess position.
 *
 * Stores the board state using bitboards along with the metadata required to
 * describe the current position precisely.
 */
typedef struct {
    /**
     * @brief Bitboards representing the board state.
     *
     * Contains 15 bitboards: 12 piece-specific bitboards, for color-specific
     * occupancy, and one for combined occupancy.
     */
    U64 bitboards[BITBOARD_COUNT];

    /**
     * @brief Number of half-moves played.
     *
     * Maximum value is `MAX_PLY_COUNT = 17697`.
     */
    U16 ply_count;

    /**
     * @brief Bitmask of CastelingRight values encoding the castling rights.
     *
     * ```
     * Bit:   7 6 5 4 3 2 1 0
     *        ---------------
     * Value: - - - - q Q k K
     * ```
     *
     * @note Bits 4, 5, 6, and 7 are reserved and should be zero.
     */
    U8 castling_rights;

    /**
     * @brief Square index of the current en passant target, or a sentinel value
     * `NO_EN_PASSANT = 0xFF` if no en passant target exists.
     *
     * Stores the index of the square passed over by a pawn that advanced two
     * squares from its starting rank on the previous move.
     *
     * An opposing pawn may capture onto this square on the immediately
     * following move, provided the capture does not leave its own king in
     * check.
     *
     * The target is cleared after any subsequent move unless that move
     * establishes a new en passant target.
     *
     * After:
     *
     *   - `1. e4 nf6`
     *
     *   - `2. e5 d5`
     *
     * ```
     *   +------------------------+
     * 8 | r  n  b  q  k  b  n  r |
     * 7 | p  p  p  .  p  p  p  p |
     * 6 | .  .  N  *  .  .  .  . | <-- en_passant = d6
     * 5 | .  .  .  p  P  .  .  . |
     * 4 | .  .  .  .  .  .  .  . |
     * 3 | .  .  .  .  .  .  .  . |
     * 2 | P  P  P  P  .  P  P  P |
     * 1 | R  N  B  Q  K  B  N  R |
     *   +------------------------+
     *     a  b  c  d  e  f  g  h
     * ```
     *
     * White can capture en passant by playing `3. exd6`
     *
     * ```
     *   +------------------------+
     * 8 | r  n  b  q  k  b  n  r |
     * 7 | p  p  p  .  p  p  p  p |
     * 6 | .  .  N  P  .  .  .  . |
     * 5 | .  .  .  .  .  .  .  . |
     * 4 | .  .  .  .  .  .  .  . |
     * 3 | .  .  .  .  .  .  .  . |
     * 2 | P  P  P  P  .  P  P  P |
     * 1 | R  N  B  Q  K  B  N  R |
     *   +------------------------+
     *     a  b  c  d  e  f  g  h
     * ```
     *
     * @note When `en_passant != NO_EN_PASSANT`:
     *
     *   - The target square must be empty.
     *
     *   - The target must lie on rank 6 if White is to move, or rank 3 if Black
     * is to move.
     *
     *   - An opposing pawn must occupy the square immediately behind the
     * target.
     */
    U8 en_passant;
} BoardPosition;

#endif
