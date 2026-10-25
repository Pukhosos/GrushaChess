from dataclasses import dataclass
from itertools import takewhile
from pathlib import Path
from re import Pattern, compile  # noqa: A004
from string import Template
from textwrap import dedent
from typing import Final, Literal

from tests.shared.constants import ROOT, TESTS

C_DEPENDENCY_DECLARATION_PREFIX: Final[str] = '// depends-on: '
C_TEST_PATTERN: Final[Pattern[str]] = compile(
    pattern=r'static void (?P<test_case>test_[a-zA-Z0-9_]+)\(\)',
)
TEST_DISPATCH_MAIN: Final[Template] = Template(
    dedent(
        """
            int main(int argc, char **argv) {
                if (argc != 2) return 1;
                int test_index = atoi(argv[1]);
                $SWITCH_TEST_DISPATCH
                return 0;
            }
        """
    ).replace('\n', ' ')
)


@dataclass(frozen=True, slots=True)
class CTestFile:
    """Represent a C test file and its build configuration."""

    path: Path
    """Path to the `.c` test file."""

    @property
    def c_build_name(self) -> str:
        """Name of the executable derived from the test file path."""

        build_name: str = str(self.path.relative_to(TESTS)).replace('/', '_')
        return build_name.removesuffix('.c')

    @property
    def c_dependencies(self) -> list[Path]:
        """Link dependencies declared at the beginning of the `.c` test file."""

        return [
            ROOT / line.removeprefix(C_DEPENDENCY_DECLARATION_PREFIX)
            for line in takewhile(
                lambda line: line.startswith(C_DEPENDENCY_DECLARATION_PREFIX),
                self.path.read_text().splitlines(),
            )
        ]

    @property
    def tests(self) -> list[str]:
        """Test function names in their order of appearance."""

        return [
            match.group('test_case')
            for match
            in C_TEST_PATTERN.finditer(self.path.read_text())
        ]  # fmt: skip

    @property
    def test_dispatch(self) -> str:
        """Generate a `C` switch statement dispatching tests by index."""

        cases: str = ' '.join(
            f'case {index}: {test}(); break;'
            for index, test
            in enumerate(self.tests)
        )  # fmt: skip
        return TEST_DISPATCH_MAIN.substitute(
            {
                'SWITCH_TEST_DISPATCH': (
                    f'switch (test_index) {{ {cases} default: return 1; }}'
                )
            }
        )

    @staticmethod
    def collect_c_tests(directory: Path) -> list[CTestFile]:
        """Collect all `.c` files in `directory` and all its subdirectories."""

        return [
            CTestFile(native_test_path)
            for native_test_path in directory.rglob('test_*.c')
            if native_test_path.is_file()
        ]

    @staticmethod
    def get_executable_name_by_file(dunder_file: str) -> str:
        """Get executable name corresponding to `__file__`."""

        relative_path: Path = Path(dunder_file).relative_to(TESTS)
        return str(relative_path).replace('/', '_').removesuffix('.py')

    def compose_compilation_command(
        self,
        native_builds_dir: Path,
        optimization_level: Literal['O0', 'O1', 'O2', 'O3'],
    ) -> list[str]:
        """`GCC` command for compiling the `C` test executable."""

        return [
            'gcc',
            '-std=c23',
            '-Wall',
            '-Wextra',
            '-Wpedantic',
            '-g',
            f'-{optimization_level}',
            '-fsanitize=address,undefined',
            '-fno-omit-frame-pointer',
            '-fno-sanitize-recover=all',
            f'-I{ROOT}',
            *map(str, self.c_dependencies),
            str(self.path),
            '-o',
            str(native_builds_dir / self.c_build_name),
            f'-DTEST_DISPATCH={self.test_dispatch}',
        ]
