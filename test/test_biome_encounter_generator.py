import importlib.util
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "generate_biome_encounter_data",
    ROOT / "tools/generate_biome_encounter_data.py",
)
GENERATOR = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(GENERATOR)


class CityBiomeTests(unittest.TestCase):
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


class StoryProgressionBstTests(unittest.TestCase):
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
