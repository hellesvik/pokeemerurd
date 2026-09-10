import unittest
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tools.generate_evolution_reference import build_entries, render_markdown


class EvolutionReferenceGeneratorTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.entries = build_entries(ROOT)
        cls.by_name = {entry.name: entry for entry in cls.entries}

    def test_includes_every_national_dex_species(self):
        base_entries = [entry for entry in self.entries if entry.is_base_form]
        regional_entries = [entry for entry in self.entries if not entry.is_base_form]
        national_dex_numbers = {entry.national_dex for entry in base_entries}
        self.assertEqual(national_dex_numbers, set(range(1, 1026)))
        self.assertEqual(len(base_entries), 1025)
        self.assertEqual(len(regional_entries), 57)
        self.assertEqual(len(self.entries), 1082)

    def test_uses_unique_human_readable_names(self):
        names = [entry.name for entry in self.entries]
        self.assertEqual(len(names), len(set(names)))

    def test_names_regional_forms_explicitly(self):
        self.assertIn("Sandshrew", self.by_name)
        self.assertIn("Alolan Sandshrew", self.by_name)
        self.assertIn("Galarian Zigzagoon", self.by_name)
        self.assertIn("Hisuian Zorua", self.by_name)
        self.assertIn("Paldean Wooper", self.by_name)
        self.assertNotIn("Alolan Pikachu", self.by_name)
        self.assertFalse(any("Totem" in name for name in self.by_name))

    def test_formats_representative_evolution_methods(self):
        self.assertIn("Level 16 → Ivysaur", self.by_name["Bulbasaur"].evolutions)
        self.assertIn("Trade → Alakazam", self.by_name["Kadabra"].evolutions)
        self.assertIn("Linking Cord → Alakazam", self.by_name["Kadabra"].evolutions)
        self.assertIn("Level 30 → Malamar", self.by_name["Inkay"].evolutions)
        self.assertIn("Level up with 999 Gimmighoul Coins in the Bag → Gholdengo", self.by_name["Gimmighoul"].evolutions)
        self.assertIn("Cracked Pot as Phony Form → Polteageist (Phony Form)", self.by_name["Sinistea"].evolutions)
        self.assertIn("Chipped Pot as Antique Form → Polteageist (Antique Form)", self.by_name["Sinistea"].evolutions)
        self.assertEqual(
            self.by_name["Milcery"].evolutions,
            ("Spin in the overworld while holding a Sweet → Alcremie (form depends on the Sweet, time, and spin)",),
        )
        self.assertIn("Trade for Shelmet → Escavalier", self.by_name["Karrablast"].evolutions)
        self.assertIn("Level 20 with an empty party slot and at least 1 Poké Ball → Shedinja", self.by_name["Nincada"].evolutions)
        self.assertIn("Level up after defeating 3 Bisharp holding Leader's Crests → Kingambit", self.by_name["Bisharp"].evolutions)
        self.assertIn("Level 20 if male → Mothim", self.by_name["Burmy"].evolutions)
        self.assertFalse(any("Mothim Plant" in method for method in self.by_name["Burmy"].evolutions))
        self.assertIn("Level 25 during the day → Lycanroc (Midday Form)", self.by_name["Rockruff"].evolutions)
        self.assertIn("Level 25 at night → Lycanroc (Midnight Form)", self.by_name["Rockruff"].evolutions)
        self.assertIn("Level 25 as Own Tempo form, at evening → Lycanroc (Dusk Form)", self.by_name["Rockruff"].evolutions)
        self.assertIn("Dark Scroll trigger → Urshifu (Single Strike Style)", self.by_name["Kubfu"].evolutions)
        self.assertIn("Water Scroll trigger → Urshifu (Rapid Strike Style)", self.by_name["Kubfu"].evolutions)

    def test_explains_form_dependent_evolutions(self):
        self.assertIn("Level 20 as Plant Cloak, if female → Wormadam (Plant Cloak)", self.by_name["Burmy"].evolutions)
        self.assertIn("Level 30 as West Sea form → Gastrodon (West Sea)", self.by_name["Shellos"].evolutions)
        self.assertIn(
            "Level up as White-Striped Form, after taking at least 294 recoil damage, if male → Basculegion (Male)",
            self.by_name["Basculin"].evolutions,
        )
        self.assertIn("Level 34 as Winter Form → Sawsbuck (Winter)", self.by_name["Deerling"].evolutions)
        self.assertIn("Trade as Super Size → Gourgeist (Super Size)", self.by_name["Pumpkaboo"].evolutions)
        self.assertIn("Level 25 as Own Tempo form, at evening → Lycanroc (Dusk Form)", self.by_name["Rockruff"].evolutions)
        self.assertIn("Cracked Pot as Phony Form → Polteageist (Phony Form)", self.by_name["Sinistea"].evolutions)
        self.assertIn(
            "Masterpiece Teacup as Artisan Form → Sinistcha (Masterpiece Form)",
            self.by_name["Poltchageist"].evolutions,
        )

    def test_renders_generation_sections_and_source_notice(self):
        markdown = render_markdown(self.entries)
        self.assertIn("Generation 1 — Kanto", markdown)
        self.assertIn("Generation 9 — Paldea", markdown)
        self.assertIn("generated from the game's species data", markdown)


if __name__ == "__main__":
    unittest.main()
