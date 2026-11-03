IndigoPlateau_PokemonCenter_1F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, IndigoPlateau_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

IndigoPlateau_PokemonCenter_1F_OnTransition::
	setrespawn HEAL_LOCATION_INDIGO_PLATEAU
	specialvar VAR_RESULT, IsNationalPokedexEnabled
	call_if_eq VAR_RESULT, TRUE, IndigoPlateau_PokemonCenter_1F_EventScript_CheckBlockDoor
	end

IndigoPlateau_PokemonCenter_1F_EventScript_CheckBlockDoor::
	goto_if_set FLAG_IS_CHAMPION, Common_EventScript_NopReturn
	setobjectxyperm LOCALID_LEAGUE_DOOR_GUARD, 4, 2
	return

IndigoPlateau_PokemonCenter_1F_EventScript_DoorGuard::
	lock
	faceplayer
	specialvar VAR_RESULT, IsNationalPokedexEnabled
	goto_if_eq VAR_RESULT, TRUE, IndigoPlateau_PokemonCenter_1F_EventScript_CheckSeviiIslandComplete
	msgbox IndigoPlateau_PokemonCenter_1F_Text_FaceEliteFourGoodLuck
	release
	end

IndigoPlateau_PokemonCenter_1F_EventScript_CheckSeviiIslandComplete::
	goto_if_set FLAG_IS_CHAMPION, IndigoPlateau_PokemonCenter_1F_EventScript_SeviiIslandComplete
	msgbox IndigoPlateau_PokemonCenter_1F_Text_LoreleiIsAbsentClosedForTimeBeing
	release
	end

IndigoPlateau_PokemonCenter_1F_EventScript_SeviiIslandComplete::
	msgbox IndigoPlateau_PokemonCenter_1F_Text_FaceEliteFourGoodLuck
	release
	end

IndigoPlateau_PokemonCenter_1F_EventScript_GymGuy::
	msgbox IndigoPlateau_PokemonCenter_1F_Text_GymGuyAdvice, MSGBOX_NPC
	end

IndigoPlateau_PokemonCenter_1F_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart IndigoPlateau_PokemonCenter_1F_Items
	msgbox gText_PleaseComeAgain
	release
	end

	.align 2
IndigoPlateau_PokemonCenter_1F_Items::
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_MAX_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_NONE
	release
	end

IndigoPlateau_PokemonCenter_1F_EventScript_Nurse::
	lock
	faceplayer
	call EventScript_PkmnCenterNurse_Frlg
	release
	end

IndigoPlateau_PokemonCenter_1F_EventScript_Collector::
	lock
	faceplayer
	goto_if_set FLAG_COLLECTOR_TRADE_COMPLETE, IndigoPlateau_Collector_Done
	msgbox IndigoPlateau_Collector_Offer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, IndigoPlateau_Collector_End
	special CollectorTrade
	goto_if_eq VAR_RESULT, 1, IndigoPlateau_Collector_NoCoins
	goto_if_eq VAR_RESULT, 2, IndigoPlateau_Collector_NoSpace
	goto_if_eq VAR_RESULT, 3, IndigoPlateau_Collector_Empty
	goto_if_eq VAR_RESULT, 4, IndigoPlateau_Collector_Done
	msgbox IndigoPlateau_Collector_Success, MSGBOX_DEFAULT
IndigoPlateau_Collector_ReceiptLoop::
	special CollectorNextReceiptItem
	goto_if_eq VAR_RESULT, 0, IndigoPlateau_Collector_End
	msgbox IndigoPlateau_Collector_Receipt, MSGBOX_DEFAULT
	goto IndigoPlateau_Collector_ReceiptLoop
IndigoPlateau_Collector_NoCoins::
	msgbox IndigoPlateau_Collector_CoinsText, MSGBOX_DEFAULT
	goto IndigoPlateau_Collector_End
IndigoPlateau_Collector_NoSpace::
	msgbox IndigoPlateau_Collector_SpaceText, MSGBOX_DEFAULT
	goto IndigoPlateau_Collector_End
IndigoPlateau_Collector_Empty::
	msgbox IndigoPlateau_Collector_EmptyText, MSGBOX_DEFAULT
	goto IndigoPlateau_Collector_End
IndigoPlateau_Collector_Done::
	msgbox IndigoPlateau_Collector_DoneText, MSGBOX_DEFAULT
IndigoPlateau_Collector_End::
	release
	end

IndigoPlateau_Collector_Offer::
	.string "COLLECTOR: For 4,000 COINS, I'll\n"
	.string "gather your missed Kanto items.\p"
	.string "Only visible item balls from\n"
	.string "before the LEAGUE. Deal?$"
IndigoPlateau_Collector_CoinsText::
	.string "You need 4,000 COINS.$"
IndigoPlateau_Collector_SpaceText::
	.string "Make room in every BAG pocket,\n"
	.string "including TMs, then come back.$"
IndigoPlateau_Collector_EmptyText::
	.string "You've found every item I collect.$"
IndigoPlateau_Collector_DoneText::
	.string "Our trade is complete!$"
IndigoPlateau_Collector_Success::
	.string "Deal! Here's your collection.$"
IndigoPlateau_Collector_Receipt::
	.string "{STR_VAR_2} × {STR_VAR_1}!$"

IndigoPlateau_PokemonCenter_1F_Text_GymGuyAdvice::
	.string "Yo!\n"
	.string "Champ in the making!\p"
	.string "At the POKéMON LEAGUE, you will\n"
	.string "face the ELITE FOUR all in a row.\p"
	.string "If you lose, you have to start all\n"
	.string "over again!\p"
	.string "This is it!\n"
	.string "Go for it!$"

IndigoPlateau_PokemonCenter_1F_Text_FaceEliteFourGoodLuck::
	.string "From here on, you face the\n"
	.string "ELITE FOUR one by one.\p"
	.string "If you win, a door opens to the\n"
	.string "next TRAINER. Good luck!$"

IndigoPlateau_PokemonCenter_1F_Text_LoreleiIsAbsentClosedForTimeBeing::
	.string "I am so sorry, but you've wasted\n"
	.string "your time coming here.\p"
	.string "LORELEI of the ELITE FOUR is\n"
	.string "absent, you see.\p"
	.string "As a result, the POKéMON LEAGUE is\n"
	.string "closed for the time being.$"

IndigoPlateau_PokemonCenter_1F_Text_AgathaWhuppedUs::
	.string "AGATHA's GHOST-type POKéMON are\n"
	.string "horrifically terrifying in toughness.\p"
	.string "I took my FIGHTING-type POKéMON\n"
	.string "and raised them to the max.\p"
	.string "I went at AGATHA feeling pretty\n"
	.string "confident, but she whupped us.\p"
	.string "That old lady's also got a really\n"
	.string "short fuse, too.\p"
	.string "It doesn't take anything to get\n"
	.string "that scary lady hollering.$"

IndigoPlateau_PokemonCenter_1F_Text_LancesCousinGymLeaderFarAway::
	.string "Maybe becoming an ELITE FOUR\n"
	.string "member is in the blood.\p"
	.string "From what I've heard, LANCE has\n"
	.string "a cousin who's a GYM LEADER\l"
	.string "somewhere far away.$"

