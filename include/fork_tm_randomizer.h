#ifndef GUARD_FORK_TM_RANDOMIZER_H
#define GUARD_FORK_TM_RANDOMIZER_H

#include "global.h"

#define FOREACH_FORK_RANDOM_TM(F) F(51) F(52) F(53) F(54) F(55) F(56) F(57) F(58) F(59) F(60) F(61) F(62) F(63) F(64) F(65) F(66) F(67) F(68) F(69) F(70) F(71) F(72) F(73) F(74) F(75) F(76) F(77) F(78) F(79) F(80) F(81) F(82) F(83) F(84) F(85) F(86) F(87) F(88) F(89) F(90) F(91) F(92) F(93) F(94) F(95) F(96) F(97) F(98) F(99) F(100) F(101) F(102) F(103) F(104) F(105) F(106) F(107) F(108) F(109) F(110) F(111) F(112) F(113) F(114) F(115) F(116) F(117) F(118) F(119) F(120) F(121) F(122) F(123) F(124) F(125) F(126) F(127) F(128) F(129) F(130)
#define FOREACH_FORK_ADDITIONAL_TM(F) F(101) F(102) F(103) F(104) F(105) F(106) F(107) F(108) F(109) F(110) F(111) F(112) F(113) F(114) F(115) F(116) F(117) F(118) F(119) F(120) F(121) F(122) F(123) F(124) F(125) F(126) F(127) F(128) F(129) F(130)
#define FORK_LILYCOVE_TM_SHOP_ITEM_COUNT 8

void InitForkRandomizedTMMoves(void);
enum Move GetForkRandomizedTMMove(enum Item item);
enum Item GetForkRandomizedTMItem(u16 tmNumber);
enum Item GetForkRandomizedTMItemFromMoveId(enum Move move);
bool32 IsForkLilycoveTmShopItem(enum Item item);
bool32 HasForkLilycoveTmShopItemBeenPurchased(enum Item item);
void MarkForkLilycoveTmShopItemPurchased(enum Item item);

#endif
