import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


class EarlyGiftScriptTests(unittest.TestCase):
    def test_oldale_employee_gives_old_rod_once(self):
        script = (ROOT / "data/maps/OldaleTown/scripts.inc").read_text()

        self.assertIn("giveitem ITEM_OLD_ROD", script)
        self.assertIn("setflag FLAG_RECEIVED_OLD_ROD", script)
        self.assertIn("goto_if_set FLAG_RECEIVED_OLD_ROD", script)
        self.assertIn("An OLD ROD lets you fish", script)

    def test_dewford_fisherman_gives_potion_once(self):
        script = (ROOT / "data/maps/DewfordTown/scripts.inc").read_text()

        self.assertIn("giveitem ITEM_POTION", script)
        self.assertIn("setflag FLAG_RECEIVED_POTION_OLDALE", script)
        self.assertIn("goto_if_set FLAG_RECEIVED_POTION_OLDALE", script)
        self.assertIn("I traded my ROD for this.", script)


if __name__ == "__main__":
    unittest.main()
