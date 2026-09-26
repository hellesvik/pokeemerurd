from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRAINERS = (ROOT / "src/data/trainers.party").read_text()
CAPS = (ROOT / "src/caps.c").read_text()


def trainer_section(trainer):
    start = TRAINERS.index(f"=== {trainer} ===")
    end = TRAINERS.find("\n=== TRAINER_", start + 4)
    return TRAINERS[start:end if end >= 0 else None]


def test_mt_chimney_tabitha_uses_the_configured_three_pokemon_team():
    team = trainer_section("TRAINER_TABITHA_MT_CHIMNEY")

    expected = (
        "Hippopotas", "Ability: Sand Stream", "Level: 23",
        "- Dig", "- Rock Tomb", "- Sand Tomb", "- Slack Off",
        "Gible", "Ability: Sand Veil", "- Bulldoze", "- Dragon Rage",
        "- Fire Fang", "- Accelerock",
        "Combusken", "Ability: Speed Boost", "Level: 25",
        "- Vacuum Wave", "- Weather Ball", "- Incinerate", "- Air Cutter",
    )

    assert team.count("Level: 23") == 2
    for entry in expected:
        assert entry in team


def test_mt_chimney_tabitha_is_not_a_level_cap_battle():
    assert "TRAINER_TABITHA_MT_CHIMNEY" not in CAPS
