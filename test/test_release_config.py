from pathlib import Path


GENERAL_CONFIG = Path("include/config/general.h").read_text()
BATTLE_CONFIG = Path("include/config/battle.h").read_text()


def test_playtesting_cheats_are_disabled_for_release():
    for setting in (
        "TEST_START_WITH_RAYQUAZA",
        "TEST_FLY_DESTINATIONS_UNRESTRICTED",
        "TEST_DISABLE_OBEDIENCE",
        "TEST_START_WITH_EMERALD_KEY_ITEMS",
        "TEST_START_WITH_ALL_HM_ACCESS",
    ):
        assert f"#define {setting} FALSE" in GENERAL_CONFIG


def test_affection_battle_bonuses_are_disabled():
    assert "#define B_AFFECTION_MECHANICS           FALSE" in BATTLE_CONFIG
