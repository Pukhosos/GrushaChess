#ifndef COMMON_SQUARE_H
#define COMMON_SQUARE_H

#include "include/common/uints.h"

/**
 * @brief Chessboard square indices using Little-Endian Rank-File (LERF)
 * mapping.
 *
 * Squares are indexed from 0 (A1) to 63 (H8), with files increasing from A to H
 * and ranks increasing from 1 to 8.
 *
 * The file and rank of a square can be extracted using:
 *
 * ```c
 * U8 file = square & 7;    // 0 = A, 7 = H
 * U8 rank = square >> 3;   // 0 = rank 1, 7 = rank 8
 * ```
 */
typedef enum : U8 {
    A1 = 0b00000000,
    B1 = 0b00000001,
    C1 = 0b00000010,
    D1 = 0b00000011,
    E1 = 0b00000100,
    F1 = 0b00000101,
    G1 = 0b00000110,
    H1 = 0b00000111,

    A2 = 0b00001000,
    B2 = 0b00001001,
    C2 = 0b00001010,
    D2 = 0b00001011,
    E2 = 0b00001100,
    F2 = 0b00001101,
    G2 = 0b00001110,
    H2 = 0b00001111,

    A3 = 0b00010000,
    B3 = 0b00010001,
    C3 = 0b00010010,
    D3 = 0b00010011,
    E3 = 0b00010100,
    F3 = 0b00010101,
    G3 = 0b00010110,
    H3 = 0b00010111,

    A4 = 0b00011000,
    B4 = 0b00011001,
    C4 = 0b00011010,
    D4 = 0b00011011,
    E4 = 0b00011100,
    F4 = 0b00011101,
    G4 = 0b00011110,
    H4 = 0b00011111,

    A5 = 0b00100000,
    B5 = 0b00100001,
    C5 = 0b00100010,
    D5 = 0b00100011,
    E5 = 0b00100100,
    F5 = 0b00100101,
    G5 = 0b00100110,
    H5 = 0b00100111,

    A6 = 0b00101000,
    B6 = 0b00101001,
    C6 = 0b00101010,
    D6 = 0b00101011,
    E6 = 0b00101100,
    F6 = 0b00101101,
    G6 = 0b00101110,
    H6 = 0b00101111,

    A7 = 0b00110000,
    B7 = 0b00110001,
    C7 = 0b00110010,
    D7 = 0b00110011,
    E7 = 0b00110100,
    F7 = 0b00110101,
    G7 = 0b00110110,
    H7 = 0b00110111,

    A8 = 0b00111000,
    B8 = 0b00111001,
    C8 = 0b00111010,
    D8 = 0b00111011,
    E8 = 0b00111100,
    F8 = 0b00111101,
    G8 = 0b00111110,
    H8 = 0b00111111,
} Square;

/**
 * @brief Single-square bitboard masks using Little-Endian Rank-File (LERF)
 * mapping.
 *
 * Each mask contains exactly one set bit corresponding to its square. A1_MASK
 * represents the least significant bit (bit 0), while H8_MASK represents the
 * most significant bit (bit 63).
 *
 * @note `SquareMask == 1ULL << Square`
 */
typedef enum : U64 {
    A1_MASK = 1ULL << A1,
    B1_MASK = 1ULL << B1,
    C1_MASK = 1ULL << C1,
    D1_MASK = 1ULL << D1,
    E1_MASK = 1ULL << E1,
    F1_MASK = 1ULL << F1,
    G1_MASK = 1ULL << G1,
    H1_MASK = 1ULL << H1,

    A2_MASK = 1ULL << A2,
    B2_MASK = 1ULL << B2,
    C2_MASK = 1ULL << C2,
    D2_MASK = 1ULL << D2,
    E2_MASK = 1ULL << E2,
    F2_MASK = 1ULL << F2,
    G2_MASK = 1ULL << G2,
    H2_MASK = 1ULL << H2,

    A3_MASK = 1ULL << A3,
    B3_MASK = 1ULL << B3,
    C3_MASK = 1ULL << C3,
    D3_MASK = 1ULL << D3,
    E3_MASK = 1ULL << E3,
    F3_MASK = 1ULL << F3,
    G3_MASK = 1ULL << G3,
    H3_MASK = 1ULL << H3,

    A4_MASK = 1ULL << A4,
    B4_MASK = 1ULL << B4,
    C4_MASK = 1ULL << C4,
    D4_MASK = 1ULL << D4,
    E4_MASK = 1ULL << E4,
    F4_MASK = 1ULL << F4,
    G4_MASK = 1ULL << G4,
    H4_MASK = 1ULL << H4,

    A5_MASK = 1ULL << A5,
    B5_MASK = 1ULL << B5,
    C5_MASK = 1ULL << C5,
    D5_MASK = 1ULL << D5,
    E5_MASK = 1ULL << E5,
    F5_MASK = 1ULL << F5,
    G5_MASK = 1ULL << G5,
    H5_MASK = 1ULL << H5,

    A6_MASK = 1ULL << A6,
    B6_MASK = 1ULL << B6,
    C6_MASK = 1ULL << C6,
    D6_MASK = 1ULL << D6,
    E6_MASK = 1ULL << E6,
    F6_MASK = 1ULL << F6,
    G6_MASK = 1ULL << G6,
    H6_MASK = 1ULL << H6,

    A7_MASK = 1ULL << A7,
    B7_MASK = 1ULL << B7,
    C7_MASK = 1ULL << C7,
    D7_MASK = 1ULL << D7,
    E7_MASK = 1ULL << E7,
    F7_MASK = 1ULL << F7,
    G7_MASK = 1ULL << G7,
    H7_MASK = 1ULL << H7,

    A8_MASK = 1ULL << A8,
    B8_MASK = 1ULL << B8,
    C8_MASK = 1ULL << C8,
    D8_MASK = 1ULL << D8,
    E8_MASK = 1ULL << E8,
    F8_MASK = 1ULL << F8,
    G8_MASK = 1ULL << G8,
    H8_MASK = 1ULL << H8,
} SquareMask;

#endif
