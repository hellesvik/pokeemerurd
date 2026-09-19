import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
EVENT_MACROS = ROOT / "asm/macros/event.inc"
SCRIPT_COMMANDS = ROOT / "src/scrcmd.c"
TRAINER_SCRIPTS = ROOT / "data/scripts/trainer_battle.inc"


class LevelCapMessageOrderTests(unittest.TestCase):
    def test_notification_runs_when_the_post_battle_script_ends(self):
        macros = EVENT_MACROS.read_text()
        dotrainer_start = macros.index("\t.macro dotrainerbattle")
        dotrainer_end = macros.index("\t.endm", dotrainer_start)
        self.assertNotIn(
            "ShowQueuedLevelCapIncreaseMessage",
            macros[dotrainer_start:dotrainer_end],
        )

        commands = SCRIPT_COMMANDS.read_text()
        end_start = commands.index("bool8 ScrCmd_end(")
        end_end = commands.index("\n}\n", end_start)
        self.assertIn(
            "TryRunQueuedLevelCapIncreaseMessage(ctx)",
            commands[end_start:end_end],
        )

        trainer_scripts = TRAINER_SCRIPTS.read_text()
        notification_start = trainer_scripts.index(
            "EventScript_ShowQueuedLevelCapIncreaseMessage::"
        )
        notification_end = trainer_scripts.index("\n\n", notification_start)
        notification = trainer_scripts[notification_start:notification_end]
        self.assertIn("special ShowQueuedLevelCapIncreaseMessage", notification)
        self.assertIn("special RestoreResultAfterLevelCapIncreaseMessage", notification)


if __name__ == "__main__":
    unittest.main()
