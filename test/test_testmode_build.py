import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def test_testmode_preprocessor_enables_only_requested_helpers():
    source = '#include "config/general.h"\n'
    result = subprocess.run(
        [
            "arm-none-eabi-cpp",
            "-dM",
            "-DMODERN=1",
            "-DTEST_MODE=1",
            "-Iinclude",
            "-",
        ],
        cwd=ROOT,
        input=source,
        text=True,
        capture_output=True,
        check=True,
    )

    defines = result.stdout
    assert "#define TEST_START_WITH_RAYQUAZA TRUE" in defines
    assert "#define TEST_FLY_DESTINATIONS_UNRESTRICTED TRUE" in defines
    assert "#define TEST_DISABLE_OBEDIENCE TRUE" in defines
    assert "#define TEST_START_WITH_EMERALD_KEY_ITEMS FALSE" in defines
    assert "#define TEST_START_WITH_ALL_HM_ACCESS TRUE" in defines


def test_make_exposes_a_separate_testmode_rom_target():
    result = subprocess.run(
        ["make", "--no-print-directory", "-pn", "testmode"],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=True,
    )

    make_database = result.stdout
    assert "ROM := pokeemerurd-testmode.gba" in make_database
    assert "OBJ_DIR := build/emerald-testmode" in make_database
