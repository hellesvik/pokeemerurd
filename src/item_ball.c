#include "global.h"
#include "coins.h"
#include "item_ball.h"
#include "event_data.h"
#include "item.h"
#include "malloc.h"
#include "new_game.h"
#include "random.h"
#include "script_menu.h"
#include "string_util.h"
#include "strings.h"
#include "fork_tm_randomizer.h"
#include "fork_run.h"
#include "fork_randomizer_catalog.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"

#define FIRST_HM_ITEM ITEM_HM01
#define LAST_HM_ITEM ITEM_HM08
#define FORK_ITEM_RANDOMIZER_VERSION 12
#if IS_FRLG
#define FIRST_ITEM_BALL_FLAG FLAG_HIDE_VIRIDIAN_FOREST_POTION
#define LAST_ITEM_BALL_FLAG FLAG_HIDE_SILPH_CO_4F_TM41
#else
#define FIRST_ITEM_BALL_FLAG FLAG_ITEM_ROUTE_102_POTION
#define LAST_ITEM_BALL_FLAG FLAG_ITEM_SAFARI_ZONE_SOUTH_EAST_BIG_PEARL
#endif

static bool8 sForkItemBallRandomizationPending;
static bool8 sForkHiddenItemRandomizationPending;

static const enum Item sForkRandomizedItemPool[] =
{
    ITEM_ABOMASITE,
    ITEM_ABSOLITE,
    ITEM_AERODACTYLITE,
    ITEM_AGGRONITE,
    ITEM_ALAKAZITE,
    ITEM_ALTARIANITE,
    ITEM_AMPHAROSITE,
    ITEM_AUDINITE,
    ITEM_BANETTITE,
    ITEM_BARBARACITE,
    ITEM_BAXCALIBRITE,
    ITEM_BEEDRILLITE,
    ITEM_BLASTOISINITE,
    ITEM_BLAZIKENITE,
    ITEM_CAMERUPTITE,
    ITEM_CHANDELURITE,
    ITEM_CHARIZARDITE_X,
    ITEM_CHARIZARDITE_Y,
    ITEM_CHESNAUGHTITE,
    ITEM_CHIMECHITE,
    ITEM_CLEFABLITE,
    ITEM_CRABOMINITE,
    ITEM_DARKRANITE,
    ITEM_DELPHOXITE,
    ITEM_DIANCITE,
    ITEM_DRAGALGITE,
    ITEM_DRAGONINITE,
    ITEM_DRAMPANITE,
    ITEM_EELEKTROSSITE,
    ITEM_EMBOARITE,
    ITEM_EXCADRITE,
    ITEM_FALINKSITE,
    ITEM_FERALIGITE,
    ITEM_FLOETTITE,
    ITEM_FROSLASSITE,
    ITEM_GALLADITE,
    ITEM_GARCHOMPITE,
    ITEM_GARDEVOIRITE,
    ITEM_GENGARITE,
    ITEM_GLALITITE,
    ITEM_GLIMMORANITE,
    ITEM_GOLISOPITE,
    ITEM_GOLURKITE,
    ITEM_GRENINJITE,
    ITEM_GYARADOSITE,
    ITEM_HAWLUCHANITE,
    ITEM_HEATRANITE,
    ITEM_HERACRONITE,
    ITEM_HOUNDOOMINITE,
    ITEM_KANGASKHANITE,
    ITEM_LATIASITE,
    ITEM_LATIOSITE,
    ITEM_LOPUNNITE,
    ITEM_LUCARIONITE,
    ITEM_MAGEARNITE,
    ITEM_MALAMARITE,
    ITEM_MANECTITE,
    ITEM_MAWILITE,
    ITEM_MEDICHAMITE,
    ITEM_MEGANIUMITE,
    ITEM_MEOWSTICITE,
    ITEM_METAGROSSITE,
    ITEM_MEWTWONITE_X,
    ITEM_MEWTWONITE_Y,
    ITEM_PIDGEOTITE,
    ITEM_PINSIRITE,
    ITEM_PYROARITE,
    ITEM_RAICHUNITE_X,
    ITEM_RAICHUNITE_Y,
    ITEM_SABLENITE,
    ITEM_SALAMENCITE,
    ITEM_SCEPTILITE,
    ITEM_SCIZORITE,
    ITEM_SCOLIPITE,
    ITEM_SCOVILLAINITE,
    ITEM_SCRAFTINITE,
    ITEM_SHARPEDONITE,
    ITEM_SKARMORITE,
    ITEM_SLOWBRONITE,
    ITEM_STARAPTITE,
    ITEM_STARMINITE,
    ITEM_STEELIXITE,
    ITEM_SWAMPERTITE,
    ITEM_TATSUGIRINITE,
    ITEM_TYRANITARITE,
    ITEM_VENUSAURITE,
    ITEM_VICTREEBELITE,
    ITEM_ZERAORITE,
    ITEM_ZYGARDITE,
    ITEM_AUSPICIOUS_ARMOR,
    ITEM_BLACK_AUGURITE,
    ITEM_CHIPPED_POT,
    ITEM_CRACKED_POT,
    ITEM_DAWN_STONE,
    ITEM_DEEP_SEA_SCALE,
    ITEM_DEEP_SEA_TOOTH,
    ITEM_DRAGON_SCALE,
    ITEM_DUBIOUS_DISC,
    ITEM_DUSK_STONE,
    ITEM_ELECTIRIZER,
    ITEM_FIRE_STONE,
    ITEM_GALARICA_CUFF,
    ITEM_GALARICA_WREATH,
    ITEM_ICE_STONE,
    ITEM_LEAF_STONE,
    ITEM_LINKING_CORD,
    ITEM_MAGMARIZER,
    ITEM_MALICIOUS_ARMOR,
    ITEM_MASTERPIECE_TEACUP,
    ITEM_METAL_ALLOY,
    ITEM_METAL_COAT,
    ITEM_MOON_STONE,
    ITEM_OVAL_STONE,
    ITEM_PEAT_BLOCK,
    ITEM_PRISM_SCALE,
    ITEM_PROTECTOR,
    ITEM_RAZOR_CLAW,
    ITEM_RAZOR_FANG,
    ITEM_REAPER_CLOTH,
    ITEM_SACHET,
    ITEM_SCROLL_OF_DARKNESS,
    ITEM_SCROLL_OF_WATERS,
    ITEM_SHINY_STONE,
    ITEM_SUN_STONE,
    ITEM_SWEET_APPLE,
    ITEM_SYRUPY_APPLE,
    ITEM_TART_APPLE,
    ITEM_THUNDER_STONE,
    ITEM_UNREMARKABLE_TEACUP,
    ITEM_UPGRADE,
    ITEM_WATER_STONE,
    ITEM_WHIPPED_DREAM,
    ITEM_ADAMANT_CRYSTAL,
    ITEM_ADAMANT_ORB,
    ITEM_BLUE_ORB,
    ITEM_BUG_MEMORY,
    ITEM_BURN_DRIVE,
    ITEM_CHILL_DRIVE,
    ITEM_CORNERSTONE_MASK,
    ITEM_DARK_MEMORY,
    ITEM_DNA_SPLICERS,
    ITEM_DOUSE_DRIVE,
    ITEM_DRAGON_MEMORY,
    ITEM_ELECTRIC_MEMORY,
    ITEM_FAIRY_MEMORY,
    ITEM_FIGHTING_MEMORY,
    ITEM_FIRE_MEMORY,
    ITEM_FLYING_MEMORY,
    ITEM_GHOST_MEMORY,
    ITEM_GRACIDEA,
    ITEM_GRASS_MEMORY,
    ITEM_GRISEOUS_CORE,
    ITEM_GRISEOUS_ORB,
    ITEM_GROUND_MEMORY,
    ITEM_HEARTHFLAME_MASK,
    ITEM_ICE_MEMORY,
    ITEM_LUSTROUS_GLOBE,
    ITEM_LUSTROUS_ORB,
    ITEM_PINK_NECTAR,
    ITEM_POISON_MEMORY,
    ITEM_PRISON_BOTTLE,
    ITEM_PSYCHIC_MEMORY,
    ITEM_PURPLE_NECTAR,
    ITEM_RED_NECTAR,
    ITEM_RED_ORB,
    ITEM_REINS_OF_UNITY,
    ITEM_REVEAL_GLASS,
    ITEM_ROCK_MEMORY,
    ITEM_ROTOM_CATALOG,
    ITEM_RUSTED_SHIELD,
    ITEM_RUSTED_SWORD,
    ITEM_SHOCK_DRIVE,
    ITEM_SOUL_DEW,
    ITEM_STEEL_MEMORY,
    ITEM_WATER_MEMORY,
    ITEM_WELLSPRING_MASK,
    ITEM_YELLOW_NECTAR,
    ITEM_ZYGARDE_CUBE,
    ITEM_ABSORB_BULB,
    ITEM_ADRENALINE_ORB,
    ITEM_AIR_BALLOON,
    ITEM_AMULET_COIN,
    ITEM_ASSAULT_VEST,
    ITEM_BERSERK_GENE,
    ITEM_BIG_ROOT,
    ITEM_BINDING_BAND,
    ITEM_BLACK_BELT,
    ITEM_BLACK_SLUDGE,
    ITEM_BOOSTER_ENERGY,
    ITEM_BRIGHT_POWDER,
    ITEM_CELL_BATTERY,
    ITEM_CHARCOAL,
    ITEM_CHOICE_BAND,
    ITEM_CHOICE_SCARF,
    ITEM_CHOICE_SPECS,
    ITEM_CLEAR_AMULET,
    ITEM_COVERT_CLOAK,
    ITEM_DESTINY_KNOT,
    ITEM_DRAGON_FANG,
    ITEM_EJECT_BUTTON,
    ITEM_EJECT_PACK,
    ITEM_EVERSTONE,
    ITEM_EVIOLITE,
    ITEM_EXPERT_BELT,
    ITEM_FLAME_ORB,
    ITEM_FOCUS_BAND,
    ITEM_FOCUS_SASH,
    ITEM_GRASSY_SEED,
    ITEM_GRIP_CLAW,
    ITEM_HEAVY_DUTY_BOOTS,
    ITEM_IRON_BALL,
    ITEM_KINGS_ROCK,
    ITEM_LEFTOVERS,
    ITEM_LIFE_ORB,
    ITEM_LIGHT_BALL,
    ITEM_LOADED_DICE,
    ITEM_MAGNET,
    ITEM_MENTAL_HERB,
    ITEM_MIRACLE_SEED,
    ITEM_MIRROR_HERB,
    ITEM_MISTY_SEED,
    ITEM_MUSCLE_BAND,
    ITEM_MYSTIC_WATER,
    ITEM_NEVER_MELT_ICE,
    ITEM_POISON_BARB,
    ITEM_POWER_HERB,
    ITEM_PROTECTIVE_PADS,
    ITEM_PSYCHIC_SEED,
    ITEM_PUNCHING_GLOVE,
    ITEM_QUICK_CLAW,
    ITEM_RING_TARGET,
    ITEM_ROCKY_HELMET,
    ITEM_ROOM_SERVICE,
    ITEM_SAFETY_GOGGLES,
    ITEM_SCOPE_LENS,
    ITEM_SHARP_BEAK,
    ITEM_SHELL_BELL,
    ITEM_SILK_SCARF,
    ITEM_SILVER_POWDER,
    ITEM_SOFT_SAND,
    ITEM_SOOTHE_BELL,
    ITEM_SPELL_TAG,
    ITEM_STICKY_BARB,
    ITEM_TERRAIN_EXTENDER,
    ITEM_THICK_CLUB,
    ITEM_THROAT_SPRAY,
    ITEM_TOXIC_ORB,
    ITEM_TWISTED_SPOON,
    ITEM_UTILITY_UMBRELLA,
    ITEM_WEAKNESS_POLICY,
    ITEM_WHITE_HERB,
    ITEM_WIDE_LENS,
    ITEM_WISE_GLASSES,
    ITEM_ZOOM_LENS,
#define ADD_TM_TO_POOL(number, move) ITEM_TM##number,
    FOREACH_GEN9_TM(ADD_TM_TO_POOL)
#undef ADD_TM_TO_POOL
};

static const enum Item sForkRandomizedHiddenItemPool[] =
{
    ITEM_POKE_BALL,
    ITEM_GREAT_BALL,
    ITEM_ULTRA_BALL,
    ITEM_PREMIER_BALL,
    ITEM_HEAL_BALL,
    ITEM_NET_BALL,
    ITEM_NEST_BALL,
    ITEM_DIVE_BALL,
    ITEM_DUSK_BALL,
    ITEM_TIMER_BALL,
    ITEM_QUICK_BALL,
    ITEM_REPEAT_BALL,
    ITEM_LUXURY_BALL,
    ITEM_TINY_MUSHROOM,
    ITEM_BIG_MUSHROOM,
    ITEM_NUGGET,
    ITEM_BIG_NUGGET,
    ITEM_BALM_MUSHROOM,
    ITEM_PEARL,
    ITEM_BIG_PEARL,
    ITEM_PEARL_STRING,
    ITEM_STARDUST,
    ITEM_STAR_PIECE,
    ITEM_COMET_SHARD,
    ITEM_RARE_BONE,
};

static const enum Item sForkGameCornerPrizes[FORK_GAME_CORNER_MENU_PRIZE_COUNT] =
{
    ITEM_FIRE_STONE,
    ITEM_WATER_STONE,
    ITEM_THUNDER_STONE,
    ITEM_LEAF_STONE,
    ITEM_ICE_STONE,
    ITEM_SUN_STONE,
    ITEM_MOON_STONE,
    ITEM_SHINY_STONE,
    ITEM_DUSK_STONE,
    ITEM_DAWN_STONE,
    ITEM_LINKING_CORD,
};

static EWRAM_DATA u8 sForkGameCornerPrizeMenuNames[FORK_GAME_CORNER_MENU_PRIZE_COUNT + 1][40];

STATIC_ASSERT(ARRAY_COUNT(sForkRandomizedItemPool) == FORK_ITEM_RANDOMIZER_POOL_COUNT, ForkItemRandomizerPoolSizeMismatch);

static u32 GetItemBallAmountFromTemplate(u32);
static u32 GetItemBallIdFromTemplate(u32);
static void EnsureForkItemRandomizerInitialized(void);
static u16 GetNextUnclaimedPoolIndex(u16 sourceId);
static u16 GetStableUnclaimedPoolIndex(u16 sourceId);
static bool32 GetForkBit(const u8 *bits, u16 index);
static void SetForkBit(u8 *bits, u16 index);

static u32 GetItemBallAmountFromTemplate(u32 itemBallId)
{
    u32 amount = gMapHeader.events->objectEvents[itemBallId].movementRangeX;

    if (amount > MAX_BAG_ITEM_CAPACITY)
        return MAX_BAG_ITEM_CAPACITY;

    return (amount == 0) ? 1 : amount;
}

static u32 GetItemBallIdFromTemplate(u32 itemBallId)
{
    enum Item itemId = gMapHeader.events->objectEvents[itemBallId].trainerRange_berryTreeId;

    return (itemId >= ITEMS_COUNT) ? (ITEM_NONE + 1) : itemId;
}

static bool32 IsForkProtectedItem(enum Item itemId)
{
    if (itemId >= FIRST_HM_ITEM && itemId <= LAST_HM_ITEM)
        return TRUE;

    switch (itemId)
    {
    case ITEM_NONE:
    case ITEM_LETTER:
    case ITEM_DEVON_GOODS:
    case ITEM_METEORITE:
    case ITEM_GO_GOGGLES:
    case ITEM_DEVON_SCOPE:
    case ITEM_BASEMENT_KEY:
    case ITEM_MAGMA_EMBLEM:
    case ITEM_STORAGE_KEY:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 GetForkBit(const u8 *bits, u16 index)
{
    return (bits[index / 8] >> (index % 8)) & 1;
}

static void SetForkBit(u8 *bits, u16 index)
{
    bits[index / 8] |= 1 << (index % 8);
}

static bool32 IsSpeciesSpecificItemAvailable(enum Item item)
{
    switch (item)
    {
    case ITEM_ADAMANT_CRYSTAL:
    case ITEM_ADAMANT_ORB:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_DIALGA);
    case ITEM_BLUE_ORB:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_KYOGRE);
    case ITEM_DNA_SPLICERS:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_KYUREM);
    case ITEM_GRISEOUS_CORE:
    case ITEM_GRISEOUS_ORB:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_GIRATINA);
    case ITEM_LUSTROUS_GLOBE:
    case ITEM_LUSTROUS_ORB:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_PALKIA);
    case ITEM_RED_ORB:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_GROUDON);
    case ITEM_REINS_OF_UNITY:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_CALYREX);
    case ITEM_RUSTED_SHIELD:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_ZAMAZENTA);
    case ITEM_RUSTED_SWORD:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_ZACIAN);
    case ITEM_SOUL_DEW:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_LATIAS);
    case ITEM_GRACIDEA:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_SHAYMIN);
    case ITEM_ROTOM_CATALOG:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_ROTOM);
    case ITEM_BURN_DRIVE:
    case ITEM_CHILL_DRIVE:
    case ITEM_DOUSE_DRIVE:
    case ITEM_SHOCK_DRIVE:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_GENESECT);
    case ITEM_REVEAL_GLASS:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_TORNADUS);
    case ITEM_PRISON_BOTTLE:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_HOOPA);
    case ITEM_ZYGARDE_CUBE:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_ZYGARDE);
    case ITEM_BUG_MEMORY:
    case ITEM_DARK_MEMORY:
    case ITEM_DRAGON_MEMORY:
    case ITEM_ELECTRIC_MEMORY:
    case ITEM_FAIRY_MEMORY:
    case ITEM_FIGHTING_MEMORY:
    case ITEM_FIRE_MEMORY:
    case ITEM_FLYING_MEMORY:
    case ITEM_GHOST_MEMORY:
    case ITEM_GRASS_MEMORY:
    case ITEM_GROUND_MEMORY:
    case ITEM_ICE_MEMORY:
    case ITEM_POISON_MEMORY:
    case ITEM_PSYCHIC_MEMORY:
    case ITEM_ROCK_MEMORY:
    case ITEM_STEEL_MEMORY:
    case ITEM_WATER_MEMORY:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_SILVALLY);
    case ITEM_PINK_NECTAR:
    case ITEM_PURPLE_NECTAR:
    case ITEM_RED_NECTAR:
    case ITEM_YELLOW_NECTAR:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_ORICORIO);
    case ITEM_CORNERSTONE_MASK:
    case ITEM_HEARTHFLAME_MASK:
    case ITEM_WELLSPRING_MASK:
        return ForkRandomizerCatalog_IsSpeciesAvailable(SPECIES_OGERPON);
    default:
        return TRUE;
    }
}

// Separate salts keep each family's choice stable and independent of allocation order.
static const enum Item sMemoryVariants[] =
{
    ITEM_BUG_MEMORY, ITEM_DARK_MEMORY, ITEM_DRAGON_MEMORY, ITEM_ELECTRIC_MEMORY,
    ITEM_FAIRY_MEMORY, ITEM_FIGHTING_MEMORY, ITEM_FIRE_MEMORY, ITEM_FLYING_MEMORY,
    ITEM_GHOST_MEMORY, ITEM_GRASS_MEMORY, ITEM_GROUND_MEMORY, ITEM_ICE_MEMORY,
    ITEM_POISON_MEMORY, ITEM_PSYCHIC_MEMORY, ITEM_ROCK_MEMORY, ITEM_STEEL_MEMORY,
    ITEM_WATER_MEMORY,
};
static const enum Item sDriveVariants[] = {ITEM_BURN_DRIVE, ITEM_CHILL_DRIVE, ITEM_DOUSE_DRIVE, ITEM_SHOCK_DRIVE};
static const enum Item sNectarVariants[] = {ITEM_PINK_NECTAR, ITEM_PURPLE_NECTAR, ITEM_RED_NECTAR, ITEM_YELLOW_NECTAR};
static const enum Item sMaskVariants[] = {ITEM_CORNERSTONE_MASK, ITEM_HEARTHFLAME_MASK, ITEM_WELLSPRING_MASK};

static bool32 IsSelectedItemFamilyVariant(enum Item item)
{
    static const struct
    {
        const enum Item *items;
        u32 count;
        u32 salt;
    } families[] =
    {
        {sMemoryVariants, ARRAY_COUNT(sMemoryVariants), 0x4D454D52},
        {sDriveVariants, ARRAY_COUNT(sDriveVariants), 0x44524956},
        {sNectarVariants, ARRAY_COUNT(sNectarVariants), 0x4E454354},
        {sMaskVariants, ARRAY_COUNT(sMaskVariants), 0x4D41534B},
    };

    for (u32 family = 0; family < ARRAY_COUNT(families); family++)
    {
        for (u32 variant = 0; variant < families[family].count; variant++)
        {
            if (families[family].items[variant] == item)
            {
                rng_value_t rng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ families[family].salt);
                return variant == LocalRandom(&rng) % families[family].count;
            }
        }
    }
    return TRUE;
}

bool32 IsForkItemEligibleForRandomizerPool(enum Item item)
{
    if (!IsSelectedItemFamilyVariant(item))
        return FALSE;
    if (gItemsInfo[item].sortType == ITEM_TYPE_MEGA_STONE)
        return ForkAreMegaEvolutionsEnabled()
            && ForkRandomizerCatalog_IsMegaStoneRelevant(item);
    if ((gItemsInfo[item].sortType == ITEM_TYPE_EVOLUTION_ITEM
      || gItemsInfo[item].sortType == ITEM_TYPE_EVOLUTION_STONE)
     && gItemsInfo[item].holdEffect == HOLD_EFFECT_NONE)
        return ForkRandomizerCatalog_IsEvolutionItemRelevant(item);
    return IsSpeciesSpecificItemAvailable(item);
}

void ResetForkItemRandomizerState(void)
{
    sForkItemBallRandomizationPending = FALSE;
    sForkHiddenItemRandomizationPending = FALSE;
    gSaveBlock3Ptr->forkItemRandomizerVersion = FORK_ITEM_RANDOMIZER_VERSION;
    gSaveBlock3Ptr->forkItemRandomizerNextScan = 0;
    memset(gSaveBlock3Ptr->forkItemRandomizerSourceAssigned, 0, sizeof(gSaveBlock3Ptr->forkItemRandomizerSourceAssigned));
    memset(gSaveBlock3Ptr->forkItemRandomizerPoolClaimed, 0, sizeof(gSaveBlock3Ptr->forkItemRandomizerPoolClaimed));

    for (u16 i = 0; i < FORK_ITEM_RANDOMIZER_SOURCE_COUNT; i++)
        gSaveBlock3Ptr->forkItemRandomizerSourceToPoolIndex[i] = FORK_ITEM_RANDOMIZER_UNASSIGNED;

    for (u16 i = 0; i < FORK_GAME_CORNER_PRIZE_COUNT; i++)
        gSaveBlock3Ptr->forkGameCornerPrizeItems[i] = ITEM_NONE;
}

static void EnsureForkItemRandomizerInitialized(void)
{
    if (gSaveBlock3Ptr->forkItemRandomizerVersion != FORK_ITEM_RANDOMIZER_VERSION)
        ResetForkItemRandomizerState();
}

void InitForkItemRandomizerSeed(void)
{
    gSaveBlock3Ptr->forkItemRandomizerSeed = Random32() ^ GetTrainerId(gSaveBlock2Ptr->playerTrainerId) ^ gSaveBlock1Ptr->dailySeed;
    ResetForkItemRandomizerState();
}

static u16 GetNextUnclaimedPoolIndex(u16 sourceId)
{
    rng_value_t localRng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ sourceId ^ gSaveBlock3Ptr->forkItemRandomizerNextScan);

    for (u16 attempt = 0; attempt < FORK_ITEM_RANDOMIZER_POOL_COUNT; attempt++)
    {
        u16 candidate = LocalRandom(&localRng) % FORK_ITEM_RANDOMIZER_POOL_COUNT;
        if (!GetForkBit(gSaveBlock3Ptr->forkItemRandomizerPoolClaimed, candidate)
         && IsForkItemEligibleForRandomizerPool(sForkRandomizedItemPool[candidate]))
        {
            SetForkBit(gSaveBlock3Ptr->forkItemRandomizerPoolClaimed, candidate);
            gSaveBlock3Ptr->forkItemRandomizerNextScan = candidate + 1;
            return candidate;
        }
    }

    return FORK_ITEM_RANDOMIZER_UNASSIGNED;
}

static u16 GetStableUnclaimedPoolIndex(u16 sourceId)
{
    rng_value_t localRng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ sourceId);
    u16 candidate = LocalRandom(&localRng) % FORK_ITEM_RANDOMIZER_POOL_COUNT;

    for (u16 attempt = 0; attempt < FORK_ITEM_RANDOMIZER_POOL_COUNT; attempt++)
    {
        if (!GetForkBit(gSaveBlock3Ptr->forkItemRandomizerPoolClaimed, candidate)
         && IsForkItemEligibleForRandomizerPool(sForkRandomizedItemPool[candidate]))
        {
            SetForkBit(gSaveBlock3Ptr->forkItemRandomizerPoolClaimed, candidate);
            return candidate;
        }
        candidate = (candidate + 1) % FORK_ITEM_RANDOMIZER_POOL_COUNT;
    }

    return GetNextUnclaimedPoolIndex(sourceId);
}

static u16 GetForkItemBallSourceId(u16 itemBallId)
{
    u16 flagId = gMapHeader.events->objectEvents[itemBallId].flagId;

    if (flagId >= FIRST_ITEM_BALL_FLAG && flagId <= LAST_ITEM_BALL_FLAG)
        return flagId - FIRST_ITEM_BALL_FLAG;
    return FORK_ITEM_RANDOMIZER_SOURCE_COUNT - 1;
}

enum Item ResolveForkRandomizedItem(enum Item itemId, u16 sourceId)
{
    if (!ForkIsItemRandomizerEnabled())
        return itemId;
    u16 poolIndex;

    EnsureForkItemRandomizerInitialized();
    if (IsForkProtectedItem(itemId))
        return itemId;
    if (GetItemPocket(itemId) == POCKET_KEY_ITEMS)
        return itemId;

    if (sourceId >= FORK_ITEM_RANDOMIZER_SOURCE_COUNT)
        return itemId;

    if (!GetForkBit(gSaveBlock3Ptr->forkItemRandomizerSourceAssigned, sourceId))
    {
        poolIndex = GetStableUnclaimedPoolIndex(sourceId);
        gSaveBlock3Ptr->forkItemRandomizerSourceToPoolIndex[sourceId] = poolIndex;
        SetForkBit(gSaveBlock3Ptr->forkItemRandomizerSourceAssigned, sourceId);
    }
    else
    {
        poolIndex = gSaveBlock3Ptr->forkItemRandomizerSourceToPoolIndex[sourceId];
    }

    if (poolIndex == FORK_ITEM_RANDOMIZER_UNASSIGNED || poolIndex >= ARRAY_COUNT(sForkRandomizedItemPool))
        return itemId;

    return sForkRandomizedItemPool[poolIndex];
}

void InitForkGameCornerPrizeCatalog(void)
{
}

enum Item GetForkGameCornerPrizeItem(u16 index)
{
    return index < ARRAY_COUNT(sForkGameCornerPrizes) ? sForkGameCornerPrizes[index] : ITEM_NONE;
}

void ScrCmd_BuildForkGameCornerPrizeMenu(struct ScriptContext *ctx)
{
    (void)ctx;
    InitForkGameCornerPrizeCatalog();
    MultichoiceDynamic_UseStaticNames();

    for (u16 i = 0; i < FORK_GAME_CORNER_MENU_PRIZE_COUNT; i++)
    {
        u8 *name = sForkGameCornerPrizeMenuNames[i];
        StringCopy(name, GetItemName(GetForkGameCornerPrizeItem(i)));
        StringAppend(name, COMPOUND_STRING("  4,000 COINS"));
        MultichoiceDynamic_PushElement((struct ListMenuItem){ .name = name, .id = i });
    }

    {
        u8 *name = sForkGameCornerPrizeMenuNames[FORK_GAME_CORNER_MENU_PRIZE_COUNT];
        StringCopy(name, gText_Exit);
        MultichoiceDynamic_PushElement((struct ListMenuItem){ .name = name, .id = LIST_CANCEL });
    }
}

void ScrCmd_GetForkGameCornerPrizeItem(struct ScriptContext *ctx)
{
    u16 destVar = ScriptReadHalfword(ctx);

    VarSet(destVar, GetForkGameCornerPrizeItem(gSpecialVar_Result));
}

enum Item ResolveForkRandomizedItemBall(enum Item itemId, u16 sourceId)
{
    sForkItemBallRandomizationPending = TRUE;
    return ResolveForkRandomizedItem(itemId, sourceId);
}

bool8 ConsumeForkItemBallRandomizationGuard(void)
{
    bool8 pending = sForkItemBallRandomizationPending;

    sForkItemBallRandomizationPending = FALSE;
    return pending;
}

bool8 ConsumeForkHiddenItemRandomizationGuard(void)
{
    bool8 pending = sForkHiddenItemRandomizationPending;

    sForkHiddenItemRandomizationPending = FALSE;
    return pending;
}

void ClearForkItemBallRandomizationGuard(void)
{
    sForkItemBallRandomizationPending = FALSE;
}

enum Item ResolveForkRandomizedHiddenItem(enum Item itemId, u16 hiddenItemFlag)
{
    if (!ForkIsItemRandomizerEnabled())
        return itemId;
    rng_value_t localRng;

    if (hiddenItemFlag < FLAG_HIDDEN_ITEMS_START)
        return itemId;

    EnsureForkItemRandomizerInitialized();
    if (IsForkProtectedItem(itemId) || GetItemPocket(itemId) == POCKET_KEY_ITEMS)
        return itemId;

    sForkHiddenItemRandomizationPending = TRUE;
    localRng = LocalRandomSeed(gSaveBlock3Ptr->forkItemRandomizerSeed ^ hiddenItemFlag ^ 0x48494444);
    return sForkRandomizedHiddenItemPool[LocalRandom(&localRng) % ARRAY_COUNT(sForkRandomizedHiddenItemPool)];
}

static u16 GetForkGymLeaderItemSourceId(enum Item itemId)
{
    if (IS_FRLG)
    {
        switch (itemId)
        {
        case ITEM_TM_ROCK_TOMB: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START;
        case ITEM_TM_WATER_PULSE: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 1;
        case ITEM_TM34: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 2;
        case ITEM_TM_GIGA_DRAIN: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 3;
        case ITEM_TM_TOXIC: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 4;
        case ITEM_TM_CALM_MIND: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 5;
        case ITEM_TM_FIRE_BLAST: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 6;
        case ITEM_TM_EARTHQUAKE: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 7;
        default: return FORK_ITEM_RANDOMIZER_UNASSIGNED;
        }
    }
    switch (itemId)
    {
    case ITEM_TM_ROCK_TOMB:  return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 0;
    case ITEM_TM_BULK_UP:    return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 1;
    case ITEM_TM34: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 2;
    case ITEM_TM_OVERHEAT:   return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 3;
    case ITEM_TM_FACADE:     return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 4;
    case ITEM_TM_AERIAL_ACE: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 5;
    case ITEM_TM_CALM_MIND:  return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 6;
    case ITEM_TM_WATER_PULSE: return FORK_ITEM_RANDOMIZER_GYM_LEADER_SOURCE_START + 7;
    default:                 return FORK_ITEM_RANDOMIZER_UNASSIGNED;
    }
}

enum Item ResolveForkRandomizedScriptItem(enum Item itemId, const u8 *scriptPtr)
{
    u16 sourceId;

    (void)scriptPtr;
    sourceId = GetForkGymLeaderItemSourceId(itemId);
    if (sourceId != FORK_ITEM_RANDOMIZER_UNASSIGNED)
        return ResolveForkRandomizedItem(itemId, sourceId);

    return itemId;
}

void GetItemBallIdAndAmountFromTemplate(void)
{
    u32 itemBallId;

    // The special is normally called by an item-ball object event. Do not
    // dereference an arbitrary object index if a malformed script invokes it.
    if (gMapHeader.events == NULL
     || gSpecialVar_LastTalked == 0
     || gSpecialVar_LastTalked > gMapHeader.events->objectEventCount)
    {
        gSpecialVar_Result = ITEM_NONE;
        gSpecialVar_0x8009 = 0;
        ClearForkItemBallRandomizationGuard();
        return;
    }

    itemBallId = gSpecialVar_LastTalked - 1;
    gSpecialVar_Result = ResolveForkRandomizedItemBall(GetItemBallIdFromTemplate(itemBallId), GetForkItemBallSourceId(itemBallId));
    gSpecialVar_0x8009 = GetItemBallAmountFromTemplate(itemBallId);
}

#if IS_FRLG
static const struct { enum Item item; u16 flag; } sCollectorItems[] = {
#include "data/fork_collector_kanto.h"
};
static EWRAM_DATA struct { enum Item item; u16 count; } sCollectorReceipt[ARRAY_COUNT(sCollectorItems)];
static EWRAM_DATA u16 sCollectorReceiptCount;
static EWRAM_DATA u16 sCollectorReceiptIndex;

void CollectorNextReceiptItem(void)
{
    gSpecialVar_Result = sCollectorReceiptIndex < sCollectorReceiptCount;
    if (!gSpecialVar_Result)
        return;
    CopyItemName(sCollectorReceipt[sCollectorReceiptIndex].item, gStringVar1);
    ConvertIntToDecimalStringN(gStringVar2, sCollectorReceipt[sCollectorReceiptIndex].count, STR_CONV_MODE_LEFT_ALIGN, 3);
    sCollectorReceiptIndex++;
}

void CollectorTrade(void)
{
    struct SaveBlock1 *before1;
    struct SaveBlock3 *before3;
    bool32 remaining = FALSE;

    sCollectorReceiptCount = sCollectorReceiptIndex = 0;
    gSpecialVar_Result = 4;
    if (FlagGet(FLAG_COLLECTOR_TRADE_COMPLETE))
        return;
    for (u32 i = 0; i < ARRAY_COUNT(sCollectorItems); i++)
        remaining |= !FlagGet(sCollectorItems[i].flag);
    if (!remaining)
    {
        gSpecialVar_Result = 3;
        return;
    }
    if (GetCoins() < 4000)
    {
        gSpecialVar_Result = 1;
        return;
    }

    before1 = Alloc(sizeof(*before1));
    before3 = Alloc(sizeof(*before3));
    if (before1 == NULL || before3 == NULL)
    {
        Free(before1);
        Free(before3);
        gSpecialVar_Result = 2;
        return;
    }
    *before1 = *gSaveBlock1Ptr;
    *before3 = *gSaveBlock3Ptr;
    for (u32 i = 0; i < ARRAY_COUNT(sCollectorItems); i++)
    {
        enum Item item;
        u16 flag = sCollectorItems[i].flag;
        u16 source = flag >= FIRST_ITEM_BALL_FLAG && flag <= LAST_ITEM_BALL_FLAG
            ? flag - FIRST_ITEM_BALL_FLAG : FORK_ITEM_RANDOMIZER_SOURCE_COUNT - 1;
        if (FlagGet(flag))
            continue;
        item = ResolveForkRandomizedItem(sCollectorItems[i].item, source);
        if (!AddBagItem(item, 1))
        {
            *gSaveBlock1Ptr = *before1;
            *gSaveBlock3Ptr = *before3;
            Free(before1);
            Free(before3);
            sCollectorReceiptCount = 0;
            gSpecialVar_Result = 2;
            return;
        }
        sCollectorReceipt[sCollectorReceiptCount].item = item;
        sCollectorReceipt[sCollectorReceiptCount++].count = 1;
        FlagSet(flag);
    }
    RemoveCoins(4000);
    FlagSet(FLAG_COLLECTOR_TRADE_COMPLETE);
    Free(before1);
    Free(before3);
    gSpecialVar_Result = 0;
}

#if TESTING
void Test_CollectorLeaveOnlyPotion(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCollectorItems); i++)
        FlagSet(sCollectorItems[i].flag);
    FlagClear(FLAG_HIDE_VIRIDIAN_FOREST_POTION);
}

void Test_CollectorLeaveOnlyEarlyBall(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCollectorItems); i++)
        FlagSet(sCollectorItems[i].flag);
    FlagClear(FLAG_HIDE_VIRIDIAN_FOREST_POKE_BALL);
}
#endif
#endif

void ResolveForkKantoItemBall(void)
{
    if (!IS_FRLG || gMapHeader.events == NULL || gSpecialVar_LastTalked == 0
        || gSpecialVar_LastTalked > gMapHeader.events->objectEventCount)
        return;
    u16 source = GetForkItemBallSourceId(gSpecialVar_LastTalked - 1);
    gSpecialVar_0x8000 = ResolveForkRandomizedItemBall(gSpecialVar_0x8000, source);
}

void ResolveForkKantoGymReward(void)
{
    gSpecialVar_0x8000 = ResolveForkRandomizedItemBall(gSpecialVar_0x8000,
        GetForkGymLeaderItemSourceId(gSpecialVar_0x8000));
}
