from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "data/maps/MossdeepCity_StevensHouse/scripts.inc"


def test_steven_gift_unlocks_after_eighth_badge_and_does_not_respawn():
    text = SCRIPT.read_text()

    assert (
        "call_if_set FLAG_BADGE08_GET, "
        "MossdeepCity_StevensHouse_EventScript_UpdateGiftVisibility"
    ) in text
    assert (
        "MossdeepCity_StevensHouse_EventScript_UpdateGiftVisibility::\n"
        "\tgoto_if_set FLAG_RECEIVED_BELDUM, "
        "MossdeepCity_StevensHouse_EventScript_KeepGiftHidden\n"
        "\tclearflag FLAG_HIDE_MOSSDEEP_CITY_STEVENS_HOUSE_BELDUM_POKEBALL"
    ) in text
    assert (
        "MossdeepCity_StevensHouse_EventScript_KeepGiftHidden::\n"
        "\tsetflag FLAG_HIDE_MOSSDEEP_CITY_STEVENS_HOUSE_BELDUM_POKEBALL"
    ) in text


def test_steven_note_uses_eighth_badge_instead_of_game_clear():
    text = SCRIPT.read_text()
    on_load = text.split("MossdeepCity_StevensHouse_OnLoad:", 1)[1].split("\n\n", 1)[0]

    assert "FLAG_BADGE08_GET" in on_load
    assert "FLAG_SYS_GAME_CLEAR" not in on_load
