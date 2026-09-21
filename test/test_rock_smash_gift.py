import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ROCK_SMASH_DUDE = ROOT / "data/maps/MauvilleCity_House1/scripts.inc"
WINSTRATE_HOUSE = ROOT / "data/maps/Route111_WinstrateFamilysHouse/scripts.inc"


class RockSmashGiftTests(unittest.TestCase):
    def test_rock_smash_dude_redirects_player_to_the_winstrates(self):
        script = ROCK_SMASH_DUDE.read_text()

        self.assertNotIn("giveitem ITEM_HM_ROCK_SMASH", script)
        self.assertIn('"I gave the HM Rock Smash to the\\n"', script)
        self.assertIn('"Winstrate family.$"', script)

    def test_winstrate_woman_gives_rock_smash_instead_of_macho_brace(self):
        script = WINSTRATE_HOUSE.read_text()

        self.assertIn("giveitem ITEM_HM_ROCK_SMASH", script)
        self.assertNotIn("giveitem ITEM_MACHO_BRACE", script)
        self.assertIn("setflag FLAG_RECEIVED_HM_ROCK_SMASH", script)
        self.assertIn("setflag FLAG_HIDE_ROUTE_111_ROCK_SMASH_TIP_GUY", script)


if __name__ == "__main__":
    unittest.main()
