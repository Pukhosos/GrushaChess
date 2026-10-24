from pathlib import Path
from re import Pattern, compile  # noqa: A004
from subprocess import CompletedProcess, run
from typing import Final, NamedTuple

from pytest import fail, mark

from tests.shared import CTestFile

NATIVE_TESTS: Final[Path] = Path(__file__).parent
C_TEST_PATTERN: Final[Pattern[str]] = compile(
    pattern=r'void (?P<test_case>test_[a-zA-Z0-9_])\(\)',
)


class CTestCase(NamedTuple):
    """Identify a `C` test by its executable name and test index."""

    build_name: str
    test_name: str
    test_index: int

    @staticmethod
    def get_test_case_id(case: CTestCase) -> str:
        return f'{case.build_name}::{case.test_name}[{case.test_index}]'

    def run_args(self, native_builds_dir: Path) -> tuple[str, str]:
        """Return the executable path and test index as subprocess arguments."""

        return (str(native_builds_dir / self.build_name), str(self.test_index))


@mark.parametrize(
    argnames='c_test_case',
    argvalues=[
        CTestCase(
            build_name=c_test_file.c_build_name,
            test_name=test_name,
            test_index=index,
        )
        for c_test_file in CTestFile.collect_c_tests(NATIVE_TESTS)
        for index, test_name in enumerate(c_test_file.tests)
    ],
    ids=CTestCase.get_test_case_id,
)
def test_native(native_builds_dir: Path, c_test_case: CTestCase) -> None:
    """Execute a `C` test case and assert that it exits successfully."""

    result: CompletedProcess[str] = run(  # noqa: S603
        c_test_case.run_args(native_builds_dir),
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        fail(
            reason=f'C test failed:\n{result.stdout}\n{result.stderr}',
            pytrace=False,
        )
