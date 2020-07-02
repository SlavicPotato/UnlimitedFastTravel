Scriptname nPFT_MCM_PlayerAlias extends ReferenceAlias  

Event OnPlayerLoadGame()
	(self.GetOwningQuest() as SKI_QuestBase).OnGameReload()
	(self.GetOwningQuest() as nPFT_MCM).PostLoadGame()
EndEvent