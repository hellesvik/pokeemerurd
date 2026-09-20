import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
EVENT_MACROS = ROOT / "asm/macros/event.inc"
SCRIPT_COMMANDS = ROOT / "src/scrcmd.c"
TRAINER_SCRIPTS = ROOT / "data/scripts/trainer_battle.inc"
CAPS = ROOT / "src/caps.c"
SCRIPT_ENGINE = ROOT / "src/script.c"


class LevelCapMessageOrderTests(unittest.TestCase):
    def test_notification_keeps_the_new_level_on_a_visible_line(self):
        self.assertIn(
            '_("LEVEL CAP RAISED!\\nYour POKéMON can reach Lv. {STR_VAR_1}.")',
            CAPS.read_text(),
        )

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

    def test_notification_does_not_hijack_immediate_map_header_scripts(self):
        caps = CAPS.read_text()
        function_start = caps.index("bool8 TryRunQueuedLevelCapIncreaseMessage(")
        function_end = caps.index("\n}\n", function_start)
        function = caps[function_start:function_end]

        self.assertIn("ScriptContext_IsGlobal(ctx)", function)

        script_engine = SCRIPT_ENGINE.read_text()
        identity_start = script_engine.index("bool8 ScriptContext_IsGlobal(")
        identity_end = script_engine.index("\n}\n", identity_start)
        identity = script_engine[identity_start:identity_end]
        self.assertIn("ctx == &sGlobalScriptContext", identity)

    def test_notification_waits_for_an_available_message_box_without_retrying_itself(self):
        caps = CAPS.read_text()

        show_start = caps.index("void ShowQueuedLevelCapIncreaseMessage(")
        show_end = caps.index("\n}\n", show_start)
        show = caps[show_start:show_end]
        self.assertNotIn("ConsumeQueuedLevelCapIncrease()", show)
        self.assertLess(show.index("ShowFieldMessage("), show.index("sQueuedLevelCapIncrease = 0"))

        run_start = caps.index("bool8 TryRunQueuedLevelCapIncreaseMessage(")
        run_end = caps.index("\n}\n", run_start)
        run = caps[run_start:run_end]
        self.assertIn("IsFieldMessageBoxHidden()", run)
        self.assertIn("sLevelCapNotificationRunning", run)


if __name__ == "__main__":
    unittest.main()
