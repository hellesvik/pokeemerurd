Common_EventScript_BattleFacilityClosed::
	lockall
	msgbox BattleFacility_Text_Closed, MSGBOX_DEFAULT
	releaseall
	end

BattleFacility_Text_Closed:
	.string "This facility is closed.$"
