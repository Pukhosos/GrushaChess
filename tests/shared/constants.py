from pathlib import Path
from typing import Final

ROOT: Final[Path] = Path(__file__).parents[2]
"""Path to the root of `GrushaChess`."""

TESTS: Final[Path] = ROOT / 'tests'
"""Path to `/tests` folder."""

NATIVE_TESTS: Final[Path] = TESTS / 'native'
"""Path to `/tests/native` folder."""
