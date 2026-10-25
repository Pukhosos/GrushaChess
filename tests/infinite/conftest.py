from collections.abc import Callable, Iterable
from io import TextIOWrapper
from pathlib import Path

from chess import Board
from chess.pgn import Game, read_game
from pytest import fixture
from zstandard import ZstdDecompressor

from tests.infinite.shared import BoardState


@fixture
def iter_positions_with_boardstate() -> Callable[
    [Path],
    Iterable[tuple[str, BoardState]],
]:
    """Iterate over chess positions from a `.zst` archive."""

    def _iter_positions_with_boardstate(
        archive_path: Path,
    ) -> Iterable[tuple[str, BoardState]]:
        with (
            archive_path.open('rb') as compressed_file,
            ZstdDecompressor().stream_reader(compressed_file) as decompressed,
            TextIOWrapper(
                decompressed,
                encoding='utf-8',
                errors='replace',
            ) as pgn_stream,
        ):
            while True:
                game: Game | None = read_game(pgn_stream)
                if game is not None and len(game.errors) == 0:
                    variant: str = game.headers.get('Variant', 'Standard')
                    if variant in ('Standard', 'Chess'):
                        board: Board = game.board()
                        yield (
                            board.fen(en_passant='fen'),
                            BoardState.from_board(board),
                        )
                        for move in game.mainline_moves():
                            board.push(move)
                            yield (
                                board.fen(en_passant='fen'),
                                BoardState.from_board(board),
                            )

    return _iter_positions_with_boardstate
