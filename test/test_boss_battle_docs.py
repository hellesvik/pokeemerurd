import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DOC = ROOT / "docs/fork/gameplay/boss_battles.md"


class BossBattleDocsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.markdown = DOC.read_text()

    def section(self, heading):
        start = self.markdown.index(heading)
        end = self.markdown.find("\n## ", start + len(heading))
        return self.markdown[start:end if end >= 0 else None]

    def test_non_custom_teams_label_abilities_as_randomized(self):
        expected_rows = {
            "## Rustboro rival": "| Ability | Randomized | Randomized |",
            "## Route 110 rival": "| Ability | Randomized | Randomized | Randomized |",
            "## Maxie — Mt. Chimney": "| Ability | Randomized | Randomized | Randomized |",
            "## Route 119 rival": "| Ability | Randomized | Randomized | Randomized |",
            "## Lilycove rival": "| Ability | Randomized | Randomized | Randomized | Randomized |",
            "## Maxie — Magma Hideout": "| Ability | Randomized | Randomized | Randomized |",
            "### Tabitha's team": "| Ability | Randomized | Randomized | Randomized |",
            "## Maxie & Tabitha — Mossdeep": "| Ability | Randomized | Randomized | Sheer Force |",
            "## Archie": "| Ability | Randomized | Randomized | Strong Jaw |",
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


if __name__ == "__main__":
    unittest.main()
