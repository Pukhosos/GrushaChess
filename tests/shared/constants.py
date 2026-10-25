from pathlib import Path
from typing import Final

ROOT: Final[Path] = Path(__file__).parents[2]
"""Path to the root of `GrushaChess`."""

TESTS: Final[Path] = ROOT / 'tests'
"""Path to `/tests` folder."""

NATIVE_TESTS: Final[Path] = TESTS / 'native'
"""Path to `/tests/native` folder."""

LICHESS_GAMES_ARCHIVE_2014_09: Final[str] = 'lichess_db_standard_rated_2014-09.pgn.zst'
"""Chess games played on Lichess in September 2014."""
