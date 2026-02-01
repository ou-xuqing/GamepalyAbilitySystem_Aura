// Copyright Ziger


#include "Game/LoadScreenSaveGame.h"

FSavedMap ULoadScreenSaveGame::GetSavedMapFromMapName(const FString& InMapName)
{
	for (FSavedMap Map : SavedMaps)
	{
		if (Map.MapName == InMapName)
		{
			return Map;
		}
	}
	return FSavedMap();
}

bool ULoadScreenSaveGame::HasSavedMap(const FString& InMapName)
{
	for (FSavedMap Map : SavedMaps)
	{
		if (Map.MapName == InMapName)
		{
			return true;
		}
	}
	return false;
}
