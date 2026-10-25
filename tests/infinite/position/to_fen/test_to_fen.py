from collections.abc import Callable, Iterable
from pathlib import Path
from subprocess import CompletedProcess, run
from typing import Final

from pytest import mark
from tests.shared import LICHESS_GAMES_ARCHIVE_2014_09, ROOT, CTestFile

FINITE_TEST_LIMIT: Final[int] = 100


@mark.infinite
def test_to_fen(
    limit_infinite_tests: bool,
    native_builds_dir: Path,
    iter_positions: Callable[[Path], Iterable[str]],
) -> None:
    archive_path: Path = ROOT / 'testdata' / LICHESS_GAMES_ARCHIVE_2014_09
    for iteration, fen in enumerate(iter_positions(archive_path)):
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
        assert result.returncode == 0, f'FEN = \"{fen}\"'  # fmt: skip
