from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def simon_party():
    source = (ROOT / "src/data/trainers.party").read_text()
    start = source.index("=== TRAINER_SIMON ===")
    end = source.index("===", start + len("=== TRAINER_SIMON ==="))
    return source[start:end]


def test_tuber_simon_has_easter_egg_team():
    party = simon_party()
    expected = [
        ("Azurill", "Cute Charm", 20, None,
         ["Attract", "Minimize", "Fake Tears", "Draining Kiss"]),
        ("Riolu", "Prankster", 1, "Focus Sash",
         ["Endeavor", "Bullet Punch"]),
        ("Magby", "Flame Body", 19, "Magmarizer",
         ["Fire Blast", "Flare Blitz", "Blast Burn", "Yawn"]),
        ("Elekid", "Static", 19, "Electirizer",
         ["Thunder", "Volt Tackle", "Bolt Strike", "Yawn"]),
        ("Happiny", "Natural Cure", 20, "Quick Claw",
         ["Final Gambit"]),
    ]

    for species, ability, level, item, moves in expected:
        heading = species if item is None else f"{species} @ {item}"
        assert heading in party
        assert f"Ability: {ability}" in party
        assert f"Level: {level}" in party
        for move in moves:
            assert f"- {move}" in party


def test_tuber_simon_uses_easter_egg_quote():
    script = (ROOT / "data/maps/Route109_SeashoreHouse/scripts.inc").read_text()
    start = script.index("Route109_SeashoreHouse_Text_SimonIntro:")
    end = script.index("Route109_SeashoreHouse_Text_SimonDefeated:", start)
    intro = script[start:end]

    assert '"Hei fer jussibassijussibassi\\n"' in intro
    assert '"jom HEI SKAAL!$"' in intro
