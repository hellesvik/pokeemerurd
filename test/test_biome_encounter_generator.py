import importlib.util
import csv
import re
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "generate_biome_encounter_data",
    ROOT / "tools/generate_biome_encounter_data.py",
)
GENERATOR = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(GENERATOR)
CATALOG_SPEC = importlib.util.spec_from_file_location(
    "generate_biome_species_catalog",
    ROOT / "tools/generate_biome_species_catalog.py",
)
CATALOG_GENERATOR = importlib.util.module_from_spec(CATALOG_SPEC)
sys.modules[CATALOG_SPEC.name] = CATALOG_GENERATOR
CATALOG_SPEC.loader.exec_module(CATALOG_GENERATOR)


class CityBiomeTests(unittest.TestCase):
    def test_species_catalog_excludes_mega_forms(self):
        species = CATALOG_GENERATOR.read_canonical_species()

        self.assertFalse(
            [entry.constant for entry in species if entry.constant.endswith("_MEGA")]
        )
        species_constants = {entry.constant for entry in species}
        self.assertIn("CLEFABLE", species_constants)
        self.assertIn("FLOETTE", species_constants)
        with CATALOG_GENERATOR.OUTPUT_CSV.open(newline="") as catalog:
            constants = [row["species_constant"] for row in csv.DictReader(catalog)]
        self.assertFalse([constant for constant in constants if constant.endswith("_MEGA")])
        self.assertIsNone(
            re.search(r"SPECIES_[A-Z0-9_]*_MEGA(?:,|\s)", GENERATOR.OUTPUT.read_text())
        )

    def test_town_and_city_encounters_use_city_biome(self):
        maps = (
            "MAP_LITTLEROOT_TOWN",
            "MAP_OLDALE_TOWN",
            "MAP_DEWFORD_TOWN",
            "MAP_LAVARIDGE_TOWN",
            "MAP_FALLARBOR_TOWN",
            "MAP_VERDANTURF_TOWN",
            "MAP_PETALBURG_CITY",
            "MAP_RUSTBORO_CITY",
            "MAP_SLATEPORT_CITY",
            "MAP_MAUVILLE_CITY",
            "MAP_LILYCOVE_CITY",
            "MAP_MOSSDEEP_CITY",
            "MAP_SOOTOPOLIS_CITY",
            "MAP_EVER_GRANDE_CITY",
        )

        for map_name in maps:
            for method in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons"):
                with self.subTest(map_name=map_name, method=method):
                    self.assertEqual(GENERATOR.biome_for(map_name, method), "City")

    def test_fortree_uses_forest_biome(self):
        for method in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons"):
            with self.subTest(method=method):
                self.assertEqual(GENERATOR.biome_for("MAP_FORTREE_CITY", method), "Forest")

    def test_pacifidlog_uses_ocean_biome(self):
        for method in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons"):
            with self.subTest(method=method):
                self.assertEqual(GENERATOR.biome_for("MAP_PACIFIDLOG_TOWN", method), "Ocean")

    def test_new_mauville_uses_city_biome(self):
        maps = ("MAP_NEW_MAUVILLE_ENTRANCE", "MAP_NEW_MAUVILLE_INSIDE")

        for map_name in maps:
            for method in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons"):
                with self.subTest(map_name=map_name, method=method):
                    self.assertEqual(GENERATOR.biome_for(map_name, method), "City")


class UnderwaterBiomeTests(unittest.TestCase):
    def test_underwater_maps_use_underwater_biome(self):
        encounter_data = GENERATOR.json.loads(GENERATOR.ENCOUNTERS.read_text())[
            "wild_encounter_groups"
        ][0]["encounters"]
        underwater_encounters = [
            entry for entry in encounter_data if entry["map"].startswith("MAP_UNDERWATER_")
        ]

        self.assertTrue(underwater_encounters)
        for entry in underwater_encounters:
            for method in GENERATOR.METHODS:
                if method in entry:
                    with self.subTest(map_name=entry["map"], method=method):
                        self.assertEqual(
                            GENERATOR.biome_for(entry["map"], method), "Underwater"
                        )

    def test_underwater_pool_is_curated_and_has_at_least_200_species(self):
        species = CATALOG_GENERATOR.read_canonical_species()
        underwater_species = {
            entry.constant
            for entry in species
            if "Underwater" in CATALOG_GENERATOR.species_biomes(entry)
        }

        self.assertGreaterEqual(len(underwater_species), 200)
        self.assertIn("MAGIKARP", underwater_species)
        self.assertIn("ABOMASNOW", underwater_species)
        self.assertIn("DRAGONITE", underwater_species)
        self.assertIn("CHANDELURE", underwater_species)
        self.assertNotIn("WINGULL", underwater_species)


class StoryProgressionBstTests(unittest.TestCase):
    def test_method_specific_ranges_supply_unique_encounter_slots(self):
        self.assertEqual(
            GENERATOR.bst_range_for_encounter("MAP_ROUTE103", "fishing_mons"),
            (160, 270),
        )
        self.assertEqual(
            GENERATOR.bst_range_for_encounter("MAP_ROUTE103", "land_mons"),
            (160, 260),
        )

        for method in ("water_mons", "fishing_mons"):
            with self.subTest(method=method):
                self.assertEqual(
                    GENERATOR.bst_range_for_encounter(
                        "MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM", method
                    ),
                    (290, 535),
                )
        self.assertEqual(
            GENERATOR.bst_range_for_encounter(
                "MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM", "land_mons"
            ),
            (380, 480),
        )

    def test_bst_ranges_follow_main_story_order(self):
        expected = (
            ("MAP_ROUTE101", (150, 250)),
            ("MAP_ROUTE103", (160, 260)),
            ("MAP_ROUTE102", (170, 270)),
            ("MAP_ROUTE104", (180, 280)),
            ("MAP_PETALBURG_WOODS", (180, 280)),
            ("MAP_ROUTE116", (190, 290)),
            ("MAP_RUSTURF_TUNNEL", (190, 290)),
            ("MAP_ROUTE105", (200, 300)),
            ("MAP_ROUTE106", (200, 300)),
            ("MAP_DEWFORD_TOWN", (200, 300)),
            ("MAP_GRANITE_CAVE_1F", (210, 310)),
            ("MAP_ROUTE107", (220, 320)),
            ("MAP_ROUTE108", (220, 320)),
            ("MAP_ROUTE109", (220, 320)),
            ("MAP_SLATEPORT_CITY", (230, 330)),
            ("MAP_ROUTE110", (240, 340)),
            ("MAP_MAUVILLE_CITY", (250, 350)),
            ("MAP_ROUTE117", (250, 350)),
            ("MAP_ROUTE111", (260, 360)),
            ("MAP_ROUTE112", (260, 360)),
            ("MAP_FIERY_PATH", (260, 360)),
            ("MAP_ROUTE113", (270, 370)),
            ("MAP_ROUTE114", (280, 380)),
            ("MAP_METEOR_FALLS_1F_1R", (290, 390)),
            ("MAP_ROUTE115", (290, 390)),
            ("MAP_JAGGED_PASS", (300, 400)),
            ("MAP_LAVARIDGE_TOWN", (300, 400)),
            ("MAP_PETALBURG_CITY", (300, 400)),
            ("MAP_ROUTE118", (310, 410)),
            ("MAP_ROUTE119", (320, 420)),
            ("MAP_FORTREE_CITY", (320, 420)),
            ("MAP_ROUTE120", (330, 430)),
            ("MAP_ROUTE121", (340, 440)),
            ("MAP_ROUTE122", (340, 440)),
            ("MAP_MT_PYRE_1F", (340, 440)),
            ("MAP_ROUTE123", (350, 450)),
            ("MAP_LILYCOVE_CITY", (350, 450)),
            ("MAP_ROUTE124", (360, 460)),
            ("MAP_MOSSDEEP_CITY", (370, 470)),
            ("MAP_ROUTE125", (380, 480)),
            ("MAP_ROUTE127", (390, 490)),
            ("MAP_ROUTE128", (400, 500)),
            ("MAP_SEAFLOOR_CAVERN_ROOM1", (410, 510)),
            ("MAP_ROUTE126", (420, 520)),
            ("MAP_SOOTOPOLIS_CITY", (430, 530)),
            ("MAP_ROUTE129", (440, 540)),
            ("MAP_ROUTE130", (450, 550)),
            ("MAP_ROUTE131", (460, 560)),
            ("MAP_SKY_PILLAR_1F", (460, 560)),
            ("MAP_EVER_GRANDE_CITY", (470, 560)),
            ("MAP_VICTORY_ROAD_1F", (490, 560)),
        )

        previous = (0, 0)
        for map_name, bst_range in expected:
            with self.subTest(map_name=map_name):
                self.assertEqual(GENERATOR.bst_range_for(map_name), bst_range)
                self.assertGreaterEqual(bst_range[0], previous[0])
                self.assertGreaterEqual(bst_range[1], previous[1])
                previous = bst_range


if __name__ == "__main__":
    unittest.main()
