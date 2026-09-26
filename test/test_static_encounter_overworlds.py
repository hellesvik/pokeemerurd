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
