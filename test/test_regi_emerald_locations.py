import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def test_white_emerald_is_in_mossdeep_and_purple_is_on_mt_pyre():
    route_117 = json.loads((ROOT / "data/maps/Route117/map.json").read_text())
    mt_pyre = json.loads((ROOT / "data/maps/MtPyre_Exterior/map.json").read_text())
    route_items = {
        event.get("item")
        for event in route_117["bg_events"]
        if event.get("type") == "hidden_item"
    }
    mossdeep_script = (ROOT / "data/maps/MossdeepCity/scripts.inc").read_text()
    mt_pyre_items = {
        event.get("item")
        for event in mt_pyre["bg_events"]
        if event.get("type") == "hidden_item"
    }

    assert "ITEM_REGI_EMERALD_PURPLE" not in route_items
    assert "MossdeepCity_EventScript_WhiteRock::" in mossdeep_script
    assert "giveitem ITEM_REGI_EMERALD_WHITE" in mossdeep_script
    assert "setflag FLAG_HIDDEN_ITEM_ROUTE_117_REPEL" in mossdeep_script
    assert "ITEM_REGI_EMERALD_PURPLE" in mt_pyre_items
    assert "ITEM_REGI_EMERALD_WHITE" not in mt_pyre_items


def test_white_and_purple_emerald_clues_match_their_swapped_locations():
    braille = (ROOT / "data/text/braille.inc").read_text()

    assert r'.braille "WHITE IS IN THE\n"' in braille
    assert r'.braille "WHITE ROCK OUT\n"' in braille
    assert '.braille "OF PLACE.$"' in braille
    assert r'.braille "PURPLE SHINES\n"' in braille
    assert r'.braille "ON THE MOUNTAIN\n"' in braille
    assert '.braille "WHERE THE DEAD REST.$"' in braille
