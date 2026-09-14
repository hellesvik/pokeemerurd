from pathlib import Path
import re


SCRIPT = Path("data/scripts/debug.inc").read_text()


def script_block(label: str) -> str:
    match = re.search(rf"^{label}::\n(.*?)(?=^[A-Za-z0-9_]+::|\Z)", SCRIPT, re.MULTILINE | re.DOTALL)
    assert match is not None
    return match.group(1)


def test_cheat_starts_do_not_grant_badges_or_key_items():
    for label in ("Debug_CheatStart", "Debug_CheatStartFrlg"):
        block = script_block(label)
        assert "setflag FLAG_BADGE" not in block
        assert "additem ITEM_" not in block
