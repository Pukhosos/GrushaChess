from collections.abc import Callable, Iterable
from json import loads as json_loads
from pathlib import Path
from subprocess import CompletedProcess, run
from typing import Final

from pytest import mark
from tests.infinite.shared import BoardState
from tests.shared import LICHESS_GAMES_ARCHIVE_2014_09, ROOT, CTestFile

FINITE_TEST_LIMIT: Final[int] = 100


@mark.infinite
def test_from_fen(
    limit_infinite_tests: bool,
    native_builds_dir: Path,
    iter_positions_with_boardstate: Callable[[Path], Iterable[tuple[str, BoardState]]],
) -> None:
    archive_path: Path = ROOT / 'testdata' / LICHESS_GAMES_ARCHIVE_2014_09
    for iteration, (fen, expected_board_state) in enumerate(
        iter_positions_with_boardstate(archive_path),
    ):
        if limit_infinite_tests and iteration > FINITE_TEST_LIMIT:
            break
        executable: str = str(
            native_builds_dir / CTestFile.get_executable_name_by_file(__file__)
        )
        result: CompletedProcess[str] = run(  # noqa: S603
            [executable, fen],
            capture_output=True,
            text=True,
            check=True,
        )
        actual_board_state: BoardState = BoardState.from_json(json_loads(result.stdout))
        assert actual_board_state == expected_board_state, (
            f'FEN = \"{fen}\"'
        )  # fmt: skip
