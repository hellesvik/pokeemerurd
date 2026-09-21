MauvilleCity_House1_MapScripts::
	.byte 0

MauvilleCity_House1_EventScript_RockSmashDude::
	lock
	faceplayer
	msgbox MauvilleCity_House1_Text_GaveRockSmashToWinstrates, MSGBOX_DEFAULT
	release
	end

MauvilleCity_House1_Text_GaveRockSmashToWinstrates:
	.string "I gave the HM Rock Smash to the\n"
	.string "Winstrate family.$"
