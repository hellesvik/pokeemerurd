from pathlib import Path
import re


FIELD_MOVE_HEADER = Path("include/field_move.h").read_text()
NEW_GAME_SOURCE = Path("src/new_game.c").read_text()


def test_test_hm_access_bypasses_field_move_gate_without_setting_badges():
    unlock_function = re.search(
        r"static inline bool32 IsFieldMoveUnlocked\([^)]*\)\n\{(.*?)\n\}",
        FIELD_MOVE_HEADER,
        re.DOTALL,
    )
    assert unlock_function is not None
    assert "TEST_START_WITH_ALL_HM_ACCESS" in unlock_function.group(1)
    assert "return TRUE" in unlock_function.group(1)

    test_setup = re.search(
        r"#if TEST_START_WITH_ALL_HM_ACCESS(.*?)#endif",
        NEW_GAME_SOURCE,
        re.DOTALL,
    )
    if test_setup is not None:
        assert "FLAG_BADGE" not in test_setup.group(1)
        assert "FlagSet" not in test_setup.group(1)
