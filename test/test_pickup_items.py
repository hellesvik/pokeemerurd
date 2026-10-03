import re
import unittest
from pathlib import Path


SOURCE = Path(__file__).resolve().parents[1] / "src/battle_script_commands.c"


class PickupItemTests(unittest.TestCase):
    def test_pickup_never_rolls_rare_candy_and_all_level_weights_total_100(self):
        source = SOURCE.read_text()
        table = source.split("static const struct PickupItem sPickupTable[] =", 1)[1].split("};", 1)[0]
        rows = re.findall(r"\{\s*(ITEM_\w+),\s*\{([^}]*)\}\s*\}", table)

        self.assertTrue(rows)
        self.assertNotIn("ITEM_RARE_CANDY", (item for item, _ in rows))
        weights = [[0 if value.strip() == "_" else int(value) for value in values.split(",") if value.strip()]
                   for _, values in rows]
        self.assertTrue(all(len(row) == 10 for row in weights))
        self.assertEqual([100] * 10, [sum(row[level] for row in weights) for level in range(10)])


if __name__ == "__main__":
    unittest.main()
