EverGrandeCity_PokemonLeague_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, EverGrandeCity_PokemonLeague_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

EverGrandeCity_PokemonLeague_1F_OnTransition:
	setrespawn HEAL_LOCATION_EVER_GRANDE_CITY_POKEMON_LEAGUE
	setflag FLAG_LANDMARK_POKEMON_LEAGUE
	call_if_unset FLAG_ENTERED_ELITE_FOUR, EverGrandeCity_PokemonLeague_1F_EventScript_GuardsBlockDoor
	end

EverGrandeCity_PokemonLeague_1F_EventScript_GuardsBlockDoor::
	setobjectxyperm LOCALID_LEAGUE_GUARD_1, 9, 2
	setobjectxyperm LOCALID_LEAGUE_GUARD_2, 10, 2
	return

EverGrandeCity_PokemonLeague_1F_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_LEAGUE_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

EverGrandeCity_PokemonLeague_1F_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart EverGrandeCity_PokemonLeague_1F_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
EverGrandeCity_PokemonLeague_1F_Pokemart:
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_MAX_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_REVIVE
	.2byte ITEM_MAX_REPEL
	pokemartlistend

@ The door guards only check for FLAG_BADGE06_GET because Winonas badge is the only one that can be skipped
@ Its assumed the player has the remaining badges
EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard::
	lockall
	goto_if_set FLAG_ENTERED_ELITE_FOUR, EverGrandeCity_PokemonLeague_1F_EventScript_GoForth
	getplayerxy VAR_TEMP_0, VAR_TEMP_1
	call_if_ge VAR_TEMP_0, 11, EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromRight
	call_if_le VAR_TEMP_0, 8, EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromLeft
	message EverGrandeCity_PokemonLeague_1F_Text_MustHaveAllGymBadges
	waitmessage
	delay 120
	goto_if_unset FLAG_BADGE06_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	closemessage
	applymovement LOCALID_LEAGUE_GUARD_1, EverGrandeCity_PokemonLeague_1F_Movement_LeftGuardOutOfWay
	applymovement LOCALID_LEAGUE_GUARD_2, EverGrandeCity_PokemonLeague_1F_Movement_RightGuardOutOfWay
	waitmovement 0
	delay 10
	playfanfare MUS_OBTAIN_BADGE
	message EverGrandeCity_PokemonLeague_1F_Text_GoForth
	waitmessage
	waitfanfare
	closemessage
	copyobjectxytoperm LOCALID_LEAGUE_GUARD_1
	copyobjectxytoperm LOCALID_LEAGUE_GUARD_2
	setflag FLAG_ENTERED_ELITE_FOUR
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromRight::
	applymovement LOCALID_PLAYER, EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromRight
	waitmovement 0
	return

EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromLeft::
	applymovement LOCALID_PLAYER, EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromLeft
	waitmovement 0
	return

EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges::
	playse SE_FAILURE
	msgbox EverGrandeCity_PokemonLeague_1F_Text_HaventObtainedAllBadges, MSGBOX_DEFAULT
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_EventScript_GoForth::
	applymovement VAR_LAST_TALKED, Common_Movement_FacePlayer
	waitmovement 0
	msgbox EverGrandeCity_PokemonLeague_1F_Text_GoForth, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromRight:
	walk_down
	walk_left
	walk_in_place_faster_up
	step_end

EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromLeft:
	walk_down
	walk_right
	walk_in_place_faster_up
	step_end

EverGrandeCity_PokemonLeague_1F_Movement_LeftGuardOutOfWay:
	walk_left
	walk_in_place_faster_down
	step_end

EverGrandeCity_PokemonLeague_1F_Movement_RightGuardOutOfWay:
	walk_right
	walk_in_place_faster_down
	step_end

EverGrandeCity_PokemonLeague_1F_Text_MustHaveAllGymBadges:
	.string "Beyond this point, only those TRAINERS\n"
	.string "who have collected all the GYM BADGES\l"
	.string "are permitted to enter.\p"
	.string "TRAINER, let us confirm that you have\n"
	.string "all the GYM BADGES.$"

EverGrandeCity_PokemonLeague_1F_Text_HaventObtainedAllBadges:
	.string "You haven't obtained all the BADGES.\p"
	.string "If you're bound for the POKéMON\n"
	.string "LEAGUE, you must return with them.$"

EverGrandeCity_PokemonLeague_1F_Text_GoForth:
	.string "TRAINER! Believe in yourself and your\n"
	.string "POKéMON, and go forth!$"


Collector_EventScript::
    lock
    faceplayer
    goto_if_set FLAG_COLLECTOR_TRADE_COMPLETE, Collector_Done
    msgbox Collector_Offer, MSGBOX_YESNO
    goto_if_eq VAR_RESULT, NO, Collector_End
    special CollectorTrade
    goto_if_eq VAR_RESULT, 1, Collector_NoCoins
    goto_if_eq VAR_RESULT, 2, Collector_NoSpace
    goto_if_eq VAR_RESULT, 3, Collector_Empty
    goto_if_eq VAR_RESULT, 4, Collector_Done
    msgbox Collector_Success, MSGBOX_DEFAULT
Collector_ReceiptLoop::
    special CollectorNextReceiptItem
    goto_if_eq VAR_RESULT, 0, Collector_End
    msgbox Collector_ReceiptText, MSGBOX_DEFAULT
    goto Collector_ReceiptLoop
Collector_End::
    release
    end
Collector_NoCoins::
    msgbox Collector_CoinsText, MSGBOX_DEFAULT
    goto Collector_End
Collector_NoSpace::
    msgbox Collector_SpaceText, MSGBOX_DEFAULT
    goto Collector_End
Collector_Empty::
    msgbox Collector_EmptyText, MSGBOX_DEFAULT
    goto Collector_End
Collector_Done::
    msgbox Collector_DoneText, MSGBOX_DEFAULT
    goto Collector_End
Collector_Offer:
    .string "COLLECTURD: I spent all my money\n"
    .string "on tokens to make this game.\p"
    .string "Then I tried to earn it back,\n"
    .string "but now Im in gambling debt. But\p"
    .string "hey, you look lazy and rich!\n"
    .string "Maybe you can pay off my debt by\p"
    .string "paying me 4000 coins? I will\n"
    .string "then give you all ball items you\p"
    .string "have not yet picked up manually.\n"
    .string "Not the hidden ones\p"
    .string "unfortunally, as I had to pawn\n"
    .string "my Itemfinder. So what do you\p"
    .string "say, 4000 coins to get the rest\n"
    .string "of the items?$"
Collector_CoinsText:
    .string "You need 4,000 COINS for our deal!$"
Collector_SpaceText:
    .string "No deal! Make room in your BAG,\n"
    .string "including your TMs, and come back.$"
Collector_EmptyText:
    .string "You've already found everything\n"
    .string "in my collection!$"
Collector_DoneText:
    .string "Thanks! Our trade is complete.\n"
    .string "I'm staying away from gambling!$"
Collector_Success:
    .string "Deal! Here's my entire collection!\n"
    .string "You paid 4,000 COINS.$"

Collector_ReceiptText:
    .string "COLLECTURD: Here's {STR_VAR_2} ×\n"
    .string "{STR_VAR_1}!$"
