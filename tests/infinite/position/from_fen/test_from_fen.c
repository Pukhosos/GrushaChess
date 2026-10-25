// depends-on: src/position/board.c

#include <inttypes.h>
#include <stdio.h>

#include "include/position/board.h"

static void print_board_position_json(const BoardPosition* bp) {
    printf("{\n");
    printf("  \"bitboards\": [\n");
    for (int i = 0; i < BITBOARD_COUNT; ++i) {
        printf(
            "    \"0x%016" PRIx64 "\"%s\n",
            (uint64_t)bp->bitboards[i],
            i == BITBOARD_COUNT - 1 ? "" : ","
        );
    }
    printf("  ],\n");
    printf("  \"ply_count\": %u,\n",        (unsigned)bp->ply_count);
    printf("  \"ply_clock\": %u,\n",        (unsigned)bp->ply_clock);
    printf("  \"castling_rights\": %u,\n",  (unsigned)bp->castling_rights);
    printf("  \"en_passant\": %u\n",        (unsigned)bp->en_passant);
    printf("}\n");
}

int main(int argc, const char** argv) {
    if (argc != 2) return 1;
    BoardPosition board_position;
    FenParsingResult result = from_fen(&board_position, argv[1]);
    if (result != FEN_OK) {
        fprintf(stderr, "FEN parsing failed: %d\n", result);
        return 1;
    }
    print_board_position_json(&board_position);
    return 0;
}
