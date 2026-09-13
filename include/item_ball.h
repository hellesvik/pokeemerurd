#ifndef GUARD_ITEM_BALL_H
#define GUARD_ITEM_BALL_H

#include "script.h"

void GetItemBallIdAndAmountFromTemplate(void);
enum Item ResolveForkRandomizedItem(enum Item itemId, u16 sourceId);
bool32 IsForkItemEligibleForRandomizerPool(enum Item item);
enum Item ResolveForkRandomizedItemBall(enum Item itemId, u16 sourceId);
enum Item ResolveForkRandomizedHiddenItem(enum Item itemId, u16 hiddenItemFlag);
enum Item ResolveForkRandomizedScriptItem(enum Item itemId, const u8 *scriptPtr);
bool8 ConsumeForkItemBallRandomizationGuard(void);
bool8 ConsumeForkHiddenItemRandomizationGuard(void);
void ClearForkItemBallRandomizationGuard(void);
void InitForkItemRandomizerSeed(void);
void ResetForkItemRandomizerState(void);
void InitForkGameCornerPrizeCatalog(void);
enum Item GetForkGameCornerPrizeItem(u16 index);
void ScrCmd_BuildForkGameCornerPrizeMenu(struct ScriptContext *ctx);
void ScrCmd_GetForkGameCornerPrizeItem(struct ScriptContext *ctx);

#endif //GUARD_ITEM_BALL_H
