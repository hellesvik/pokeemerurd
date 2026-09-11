import unittest
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "include/config/pokemon.h"
DAYCARE = ROOT / "src/daycare.c"


class EggHatchingTests(unittest.TestCase):
    def test_egg_cycle_speed_is_configured_as_ten_times_faster(self):
        config = CONFIG.read_text()
        daycare = DAYCARE.read_text()
        self.assertRegex(config, r"#define P_EGG_CYCLE_SPEED_MULTIPLIER\s+10\b")
        self.assertIn("P_EGG_CYCLE_SPEED_MULTIPLIER", daycare)
        self.assertIn("EGG_CYCLE_INTERVAL(128)", daycare)


if __name__ == "__main__":
    unittest.main()
