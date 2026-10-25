from collections.abc import Mapping
from dataclasses import dataclass
from typing import Final, Self

from chess import Board

from tests.infinite.shared.chess_piece import ChessPiece

PIECE_SYMBOLS: Final[str] = 'PpNnBbRrQqKk'
BITBOARD_COUNT: Final[int] = 15
NO_EN_PASSANT: Final[int] = 0xFF


@dataclass(frozen=True, slots=True)
class BoardState:
    """Immutable snapshot of a chess position."""

    pieces: list[ChessPiece]
    white_to_move: bool
    castling_rights: int
    en_passant_square: int
    halfmove_clock: int

    @classmethod
    def from_board(cls, board: Board) -> Self:
        """Extract a stable snapshot from a `python-chess` `Board`."""

        castling_rights: str = board.castling_xfen()
        return cls(
            pieces=sorted(
                ChessPiece(square, piece.symbol())
                for square, piece in board.piece_map().items()
            ),
            white_to_move=board.turn,
            castling_rights=(
                ('K' in castling_rights) << 0
                | ('k' in castling_rights) << 1
                | ('Q' in castling_rights) << 2
                | ('q' in castling_rights) << 3
            ),
            en_passant_square=(
                board.ep_square if board.ep_square is not None else NO_EN_PASSANT
            ),
            halfmove_clock=board.halfmove_clock,
        )

    @classmethod
    def from_json(cls, data: Mapping[str, object]) -> Self:
        """Reconstruct a board snapshot from the native JSON representation."""

        raw_bitboards: object = data['bitboards']
        if not isinstance(raw_bitboards, list):
            raise ValueError('Expected a list of bitboards')
        bitboards: list[int] = [
            int(bitboard, 16) for bitboard in raw_bitboards if isinstance(bitboard, str)
        ]
        if len(bitboards) != BITBOARD_COUNT:
            raise ValueError('Expected exactly 15 bitboards')
        pieces: list[ChessPiece] = []
        for symbol, bitboard in zip(PIECE_SYMBOLS, bitboards[:12], strict=True):
            while bitboard != 0:
                square: int = (bitboard & -bitboard).bit_length() - 1
                pieces.append(ChessPiece(square, symbol))
                bitboard &= bitboard - 1  # noqa: PLW2901
        return cls(
            pieces=sorted(pieces),
            white_to_move=(
                int(data['ply_count']) % 2 == 0  # type: ignore[call-overload]
            ),
            castling_rights=int(data['castling_rights']),  # type: ignore[call-overload]
            en_passant_square=int(data['en_passant']),  # type: ignore[call-overload]
            halfmove_clock=int(data['ply_clock']),  # type: ignore[call-overload]
        )
