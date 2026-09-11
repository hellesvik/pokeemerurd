import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRADE_DATA = ROOT / "src/data/trade.h"
TRADE_CODE = ROOT / "src/trade.c"


class PacifidlogTradeTests(unittest.TestCase):
    def test_pacifidlog_trade_definition(self):
        source = TRADE_DATA.read_text()
        start = source.index("[INGAME_TRADE_HORSEA]")
        end = source.index("[INGAME_TRADE_MEOWTH]", start)
        entry = source[start:end]

        self.assertIn('.nickname = _("ONKELDURR")', entry)
        self.assertIn(".species = SPECIES_CONKELDURR", entry)
        self.assertIn(".heldItem = ITEM_ROCKY_HELMET", entry)
        self.assertIn(".requestedSpecies = SPECIES_MEW", entry)

    def test_pacifidlog_trade_has_fixed_level_and_single_move(self):
        source = TRADE_CODE.read_text()
        start = source.index("static void CreateInGameTradePokemonInternal")
        end = source.index("static enum Species GetForkInGameTradeSpecies", start)
        function = source[start:end]

        self.assertIn("whichInGameTrade == INGAME_TRADE_HORSEA", function)
        self.assertIn("level = 69", function)
        self.assertIn("MOVE_STEEL_BEAM", function)
        self.assertIn("MOVE_NONE", function)


if __name__ == "__main__":
    unittest.main()
