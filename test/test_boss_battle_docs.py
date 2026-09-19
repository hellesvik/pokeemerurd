import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DOC = ROOT / "docs/player/boss_battles.md"
TRAINERS = ROOT / "src/data/trainers.party"
CAPS = ROOT / "src/caps.c"
BATTLE_MAIN = ROOT / "src/battle_main.c"
TRAINERPROC = ROOT / "tools/trainerproc/main.c"
CHAMPION_SCRIPT = ROOT / "data/maps/EverGrandeCity_ChampionsRoom/scripts.inc"


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
        self.assertIn("HP: 91<br>ATK: 54<br>DEF: 47<br>SpA: 37<br>SpD: 43<br>Spe: 61", docs)
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
        self.assertIn("HP: 138<br>ATK: 159<br>DEF: 89<br>SpA: 133<br>SpD: 89<br>Spe: 106", docs)
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
        self.assertIn("HP: 106<br>ATK: 82<br>DEF: 78<br>SpA: 65<br>SpD: 68<br>Spe: 64", docs)

    def test_summary_lists_all_double_battles(self):
        self.assertIn(
            "**Wattson, Shelly, Tate & Liza, and Maxie & Tabitha are double battles; all other teams below are single battles.**",
            self.markdown,
        )


if __name__ == "__main__":
    unittest.main()
