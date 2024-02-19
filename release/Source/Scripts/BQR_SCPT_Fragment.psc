;BEGIN FRAGMENT CODE - Do not edit anything between this and the end comment
;NEXT FRAGMENT INDEX 3
SCRIPTNAME BQR_SCPT_Fragment Extends QUEST Hidden

;BEGIN ALIAS PROPERTY Location
;ALIAS PROPERTY TYPE LocationAlias
LOCATIONALIAS Property Alias_Location Auto
;END ALIAS PROPERTY

;BEGIN ALIAS PROPERTY Hold
;ALIAS PROPERTY TYPE LocationAlias
LOCATIONALIAS Property Alias_Hold Auto
;END ALIAS PROPERTY

;BEGIN ALIAS PROPERTY Boss
;ALIAS PROPERTY TYPE ReferenceAlias
REFERENCEALIAS Property Alias_Boss Auto
;END ALIAS PROPERTY

;BEGIN ALIAS PROPERTY Jarl
;ALIAS PROPERTY TYPE ReferenceAlias
REFERENCEALIAS Property Alias_Jarl Auto
;END ALIAS PROPERTY

;BEGIN ALIAS PROPERTY Steward
;ALIAS PROPERTY TYPE ReferenceAlias
REFERENCEALIAS Property Alias_Steward Auto
;END ALIAS PROPERTY

;BEGIN ALIAS PROPERTY MapMarker
;ALIAS PROPERTY TYPE ReferenceAlias
REFERENCEALIAS Property Alias_MapMarker Auto
;END ALIAS PROPERTY

;BEGIN FRAGMENT Fragment_1
FUNCTION Fragment_1()
;BEGIN CODE
SetObjectiveCompleted(10)

IF ( Alias_Steward.GetReference() )
	SetObjectiveDisplayed(20)
ELSE
	SetObjectiveDisplayed(30)
ENDIF
;END CODE
ENDFUNCTION
;END FRAGMENT

;BEGIN FRAGMENT Fragment_0
FUNCTION Fragment_0()
;BEGIN CODE
BQR.SetQuestCount(1)

SetObjectiveDisplayed(10)

Alias_MapMarker.GetReference().AddToMap()
;END CODE
ENDFUNCTION
;END FRAGMENT

;BEGIN FRAGMENT Fragment_2
FUNCTION Fragment_2()
;BEGIN CODE
CompleteAllObjectives()

BQR.RewardPlayer(Alias_Location.GetLocation(), Alias_Boss.GetActorReference().GetLevel())

Stop()

Reset()
;END CODE
ENDFUNCTION
;END FRAGMENT

;END FRAGMENT CODE - Do not edit anything between this and the begin comment