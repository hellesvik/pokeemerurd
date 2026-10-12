import unittest
from pathlib import Path
import re
import subprocess


ROOT = Path(__file__).resolve().parents[1]
DOC = ROOT / "docs/player/boss_battles.md"
TRAINERS = ROOT / "src/data/trainers.party"
CAPS = ROOT / "src/caps.c"
BATTLE_MAIN = ROOT / "src/battle_main.c"
TRAINERPROC = ROOT / "tools/trainerproc/main.c"
CHAMPION_SCRIPT = ROOT / "data/maps/EverGrandeCity_ChampionsRoom/scripts.inc"
MOVES_INFO = ROOT / "src/data/moves_info.h"
STAT_LABELS = ("HP", "ATK", "DEF", "SpA", "SpD", "Spe")
STAT_FIELDS = ("baseHP", "baseAttack", "baseDefense", "baseSpAttack", "baseSpDefense", "baseSpeed")


def party_sections(path):
    return dict(re.findall(r"^=== (\w+) ===\n(.*?)(?=^=== |\Z)", path.read_text(), re.M | re.S))


def party_mons(section):
    return [block for block in section.strip().split("\n\n") if re.search(r"^Level: ", block, re.M)]


def stat_spread(block, field, default):
    spread = dict.fromkeys(STAT_LABELS, default)
    match = re.search(rf"^{field}: (.+)$", block, re.M)
    if match:
        for value, label in re.findall(r"(\d+) (HP|Atk|Def|SpA|SpD|Spe)", match[1]):
            spread[label.upper() if label in ("Atk", "Def") else label] = int(value)
    return spread


def species_stats():
    # Preprocess the actual configuration so disabled forms, stat macros, and
    # generation-dependent base stats follow the same settings as the ROM.
    source = '#define TRUE 1\n#define FALSE 0\n#include "config/general.h"\n#include "config/pokemon.h"\n'
    for path in sorted((ROOT / "src/data/pokemon/species_info").glob("gen_*_families.h")):
        source += f'#include "{path.relative_to(ROOT)}"\n'
    result = subprocess.run(
        ["cpp", "-P", "-I", str(ROOT / "include"), "-I", str(ROOT), "-"],
        input=source, text=True, capture_output=True, check=True,
    ).stdout
    stats = {}
    for match in re.finditer(r"\[(SPECIES_\w+)\]\s*=\s*\{(.*?)(?=\[SPECIES_|\Z)", result, re.S):
        fields = dict(re.findall(r"\.(base\w+)\s*=\s*([^,]+),", match[2]))
        if not all(field in fields for field in STAT_FIELDS):
            continue
        stats[match[1]] = tuple(fields[field].strip() for field in STAT_FIELDS)
    aliases = dict(re.findall(r"\b(SPECIES_\w+)\s*=\s*(SPECIES_\w+)",
                              (ROOT / "include/constants/species.h").read_text()))
    for alias, target in aliases.items():
        while target in aliases:
            target = aliases[target]
        if target in stats:
            stats[alias] = stats[target]
    return stats


def base_stat_value(expression):
    expression = expression.strip("() ")
    conditional = re.fullmatch(r"(\d+)\s*>=\s*(\d+)\s*\?\s*(\d+)\s*:\s*(\d+)", expression)
    if conditional:
        left, right, yes, no = map(int, conditional.groups())
        return yes if left >= right else no
    # Fail on unsupported expressions instead of silently guessing.
    return int(expression)


def source_constant(name, prefix):
    return prefix + re.sub(r"[^A-Z0-9]+", "_", name.upper()).strip("_")


class BossBattleDocsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.markdown = DOC.read_text()

    def section(self, heading):
        start = self.markdown.index(heading)
        end = self.markdown.find("\n## ", start + len(heading))
        return self.markdown[start:end if end >= 0 else None]

    def test_team_ability_rows_match_configured_overrides(self):
        expected_rows = {
            "## Rustboro rival": "| Ability | Randomized | Randomized | Randomized | Randomized |",
            "## Route 110 rival": "| Ability | Randomized | Randomized | Randomized | Randomized | Randomized |",
            "## Maxie — Mt. Chimney": "| Ability | Intimidate | Reckless | Chlorophyll | Solid Rock |",
            "## Route 119 rival": "| Ability | Randomized | Randomized | Randomized | Randomized | Randomized |",
            "## Lilycove rival": "| Ability | Randomized | Randomized | Randomized | Randomized | Randomized | Randomized |",
            "## Maxie — Magma Hideout": "| Ability | Sand Spit | Sand Rush | Reckless | Storm Drain | Solid Rock → Sheer Force |",
            "### Tabitha's team": "| Ability | Sand Stream | Sand Veil | Speed Boost |",
            "## Maxie & Tabitha — Mossdeep": "| Ability | Sand Veil | Sand Rush | Solid Rock → Sheer Force |",
            "## Archie": "| Ability | Drizzle | Swift Swim | Infiltrator | Lightning Rod | Intimidate | Speed Boost → Strong Jaw |",
        }
        for heading, row in expected_rows.items():
            with self.subTest(heading=heading):
                self.assertIn(row, self.section(heading))

    def test_every_move_has_the_correct_type_marker(self):
        move_types = {}
        move_data = MOVES_INFO.read_text()
        blocks = re.finditer(r"\[MOVE_[A-Z0-9_]+\]\s*=\s*\{(.*?)(?=\n\s*\[MOVE_|\Z)", move_data, re.S)
        for block_match in blocks:
            block = block_match.group(1)
            name = re.search(r'\.name\s*=\s*(?:COMPOUND_STRING\()?\"([^\"]+)\"', block)
            move_type = re.search(r"\.type\s*=.*?TYPE_([A-Z]+)", block)
            if name and move_type:
                key = re.sub(r"[^a-z0-9]", "", name.group(1).lower())
                move_types[key] = move_type.group(1).title()

        errors = []
        marker = re.compile(r'<img [^>]*graphics/types/([a-z]+)\.png[^>]*alt="([^"]+)"[^>]*>\s*(.+)')
        for line_number, line in enumerate(self.markdown.splitlines(), 1):
            if not line.startswith("| Moves |"):
                continue
            for cell in line.removeprefix("| Moves |").removesuffix("|").split("|"):
                for rendered_move in cell.strip().split("<br>"):
                    match = marker.fullmatch(rendered_move.strip())
                    if not match:
                        errors.append(f"line {line_number}: missing type marker for {rendered_move.strip()}")
                        continue
                    icon_type, alt_type, move_name = match.groups()
                    key = re.sub(r"[^a-z0-9]", "", move_name.lower())
                    expected_type = move_types.get(key)
                    if expected_type is None:
                        errors.append(f"line {line_number}: unknown move {move_name}")
                        continue
                    expected_icon = "fight" if expected_type == "Fighting" else expected_type.lower()
                    if icon_type != expected_icon or alt_type != expected_type:
                        errors.append(
                            f"line {line_number}: {move_name} is {expected_type}, marked {alt_type}/{icon_type}"
                        )

        self.assertEqual(errors, [])

    def test_stat_labels_and_values_never_wrap_apart(self):
        stat_rows = [line for line in self.markdown.splitlines() if line.startswith("| Stats |")]
        self.assertGreater(len(stat_rows), 0)
        for row in stat_rows:
            with self.subTest(row=row[:80]):
                self.assertNotRegex(row, r"(?:HP|ATK|DEF|SpA|SpD|Spe): ")
                labels = re.findall(r"(?:HP|ATK|DEF|SpA|SpD|Spe):", row)
                protected_values = re.findall(r"(?:HP|ATK|DEF|SpA|SpD|Spe):&nbsp;\d+", row)
                self.assertEqual(len(protected_values), len(labels))

    def test_every_documented_stat_matches_game_data(self):
        trainers = party_sections(TRAINERS)
        partners = party_sections(ROOT / "src/data/battle_partners.party")
        bases = species_stats()
        encounters = {
            "Roxanne": "ROXANNE_1", "Brawley": "BRAWLY_1", "Wattson": "WATTSON_1",
            "Maxie — Mt. Chimney": "MAXIE_MT_CHIMNEY", "Flannery": "FLANNERY_1",
            "Norman": "NORMAN_1", "Shelly — Weather Institute": "SHELLY_WEATHER_INSTITUTE",
            "Winona": "WINONA_1", "Maxie — Magma Hideout": "MAXIE_MAGMA_HIDEOUT",
            "Aqua Admin Matt": "MATT", "Tate & Liza": "TATE_AND_LIZA_1",
            "Maxie & Tabitha — Mossdeep": "MAXIE_MOSSDEEP", "Archie": "ARCHIE",
            "Juan": "JUAN_1", "Wally": "WALLY_VR_1", "Sidney": "SIDNEY",
            "Phoebe": "PHOEBE", "Glacia": "GLACIA", "Drake": "DRAKE",
            "Steven — Champion": "STEVEN",
        }
        mega_forms = {item: species for species, item in re.findall(
            r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_\w+),\s*(ITEM_\w+)",
            (ROOT / "src/data/pokemon/form_change_tables.h").read_text(),
        )}
        checked_rows = 0
        checked_values = 0
        heading = subsection = ""
        rows = {}
        for line_number, line in enumerate(self.markdown.splitlines(), 1):
            if line.startswith("## "):
                heading = line[3:]
                subsection = ""
            elif line.startswith("### "):
                subsection = line[4:]
            elif line.startswith("| |"):
                rows = {"names": [cell.strip() for cell in line.split("|")[2:-1]]}
            elif line.startswith("| "):
                cells = [cell.strip() for cell in line.split("|")[1:-1]]
                rows[cells[0]] = cells[1:]
                if cells[0] != "Stats":
                    continue
                checked_rows += 1
                is_partner = subsection.startswith("Steven's team")
                if is_partner:
                    team = party_mons(partners["PARTNER_STEVEN"])
                elif subsection == "Tabitha's team":
                    team = party_mons(trainers["TRAINER_TABITHA_MOSSDEEP"])
                else:
                    encounter = next((value for name, value in encounters.items()
                                      if heading == name or heading.startswith(name + " —")), None)
                    self.assertIsNotNone(encounter, f"Unmapped Stats row at line {line_number}: {heading}")
                    team = party_mons(trainers[f"TRAINER_{encounter}"])
                self.assertEqual(len(cells[1:]), len(team), f"line {line_number}: stat columns")
                self.assertEqual(len(rows["names"]), len(team), f"line {line_number}: Pokémon columns")
                self.assertEqual(len(rows["Level"]), len(team), f"line {line_number}: level columns")
                for index, (cell, mon) in enumerate(zip(cells[1:], team)):
                    name, _, item = mon.splitlines()[0].partition(" @ ")
                    species = source_constant(name, "SPECIES_")
                    species = mega_forms.get(source_constant(item, "ITEM_"), species)
                    level = int(re.search(r"^Level: (\d+)$", mon, re.M)[1])
                    ivs = stat_spread(mon, "IVs", 31) if is_partner else dict.fromkeys(STAT_LABELS, 31)
                    evs = stat_spread(mon, "EVs", 0)
                    nature_match = re.search(r"^(\w+) Nature$", mon, re.M)
                    nature = nature_match[1] if is_partner and nature_match else "Hardy"
                    # Stat order here is HP, Attack, Defense, Sp. Attack,
                    # Sp. Defense, Speed, as in CalculateMonStats.
                    nature_changes = {"Hardy": (None, None), "Brave": ("ATK", "Spe"),
                                      "Impish": ("DEF", "SpA"), "Adamant": ("ATK", "SpA")}
                    raised, lowered = nature_changes[nature]
                    rendered = cell.split("<br>")
                    self.assertEqual(len(rendered), 6, f"line {line_number}: {name}")
                    self.assertEqual(int(rows["Level"][index]), level, f"{heading}: {name} level")
                    for label, expression, documented in zip(STAT_LABELS, bases[species], rendered):
                        with self.subTest(line=line_number, team=heading, pokemon=rows["names"][index], stat=label):
                            self.assertRegex(documented, rf"^{label}:&nbsp;\d+$")
                            base = base_stat_value(expression)
                            expected = ((2 * base + ivs[label] + evs[label] // 4) * level) // 100
                            if label == "HP":
                                expected += level + 10
                                if species == "SPECIES_SHEDINJA":
                                    expected = 1
                            else:
                                expected += 5
                                if label == raised:
                                    expected = expected * 110 // 100
                                elif label == lowered:
                                    expected = expected * 90 // 100
                            checked_values += 1
                            self.assertEqual(int(documented.split("&nbsp;")[1]), expected,
                                             f"{species}, level {level}, {nature}, IV {ivs[label]}, EV {evs[label]}")
        self.assertEqual(checked_rows, self.markdown.count("| Stats |"))
        self.assertEqual(checked_values, sum(
            6 * (line.count("|") - 2) for line in self.markdown.splitlines()
            if line.startswith("| Stats |")
        ))
        self.assertGreater(checked_rows, 0)

    def test_tate_and_liza_stats_use_battle_stat_order(self):
        section = self.section("## Tate & Liza")
        expected = (
            (117, 79, 75, 95, 75, 95),
            (122, 65, 83, 102, 83, 97),
            (123, 64, 97, 97, 110, 72),
            (157, 72, 81, 123, 89, 43),
            (140, 64, 72, 97, 89, 76),
            (140, 97, 89, 64, 72, 76),
        )

        for stats in expected:
            rendered = (
                f"HP:&nbsp;{stats[0]}<br>ATK:&nbsp;{stats[1]}<br>"
                f"DEF:&nbsp;{stats[2]}<br>SpA:&nbsp;{stats[3]}<br>"
                f"SpD:&nbsp;{stats[4]}<br>Spe:&nbsp;{stats[5]}"
            )
            self.assertIn(rendered, section)

    def test_maxie_and_tabitha_stats_match_runtime_forms(self):
        section = self.section("## Maxie & Tabitha")
        expected = (
            (128, 97, 123, 55, 81, 97),    # Gliscor, level 42
            (139, 112, 95, 57, 95, 87),    # Stoutland, level 43
            (129, 124, 106, 146, 111, 36),  # Mega Camerupt, level 44
            (155, 112, 117, 75, 78, 57),   # Hippowdon, level 42
            (124, 95, 74, 61, 65, 88),     # Gabite, level 43
            (138, 159, 89, 133, 89, 106),  # Mega Blaziken, level 44
        )

        for stats in expected:
            rendered = (
                f"HP:&nbsp;{stats[0]}<br>ATK:&nbsp;{stats[1]}<br>"
                f"DEF:&nbsp;{stats[2]}<br>SpA:&nbsp;{stats[3]}<br>"
                f"SpD:&nbsp;{stats[4]}<br>Spe:&nbsp;{stats[5]}"
            )
            self.assertIn(rendered, section)

    def test_explains_fixed_and_randomized_ability_labels(self):
        self.assertIn(
            "Abilities shown by name are fixed team overrides. **Randomized** abilities use the save's seeded species-family ability roll",
            self.markdown,
        )

    def test_rival_sections_use_full_variant_tables_without_summary_tables(self):
        for heading in ("## Rustboro rival", "## Route 110 rival", "## Route 119 rival", "## Lilycove rival"):
            section = self.section(heading)
            with self.subTest(heading=heading):
                self.assertNotIn("| Player chose | Brendan's team | May's team |", section)
                self.assertEqual(section.count("### Player chose "), 3)
                for starter in ("Treecko", "Torchic", "Mudkip"):
                    self.assertIn(f"### Player chose {starter}", section)

    def test_league_level_progression_matches_the_game_design(self):
        trainer_data = TRAINERS.read_text()
        expected = {
            "TRAINER_SIDNEY": [74, 75, 76, 77, 78, 79],
            "TRAINER_PHOEBE": [76, 76, 76, 77, 78, 79],
            "TRAINER_GLACIA": [77, 77, 77, 77, 77, 79],
            "TRAINER_DRAKE": [78, 78, 78, 78, 79, 79],
            "TRAINER_STEVEN": [79, 79, 79, 79, 80, 80],
        }
        for trainer, levels in expected.items():
            start = trainer_data.index(f"=== {trainer} ===")
            end = trainer_data.find("\n=== TRAINER_", start + 4)
            section = trainer_data[start:end if end >= 0 else None]
            actual = [int(line.removeprefix("Level: ")) for line in section.splitlines() if line.startswith("Level: ")]
            self.assertEqual(actual, levels, trainer)

        caps = CAPS.read_text()
        self.assertIn("static const u16 sCapSteven[] = { TRAINER_STEVEN, TRAINER_NONE };", caps)
        self.assertIn("{ 80, sCapSteven },", caps)

    def test_champion_room_uses_documented_steven_team(self):
        self.assertIn(
            "trainerbattle_no_intro TRAINER_STEVEN, EverGrandeCity_ChampionsRoom_Text_Defeat",
            CHAMPION_SCRIPT.read_text(),
        )

    def test_glacias_vanilluxe_holds_icy_rock(self):
        trainer_data = TRAINERS.read_text()
        start = trainer_data.index("=== TRAINER_GLACIA ===")
        end = trainer_data.find("\n=== TRAINER_", start + 4)
        team = trainer_data[start:end if end >= 0 else None]

        self.assertIn("Vanilluxe @ Icy Rock", team)
        self.assertIn("| Held item | Icy Rock | Leftovers | Lum Berry | Choice Scarf | Choice Band | Glalitite |", self.section("## Glacia — Single battle"))

    def test_aqua_admin_matt_team_and_level_cap_match_design(self):
        trainer_data = TRAINERS.read_text()
        start = trainer_data.index("=== TRAINER_MATT ===")
        end = trainer_data.index("\n=== TRAINER_", start + 4)
        team = trainer_data[start:end]

        expected = (
            "Aurorus @ Icy Rock", "Ability: Snow Warning", "Level: 37",
            "Blastoise @ Lagging Tail", "Ability: Rain Dish",
            "Sandslash-Alola @ Occa Berry", "Ability: Slush Rush", "Level: 38",
            "Sealeo @ Leftovers", "Ability: Ice Body",
            "Piloswine @ Eviolite", "Ability: Snow Cloak",
            "Gyarados @ Gyaradosite", "Ability: Intimidate", "Level: 40",
            "- Aurora Veil", "- Aqua Tail", "- Spiky Shield", "- Chilling Water",
            "- Sleep Talk", "- Blizzard", "- Snarl",
        )
        self.assertEqual(team.count("IVs: 31 HP / 31 Atk / 31 Def / 31 SpA / 31 SpD / 31 Spe"), 6)
        for entry in expected:
            self.assertIn(entry, team)

        caps = CAPS.read_text()
        self.assertIn("static const u16 sCapMatt[] = { TRAINER_MATT, TRAINER_NONE };", caps)
        self.assertIn("{ 40, sCapMatt },", caps)

        docs = self.section("## Aqua Admin Matt")
        self.assertIn("| Level | 37 | 37 | 38 | 37 | 38 | 40 |", docs)
        self.assertIn("| Held item | Icy Rock | Lagging Tail | Occa Berry | Leftovers | Eviolite | Gyaradosite |", docs)

    def test_rivals_receive_story_boss_ivs(self):
        source = BATTLE_MAIN.read_text()
        start = source.index("static bool32 IsMaxIvTrainerClass")
        end = source.index("\n}\n", start)
        self.assertIn("case TRAINER_CLASS_RIVAL:", source[start:end])
        self.assertIn("u32 ivs = IsMaxIvTrainerClass(trainer->trainerClass)", source)

    def test_all_bosses_use_hardy_nature(self):
        battle_main = BATTLE_MAIN.read_text()
        self.assertIn(
            "if (IsStoryBossTrainerClass(trainer->trainerClass))\n"
            "                ModifyPersonalityForNature(&personalityValue, NATURE_HARDY);",
            battle_main,
        )
        self.assertIn(
            'fprintf(f, "            .nature = NATURE_HARDY,\\n");',
            TRAINERPROC.read_text(),
        )
        self.assertNotIn("Random nature", self.markdown)
        self.assertNotIn("roll their natures normally", self.markdown)

    def test_lilycove_rival_parties_match_design(self):
        trainer_data = TRAINERS.read_text()
        expected = {
            "MUDKIP": (
                "Vibrava @ Soft Sand", "Relicanth @ Rocky Helmet", "Torkoal @ White Herb",
                "Grumpig @ Sitrus Berry", "Swellow @ Sharp Beak", "Sceptile @ Grass Gem",
                "- Rock Smash", "- Dive", "- Shell Smash", "- Bounce", "- Extreme Speed", "- Leaf Blade",
            ),
            "TORCHIC": (
                "Vibrava @ Soft Sand", "Torkoal @ White Herb", "Breloom @ Loaded Dice",
                "Banette @ Spell Tag", "Exploud @ Expert Belt", "Swampert @ Water Gem",
                "- Fly", "- Sunny Day", "- Spore", "- Phantom Force", "- Surf", "- Earthquake",
            ),
            "TREECKO": (
                "Vibrava @ Soft Sand", "Ludicolo @ Rocky Helmet", "Tropius @ Power Herb",
                "Chimecho @ Sitrus Berry", "Exploud @ Expert Belt", "Blaziken @ Fire Gem",
                "- Rock Smash", "- Giga Drain", "- Solarbeam", "- Flash", "- Strength", "- Return",
            ),
        }
        for rival in ("BRENDAN", "MAY"):
            for starter, entries in expected.items():
                trainer = f"TRAINER_{rival}_LILYCOVE_{starter}"
                start = trainer_data.index(f"=== {trainer} ===")
                end = trainer_data.find("\n=== TRAINER_", start + 4)
                section = trainer_data[start:end if end >= 0 else None]
                self.assertEqual(section.count("IVs: 31 HP / 31 Atk / 31 Def / 31 SpA / 31 SpD / 31 Spe"), 6, trainer)
                self.assertNotIn("Ability:", section, trainer)
                for entry in entries:
                    self.assertIn(entry, section, trainer)

    def test_story_rival_parties_match_current_design(self):
        trainer_data = TRAINERS.read_text()
        expected = {
            "RUSTBORO": {
                "TREECKO": ("Taillow\nLevel: 13", "Shroomish\nLevel: 13", "Lotad\nLevel: 14", "Combusken\nLevel: 16", "- Protect", "- Cut", "- Water Gun", "- Ember"),
                "TORCHIC": ("Taillow\nLevel: 13", "Shroomish\nLevel: 13", "Lotad\nLevel: 14", "Marshtomp\nLevel: 16", "- Protect", "- Cut", "- Water Gun", "- Mud Shot"),
                "MUDKIP": ("Taillow\nLevel: 13", "Shroomish\nLevel: 13", "Lotad\nLevel: 14", "Grovyle\nLevel: 16", "- Protect", "- Cut", "- Water Gun", "- Leafage"),
            },
            "ROUTE_110": {
                "TREECKO": ("Roselia\nLevel: 16", "Lombre\nLevel: 17", "Electrike\nLevel: 17", "Aron\nLevel: 19", "Combusken @ Oran Berry\nLevel: 20", "- Sleep Powder", "- Cut", "- Flash", "- Iron Defense", "- Flame Charge"),
                "TORCHIC": ("Shroomish\nLevel: 16", "Electrike\nLevel: 17", "Gulpin\nLevel: 17", "Aron\nLevel: 19", "Marshtomp\nLevel: 20", "- Spore", "- Flash", "- Stockpile", "- Iron Defense", "- Mud Shot"),
                "MUDKIP": ("Taillow\nLevel: 16", "Electrike\nLevel: 17", "Volbeat\nLevel: 17", "Aron\nLevel: 19", "Grovyle @ Lum Berry\nLevel: 20", "- Agility", "- Flash", "- Silver Wind", "- Iron Defense", "- Fury Cutter"),
            },
            "ROUTE_119": {
                "TREECKO": ("Trapinch @ Yache Berry\nLevel: 31", "Loudred @ Chople Berry\nLevel: 31", "Lombre @ Coba Berry\nLevel: 30", "Tropius @ Sitrus Berry\nLevel: 30", "Combusken @ Liechi Berry\nLevel: 32", "- Body Slam", "- Strength", "- Surf", "- Fly", "- Flamethrower"),
                "TORCHIC": ("Trapinch @ Yache Berry\nLevel: 31", "Loudred @ Chople Berry\nLevel: 31", "Breloom @ Coba Berry\nLevel: 30", "Torkoal @ Sitrus Berry\nLevel: 30", "Marshtomp @ Petaya Berry\nLevel: 32", "- Body Slam", "- Strength", "- Spore", "- Rapid Spin", "- Muddy Water"),
                "MUDKIP": ("Trapinch @ Yache Berry\nLevel: 31", "Feebas @ Focus Sash\nLevel: 31", "Swellow @ Wacan Berry\nLevel: 30", "Torkoal @ Sitrus Berry\nLevel: 30", "Grovyle @ Salac Berry\nLevel: 32", "- Rock Smash", "- Flail", "- Roost", "- Rapid Spin", "- Growth"),
            },
            "LILYCOVE": {
                "TREECKO": ("Vibrava @ Soft Sand", "Ludicolo @ Rocky Helmet", "Tropius @ Power Herb", "Chimecho @ Sitrus Berry", "Exploud @ Expert Belt", "Blaziken @ Fire Gem", "- Return"),
                "TORCHIC": ("Vibrava @ Soft Sand", "Torkoal @ White Herb", "Breloom @ Loaded Dice", "Banette @ Spell Tag", "Exploud @ Expert Belt", "Swampert @ Water Gem", "- Phantom Force"),
                "MUDKIP": ("Vibrava @ Soft Sand", "Relicanth @ Rocky Helmet", "Torkoal @ White Herb", "Grumpig @ Sitrus Berry", "Swellow @ Sharp Beak", "Sceptile @ Grass Gem", "- Extreme Speed"),
            },
        }
        for encounter, variants in expected.items():
            for rival in ("BRENDAN", "MAY"):
                for starter, entries in variants.items():
                    trainer = f"TRAINER_{rival}_{encounter}_{starter}"
                    start = trainer_data.index(f"=== {trainer} ===")
                    end = trainer_data.find("\n=== TRAINER_", start + 4)
                    section = trainer_data[start:end if end >= 0 else None]
                    expected_count = 4 if encounter == "RUSTBORO" else 5 if encounter in ("ROUTE_110", "ROUTE_119") else 6
                    self.assertEqual(section.count("IVs: 31 HP / 31 Atk / 31 Def / 31 SpA / 31 SpD / 31 Spe"), expected_count, trainer)
                    self.assertNotIn("Ability:", section, trainer)
                    for entry in entries:
                        self.assertIn(entry, section, trainer)

    def test_norman_uses_furret_with_linoones_set(self):
        trainer_data = TRAINERS.read_text()
        start = trainer_data.index("=== TRAINER_NORMAN_1 ===")
        end = trainer_data.index("\n=== TRAINER_", start + 4)
        section = trainer_data[start:end]
        self.assertIn("Furret @ Liechi Berry", section)
        self.assertIn("Ability: Adaptability", section)
        for move in ("Extreme Speed", "Facade", "Shadow Claw", "Dig"):
            self.assertIn(f"- {move}", section)
        self.assertNotIn("Linoone", section)

        docs = self.section("## Norman")
        self.assertIn("| | Furret | Zangoose | Tauros | Bouffalant | Slaking |", docs)
        self.assertIn("HP:&nbsp;91<br>ATK:&nbsp;54<br>DEF:&nbsp;47<br>SpA:&nbsp;37<br>SpD:&nbsp;43<br>Spe:&nbsp;61", docs)
        self.assertNotIn("Linoone", docs)

    def test_winonas_swellow_uses_tailwind(self):
        trainer_data = TRAINERS.read_text()
        start = trainer_data.index("=== TRAINER_WINONA_1 ===")
        end = trainer_data.index("\n=== TRAINER_", start + 4)
        section = trainer_data[start:end]
        swellow = section[section.index("Swellow @ Sharp Beak"):section.index("\n\nTropius")]
        self.assertIn("- Tailwind", swellow)
        self.assertNotIn("- U-Turn", swellow)

        docs = self.section("## Winona")
        moves = docs[docs.index("| Moves |"):]
        swellow_moves = moves.split(" | ", 2)[1]
        self.assertIn("Tailwind", swellow_moves)
        self.assertNotIn("U-Turn", swellow_moves)

    def test_lilycove_rival_tables_show_pokemon_and_type_sprites(self):
        docs = self.section("## Lilycove rival")
        self.assertEqual(docs.count("| Sprite |"), 3)
        self.assertEqual(docs.count("| Type |"), 3)
        for pokemon in ("vibrava", "ludicolo", "tropius", "chimecho", "exploud", "blaziken", "torkoal", "breloom", "banette", "swampert", "relicanth", "grumpig", "swellow", "sceptile"):
            self.assertIn(f"graphics/pokemon/{pokemon}/anim_front.png", docs)
        for pokemon_type in ("ground", "dragon", "water", "rock", "fire", "psychic", "normal", "grass", "flying", "fight"):
            self.assertIn(f"graphics/types/{pokemon_type}.png", docs)

    def test_archie_table_has_consistent_columns(self):
        docs = self.section("## Archie")
        rows = [line for line in docs.splitlines() if line.startswith("|")]
        expected_pipes = rows[0].count("|")
        for row in rows:
            self.assertEqual(row.count("|"), expected_pipes, row)

    def test_tabitha_uses_mega_blaziken(self):
        trainer_data = TRAINERS.read_text()
        start = trainer_data.index("=== TRAINER_TABITHA_MOSSDEEP ===")
        end = trainer_data.index("\n=== TRAINER_", start + 4)
        section = trainer_data[start:end]
        self.assertIn("Blaziken @ Blazikenite", section)
        self.assertIn("Ability: Speed Boost", section)
        for move in ("Heat Wave", "Aura Sphere", "Weather Ball", "Scorching Sands"):
            self.assertIn(f"- {move}", section)
        self.assertNotIn("Volcanion", section)

        docs = self.section("## Maxie & Tabitha")
        self.assertIn("| | Hippowdon | Gabite | Mega Blaziken |", docs)
        self.assertIn("HP:&nbsp;138<br>ATK:&nbsp;159<br>DEF:&nbsp;89<br>SpA:&nbsp;133<br>SpD:&nbsp;89<br>Spe:&nbsp;106", docs)
        self.assertNotIn("Volcanion", docs)

    def test_shelly_weather_institute_has_feraligatr(self):
        trainer_data = TRAINERS.read_text()
        start = trainer_data.index("=== TRAINER_SHELLY_WEATHER_INSTITUTE ===")
        end = trainer_data.index("\n=== TRAINER_", start + 4)
        section = trainer_data[start:end]
        self.assertIn("Feraligatr @ Power Herb", section)
        self.assertIn("Ability: Strong Jaw", section)
        self.assertIn("Level: 32", section)
        for move in ("Ice Fang", "Bite", "Dive", "Thunder Fang"):
            self.assertIn(f"- {move}", section)

        docs = self.section("## Shelly")
        self.assertIn("| | Politoed | Castform | Whiscash | Dragonair | Feraligatr |", docs)
        self.assertIn("HP:&nbsp;106<br>ATK:&nbsp;82<br>DEF:&nbsp;78<br>SpA:&nbsp;65<br>SpD:&nbsp;68<br>Spe:&nbsp;64", docs)

    def test_summary_lists_all_double_battles(self):
        self.assertIn(
            "**Wattson, Shelly, Tate & Liza, and Maxie & Tabitha are double battles; all other teams below are single battles.**",
            self.markdown,
        )


if __name__ == "__main__":
    unittest.main()
