#include "global.h"
#include "daycare.h"
#include "event_data.h"
#include "malloc.h"
#include "party_menu.h"
#include "regions.h"
#include "test/overworld_script.h"
#include "test/test.h"

void Test_GiveEggFromDaycare(void);

// We don't run the StoreSelectedPokemonInDaycare special because it relies on calling the
// party select screen and the GetCursorSelectionMonId function, so we store directly to the struct.
#define STORE_IN_DAYCARE_AND_GET_EGG()                                          \
    StorePokemonInDaycare(&gParties[B_TRAINER_PLAYER][0], &gSaveBlock1Ptr->daycare.mons[0]);  \
    StorePokemonInDaycare(&gParties[B_TRAINER_PLAYER][0], &gSaveBlock1Ptr->daycare.mons[1]);  \
    Test_GiveEggFromDaycare();

TEST("(Daycare) Pokémon generate Eggs of the lowest member of the evolutionary family")
{
    ASSUME(P_FAMILY_PIKACHU == TRUE);
    ASSUME(P_GEN_2_CROSS_EVOS == TRUE);

    ZeroPlayerPartyMons();
    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_PIKACHU, 100, gender=MON_MALE;
        givemon SPECIES_PIKACHU, 100, gender=MON_FEMALE;
    );
    STORE_IN_DAYCARE_AND_GET_EGG();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_PICHU);
}

TEST("(Daycare) Pokémon offspring species is based off the mother's species")
{
    u32 offspring = 0;
    ASSUME(P_FAMILY_PIKACHU == TRUE);
    ASSUME(P_GEN_2_CROSS_EVOS == TRUE);
    ASSUME(P_FAMILY_RIOLU == TRUE);

    ZeroPlayerPartyMons();
    PARAMETRIZE { offspring = SPECIES_RIOLU; RUN_OVERWORLD_SCRIPT(givemon SPECIES_PIKACHU, 100, gender=MON_MALE;   givemon SPECIES_LUCARIO, 100, gender=MON_FEMALE, item=ITEM_NONE;     ); }
    PARAMETRIZE { offspring = SPECIES_PICHU; RUN_OVERWORLD_SCRIPT(givemon SPECIES_PIKACHU, 100, gender=MON_FEMALE; givemon SPECIES_LUCARIO, 100, gender=MON_MALE;); }
    STORE_IN_DAYCARE_AND_GET_EGG();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), offspring);
}

TEST("(Daycare) Pokémon can breed with Ditto if they don't belong to the Ditto or No Eggs Discovered group")
{
    u32 j = 0;
    u32 parentSpecies = 0;

    ZeroPlayerPartyMons();
    for (j = 1; j < NUM_SPECIES; j++)
    {
        if (IsSpeciesEnabled(j))
            PARAMETRIZE { parentSpecies = j; }
    }
    VarSet(VAR_TEMP_C, parentSpecies);
    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_DITTO, 100; givemon VAR_TEMP_C, 100;
    );
    STORE_IN_DAYCARE_AND_GET_EGG();

    if (gSpeciesInfo[parentSpecies].eggGroups[0] != EGG_GROUP_NO_EGGS_DISCOVERED
     && gSpeciesInfo[parentSpecies].eggGroups[0] != EGG_GROUP_DITTO)
        EXPECT_NE(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_NONE);
    else
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_NONE);
}

TEST("(Daycare) Shellos' form is always based on the mother's form")
{
    u32 offspring = 0;
    ASSUME(P_FAMILY_MEOWTH == TRUE);
    ASSUME(P_ALOLAN_FORMS == TRUE);
    ASSUME(P_GALARIAN_FORMS == TRUE);

    ZeroPlayerPartyMons();
    PARAMETRIZE { offspring = SPECIES_SHELLOS_WEST; RUN_OVERWORLD_SCRIPT(givemon SPECIES_SHELLOS_EAST, 1, gender=MON_MALE; givemon SPECIES_SHELLOS_WEST, 1, gender=MON_FEMALE, item=ITEM_NONE;     ); }
    PARAMETRIZE { offspring = SPECIES_SHELLOS_WEST; RUN_OVERWORLD_SCRIPT(givemon SPECIES_SHELLOS_EAST, 1, gender=MON_MALE, item=ITEM_EVERSTONE; givemon SPECIES_SHELLOS_WEST, 1, gender=MON_FEMALE, item=ITEM_NONE;     ); }
    PARAMETRIZE { offspring = SPECIES_SHELLOS_WEST; RUN_OVERWORLD_SCRIPT(givemon SPECIES_SHELLOS_EAST, 1, gender=MON_MALE; givemon SPECIES_SHELLOS_WEST, 1, gender=MON_FEMALE, item=ITEM_EVERSTONE;); }
    PARAMETRIZE { offspring = SPECIES_SHELLOS_EAST; RUN_OVERWORLD_SCRIPT(givemon SPECIES_SHELLOS_WEST, 1, gender=MON_MALE; givemon SPECIES_SHELLOS_EAST, 1, gender=MON_FEMALE, item=ITEM_NONE;     ); }
    PARAMETRIZE { offspring = SPECIES_SHELLOS_EAST; RUN_OVERWORLD_SCRIPT(givemon SPECIES_SHELLOS_WEST, 1, gender=MON_MALE, item=ITEM_EVERSTONE; givemon SPECIES_SHELLOS_EAST, 1, gender=MON_FEMALE, item=ITEM_NONE;     ); }
    PARAMETRIZE { offspring = SPECIES_SHELLOS_EAST; RUN_OVERWORLD_SCRIPT(givemon SPECIES_SHELLOS_WEST, 1, gender=MON_MALE; givemon SPECIES_SHELLOS_EAST, 1, gender=MON_FEMALE, item=ITEM_EVERSTONE;); }
    STORE_IN_DAYCARE_AND_GET_EGG();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), offspring);
}

TEST("(Daycare) Pokémon with regional forms give the correct offspring")
{
    u32 region = 0, offspring = 0, species1 = 0, item1 = 0, species2 = 0, item2 = 0;

    ZeroPlayerPartyMons();

    region = GetCurrentRegion();
    if (region == REGION_ALOLA) {
        PARAMETRIZE { offspring=SPECIES_MEOWTH_ALOLA;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_ALOLA;  item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_ALOLA;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_ALOLA;  item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_ALOLA;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR;  item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR;  item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_ALOLA;  species1=SPECIES_DIGLETT;       item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR;  item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_DIGLETT;       item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR;  item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_PERRSERKER;    item1=ITEM_EVERSTONE; species2=SPECIES_PERSIAN;       item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH;        species1=SPECIES_PERRSERKER;    item1=ITEM_EVERSTONE; species2=SPECIES_PERSIAN;       item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH;        species1=SPECIES_PERSIAN_ALOLA; item1=ITEM_EVERSTONE; species2=SPECIES_PERSIAN;       item2=ITEM_EVERSTONE; }
    } else if (region == REGION_GALAR) {
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_ALOLA;  item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_ALOLA;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_ALOLA;  item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR;  item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR;  item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_DIGLETT;       item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR;  item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_DIGLETT;       item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR;  item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_PERRSERKER;    item1=ITEM_EVERSTONE; species2=SPECIES_PERSIAN;       item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH;        species1=SPECIES_PERRSERKER;    item1=ITEM_EVERSTONE; species2=SPECIES_PERSIAN;       item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH;        species1=SPECIES_PERSIAN_ALOLA; item1=ITEM_EVERSTONE; species2=SPECIES_PERSIAN;       item2=ITEM_EVERSTONE; }
    } else {
        PARAMETRIZE { offspring=SPECIES_MEOWTH;        species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_ALOLA,  item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_ALOLA;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_ALOLA,  item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH;        species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR,  item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_MEOWTH;        item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR,  item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH;        species1=SPECIES_DIGLETT;       item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR,  item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_DIGLETT;       item1=ITEM_NONE;      species2=SPECIES_MEOWTH_GALAR,  item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH_GALAR;  species1=SPECIES_PERRSERKER;    item1=ITEM_EVERSTONE; species2=SPECIES_PERSIAN,       item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_MEOWTH;        species1=SPECIES_PERRSERKER;    item1=ITEM_EVERSTONE; species2=SPECIES_PERSIAN,       item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_MEOWTH;        species1=SPECIES_PERSIAN_ALOLA; item1=ITEM_EVERSTONE; species2=SPECIES_PERSIAN,       item2=ITEM_EVERSTONE; }
    }

    if (region == REGION_HISUI) {
        PARAMETRIZE { offspring=SPECIES_SNEASEL_HISUI; species1=SPECIES_SNEASEL;       item1=ITEM_NONE;      species2=SPECIES_SNEASEL_HISUI, item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_SNEASEL;       species1=SPECIES_SNEASEL;       item1=ITEM_EVERSTONE; species2=SPECIES_SNEASEL_HISUI, item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_SNEASEL_HISUI; species1=SPECIES_SNEASEL;       item1=ITEM_NONE;      species2=SPECIES_SNEASEL_HISUI, item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_SNEASEL;       species1=SPECIES_SNEASLER;      item1=ITEM_EVERSTONE; species2=SPECIES_WEAVILE,       item2=ITEM_EVERSTONE; }
    } else {
        PARAMETRIZE { offspring=SPECIES_SNEASEL;       species1=SPECIES_SNEASEL;       item1=ITEM_NONE;      species2=SPECIES_SNEASEL_HISUI, item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_SNEASEL;       species1=SPECIES_SNEASEL;       item1=ITEM_EVERSTONE; species2=SPECIES_SNEASEL_HISUI, item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_SNEASEL_HISUI; species1=SPECIES_SNEASEL;       item1=ITEM_NONE;      species2=SPECIES_SNEASEL_HISUI, item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_SNEASEL;       species1=SPECIES_SNEASLER;      item1=ITEM_EVERSTONE; species2=SPECIES_WEAVILE,       item2=ITEM_EVERSTONE; }
    }

    if (region == REGION_PALDEA) {
        PARAMETRIZE { offspring=SPECIES_WOOPER_PALDEA; species1=SPECIES_WOOPER;        item1=ITEM_NONE;      species2=SPECIES_WOOPER_PALDEA, item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_WOOPER;        species1=SPECIES_WOOPER;        item1=ITEM_EVERSTONE; species2=SPECIES_WOOPER_PALDEA, item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_WOOPER_PALDEA; species1=SPECIES_WOOPER;        item1=ITEM_NONE;      species2=SPECIES_WOOPER_PALDEA, item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_WOOPER;        species1=SPECIES_CLODSIRE;      item1=ITEM_EVERSTONE; species2=SPECIES_QUAGSIRE,      item2=ITEM_EVERSTONE; }
    } else {
        PARAMETRIZE { offspring=SPECIES_WOOPER;        species1=SPECIES_WOOPER;        item1=ITEM_NONE;      species2=SPECIES_WOOPER_PALDEA, item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_WOOPER;        species1=SPECIES_WOOPER;        item1=ITEM_EVERSTONE; species2=SPECIES_WOOPER_PALDEA, item2=ITEM_NONE;      }
        PARAMETRIZE { offspring=SPECIES_WOOPER_PALDEA; species1=SPECIES_WOOPER;        item1=ITEM_NONE;      species2=SPECIES_WOOPER_PALDEA, item2=ITEM_EVERSTONE; }
        PARAMETRIZE { offspring=SPECIES_WOOPER;        species1=SPECIES_CLODSIRE;      item1=ITEM_EVERSTONE; species2=SPECIES_QUAGSIRE,      item2=ITEM_EVERSTONE; }
    }
    ASSUME(IsSpeciesEnabled(species1) == TRUE);
    ASSUME(IsSpeciesEnabled(species2) == TRUE);
    ASSUME(IsSpeciesEnabled(offspring) == TRUE);

    VarSet(VAR_0x8000, species1);
    VarSet(VAR_0x8001, item1);
    VarSet(VAR_0x8002, species2);
    VarSet(VAR_0x8003, item2);

    RUN_OVERWORLD_SCRIPT(givemon VAR_0x8000, 1, gender=MON_MALE,   item=VAR_0x8001;);
    RUN_OVERWORLD_SCRIPT(givemon VAR_0x8002, 1, gender=MON_FEMALE, item=VAR_0x8003;);

    STORE_IN_DAYCARE_AND_GET_EGG();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), offspring);
}

TEST("Day Care closure prevents training and breeding while walking")
{
    struct Pokemon mon;
    ZeroPlayerPartyMons();
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    CreateMon(&mon, SPECIES_PIKACHU, 20, 0, OTID_STRUCT_PRESET(0));
    gSaveBlock1Ptr->daycare.mons[0].mon = mon.box;
    CreateMon(&mon, SPECIES_DITTO, 20, 0, OTID_STRUCT_PRESET(0));
    gSaveBlock1Ptr->daycare.mons[1].mon = mon.box;
    gSaveBlock1Ptr->daycare.mons[0].steps = 255;
    gSaveBlock1Ptr->daycare.mons[1].steps = 255;
#if IS_FRLG
    gSaveBlock1Ptr->route5DayCareMon.mon = mon.box;
    gSaveBlock1Ptr->route5DayCareMon.steps = 255;
#endif
    struct DaycareMon before[DAYCARE_MON_COUNT];
    memcpy(before, gSaveBlock1Ptr->daycare.mons, sizeof(before));
    FlagClear(FLAG_PENDING_DAYCARE_EGG);
    for (u32 i = 0; i < 2048; i++)
        EXPECT(!ShouldEggHatch());
    EXPECT_EQ(memcmp(before, gSaveBlock1Ptr->daycare.mons, sizeof(before)), 0);
    EXPECT_EQ(gSaveBlock1Ptr->daycare.offspringPersonality, 0);
    EXPECT(!FlagGet(FLAG_PENDING_DAYCARE_EGG));
#if IS_FRLG
    EXPECT_EQ(gSaveBlock1Ptr->route5DayCareMon.steps, 255);
#endif
}

TEST("Day Care closure blocks its egg special even with compatible parents and a pending Egg")
{
    struct Pokemon mon;
    ZeroPlayerPartyMons();
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    CreateMon(&mon, SPECIES_PIKACHU, 20, 0, OTID_STRUCT_PRESET(0));
    gSaveBlock1Ptr->daycare.mons[0].mon = mon.box;
    CreateMon(&mon, SPECIES_DITTO, 20, 0, OTID_STRUCT_PRESET(0));
    gSaveBlock1Ptr->daycare.mons[1].mon = mon.box;
    gSaveBlock1Ptr->daycare.offspringPersonality = 12345;
    FlagSet(FLAG_PENDING_DAYCARE_EGG);
    RUN_OVERWORLD_SCRIPT(special GiveEggFromDaycare;);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 0);
    for (u32 i = 0; i < PARTY_SIZE; i++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES), SPECIES_NONE);
}

TEST("Day Care closure preserves scripted gift Eggs and party hatching")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_WYNAUT; }
    PARAMETRIZE { species = SPECIES_TOGEPI; }
    PARAMETRIZE { species = SPECIES_PICHU; }
    ZeroPlayerPartyMons();
    VarSet(VAR_TEMP_C, species);
    RUN_OVERWORLD_SCRIPT(giveegg VAR_TEMP_C;);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 1);
    struct Pokemon *egg = &gParties[B_TRAINER_PLAYER][0];
    EXPECT_EQ(GetMonData(egg, MON_DATA_SPECIES), species);
    EXPECT(GetMonData(egg, MON_DATA_IS_EGG));
    u32 cycles = 1;
    SetMonData(egg, MON_DATA_FRIENDSHIP, &cycles);
    gSaveBlock1Ptr->daycare.stepCounter = 0;
    bool32 ready = FALSE;
    for (u32 i = 0; i < 1024 && !ready; i++)
        ready = ShouldEggHatch();
    EXPECT(ready);
    EXPECT_EQ(gSpecialVar_0x8004, 0);
    EXPECT_EQ(GetMonData(egg, MON_DATA_FRIENDSHIP), 0);
}
