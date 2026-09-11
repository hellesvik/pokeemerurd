import unittest
from pathlib import Path


TRADE = Path(__file__).resolve().parents[1] / "src/trade.c"


class NpcTradeIvTests(unittest.TestCase):
    def test_npc_trade_creation_requests_random_ivs(self):
        source = TRADE.read_text()
        start = source.index("static void CreateInGameTradePokemonInternal")
        end = source.index("static enum Species GetForkInGameTradeSpecies", start)
        function = source[start:end]
        self.assertIn("CreateMonWithIVs(pokemon, species, level, inGameTrade->personality", function)
        self.assertIn("USE_RANDOM_IVS", function)
        self.assertNotIn("SetMonData(pokemon, MON_DATA_HP_IV", function)


if __name__ == "__main__":
    unittest.main()
