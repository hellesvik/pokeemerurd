from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

REWARD_TEXTS = {
    "data/maps/RustboroCity_Gym/scripts.inc": "RustboroCity_Gym_Text_ExplainRockTomb",
    "data/maps/DewfordTown_Gym/scripts.inc": "DewfordTown_Gym_Text_ExplainBulkUp",
    "data/maps/MauvilleCity_Gym/scripts.inc": "MauvilleCity_Gym_Text_ExplainShockWave",
    "data/maps/LavaridgeTown_Gym_1F/scripts.inc": "LavaridgeTown_Gym_1F_Text_ExplainOverheat",
    "data/maps/PetalburgCity_Gym/scripts.inc": "PetalburgCity_Gym_Text_ExplainFacade",
    "data/maps/FortreeCity_Gym/scripts.inc": "FortreeCity_Gym_Text_ExplainAerialAce",
    "data/maps/MossdeepCity_Gym/scripts.inc": "MossdeepCity_Gym_Text_ExplainCalmMind",
    "data/maps/SootopolisCity_Gym_1F/scripts.inc": "SootopolisCity_Gym_1F_Text_ExplainWaterPulse",
}


def test_every_gym_leader_names_the_item_actually_awarded():
    for relative_path, label in REWARD_TEXTS.items():
        source = (ROOT / relative_path).read_text()
        text = source.split(f"{label}:", 1)[1].split("\n\n", 1)[0]

        assert "{STR_VAR_1}" in text, relative_path
        assert f"bufferitemname STR_VAR_1, VAR_0x8000\n\tmsgbox {label}" in source, relative_path
