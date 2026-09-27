import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def map_data(name):
    return json.loads((ROOT / "data/maps" / name / "map.json").read_text())


def script(name):
    return (ROOT / "data/maps" / name / "scripts.inc").read_text()


def test_regi_encounter_objects_use_the_randomized_species_graphics_variable():
    encounters = {
        "DesertRuins": "SPECIES_REGIROCK",
        "IslandCave": "SPECIES_REGICE",
        "AncientTomb": "SPECIES_REGISTEEL",
    }
    for map_name, species in encounters.items():
        assert map_data(map_name)["object_events"][0]["graphics_id"] == "OBJ_EVENT_GFX_VAR_0"
        source = script(map_name)
        assert f"setvar VAR_0x8004, {species}" in source
        assert "specialvar VAR_OBJ_GFX_ID_0, GetForkRandomizedStaticEncounterGraphicsId" in source
        assert "specialvar VAR_RESULT, GetForkRandomizedStaticEncounterSpeciesForScript" in source
        assert "playmoncry VAR_RESULT, CRY_MODE_ENCOUNTER" in source


def test_only_rayquazas_final_interactive_object_is_randomized():
    objects = map_data("SkyPillar_Top")["object_events"]
    assert objects[0]["graphics_id"] == "OBJ_EVENT_GFX_RAYQUAZA"
    assert objects[1]["graphics_id"] == "OBJ_EVENT_GFX_VAR_0"

    source = script("SkyPillar_Top")
    assert "specialvar VAR_OBJ_GFX_ID_0, GetForkRandomizedStaticEncounterGraphicsId" in source
    encounter = source[source.index("SkyPillar_Top_EventScript_Rayquaza::"):]
    assert "specialvar VAR_RESULT, GetForkRandomizedStaticEncounterSpeciesForScript" in encounter
    assert "playmoncry VAR_RESULT, CRY_MODE_ENCOUNTER" in encounter


def test_randomized_legendary_static_encounters_are_consumed_before_battle():
    encounters = {
        "DesertRuins": ("FLAG_DEFEATED_REGIROCK", "StartRegiBattle"),
        "IslandCave": ("FLAG_DEFEATED_REGICE", "StartRegiBattle"),
        "AncientTomb": ("FLAG_DEFEATED_REGISTEEL", "StartRegiBattle"),
        "SkyPillar_Top": ("FLAG_DEFEATED_RAYQUAZA", "BattleSetup_StartLegendaryBattle"),
        "TerraCave_End": ("FLAG_DEFEATED_GROUDON", "BattleSetup_StartLegendaryBattle"),
        "MarineCave_End": ("FLAG_DEFEATED_KYOGRE", "BattleSetup_StartLegendaryBattle"),
    }

    for map_name, (defeated_flag, battle_special) in encounters.items():
        source = script(map_name)
        battle_start = source.index(f"special {battle_special}")
        preceding_script = source[:battle_start]
        assert preceding_script.rfind(f"setflag {defeated_flag}") > preceding_script.rfind("setwildbattle ")
