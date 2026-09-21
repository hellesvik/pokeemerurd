import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SKY_PILLAR_TOP = ROOT / "data/maps/SkyPillar_Top/scripts.inc"


class SpecialEncounterLevelTests(unittest.TestCase):
    def test_rayquaza_static_encounter_is_level_45(self):
        self.assertIn(
            "setwildbattle SPECIES_RAYQUAZA, 45",
            SKY_PILLAR_TOP.read_text(),
        )


if __name__ == "__main__":
    unittest.main()
