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
     * @brief Halfmoves since the last pawn move or capture.
     *
     * Used to enforce the 50-move and 75-move rules:
     *
     *   - 50-move rule: After 100 halfmoves (50 moves by each player) without a
     * pawn move or capture, either player may claim a draw.
     *
     *   - 75-move rule: After 150 halfmoves (75 moves by each player) without a
     * pawn move or capture, the game is automatically drawn, unless the final
     * move delivers checkmate.
     *
     * @note Resets to zero after any pawn move or capture, otherwise
     * incremented after every move by either player.
     * */
    U16 ply_clock;

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

/** @brief FEN parsing result. */
typedef enum {
    /**
     * @brief FEN string parsed successfully.
     *
     * All six FEN fields are valid, and the resulting position has been
     * successfully stored in the destination `BoardPosition`.
     */
    FEN_OK = 0x00,

    /**
     * @brief Invalid FEN string structure.
     *
     * The FEN string does not conform to the expected overall format.
     */
    FEN_INVALID_FORMAT = 0x01,

    /**
     * @brief Invalid piece placement field.
     *
     * The first FEN field does not describe a valid 8x8 chessboard.
     */
    FEN_INVALID_PIECE_PLACEMENT = 0x02,

    /**
     * @brief Invalid side-to-move field.
     *
     * The second FEN field must contain exactly one character:
     *
     *   - 'w' for White to move.
     *
     *   - 'b' for Black to move.
     *
     * Any other value is invalid.
     */
    FEN_INVALID_SIDE_TO_MOVE = 0x03,

    /**
     * @brief Invalid castling rights field.
     *
     * The third FEN field must contain either '-' (no castling rights) or a
     * valid combination of the following characters:
     *
     *   - 'K' for White kingside castling.
     *
     *   - 'Q' for White queenside castling.
     *
     *   - 'k' for Black kingside castling.
     *
     *   - 'q' for Black queenside castling.
     *
     * @note This result indicates invalid field syntax.
     */
    FEN_INVALID_CASTLING_RIGHTS = 0x04,

    /**
     * @brief Invalid en passant target square field.
     *
     * The fourth FEN field must contain either '-' (no en passant target) or a
     * valid target square in algebraic notation.
     *
     * The target square must:
     *
     *   - Have a file between 'a' and 'h'.
     *
     *   - Have rank '3' or '6'.
     *
     *   - Be on rank '3' when Black is to move.
     *
     *   - Be on rank '6' when White is to move.
     *
     * @note A valid en passant target does not necessarily imply that an en
     * passant capture is currently legal.
     */
    FEN_INVALID_EN_PASSANT = 0x05,

    /**
     * @brief Invalid halfmove clock field.
     *
     * The fifth FEN field must contain a non-negative decimal integer
     * representing the number of halfmoves since the last pawn move or capture.
     */
    FEN_INVALID_HALFMOVE_CLOCK = 0x06,

    /**
     * @brief Invalid fullmove number field.
     *
     * The sixth FEN field must contain a positive decimal integer representing
     * the current fullmove number.
     *
     * The fullmove number starts at 1 and increments after each move by Black.
     */
    FEN_INVALID_FULLMOVE_NUMBER = 0x07,

    /** @brief `board_position` is NULL. */
    FEN_NULL_BOARD_POSITION = 0x08,

    /**
     * @brief `fen` is NULL.
     *
     * The source FEN string pointer is NULL. No parsing is performed, and the
     * destination position remains unchanged.
     */
    FEN_NULL_FEN = 0x09,
} FenParsingResult;

/**
 * @brief Parse a FEN string into a chess position.
 *
 * @param board_position `BoardPosition` to parse FEN into.
 * @param fen Null-terminated FEN string to parse.
 *
 * @return `FenParsingResult`.
 *
 * @note On failure, `board_position` remain unchanged.
 */
FenParsingResult from_fen(BoardPosition* board_position, const char* fen);

#endif
