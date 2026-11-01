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

BIOMES = ("City", "Grassland", "Forest", "Mountain", "Cave", "Desert", "Volcanic", "Snow and Ice", "Marsh and Swamp", "Freshwater", "Ocean", "Beach and Coast", "Underwater")
BIOME_ENUM = {biome: "FORK_BIOME_" + biome.upper().replace(" ", "_").replace("AND", "") for biome in BIOMES}
METHODS = {
    "land_mons": ("WILD_AREA_LAND", "FORK_LAND_RANDOMIZED_SLOT_COUNT"),
    "water_mons": ("WILD_AREA_WATER", "FORK_WATER_RANDOMIZED_SLOT_COUNT"),
    "rock_smash_mons": ("WILD_AREA_ROCKS", "FORK_ROCK_RANDOMIZED_SLOT_COUNT"),
    "fishing_mons": ("WILD_AREA_FISHING", "FORK_FISHING_RANDOMIZED_SLOT_COUNT"),
}


def biome_for(name: str, method: str) -> str:
    upper = name.upper()
    if "UNDERWATER" in upper:
        return "Underwater"
    if "SHOAL_CAVE" in upper:
        return "Snow and Ice"
    if "LAVARIDGE_TOWN" in upper:
        return "Volcanic"
    if any(token in upper for token in ("DEWFORD_TOWN", "LILYCOVE_CITY", "MOSSDEEP_CITY", "SOOTOPOLIS_CITY")):
        return "Beach and Coast"
    if "EVER_GRANDE_CITY" in upper:
        return "Mountain"
    if "PACIFIDLOG_TOWN" in upper:
        return "Ocean"
    if "FORTREE_CITY" in upper:
        return "Forest"
    if "NEW_MAUVILLE" in upper:
        return "City"
    if "_TOWN" in upper or "_CITY" in upper:
        return "City"
    if method in ("water_mons", "fishing_mons"):
        if any(token in upper for token in ("ROUTE105", "ROUTE106", "ROUTE107", "ROUTE108", "ROUTE109", "ROUTE124", "ROUTE125", "ROUTE126", "ROUTE127", "ROUTE128", "ROUTE129", "ROUTE130", "ROUTE131", "ROUTE132", "ROUTE133", "ROUTE134", "PACIFIDLOG", "DEWFORD", "SLATEPORT", "LILYCOVE", "MOSSDEEP", "SOOTOPOLIS", "EVER_GRANDE", "SEAFLOOR", "UNDERWATER", "ABANDONED_SHIP", "MARINE_CAVE", "NAVEL_ROCK", "BIRTH_ISLAND")):
            return "Ocean"
        return "Freshwater"
    if any(token in upper for token in ("DESERT", "ROUTE111", "MIRAGE_TOWER")):
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
        (("ROUTE101", "LITTLEROOT", "OLDALE"), (150, 250)),
        (("ROUTE103",), (160, 260)),
        (("ROUTE102",), (170, 270)),
        (("ROUTE104", "RUSTBORO_CITY", "PETALBURG_WOODS"), (180, 280)),
        (("ROUTE116", "RUSTURF_TUNNEL"), (190, 290)),
        (("ROUTE105", "ROUTE106", "DEWFORD"), (200, 300)),
        (("GRANITE_CAVE",), (210, 310)),
        (("ROUTE107", "ROUTE108", "ROUTE109"), (220, 320)),
        (("SLATEPORT_CITY",), (230, 330)),
        (("ROUTE110",), (240, 340)),
        (("ROUTE117", "MAUVILLE_CITY", "VERDANTURF_TOWN"), (250, 350)),
        (("ROUTE111", "ROUTE112", "FIERY_PATH", "MIRAGE_TOWER"), (260, 360)),
        (("ROUTE113", "FALLARBOR_TOWN"), (270, 370)),
        (("ROUTE114",), (280, 380)),
        (("METEOR_FALLS", "ROUTE115"), (290, 390)),
        (("JAGGED_PASS", "LAVARIDGE_TOWN", "PETALBURG_CITY"), (300, 400)),
        (("ROUTE118",), (310, 410)),
        (("NEW_MAUVILLE", "ABANDONED_SHIP"), (310, 410)),
        (("ROUTE119", "SAFARI_ZONE", "FORTREE_CITY"), (320, 420)),
        (("ROUTE120",), (330, 430)),
        (("ROUTE121", "ROUTE122", "MT_PYRE"), (340, 440)),
        (("ROUTE123", "LILYCOVE"), (350, 450)),
        (("ROUTE124", "UNDERWATER_ROUTE124", "MAGMA_HIDEOUT"), (360, 460)),
        (("MOSSDEEP_CITY",), (370, 470)),
        (("ROUTE125", "SHOAL_CAVE"), (380, 480)),
        (("ROUTE127",), (390, 490)),
        (("ROUTE128",), (400, 500)),
        (("SEAFLOOR_CAVERN",), (410, 510)),
        (("ROUTE126", "UNDERWATER_ROUTE126"), (420, 520)),
        (("CAVE_OF_ORIGIN", "SOOTOPOLIS_CITY"), (430, 530)),
        (("ROUTE129",), (440, 540)),
        (("ROUTE130",), (450, 550)),
        (("ROUTE131", "ROUTE132", "ROUTE133", "ROUTE134", "SKY_PILLAR", "PACIFIDLOG"), (460, 560)),
        (("EVER_GRANDE",), (470, 560)),
        (("ARTISAN_CAVE", "ALTERING_CAVE", "DESERT_UNDERPASS"), (490, 560)),
    )
    for tokens, bst_range in ranges:
        if any(token in upper for token in tokens):
            return bst_range
    return 450, 550


def bst_range_for_encounter(name: str, method: str) -> tuple[int, int]:
    min_bst, max_bst = bst_range_for(name)
    if name == "MAP_ROUTE103" and method == "fishing_mons":
        max_bst = 270
    if "SHOAL_CAVE" in name and method in ("water_mons", "fishing_mons"):
        min_bst, max_bst = 290, 535
    return min_bst, max_bst


def kanto_range(name: str) -> tuple[int, int]:
    # Exact route matching avoids confusing Route 1 with Routes 10-19/101.
    route = name.removeprefix("MAP_ROUTE").split("_")[0]
    if route.isdigit():
        n = int(route)
        bands = {1:150, 2:170, 22:170, 3:190, 4:210, 24:220, 25:230,
                 5:240, 6:250, 11:250, 9:260, 10:270, 7:280, 8:280,
                 12:320, 13:330, 14:340, 15:350, 16:320, 17:330, 18:340,
                 19:380, 20:390, 21:400, 23:470}
        if n in bands:
            lo = bands[n]
            return lo, min(lo + 100, 560)
    for tokens, lo in (
        (("PALLET", "VIRIDIAN_CITY"), 150),
        (("VIRIDIAN_FOREST", "PEWTER"), 180),
        (("MT_MOON",), 200), (("CERULEAN_CITY",), 220),
        (("DIGLETTS", "VERMILION", "SSANNE"), 250),
        (("ROCK_TUNNEL",), 270), (("LAVENDER", "POKEMON_TOWER"), 290),
        (("CELADON", "ROCKET_HIDEOUT"), 300),
        (("SAFFRON", "SILPH"), 340), (("FUCHSIA", "SAFARI"), 350),
        (("POWER_PLANT",), 370), (("SEAFOAM",), 390),
        (("CINNABAR", "POKEMON_MANSION"), 420),
        (("VICTORY_ROAD", "INDIGO"), 490), (("CERULEAN_CAVE",), 490),
        (("ONE_ISLAND", "TWO_ISLAND", "THREE_ISLAND", "MT_EMBER"), 440),
    ):
        if any(token in name for token in tokens):
            return lo, min(lo + 100, 560)
    return 460, 560  # Postgame Sevii locations.


def kanto_water_range(name: str) -> tuple[int, int]:
    lo, hi = kanto_range(name)
    # Six distinct fishing species must fit even with only Gen 3 enabled.
    return max(150, lo - 100), max(400, hi)


def kanto_biome(name: str, method: str) -> str:
    if method in ("water_mons", "fishing_mons"):
        return "Freshwater" if any(x in name for x in ("CERULEAN", "SAFARI", "FUCHSIA", "ROUTE24", "ROUTE25", "ROUTE6", "FOUR_ISLAND")) else "Ocean"
    if any(x in name for x in ("SEAFOAM", "ICEFALL")):
        return "Snow and Ice"
    if "MT_EMBER" in name:
        return "Volcanic"
    if "SAFARI" in name:
        return "Marsh and Swamp"
    if "FOREST" in name:
        return "Forest"
    if any(x in name for x in ("CAVE", "TUNNEL", "CHAMBER", "MT_MOON", "VICTORY_ROAD")):
        return "Cave"
    if any(x in name for x in ("CITY", "TOWN", "MANSION", "TOWER", "POWER_PLANT")):
        return "City"
    return "Grassland"


def main() -> None:
    pools: dict[str, list[str]] = defaultdict(list)
    with CATALOG.open(newline="") as catalog:
        for row in csv.DictReader(catalog):
            pools[row["biome"]].append(row["species_constant"])
    encounter_data = json.loads(ENCOUNTERS.read_text())["wild_encounter_groups"][0]["encounters"]
    assignments = []
    for entry in encounter_data:
        kanto = "FireRed" in entry["base_label"] or "LeafGreen" in entry["base_label"]
        for field, (area, slots) in METHODS.items():
            if field in entry:
                row = (entry["map"], area,
                       kanto_biome(entry["map"], field) if kanto else biome_for(entry["map"], field),
                       *((kanto_water_range(entry["map"]) if field in ("water_mons", "fishing_mons") else kanto_range(entry["map"])) if kanto else bst_range_for_encounter(entry["map"], field)), slots)
                if kanto and field == "rock_smash_mons":
                    row = (*row[:3], min(row[3], 300), row[4], row[5])
                if row not in assignments:
                    assignments.append(row)
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
