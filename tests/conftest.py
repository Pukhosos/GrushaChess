from pathlib import Path
from subprocess import CompletedProcess, run

from filelock import FileLock
from pytest import TempPathFactory, fixture

from tests.shared import NATIVE_TESTS, ROOT, TESTS, CTestFile


@fixture(scope='session')
def project_root_path() -> Path:
    """Path to the root of `GrushaChess`."""

    return ROOT


@fixture(scope='session')
def tests_path() -> Path:
    """Path to `/tests` folder."""

    return TESTS


@fixture(scope='session')
def native_tests_path() -> Path:
    """Path to `/tests/native` folder."""

    return NATIVE_TESTS


@fixture(scope='session')
def shared_temp_directory(tmp_path_factory: TempPathFactory, worker_id: str) -> Path:
    """Temporary directory for multithread synchronization during initial test setup."""

    return (
        tmp_path_factory.getbasetemp()
        if worker_id == 'master'
        else tmp_path_factory.getbasetemp().parent
    )


@fixture(scope='session')
def native_builds_dir(shared_temp_directory: Path) -> Path:
    """Temporary directory for compiled native test executables."""

    native_dir: Path = shared_temp_directory / 'native_builds'
    native_dir.mkdir(exist_ok=True)
    return native_dir


@fixture(scope='session', autouse=True)
def build_native_tests(
    shared_temp_directory: Path,
    native_builds_dir: Path,
) -> None:
    """Compile all discovered `.c` test files once per pytest session."""

    lock_file: Path = shared_temp_directory / 'grusha-chess-test-setup.lock'
    done_file: Path = shared_temp_directory / 'grusha-chess-test-setup.done'
    with FileLock(lock_file):
        if not done_file.exists():
            for c_test in CTestFile.collect_c_tests(NATIVE_TESTS):
                build_result: CompletedProcess[str] = run(  # noqa: S603
                    c_test.compose_compilation_command(
                        native_builds_dir=native_builds_dir,
                        optimization_level='O3',
                    ),
                    capture_output=True,
                    text=True,
                    check=False,
                )
                assert build_result.returncode == 0, (
                    f'C compilation failed:\n{build_result.stderr}'
                )
            done_file.touch()
