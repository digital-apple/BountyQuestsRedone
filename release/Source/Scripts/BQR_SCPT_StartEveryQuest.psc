;BEGIN FRAGMENT CODE - Do not edit anything between this and the end comment
;NEXT FRAGMENT INDEX 1
SCRIPTNAME BQR_SCPT_StartEveryQuest Extends TOPICINFO Hidden

;BEGIN FRAGMENT Fragment_0
FUNCTION Fragment_0(OBJECTREFERENCE akSpeakerRef)
ACTOR akSpeaker = akSpeakerRef As ACTOR
;BEGIN CODE
BQR.StartEveryQuest((Self.GetOwningQuest().GetAliasByName("Hold") As LOCATIONALIAS).GetLocation(), Type)

Utility.Wait(0.1)

BQR.UpdateDialogueMenu((Self.GetOwningQuest().GetAliasByName("Hold") As LOCATIONALIAS).GetLocation())
;END CODE
ENDFUNCTION
;END FRAGMENT

;END FRAGMENT CODE - Do not edit anything between this and the begin comment

INT Property Type Auto