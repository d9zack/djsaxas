/*
	FireLabs Dev 1.0
	https://www.fire-m2labs.dev
*/
#ifndef __INC_METIN_II_GAME_DUNGEON_INFO_H__
#define __INC_METIN_II_GAME_DUNGEON_INFO_H__
#pragma once
#include "constants.h"

class DungeonInfo : public singleton<DungeonInfo>
{
	public:
		DungeonInfo();
		~DungeonInfo();
	
	void	UpdateDungeonRank(LPCHARACTER ch, int dungeonID, int rankingType);
	int32_t GetTeleportLocation(int x, int y);
	bool PlayerCanWarp(int32_t teleportLocationIndex, LPCHARACTER ch) const;
	void	UpdateRankLog(LPCHARACTER ch, int dungeonID, int finished, int fastestTime, int highestDamage);
	int		GetRank(LPCHARACTER ch, int dungeonID, int pointType);
	void	Update(LPCHARACTER ch);
};
#endif