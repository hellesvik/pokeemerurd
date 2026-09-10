#!/usr/bin/env python3
"""Generate the player-facing evolution reference from species source data."""

from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "docs/fork/reference/pokemon_evolution_methods.md"

GENERATION_NAMES = {
    1: "Kanto",
    2: "Johto",
    3: "Hoenn",
    4: "Sinnoh",
    5: "Unova",
    6: "Kalos",
    7: "Alola",
    8: "Galar and Hisui",
    9: "Paldea",
}
GENERATION_ENDS = (151, 251, 386, 493, 649, 721, 809, 905, 1025)
REGIONS = {"ALOLA": ("Alolan", 7), "GALAR": ("Galarian", 8), "HISUI": ("Hisuian", 8), "PALDEA": ("Paldean", 9)}

DISPLAY_OVERRIDES = {
    "MR_MIME": "Mr. Mime", "MIME_JR": "Mime Jr.", "MR_RIME": "Mr. Rime",
    "FARFETCHD": "Farfetch'd", "SIRFETCHD": "Sirfetch'd", "HO_OH": "Ho-Oh",
    "PORYGON_Z": "Porygon-Z", "TYPE_NULL": "Type: Null", "FLABEBE": "Flabébé",
    "NIDORAN_F": "Nidoran♀", "NIDORAN_M": "Nidoran♂", "JANGMO_O": "Jangmo-o",
    "HAKAMO_O": "Hakamo-o", "KOMMO_O": "Kommo-o", "WO_CHIEN": "Wo-Chien",
    "CHIEN_PAO": "Chien-Pao", "TING_LU": "Ting-Lu", "CHI_YU": "Chi-Yu",
    "POKE_BALL": "Poké Ball", "LEADERS_CREST": "Leader's Crest",
    "KINGS_ROCK": "King's Rock",
    "SCROLL_OF_DARKNESS": "Scroll of Darkness", "SCROLL_OF_WATERS": "Scroll of Waters",
}

EVOLUTION_TARGET_OVERRIDES = {
    "SPECIES_MOTHIM_PLANT": "Mothim", "SPECIES_MOTHIM_SANDY": "Mothim", "SPECIES_MOTHIM_TRASH": "Mothim",
    "SPECIES_WORMADAM_PLANT": "Wormadam (Plant Cloak)", "SPECIES_WORMADAM_SANDY": "Wormadam (Sandy Cloak)", "SPECIES_WORMADAM_TRASH": "Wormadam (Trash Cloak)",
    "SPECIES_GASTRODON_EAST": "Gastrodon (East Sea)", "SPECIES_GASTRODON_WEST": "Gastrodon (West Sea)",
    "SPECIES_BASCULEGION_M": "Basculegion (Male)", "SPECIES_BASCULEGION_F": "Basculegion (Female)",
    "SPECIES_SAWSBUCK_SPRING": "Sawsbuck (Spring)", "SPECIES_SAWSBUCK_SUMMER": "Sawsbuck (Summer)",
    "SPECIES_SAWSBUCK_AUTUMN": "Sawsbuck (Autumn)", "SPECIES_SAWSBUCK_WINTER": "Sawsbuck (Winter)",
    "SPECIES_MEOWSTIC_M": "Meowstic (Male)", "SPECIES_MEOWSTIC_F": "Meowstic (Female)",
    "SPECIES_GOURGEIST_SMALL": "Gourgeist (Small)", "SPECIES_GOURGEIST_AVERAGE": "Gourgeist (Average)",
    "SPECIES_GOURGEIST_LARGE": "Gourgeist (Large)", "SPECIES_GOURGEIST_SUPER": "Gourgeist (Super Size)",
    "SPECIES_LYCANROC_MIDDAY": "Lycanroc (Midday Form)", "SPECIES_LYCANROC_MIDNIGHT": "Lycanroc (Midnight Form)",
    "SPECIES_LYCANROC_DUSK": "Lycanroc (Dusk Form)",
    "SPECIES_TOXTRICITY_AMPED": "Toxtricity (Amped Form)", "SPECIES_TOXTRICITY_LOW_KEY": "Toxtricity (Low Key Form)",
    "SPECIES_POLTEAGEIST_PHONY": "Polteageist (Phony Form)", "SPECIES_POLTEAGEIST_ANTIQUE": "Polteageist (Antique Form)",
    "SPECIES_URSHIFU_SINGLE_STRIKE": "Urshifu (Single Strike Style)", "SPECIES_URSHIFU_RAPID_STRIKE": "Urshifu (Rapid Strike Style)",
    "SPECIES_OINKOLOGNE_M": "Oinkologne (Male)", "SPECIES_OINKOLOGNE_F": "Oinkologne (Female)",
    "SPECIES_MAUSHOLD_FOUR": "Maushold (Family of Four)", "SPECIES_MAUSHOLD_THREE": "Maushold (Family of Three)",
    "SPECIES_DUDUNSPARCE_TWO_SEGMENT": "Dudunsparce (Two-Segment Form)", "SPECIES_DUDUNSPARCE_THREE_SEGMENT": "Dudunsparce (Three-Segment Form)",
    "SPECIES_SINISTCHA_UNREMARKABLE": "Sinistcha (Unremarkable Form)", "SPECIES_SINISTCHA_MASTERPIECE": "Sinistcha (Masterpiece Form)",
}

SCRIPT_TRIGGER_NAMES = {
    "EVO_TRIGGER_TABLET_CURSE": "Tablet Curse trigger",
    "EVO_TRIGGER_DARK_SCROLL": "Dark Scroll trigger",
    "EVO_TRIGGER_WATER_SCROLL": "Water Scroll trigger",
}

# Some mechanically distinct forms share one Pokédex entry. Keep one readable
# row for the species, but say which source form an evolution actually needs.
FORM_EVOLUTION_CONTEXT = {
    ("SPECIES_BURMY_PLANT", "SPECIES_WORMADAM_PLANT"): "as Plant Cloak",
    ("SPECIES_BURMY_SANDY", "SPECIES_WORMADAM_SANDY"): "as Sandy Cloak",
    ("SPECIES_BURMY_TRASH", "SPECIES_WORMADAM_TRASH"): "as Trash Cloak",
    ("SPECIES_SHELLOS_WEST", "SPECIES_GASTRODON_WEST"): "as West Sea form",
    ("SPECIES_SHELLOS_EAST", "SPECIES_GASTRODON_EAST"): "as East Sea form",
    ("SPECIES_BASCULIN_WHITE_STRIPED", "SPECIES_BASCULEGION_M"): "as White-Striped Form",
    ("SPECIES_BASCULIN_WHITE_STRIPED", "SPECIES_BASCULEGION_F"): "as White-Striped Form",
    ("SPECIES_DEERLING_SPRING", "SPECIES_SAWSBUCK_SPRING"): "as Spring Form",
    ("SPECIES_DEERLING_SUMMER", "SPECIES_SAWSBUCK_SUMMER"): "as Summer Form",
    ("SPECIES_DEERLING_AUTUMN", "SPECIES_SAWSBUCK_AUTUMN"): "as Autumn Form",
    ("SPECIES_DEERLING_WINTER", "SPECIES_SAWSBUCK_WINTER"): "as Winter Form",
    ("SPECIES_PUMPKABOO_AVERAGE", "SPECIES_GOURGEIST_AVERAGE"): "as Average Size",
    ("SPECIES_PUMPKABOO_SMALL", "SPECIES_GOURGEIST_SMALL"): "as Small Size",
    ("SPECIES_PUMPKABOO_LARGE", "SPECIES_GOURGEIST_LARGE"): "as Large Size",
    ("SPECIES_PUMPKABOO_SUPER", "SPECIES_GOURGEIST_SUPER"): "as Super Size",
    ("SPECIES_ROCKRUFF_OWN_TEMPO", "SPECIES_LYCANROC_DUSK"): "as Own Tempo form",
    ("SPECIES_SINISTEA_PHONY", "SPECIES_POLTEAGEIST_PHONY"): "as Phony Form",
    ("SPECIES_SINISTEA_ANTIQUE", "SPECIES_POLTEAGEIST_ANTIQUE"): "as Antique Form",
    ("SPECIES_POLTCHAGEIST_COUNTERFEIT", "SPECIES_SINISTCHA_UNREMARKABLE"): "as Counterfeit Form",
    ("SPECIES_POLTCHAGEIST_ARTISAN", "SPECIES_SINISTCHA_MASTERPIECE"): "as Artisan Form",
}


@dataclass(frozen=True)
class EvolutionEntry:
    constant: str
    name: str
    national_dex: int
    generation: int
    is_base_form: bool
    evolutions: tuple[str, ...]


def _matching(text: str, start: int, opening: str, closing: str) -> int:
    depth = 0
    in_string = False
    escaped = False
    for pos in range(start, len(text)):
        char = text[pos]
        if in_string:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                in_string = False
            continue
        if char == '"':
            in_string = True
        elif char == opening:
            depth += 1
        elif char == closing:
            depth -= 1
            if depth == 0:
                return pos
    raise ValueError(f"unterminated {opening}{closing} group")


def _split_top_level(text: str) -> list[str]:
    parts: list[str] = []
    start = 0
    parens = braces = 0
    for pos, char in enumerate(text):
        if char == "(": parens += 1
        elif char == ")": parens -= 1
        elif char == "{": braces += 1
        elif char == "}": braces -= 1
        elif char == "," and parens == 0 and braces == 0:
            part = text[start:pos].strip()
            if part: parts.append(part)
            start = pos + 1
    part = text[start:].strip()
    if part: parts.append(part)
    return parts


def _title_constant(value: str, prefix: str = "") -> str:
    value = value.removeprefix(prefix)
    if value in DISPLAY_OVERRIDES:
        return DISPLAY_OVERRIDES[value]
    words = value.lower().split("_")
    return " ".join(word.capitalize() for word in words)


def _generation_for_dex(number: int) -> int:
    return next(index for index, end in enumerate(GENERATION_ENDS, 1) if number <= end)


def _region_for_constant(constant: str) -> tuple[str, int] | None:
    tokens = constant.removeprefix("SPECIES_").split("_")
    for token, region in REGIONS.items():
        if token in tokens:
            return region
    return None


def _is_player_facing_regional(record: dict[str, object]) -> bool:
    constant = str(record["constant"])
    if _region_for_constant(constant) is None:
        return False
    # These are implementation/battle states, not distinct obtainable regional
    # species a player needs to look up in an evolution reference.
    return constant not in {
        "SPECIES_PIKACHU_ALOLA",
        "SPECIES_RATICATE_ALOLA_TOTEM",
        "SPECIES_MAROWAK_ALOLA_TOTEM",
        "SPECIES_DARMANITAN_GALAR_ZEN",
    }


def _form_detail(constant: str, region_token: str) -> str:
    tokens = constant.removeprefix("SPECIES_").split("_")
    region_index = tokens.index(region_token)
    detail = tokens[region_index + 1:]
    if not detail:
        return ""
    replacements = {"M": "Male", "F": "Female"}
    return " (" + " ".join(replacements.get(token, token.capitalize()) for token in detail) + ")"


def _parse_national_dex(root: Path) -> dict[str, int]:
    text = (root / "include/constants/pokedex.h").read_text()
    constants = re.findall(r"^\s*(NATIONAL_DEX_[A-Z0-9_]+),?", text, re.MULTILINE)
    return {constant: number for number, constant in enumerate(constants) if constant != "NATIONAL_DEX_NONE"}


def _parse_species_blocks(root: Path, dex: dict[str, int]) -> list[dict[str, object]]:
    records: list[dict[str, object]] = []
    pattern = re.compile(r"^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", re.MULTILINE)
    for path in sorted((root / "src/data/pokemon/species_info").glob("gen_*_families.h")):
        text = path.read_text()
        for match in pattern.finditer(text):
            opening = text.index("{", match.start())
            block = text[opening:_matching(text, opening, "{", "}") + 1]
            name_match = re.search(r'\.speciesName\s*=\s*_\("([^"]+)"\)', block)
            dex_match = re.search(r"\.natDexNum\s*=\s*(NATIONAL_DEX_[A-Z0-9_]+)", block)
            if not name_match or not dex_match or dex_match.group(1) not in dex:
                continue
            records.append({
                "constant": match.group(1),
                "display_name": name_match.group(1),
                "dex_constant": dex_match.group(1),
                "national_dex": dex[dex_match.group(1)],
                "block": block,
            })
    # These families use source macros rather than ordinary initializer blocks.
    # Keep their player-facing base entries explicit so completeness checks do
    # not depend on expanding the C preprocessor's graphics-heavy macros.
    synthetic_evolutions = {
        "SCATTERBUG": "{.evolutions = EVOLUTION({EVO_LEVEL, 9, SPECIES_SPEWPA})}",
        "SPEWPA": "{.evolutions = EVOLUTION({EVO_LEVEL, 12, SPECIES_VIVILLON})}",
        "FLABEBE": "{.evolutions = EVOLUTION({EVO_LEVEL, 19, SPECIES_FLOETTE})}",
    }
    present = {int(record["national_dex"]) for record in records}
    for dex_constant, number in dex.items():
        if number in present:
            continue
        suffix = dex_constant.removeprefix("NATIONAL_DEX_")
        records.append({
            "constant": "SPECIES_" + suffix,
            "display_name": _title_constant(suffix),
            "dex_constant": dex_constant,
            "national_dex": number,
            "block": synthetic_evolutions.get(suffix, "{}"),
        })
    return records


def _human_name(record: dict[str, object]) -> str:
    constant = str(record["constant"])
    base = str(record["display_name"])
    region = _region_for_constant(constant)
    if region is None:
        return base
    adjective, _ = region
    region_token = next(token for token, value in REGIONS.items() if value == region)
    return adjective + " " + base + _form_detail(constant, region_token)


def _extract_evolution_groups(block: str) -> list[str]:
    marker = ".evolutions = EVOLUTION("
    start = block.find(marker)
    if start < 0:
        return []
    opening = start + len(marker) - 1
    content = block[opening + 1:_matching(block, opening, "(", ")")]
    content = re.sub(r"^\s*#.*$", "", content, flags=re.MULTILINE)
    groups: list[str] = []
    pos = 0
    while pos < len(content):
        opening = content.find("{", pos)
        if opening < 0: break
        closing = _matching(content, opening, "{", "}")
        groups.append(content[opening + 1:closing])
        pos = closing + 1
    return groups


def _condition_text(condition: str) -> str:
    values = _split_top_level(condition)
    kind = values[0]
    args = values[1:]
    friendly = lambda value: _title_constant(value, value.split("_")[0] + "_")
    if kind == "IF_MIN_FRIENDSHIP": return "with high friendship"
    if kind == "IF_MIN_BEAUTY": return f"with at least {args[0]} Beauty"
    if kind == "IF_TIME": return "at " + _title_constant(args[0], "TIME_").lower()
    if kind == "IF_NOT_TIME": return "during the day" if args[0] == "TIME_NIGHT" else "outside " + _title_constant(args[0], "TIME_").lower()
    if kind == "IF_HOLD_ITEM": return "while holding " + _title_constant(args[0], "ITEM_")
    if kind == "IF_KNOWS_MOVE": return "while knowing " + _title_constant(args[0], "MOVE_")
    if kind == "IF_KNOWS_MOVE_TYPE": return "while knowing a " + _title_constant(args[0], "TYPE_") + "-type move"
    if kind == "IF_IN_MAP": return "in " + _title_constant(args[0], "MAP_")
    if kind == "IF_IN_MAPSEC": return "in " + _title_constant(args[0], "MAPSEC_")
    if kind == "IF_GENDER": return "if " + _title_constant(args[0], "MON_").lower()
    if kind == "IF_ATK_GT_DEF": return "if Attack is higher than Defense"
    if kind == "IF_ATK_LT_DEF": return "if Attack is lower than Defense"
    if kind == "IF_ATK_EQ_DEF": return "if Attack equals Defense"
    if kind == "IF_SPECIES_IN_PARTY": return "with " + _title_constant(args[0], "SPECIES_") + " in the party"
    if kind == "IF_TYPE_IN_PARTY": return "with a " + _title_constant(args[0], "TYPE_") + "-type Pokémon in the party"
    if kind == "IF_TRADE_PARTNER_SPECIES": return "when traded for " + _title_constant(args[0], "SPECIES_")
    if kind == "IF_WEATHER": return "during " + _title_constant(args[0], "WEATHER_").lower()
    if kind == "IF_MIN_OVERWORLD_STEPS": return f"after walking {args[0]} steps with it outside its Poké Ball"
    if kind == "IF_BAG_ITEM_COUNT":
        item = _title_constant(args[0], "ITEM_")
        if args[1] != "1": item += "s"
        return f"with {args[1]} {item} in the Bag"
    if kind == "IF_USED_MOVE_X_TIMES": return f"after using {_title_constant(args[0], 'MOVE_')} {args[1]} times"
    if kind == "IF_CRITICAL_HITS_GE": return f"after landing at least {args[0]} critical hits in one battle"
    if kind == "IF_RECOIL_DAMAGE_GE": return f"after taking at least {args[0]} recoil damage"
    if kind == "IF_CURRENT_DAMAGE_GE": return f"after taking at least {args[0]} damage"
    if kind == "IF_DEFEAT_X_WITH_ITEMS":
        item = _title_constant(args[1], "ITEM_")
        if args[-1] != "1": item += "s"
        return f"after defeating {args[-1]} {_title_constant(args[0], 'SPECIES_')} holding {item}"
    if kind == "IF_REGION": return "in " + _title_constant(args[0], "REGION_")
    if kind == "IF_NOT_REGION": return "outside " + _title_constant(args[0], "REGION_")
    if kind == "IF_AMPED_NATURE": return "with an Amped-form nature"
    if kind == "IF_LOW_KEY_NATURE": return "with a Low Key-form nature"
    if kind.startswith("IF_PID_"): return "based on its personality value"
    return "when " + friendly(kind)


def _format_evolution(group: str, names: dict[str, str], source: str = "") -> str | None:
    values = _split_top_level(group)
    if len(values) < 3 or values[0] == "EVO_NONE":
        return None
    method, param, target = values[:3]
    target_name = EVOLUTION_TARGET_OVERRIDES.get(target, names.get(target, _title_constant(target, "SPECIES_")))
    if method in ("EVO_LEVEL", "EVO_LEVEL_BATTLE_ONLY"):
        action = f"Level {param}" if param != "0" else "Level up"
        if method == "EVO_LEVEL_BATTLE_ONLY": action += " in battle"
    elif method == "EVO_TRADE": action = "Trade"
    elif method == "EVO_ITEM": action = _title_constant(param, "ITEM_")
    elif method == "EVO_SCRIPT_TRIGGER": action = SCRIPT_TRIGGER_NAMES.get(param, "Script/event trigger")
    elif method == "EVO_BATTLE_END": action = "At the end of a battle"
    elif method == "EVO_SPIN": action = "Spin in the overworld"
    elif method == "EVO_SPLIT_FROM_EVO": action = "Appears when its companion evolves"
    else: action = _title_constant(method, "EVO_")
    conditions: list[str] = []
    if context := FORM_EVOLUTION_CONTEXT.get((source, target)):
        conditions.append(context)
    if len(values) > 3 and "CONDITIONS(" in values[3]:
        raw = values[3]
        opening = raw.index("(")
        inner = raw[opening + 1:_matching(raw, opening, "(", ")")]
        pos = 0
        while (start := inner.find("{", pos)) >= 0:
            end = _matching(inner, start, "{", "}")
            conditions.append(_condition_text(inner[start + 1:end]))
            pos = end + 1
    if method == "EVO_TRADE" and len(conditions) == 1 and conditions[0].startswith("when traded for "):
        action = "Trade for " + conditions.pop().removeprefix("when traded for ")
    suffix = (" " + ", ".join(conditions)) if conditions else ""
    return f"{action}{suffix} → {target_name}"


def build_entries(root: Path = ROOT) -> list[EvolutionEntry]:
    dex = _parse_national_dex(root)
    records = _parse_species_blocks(root, dex)
    records_by_dex: dict[int, list[dict[str, object]]] = {}
    for record in records:
        records_by_dex.setdefault(int(record["national_dex"]), []).append(record)

    selected: list[tuple[dict[str, object], bool]] = []
    for number in range(1, max(dex.values()) + 1):
        candidates = records_by_dex.get(number, [])
        if not candidates:
            raise ValueError(f"missing species data for National Dex #{number}")
        base_candidates = [record for record in candidates if _region_for_constant(str(record["constant"])) is None]
        selected.append(((base_candidates or candidates)[0], True))
        selected.extend((record, False) for record in candidates if _is_player_facing_regional(record))

    all_names = {str(record["constant"]): _human_name(record) for record in records}
    entries: list[EvolutionEntry] = []
    used_names: set[str] = set()
    for record, is_base in selected:
        constant = str(record["constant"])
        name = _human_name(record)
        if name in used_names:
            continue
        used_names.add(name)
        region = _region_for_constant(constant)
        generation = region[1] if region else _generation_for_dex(int(record["national_dex"]))
        evolution_sources = ([candidate for candidate in records_by_dex[int(record["national_dex"])]
                              if _region_for_constant(str(candidate["constant"])) is None]
                             if is_base else [record])
        evolution_groups = ((group, str(source["constant"]))
                            for source in evolution_sources
                            for group in _extract_evolution_groups(str(source["block"])))
        evolutions = tuple(dict.fromkeys(filter(None, (_format_evolution(group, all_names, source)
                                                       for group, source in evolution_groups))))
        if name == "Milcery" and evolutions:
            evolutions = ("Spin in the overworld while holding a Sweet → Alcremie (form depends on the Sweet, time, and spin)",)
        elif name == "Nincada":
            evolutions = tuple("Level 20 with an empty party slot and at least 1 Poké Ball → Shedinja"
                               if evolution.endswith("→ Shedinja") else evolution for evolution in evolutions)
        entries.append(EvolutionEntry(constant, name, int(record["national_dex"]), generation, is_base, evolutions))
    return sorted(entries, key=lambda entry: (entry.generation, entry.national_dex, not entry.is_base_form, entry.name))


def render_markdown(entries: list[EvolutionEntry]) -> str:
    lines = [
        "# Pokémon Evolution Reference", "",
        "This page is generated from the game's species data. It lists every National Dex Pokémon enabled in the fork, plus separately named regional forms. Battle-only and cosmetic forms are folded into their base Pokémon so the table stays useful to players.", "",
        "Regional forms are listed under the generation in which that form debuted. When an evolution only works for a particular non-regional form, the required form is stated in the method. The evolution method column is empty when that Pokémon cannot evolve; multiple methods are shown on separate lines.", "",
        '<div class="table-scroll">', "<table>",
        "<thead><tr><th>Pokémon</th><th>Evolution method</th></tr></thead>", "<tbody>",
    ]
    current_generation = 0
    for entry in entries:
        if entry.generation != current_generation:
            current_generation = entry.generation
            lines.append(f'<tr><th colspan="2">Generation {current_generation} — {GENERATION_NAMES[current_generation]}</th></tr>')
        methods = "<br>".join(entry.evolutions)
        lines.append(f"<tr><td>{entry.name}</td><td>{methods}</td></tr>")
    lines.extend(["</tbody>", "</table>", "</div>", ""])
    return "\n".join(lines)


def main() -> None:
    entries = build_entries(ROOT)
    OUTPUT.write_text(render_markdown(entries))
    print(f"Wrote {OUTPUT.relative_to(ROOT)} with {len(entries)} unique Pokémon/form entries.")


if __name__ == "__main__":
    main()
