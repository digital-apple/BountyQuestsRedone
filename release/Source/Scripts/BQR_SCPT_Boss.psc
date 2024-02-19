SCRIPTNAME BQR_SCPT_Boss Extends REFERENCEALIAS  

EVENT OnDeath(ACTOR akKiller)
    GetOwningQuest().SetStage(20)
ENDEVENT