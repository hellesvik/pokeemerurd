from pathlib import Path
import re


SOURCE = Path("src/item_use.c").read_text()


def function_body(name: str) -> str:
    match = re.search(rf"static void {name}\(u8 taskId\)\n\{{(.*?)\n\}}", SOURCE, re.DOTALL)
    assert match is not None
    return match.group(1)


def test_registered_infinite_repel_uses_the_overworld_message_renderer():
    body = function_body("Task_UseRepel")

    assert "tUsingRegisteredKeyItem" in body
    assert "DisplayItemMessageOnField" in body
