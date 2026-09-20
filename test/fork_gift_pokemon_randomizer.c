#include "global.h"
#include "fork_gift_pokemon_randomizer.h"
#include "fork_ability_randomizer.h"
#include "fork_encounter_randomizer.h"
#include "fork_randomizer_catalog.h"
#include "fork_run.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"

static bool8 IsPseudoLegendaryFirstStage(enum Species species)
{
    switch (species)
    {
    case SPECIES_DRATINI:
    case SPECIES_LARVITAR:
    case SPECIES_BAGON:
    case SPECIES_GIBLE:
    case SPECIES_DEINO:
    case SPECIES_GOOMY:
    case SPECIES_JANGMO_O:
    case SPECIES_DREEPY:
    case SPECIES_FRIGIBAX:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool8 IsOfficialStarterSpecies(enum Species species)
{
    switch (species)
    {
    case SPECIES_BULBASAUR: case SPECIES_CHARMANDER: case SPECIES_SQUIRTLE:
    case SPECIES_CHIKORITA: case SPECIES_CYNDAQUIL: case SPECIES_TOTODILE:
    case SPECIES_TREECKO: case SPECIES_TORCHIC: case SPECIES_MUDKIP:
    case SPECIES_TURTWIG: case SPECIES_CHIMCHAR: case SPECIES_PIPLUP:
    case SPECIES_SNIVY: case SPECIES_TEPIG: case SPECIES_OSHAWOTT:
    case SPECIES_CHESPIN: case SPECIES_FENNEKIN: case SPECIES_FROAKIE:
    case SPECIES_ROWLET: case SPECIES_LITTEN: case SPECIES_POPPLIO:
    case SPECIES_GROOKEY: case SPECIES_SCORBUNNY: case SPECIES_SOBBLE:
    case SPECIES_SPRIGATITO: case SPECIES_FUECOCO: case SPECIES_QUAXLY:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool8 IsWeatherAbility(enum Ability ability)
{
    switch (ability)
    {
    case ABILITY_DRIZZLE:
    case ABILITY_DROUGHT:
    case ABILITY_SAND_STREAM:
    case ABILITY_SNOW_WARNING:
    case ABILITY_SAND_SPIT:
    case ABILITY_PRIMORDIAL_SEA:
    case ABILITY_DESOLATE_LAND:
    case ABILITY_DELTA_STREAM:
    case ABILITY_ORICHALCUM_PULSE:
    case ABILITY_SWIFT_SWIM:
    case ABILITY_RAIN_DISH:
    case ABILITY_HYDRATION:
    case ABILITY_DRY_SKIN:
    case ABILITY_CHLOROPHYLL:
    case ABILITY_SOLAR_POWER:
    case ABILITY_LEAF_GUARD:
    case ABILITY_FLOWER_GIFT:
    case ABILITY_HARVEST:
    case ABILITY_PROTOSYNTHESIS:
    case ABILITY_SAND_VEIL:
    case ABILITY_SAND_RUSH:
    case ABILITY_SAND_FORCE:
    case ABILITY_SNOW_CLOAK:
    case ABILITY_ICE_BODY:
    case ABILITY_SLUSH_RUSH:
    case ABILITY_ICE_FACE:
    case ABILITY_FORECAST:
    case ABILITY_CLOUD_NINE:
    case ABILITY_AIR_LOCK:
    case ABILITY_OVERCOAT:
    case ABILITY_TERAFORM_ZERO:
        return TRUE;
    default:
        return FALSE;
    }
}

TEST("Weather Institute gives Castform when Pokemon randomization is disabled")
{
    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT,
                                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                                 TRUE, GEN_9, TRUE, FALSE);

    EXPECT_EQ(GetForkRandomizedWeatherInstituteGiftSpecies(), SPECIES_CASTFORM);
}

TEST("Weather Institute randomized gift has a weather ability")
{
    enum Species first;
    enum Species second;
    enum Ability ability;

    ForkConfigureGameplayOptions(FALSE, FALSE, FORK_FAINT_RULE_WHITEOUT,
                                 FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,
                                 TRUE, GEN_9, TRUE, FALSE);
    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x57454154;

    first = GetForkRandomizedWeatherInstituteGiftSpecies();
    second = GetForkRandomizedWeatherInstituteGiftSpecies();
    ability = GetForkRandomizedAbility(first);

    EXPECT_EQ(first, second);
    EXPECT(ForkRandomizerCatalog_IsSpeciesAvailable(first));
    EXPECT_EQ(first, GET_BASE_SPECIES_ID(first));
    EXPECT(IsWeatherAbility(ability));
    Test_MgbaPrintf("Weather Institute test gift: %S with %S\n",
                    gSpeciesInfo[first].speciesName, gAbilitiesInfo[ability].name);
}

TEST("Fork starter choices use distinct Generation 3 Pokémon from 275 to 325 BST")
{
    enum Species starters[3];

    for (u32 seed = 1; seed <= 32; seed++)
    {
        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        for (u8 i = 0; i < ARRAY_COUNT(starters); i++)
        {
            starters[i] = GetForkRandomizedStarterSpecies(i);
            EXPECT_LE((u16)gSpeciesInfo[starters[i]].natDexNum, NATIONAL_DEX_DEOXYS);
            EXPECT_GE(GetSpeciesBaseStatTotal(starters[i]), 275);
            EXPECT_LE(GetSpeciesBaseStatTotal(starters[i]), 325);
        }
        EXPECT_NE(starters[0], starters[1]);
        EXPECT_NE(starters[0], starters[2]);
        EXPECT_NE(starters[1], starters[2]);
    }
}

TEST("Fork starter randomizer can choose non-starter Pokémon")
{
    bool8 foundNonStarter = FALSE;

    for (u32 seed = 1; seed <= 32; seed++)
    {
        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        for (u8 slot = 0; slot < 3; slot++)
            if (!IsOfficialStarterSpecies(GetForkRandomizedStarterSpecies(slot)))
                foundNonStarter = TRUE;
    }

    EXPECT(foundNonStarter);
}

TEST("Steven's gift is a Generation-compatible pseudo-legendary first stage")
{
    enum Species species;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x87654321;
    species = GetForkRandomizedStevenGiftSpecies();

    EXPECT_LE((u16)gSpeciesInfo[species].natDexNum, NATIONAL_DEX_DEOXYS);
    EXPECT(IsPseudoLegendaryFirstStage(species));
}

TEST("Fork starter receives at least two perfect IVs")
{
    struct Pokemon mon;
    u8 perfectIvs = 0;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x13572468;
    CreateMon(&mon, SPECIES_TREECKO, 5, 0, OTID_STRUCT_PRESET(0));
    ApplyForkStarterPerfectIvs(&mon, 0);

    for (u8 stat = 0; stat < NUM_STATS; stat++)
        if (GetMonData(&mon, MON_DATA_HP_IV + stat) == MAX_PER_STAT_IVS)
            perfectIvs++;
    EXPECT_GE(perfectIvs, 2);
}

TEST("Fortree trade uses its generation-filtered random pool")
{
    enum Species species;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x10293847;
    species = GetForkRandomizedFortreeTradeSpecies();

    EXPECT(species == SPECIES_BLASTOISE || species == SPECIES_SCIZOR || species == SPECIES_TORKOAL
        || species == SPECIES_MAGMORTAR || species == SPECIES_GOLURK || species == SPECIES_DRAGALGE
        || species == SPECIES_SHIINOTIC || species == SPECIES_MORPEKO || species == SPECIES_DACHSBUN);
    EXPECT_LE((u16)gSpeciesInfo[species].natDexNum, GetForkMaxNationalDex());
}

TEST("Fortree trade keeps the selected ability and shiny treatment")
{
    enum Species species;
    enum Ability expectedAbility;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x56473829;
    species = GetForkRandomizedFortreeTradeSpecies();

    switch (species)
    {
    case SPECIES_BLASTOISE: expectedAbility = ABILITY_MEGA_LAUNCHER; break;
    case SPECIES_SCIZOR: expectedAbility = ABILITY_TECHNICIAN; break;
    case SPECIES_TORKOAL: expectedAbility = ABILITY_DROUGHT; break;
    case SPECIES_MAGMORTAR: expectedAbility = ABILITY_FLASH_FIRE; break;
    case SPECIES_GOLURK: expectedAbility = ABILITY_UNSEEN_FIST; break;
    case SPECIES_DRAGALGE: expectedAbility = ABILITY_REGENERATOR; break;
    case SPECIES_SHIINOTIC: expectedAbility = ABILITY_POISON_HEAL; break;
    case SPECIES_MORPEKO: expectedAbility = ABILITY_HUNGER_SWITCH; break;
    case SPECIES_DACHSBUN: expectedAbility = ABILITY_WELL_BAKED_BODY; break;
    default: expectedAbility = ABILITY_NONE; break;
    }
    EXPECT_EQ(GetForkRandomizedFortreeTradeAbility(), expectedAbility);
    EXPECT_EQ(GetForkRandomizedFortreeTradeIsShiny(), species == SPECIES_GOLURK);
}

TEST("Rustboro trade uses its generation-filtered random pool")
{
    enum Species species;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x91827364;
    species = GetForkRandomizedRustboroTradeSpecies();

    EXPECT(species == SPECIES_PSYDUCK || species == SPECIES_CHIKORITA || species == SPECIES_MAWILE
        || species == SPECIES_SPIRITOMB || species == SPECIES_EELEKTROSS || species == SPECIES_MALAMAR
        || species == SPECIES_WISHIWASHI || species == SPECIES_CURSOLA || species == SPECIES_PALAFIN);
    EXPECT_LE((u16)gSpeciesInfo[species].natDexNum, GetForkMaxNationalDex());
}

TEST("Rustboro trade assigns its special held items and moves")
{
    enum Species species;
    enum Item heldItem;
    enum Move move;

    gSaveBlock3Ptr->forkItemRandomizerSeed = 0x19283746;
    species = GetForkRandomizedRustboroTradeSpecies();
    heldItem = GetForkRandomizedRustboroTradeHeldItem();
    move = GetForkRandomizedRustboroTradeMove(0);

    if (species == SPECIES_PSYDUCK)
        EXPECT_EQ(heldItem, ITEM_EVIOLITE);
    else if (species == SPECIES_CHIKORITA)
        EXPECT_EQ(heldItem, ITEM_MEGANIUMITE);
    else if (species == SPECIES_MAWILE)
        EXPECT_EQ(heldItem, ITEM_MAWILITE);
    else
        EXPECT_EQ(heldItem, ITEM_NONE);

    if (species == SPECIES_WISHIWASHI)
        EXPECT_EQ(move, MOVE_SHADOW_SNEAK);
    else if (species == SPECIES_EELEKTROSS)
        EXPECT_EQ(move, MOVE_THUNDERBOLT);
    else
        EXPECT_EQ(move, MOVE_NONE);
}

TEST("Rustboro Psyduck receives its special moveset")
{
    bool8 foundPsyduck = FALSE;

    for (u32 seed = 1; seed <= 64; seed++)
    {
        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        if (GetForkRandomizedRustboroTradeSpecies() == SPECIES_PSYDUCK)
        {
            foundPsyduck = TRUE;
            EXPECT_EQ(GetForkRandomizedRustboroTradeMove(0), MOVE_FOLLOW_ME);
            EXPECT_EQ(GetForkRandomizedRustboroTradeMove(1), MOVE_CONFUSION);
            EXPECT_EQ(GetForkRandomizedRustboroTradeMove(2), MOVE_CONFUSE_RAY);
            EXPECT_EQ(GetForkRandomizedRustboroTradeMove(3), MOVE_EXPLOSION);
            break;
        }
    }
    EXPECT(foundPsyduck);
}

TEST("Fork starter's other IVs can also roll perfect")
{
    struct Pokemon mon;
    bool8 foundExtraPerfectIv = FALSE;

    for (u32 seed = 1; seed <= 128; seed++)
    {
        u8 perfectIvs = 0;

        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        CreateMon(&mon, SPECIES_TREECKO, 5, 0, OTID_STRUCT_PRESET(0));
        ApplyForkStarterPerfectIvs(&mon, 0);
        for (u8 stat = 0; stat < NUM_STATS; stat++)
            if (GetMonData(&mon, MON_DATA_HP_IV + stat) == MAX_PER_STAT_IVS)
                perfectIvs++;
        if (perfectIvs > 2)
            foundExtraPerfectIv = TRUE;
    }

    EXPECT(foundExtraPerfectIv);
}

TEST("Fork starter perfect IV stats vary between saves")
{
    struct Pokemon mon;
    u8 firstPair = 0;
    bool8 foundDifferentPair = FALSE;

    for (u32 seed = 1; seed <= 32; seed++)
    {
        u8 pair = 0;

        gSaveBlock3Ptr->forkItemRandomizerSeed = seed;
        CreateMon(&mon, SPECIES_TREECKO, 5, 0, OTID_STRUCT_PRESET(0));
        ApplyForkStarterPerfectIvs(&mon, 0);
        for (u8 stat = 0; stat < NUM_STATS; stat++)
            if (GetMonData(&mon, MON_DATA_HP_IV + stat) == MAX_PER_STAT_IVS)
                pair |= 1 << stat;
        if (seed == 1)
            firstPair = pair;
        else if (pair != firstPair)
            foundDifferentPair = TRUE;
    }

    EXPECT(foundDifferentPair);
}
