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
            "MAP_PACIFIDLOG_TOWN",
            "MAP_PETALBURG_CITY",
            "MAP_RUSTBORO_CITY",
            "MAP_SLATEPORT_CITY",
            "MAP_MAUVILLE_CITY",
            "MAP_LILYCOVE_CITY",
            "MAP_MOSSDEEP_CITY",
            "MAP_SOOTOPOLIS_CITY",
            "MAP_FORTREE_CITY",
            "MAP_EVER_GRANDE_CITY",
        )

        for map_name in maps:
            for method in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons"):
                with self.subTest(map_name=map_name, method=method):
                    self.assertEqual(GENERATOR.biome_for(map_name, method), "City")


if __name__ == "__main__":
    unittest.main()
