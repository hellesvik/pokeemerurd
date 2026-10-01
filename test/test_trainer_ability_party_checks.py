import unittest
from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[1] / "src/battle_script_commands.c").read_text()


class TrainerAbilityPartyCheckTests(unittest.TestCase):
    def test_poke_flute_checks_effective_party_ability(self):
        body = SOURCE.split("static void UpdatePokeFlutePartyStatus", 1)[1].split("void BS_CheckPokeFlute", 1)[0]
        self.assertIn("GetMonAbility(&party[i])", body)
        self.assertNotIn("GetAbilityBySpecies(species, abilityNum)", body)

    def test_heal_bell_checks_effective_inactive_party_ability(self):
        body = SOURCE.split("static void Cmd_healpartystatus(void)\n{", 1)[1].split("static void Cmd_cursetarget", 1)[0]
        self.assertIn("ability = GetMonAbility(&party[i]);", body)
        self.assertNotIn("ability = GetAbilityBySpecies(species, abilityNum);", body)


if __name__ == "__main__":
    unittest.main()
