#!/usr/bin/env python3
"""Generate the compiled biome encounter pools and explicit map assignments."""

from __future__ import annotations

import csv
import json
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / "docs/fork/reference/biome_species_catalog.csv"
ENCOUNTERS = ROOT / "src/data/wild_encounters.json"
OUTPUT = ROOT / "src/data/fork_biome_encounter_data.h"
ASSIGNMENTS_CSV = ROOT / "docs/fork/reference/biome_encounter_assignments.csv"

BIOMES = ("Grassland", "Forest", "Mountain", "Cave", "Desert", "Volcanic", "Snow and Ice", "Marsh and Swamp", "Freshwater", "Ocean", "Beach and Coast")
BIOME_ENUM = {biome: "FORK_BIOME_" + biome.upper().replace(" ", "_").replace("AND", "") for biome in BIOMES}
METHODS = {
    "land_mons": ("WILD_AREA_LAND", "LAND_WILD_COUNT"),
    "water_mons": ("WILD_AREA_WATER", "WATER_WILD_COUNT"),
    "rock_smash_mons": ("WILD_AREA_ROCKS", "ROCK_WILD_COUNT"),
    "fishing_mons": ("WILD_AREA_FISHING", "FISH_WILD_COUNT"),
}


def biome_for(name: str, method: str) -> str:
    upper = name.upper()
    if method in ("water_mons", "fishing_mons"):
        if any(token in upper for token in ("ROUTE105", "ROUTE106", "ROUTE107", "ROUTE108", "ROUTE109", "ROUTE124", "ROUTE125", "ROUTE126", "ROUTE127", "ROUTE128", "ROUTE129", "ROUTE130", "ROUTE131", "ROUTE132", "ROUTE133", "ROUTE134", "PACIFIDLOG", "DEWFORD", "SLATEPORT", "LILYCOVE", "MOSSDEEP", "SOOTOPOLIS", "EVER_GRANDE", "SEAFLOOR", "UNDERWATER", "ABANDONED_SHIP", "MARINE_CAVE", "NAVEL_ROCK", "BIRTH_ISLAND")):
            return "Ocean"
        return "Freshwater"
    if any(token in upper for token in ("DESERT", "ROUTE111")):
        return "Desert"
    if any(token in upper for token in ("FIERY", "LAVARIDGE", "CHIMNEY", "JAGGED", "ROUTE113", "VOLCANO")):
        return "Volcanic"
    if any(token in upper for token in ("CAVE", "CAVERN", "TUNNEL", "UNDERWATER", "SEAFLOOR", "ORIGIN", "SHOAL", "VICTORY_ROAD", "GRANITE", "METEOR_FALLS", "ALTERING")):
        return "Cave"
    if any(token in upper for token in ("MT_", "SKY_PILLAR", "ROUTE112", "ROUTE114", "ROUTE115", "ROUTE128")):
        return "Mountain"
    if any(token in upper for token in ("SNOW", "GLACIER", "ICEFALL")):
        return "Snow and Ice"
    if any(token in upper for token in ("MARSH", "SWAMP", "SAFARI", "ROUTE120", "ROUTE121", "ROUTE122")):
        return "Marsh and Swamp"
    if any(token in upper for token in ("WOODS", "FOREST", "ROUTE119", "PETALBURG")):
        return "Forest"
    if any(token in upper for token in ("BEACH", "ROUTE104", "ROUTE109", "ROUTE110", "ROUTE118", "ROUTE123", "SHORE")):
        return "Beach and Coast"
    return "Grassland"


def bst_range_for(name: str) -> tuple[int, int]:
    upper = name.upper()
    if "VICTORY_ROAD" in upper:
        return 490, 560
    ranges = (
        (("ROUTE101", "PETALBURG_CITY"), (150, 250)),
        (("ROUTE102",), (160, 260)),
        (("ROUTE103",), (170, 270)),
        (("ROUTE104", "PETALBURG_WOODS"), (180, 280)),
        (("ROUTE105",), (190, 290)),
        (("ROUTE106", "DEWFORD"), (200, 300)),
        (("ROUTE107", "ROUTE108", "ROUTE109"), (210, 310)),
        (("GRANITE_CAVE",), (220, 320)),
        (("ROUTE110",), (230, 330)),
        (("ROUTE111",), (240, 340)),
        (("ROUTE112", "FIERY_PATH", "JAGGED_PASS"), (250, 350)),
        (("ROUTE113",), (260, 360)),
        (("ROUTE114", "RUSTURF_TUNNEL"), (270, 370)),
        (("ROUTE115",), (280, 380)),
        (("ROUTE116",), (290, 390)),
        (("ROUTE117", "NEW_MAUVILLE"), (300, 400)),
        (("ROUTE118",), (310, 410)),
        (("ROUTE119", "SAFARI_ZONE"), (320, 420)),
        (("ROUTE120", "MT_PYRE"), (330, 430)),
        (("ROUTE121", "ROUTE122"), (340, 440)),
        (("ROUTE123", "LILYCOVE"), (350, 450)),
        (("ABANDONED_SHIP", "SLATEPORT"), (360, 460)),
        (("ROUTE124", "ROUTE125"), (370, 470)),
        (("SHOAL_CAVE",), (380, 480)),
        (("ROUTE126", "UNDERWATER_ROUTE126", "MOSSDEEP"), (390, 490)),
        (("METEOR_FALLS",), (400, 500)),
        (("ROUTE127",), (410, 510)),
        (("ROUTE128", "SEAFLOOR_CAVERN"), (420, 520)),
        (("ROUTE129",), (430, 530)),
        (("ROUTE130",), (440, 540)),
        (("ROUTE131", "SKY_PILLAR", "EVER_GRANDE"), (450, 550)),
        (("ROUTE132", "ROUTE133", "ROUTE134", "CAVE_OF_ORIGIN", "ARTISAN_CAVE", "SOOTOPOLIS"), (450, 550)),
        (("MAGMA_HIDEOUT",), (400, 500)),
        (("MIRAGE_TOWER", "DESERT_UNDERPASS"), (260, 360)),
        (("PACIFIDLOG",), (360, 460)),
    )
    for tokens, bst_range in ranges:
        if any(token in upper for token in tokens):
            return bst_range
    return 450, 550


def main() -> None:
    pools: dict[str, list[str]] = defaultdict(list)
    with CATALOG.open(newline="") as catalog:
        for row in csv.DictReader(catalog):
            pools[row["biome"]].append(row["species_constant"])
    encounter_data = json.loads(ENCOUNTERS.read_text())["wild_encounter_groups"][0]["encounters"]
    assignments = []
    for entry in encounter_data:
        # The common source contains FireRed/LeafGreen entries behind #ifdefs.
        if "FireRed" in entry["base_label"] or "LeafGreen" in entry["base_label"]:
            continue
        for field, (area, slots) in METHODS.items():
            if field in entry:
                assignments.append((entry["map"], area, biome_for(entry["map"], field), *bst_range_for(entry["map"]), slots))
    lines = ["// Auto-generated by tools/generate_biome_encounter_data.py.", ""]
    for biome in BIOMES:
        identifier = BIOME_ENUM[biome].replace("FORK_BIOME_", "")
        lines.append(f"static const enum Species sForkBiomePool_{identifier}[] =")
        lines.append("{")
        lines.extend(f"    {species}," for species in sorted(set(pools[biome])))
        lines.extend(["};", ""])
    lines.append("static const struct ForkEncounterAssignment sForkEncounterAssignments[] =")
    lines.append("{")
    for map_name, area, biome, min_bst, max_bst, slots in assignments:
        lines.append(f"    {{ MAP_GROUP({map_name}), MAP_NUM({map_name}), {area}, {BIOME_ENUM[biome]}, {min_bst}, {max_bst}, {slots} }},")
    lines.extend(["};", ""])
    OUTPUT.write_text("\n".join(lines))
    with ASSIGNMENTS_CSV.open("w", newline="") as output:
        writer = csv.writer(output, lineterminator="\n")
        writer.writerow(("map", "encounter_method", "biome", "min_bst", "max_bst", "slot_count"))
        for map_name, area, biome, min_bst, max_bst, slots in assignments:
            writer.writerow((map_name, area, biome, min_bst, max_bst, slots))
    print(f"Wrote {OUTPUT.relative_to(ROOT)} and {ASSIGNMENTS_CSV.relative_to(ROOT)} with {len(assignments)} explicit encounter-table assignments.")


if __name__ == "__main__":
    main()
