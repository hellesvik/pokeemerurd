import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DOC = ROOT / "docs/player/boss_battles.md"
TRAINERS = ROOT / "src/data/trainers.party"
CAPS = ROOT / "src/caps.c"
BATTLE_MAIN = ROOT / "src/battle_main.c"
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
            "## Rustboro rival": "| Ability | Randomized | Randomized |",
            "## Route 110 rival": "| Ability | Randomized | Randomized | Randomized |",
            "## Maxie — Mt. Chimney": "| Ability | Intimidate | Reckless | Chlorophyll | Solid Rock |",
            "## Route 119 rival": "| Ability | Randomized | Randomized | Randomized |",
            "## Lilycove rival": "| Ability | Randomized | Randomized | Randomized | Randomized |",
            "## Maxie — Magma Hideout": "| Ability | Sand Spit | Sand Rush | Reckless | Storm Drain | Solid Rock → Sheer Force |",
            "### Tabitha's team": "| Ability | Sand Stream | Sand Veil | Water Absorb |",
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

    def test_lists_every_starter_and_rival_variant(self):
        expected_rows = {
            "## Rustboro rival": (
                "| Treecko | Lotad Lv. 13<br>Torchic Lv. 15 | Lotad Lv. 13<br>Torchic Lv. 15 |",
                "| Torchic | Slugma Lv. 13<br>Mudkip Lv. 15 | Torkoal Lv. 13<br>Mudkip Lv. 15 |",
                "| Mudkip | Wingull Lv. 13<br>Treecko Lv. 15 | Wingull Lv. 13<br>Treecko Lv. 15 |",
            ),
            "## Route 110 rival": (
                "| Treecko | Wingull Lv. 18<br>Lombre Lv. 18<br>Combusken Lv. 20 | Wingull Lv. 18<br>Lombre Lv. 18<br>Combusken Lv. 20 |",
                "| Torchic | Lombre Lv. 18<br>Slugma Lv. 18<br>Marshtomp Lv. 20 | Lombre Lv. 18<br>Slugma Lv. 18<br>Marshtomp Lv. 20 |",
                "| Mudkip | Slugma Lv. 18<br>Wingull Lv. 18<br>Grovyle Lv. 20 | Wingull Lv. 18<br>Slugma Lv. 18<br>Grovyle Lv. 20 |",
            ),
            "## Route 119 rival": (
                "| Treecko | Pelipper Lv. 29<br>Lombre Lv. 29<br>Combusken Lv. 31 | Pelipper Lv. 29<br>Lombre Lv. 29<br>Combusken Lv. 31 |",
                "| Torchic | Lombre Lv. 29<br>Slugma Lv. 29<br>Marshtomp Lv. 31 | Lombre Lv. 29<br>Slugma Lv. 29<br>Marshtomp Lv. 31 |",
                "| Mudkip | Slugma Lv. 29<br>Pelipper Lv. 29<br>Grovyle Lv. 31 | Slugma Lv. 29<br>Lombre Lv. 29<br>Grovyle Lv. 31 |",
            ),
            "## Lilycove rival": (
                "| Treecko | Tropius Lv. 31<br>Pelipper Lv. 32<br>Ludicolo Lv. 32<br>Combusken Lv. 34 | Tropius Lv. 31<br>Pelipper Lv. 32<br>Ludicolo Lv. 32<br>Combusken Lv. 34 |",
                "| Torchic | Tropius Lv. 31<br>Ludicolo Lv. 32<br>Slugma Lv. 32<br>Marshtomp Lv. 34 | Tropius Lv. 31<br>Ludicolo Lv. 32<br>Slugma Lv. 32<br>Marshtomp Lv. 34 |",
                "| Mudkip | Tropius Lv. 31<br>Slugma Lv. 32<br>Pelipper Lv. 32<br>Grovyle Lv. 34 | Tropius Lv. 31<br>Slugma Lv. 32<br>Pelipper Lv. 32<br>Grovyle Lv. 34 |",
            ),
        }
        for heading, rows in expected_rows.items():
            section = self.section(heading)
            self.assertIn("| Player chose | Brendan's team | May's team |", section)
            for row in rows:
                with self.subTest(heading=heading, row=row):
                    self.assertIn(row, section)

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

    def test_summary_lists_all_double_battles(self):
        self.assertIn(
            "**Wattson, Shelly, Tate & Liza, and Maxie & Tabitha are double battles; all other teams below are single battles.**",
            self.markdown,
        )


if __name__ == "__main__":
    unittest.main()
