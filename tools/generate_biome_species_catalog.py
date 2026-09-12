#!/usr/bin/env python3
"""Generate the biome encounter species catalog and its ODT rendition.

This catalog deliberately uses canonical National Dex species, rather than
cosmetic, battle-only, or regional form entries. The generated CSV is the
implementation-oriented source of truth for Pokémon-to-biome assignments.
"""

from __future__ import annotations

import csv
import re
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SPECIES_INFO_DIR = REPO_ROOT / "src/data/pokemon/species_info"
OUTPUT_CSV = REPO_ROOT / "docs/fork/reference/biome_species_catalog.csv"


# The species data records Mythicals, Ultra Beasts and Paradox Pokémon with
# flags.  It has no equivalent Legendary flag, so Legendary species are kept
# here explicitly.  Forms resolve to their base species before this list is
# applied.
LEGENDARY_SPECIES = {
    "ARTICUNO", "ZAPDOS", "MOLTRES", "MEWTWO",
    "RAIKOU", "ENTEI", "SUICUNE", "LUGIA", "HO_OH",
    "REGIROCK", "REGICE", "REGISTEEL", "LATIAS", "LATIOS", "KYOGRE", "GROUDON", "RAYQUAZA",
    "UXIE", "MESPRIT", "AZELF", "DIALGA", "PALKIA", "HEATRAN", "REGIGIGAS", "GIRATINA", "CRESSELIA",
    "COBALION", "TERRAKION", "VIRIZION", "TORNADUS", "THUNDURUS", "RESHIRAM", "ZEKROM", "LANDORUS", "KYUREM",
    "XERNEAS", "YVELTAL", "ZYGARDE",
    "TYPE_NULL", "SILVALLY", "COSMOG", "COSMOEM", "SOLGALEO", "LUNALA", "TAPU_KOKO", "TAPU_LELE", "TAPU_BULU", "TAPU_FINI", "NECROZMA",
    "KUBFU", "URSHIFU", "ZACIAN", "ZAMAZENTA", "ETERNATUS", "REGIELEKI", "REGIDRAGO", "GLASTRIER", "SPECTRIER", "CALYREX", "ENAMORUS",
    "WO_CHIEN", "CHIEN_PAO", "TING_LU", "CHI_YU", "KORAIDON", "MIRAIDON", "OGERPON", "TERAPAGOS",
}


# A species belongs to the union of the biomes associated with its one or two
# types. This deliberately broad first-pass assignment gives later encounter
# table curation a defined, auditable candidate set without claiming that all
# listed species will occupy every route or every encounter method.
BIOME_ORDER = (
    "City",
    "Grassland",
    "Forest",
    "Mountain",
    "Cave",
    "Desert",
    "Volcanic",
    "Snow and Ice",
    "Marsh and Swamp",
    "Freshwater",
    "Ocean",
    "Beach and Coast",
)

TYPE_BIOMES = {
    "NORMAL": ("City", "Grassland", "Forest", "Mountain"),
    "FIRE": ("Volcanic", "Mountain", "Desert"),
    "WATER": ("City", "Freshwater", "Ocean", "Beach and Coast", "Marsh and Swamp"),
    "ELECTRIC": ("City", "Grassland", "Mountain"),
    "GRASS": ("Grassland", "Forest", "Marsh and Swamp"),
    "ICE": ("Snow and Ice", "Mountain"),
    "FIGHTING": ("City", "Mountain", "Grassland"),
    "POISON": ("City", "Marsh and Swamp", "Cave", "Forest"),
    "GROUND": ("Desert", "Mountain", "Cave"),
    "FLYING": ("City", "Grassland", "Forest", "Mountain", "Beach and Coast"),
    "PSYCHIC": ("City", "Forest", "Mountain", "Cave"),
    "BUG": ("City", "Grassland", "Forest", "Marsh and Swamp"),
    "ROCK": ("Cave", "Mountain", "Desert", "Beach and Coast"),
    "GHOST": ("City", "Cave", "Forest", "Marsh and Swamp"),
    "DRAGON": ("Mountain", "Cave", "Ocean"),
    "DARK": ("City", "Cave", "Forest", "Marsh and Swamp"),
    "STEEL": ("City", "Cave", "Mountain"),
    "FAIRY": ("City", "Forest", "Grassland"),
}


# A small number of families use C helper macros for their full initializer or
# for their type expression. They are valid canonical species, but cannot be
# recovered from an initializer-only parser. Keep their published base types
# here so the catalog remains complete while retaining a simple, robust reader
# for the ordinary species records.
MACRO_INITIALIZER_SPECIES = {
    "NATIONAL_DEX_ALCREMIE": ("ALCREMIE", "Alcremie", ("FAIRY",), False),
    "NATIONAL_DEX_ARCEUS": ("ARCEUS", "Arceus", ("NORMAL",), True),
    "NATIONAL_DEX_CLEFAIRY": ("CLEFAIRY", "Clefairy", ("FAIRY",), False),
    "NATIONAL_DEX_CLEFFA": ("CLEFFA", "Cleffa", ("FAIRY",), False),
    "NATIONAL_DEX_FLABEBE": ("FLABEBE", "Flabébé", ("FAIRY",), False),
    "NATIONAL_DEX_FLORGES": ("FLORGES", "Florges", ("FAIRY",), False),
    "NATIONAL_DEX_FURFROU": ("FURFROU", "Furfrou", ("NORMAL",), False),
    "NATIONAL_DEX_GENESECT": ("GENESECT", "Genesect", ("BUG", "STEEL"), True),
    "NATIONAL_DEX_IGGLYBUFF": ("IGGLYBUFF", "Igglybuff", ("NORMAL", "FAIRY"), False),
    "NATIONAL_DEX_JIGGLYPUFF": ("JIGGLYPUFF", "Jigglypuff", ("NORMAL", "FAIRY"), False),
    "NATIONAL_DEX_MINIOR": ("MINIOR", "Minior", ("ROCK", "FLYING"), False),
    "NATIONAL_DEX_MOTHIM": ("MOTHIM", "Mothim", ("BUG", "FLYING"), False),
    "NATIONAL_DEX_OGERPON": ("OGERPON", "Ogerpon", ("GRASS",), True),
    "NATIONAL_DEX_SCATTERBUG": ("SCATTERBUG", "Scatterbug", ("BUG",), False),
    "NATIONAL_DEX_SPEWPA": ("SPEWPA", "Spewpa", ("BUG",), False),
    "NATIONAL_DEX_SILVALLY": ("SILVALLY", "Silvally", ("NORMAL",), True),
    "NATIONAL_DEX_TOGEPI": ("TOGEPI", "Togepi", ("FAIRY",), False),
    "NATIONAL_DEX_UNOWN": ("UNOWN", "Unown", ("PSYCHIC",), False),
    "NATIONAL_DEX_VIVILLON": ("VIVILLON", "Vivillon", ("BUG", "FLYING"), False),
    "NATIONAL_DEX_WIGGLYTUFF": ("WIGGLYTUFF", "Wigglytuff", ("NORMAL", "FAIRY"), False),
}


@dataclass(frozen=True)
class Species:
    constant: str
    name: str
    national_dex: str
    types: tuple[str, ...]
    excluded: bool


def c_initializer_blocks(text: str):
    """Yield SPECIES_* constant names and their complete brace blocks."""
    # Restrict this to explicit brace initializers. Without the brace, a macro
    # initializer (such as an Unown form) could incorrectly consume the next
    # ordinary species block.
    pattern = re.compile(r"^[ \t]*\[SPECIES_([A-Z0-9_]+)\][ \t]*=[ \t]*\r?\n[ \t]*\{", re.MULTILINE)
    for match in pattern.finditer(text):
        start = match.end() - 1
        depth = 0
        for index in range(start, len(text)):
            if text[index] == "{":
                depth += 1
            elif text[index] == "}":
                depth -= 1
                if depth == 0:
                    yield match.group(1), text[start:index + 1]
                    break


def get_field(pattern: str, block: str) -> str | None:
    match = re.search(pattern, block, re.DOTALL)
    return match.group(1) if match else None


def read_canonical_species() -> list[Species]:
    by_national_dex: dict[str, Species] = {}
    for path in sorted(SPECIES_INFO_DIR.glob("gen_*_families.h")):
        for constant, block in c_initializer_blocks(path.read_text()):
            national_dex = get_field(r"\.natDexNum\s*=\s*(NATIONAL_DEX_[A-Z0-9_]+)", block)
            name = get_field(r'\.speciesName\s*=\s*_\("([^"]+)"\)', block)
            types_field = get_field(r"\.types\s*=\s*MON_TYPES\(([^)]*)\)", block)
            if not national_dex or not name or not types_field or national_dex in by_national_dex:
                continue
            types = tuple(re.findall(r"TYPE_([A-Z]+)", types_field))
            if not types:
                continue
            excluded = (
                constant in LEGENDARY_SPECIES
                or ".isMythical = TRUE" in block
                or ".isUltraBeast = TRUE" in block
                or ".isParadox = TRUE" in block
            )
            by_national_dex[national_dex] = Species(constant, name, national_dex, types, excluded)
    for national_dex, (constant, name, types, excluded) in MACRO_INITIALIZER_SPECIES.items():
        by_national_dex.setdefault(national_dex, Species(constant, name, national_dex, types, excluded))
    return sorted(by_national_dex.values(), key=lambda species: int(species.national_dex.rsplit("_", 1)[-1]) if species.national_dex.rsplit("_", 1)[-1].isdigit() else species.name)


def species_biomes(species: Species) -> tuple[str, ...]:
    biomes = {biome for species_type in species.types for biome in TYPE_BIOMES[species_type]}
    return tuple(biome for biome in BIOME_ORDER if biome in biomes)


def write_csv(species: list[Species]) -> int:
    eligible = [entry for entry in species if not entry.excluded]
    entries_by_biome: dict[str, list[Species]] = defaultdict(list)
    for entry in eligible:
        for biome in species_biomes(entry):
            entries_by_biome[biome].append(entry)

    uncovered = [entry for entry in eligible if not species_biomes(entry)]
    if uncovered:
        raise RuntimeError(f"Eligible species without a biome: {uncovered}")
    with OUTPUT_CSV.open("w", newline="") as output:
        writer = csv.writer(output, lineterminator="\n")
        writer.writerow(("biome", "species_constant", "species_name", "national_dex", "types"))
        for biome in BIOME_ORDER:
            for entry in sorted(entries_by_biome[biome], key=lambda entry: entry.name.casefold()):
                writer.writerow((biome, f"SPECIES_{entry.constant}", entry.name, entry.national_dex, "/".join(entry.types)))
    return len(eligible)


def main() -> int:
    species = read_canonical_species()
    if not species:
        raise RuntimeError("No canonical species could be read")
    eligible_count = write_csv(species)
    print(f"Wrote {OUTPUT_CSV.relative_to(REPO_ROOT)} ({eligible_count} eligible canonical species).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
