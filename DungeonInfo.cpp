/*
	FireLabs Dev 1.0
	https://www.fire-m2labs.dev
*/
#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "desc.h"
#include "char.h"
#include "db.h"
#include "config.h"
#include "DungeonInfo.h"
#include "affect.h"
#include "item.h"
#include "questmanager.h"
#include <boost/algorithm/string.hpp>

DungeonInfo::DungeonInfo()
{
}

DungeonInfo::~DungeonInfo()
{
}

int32_t DungeonInfo::GetTeleportLocation(int x, int y)
{
	for (int i = 0; i < DUNGEON_TABLE_LENGTH; i++)
    {
		const auto& entry = dungeonTable[i];

        if (entry.map_coords[0] == x && entry.map_coords[1] == y)
            return i;
    }
	
    return -1;
}

bool DungeonInfo::PlayerCanWarp(int32_t teleportLocationIndex, LPCHARACTER ch) const
{
	if (!ch)
		return false;

	if (teleportLocationIndex >= DUNGEON_TABLE_LENGTH || teleportLocationIndex < 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "This teleport position does not exist.");
		return false;
	}
	
	const auto& entry = dungeonTable[teleportLocationIndex];
	
	if (!ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You can't teleport yet.");
		return false;
	}

	if (ch->GetLevel() < entry.min_level)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You must have reached at least the level %d to be able to teleport.", entry.min_level);
		return false;
	}

	// if (ch->GetLevel() > entry.max_level)
		// return false;

	return true;
}

void DungeonInfo::UpdateDungeonRank(LPCHARACTER ch, int dungeonID, int rankingType)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "CleanDungeonRanking");

	char query[8192];
	
	if (rankingType == 1)
		snprintf(query, sizeof(query),"SELECT name, level, finished FROM log.dungeon_ranking WHERE dungeon_id = '%d' ORDER BY finished DESC LIMIT 100;", dungeonID);
	else if (rankingType == 2)
		snprintf(query, sizeof(query), "SELECT name, level, fastest_time FROM log.dungeon_ranking WHERE dungeon_id = '%d' ORDER BY fastest_time ASC LIMIT 100;", dungeonID);
	else
		snprintf(query, sizeof(query), "SELECT name, level, highest_damage FROM log.dungeon_ranking WHERE dungeon_id = '%d' ORDER BY highest_damage DESC LIMIT 100;", dungeonID);
	
	SQLMsg * pkMsg(DBManager::instance().DirectQuery(query));
	SQLResult * Res = pkMsg->Get();
	
	if (Res->uiNumRows > 0) {
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(Res->pSQLResult)) != NULL)
		{
			int index = 0;
			ch->ChatPacket(CHAT_TYPE_COMMAND, "UpdateDungeonRanking %s %d %d", row[index++], atoi(row[index++]), atoi(row[index++]));
		}
	}
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenDungeonRanking");
}

void DungeonInfo::UpdateRankLog(LPCHARACTER ch, int dungeonID, int finished, int fastestTime, int highestDamage)
{
	if (ch == NULL)
		return;

	if (!ch->IsPC())
		return;
	
	SQLMsg * pkMsg(DBManager::instance().DirectQuery("SELECT * FROM log.dungeon_ranking WHERE dungeon_id = '%d' and name = '%s';",dungeonID, ch->GetName()));
	SQLResult * Res = pkMsg->Get();
	
	if (Res->uiNumRows > 0)
		DBManager::instance().DirectQuery("UPDATE log.dungeon_ranking SET level = '%d', finished = finished + '%d', fastest_time = '%d', highest_damage = '%d' WHERE dungeon_id = '%d' and name = '%s';", ch->GetLevel(),finished,fastestTime,highestDamage, dungeonID, ch->GetName());
	else
		DBManager::instance().DirectQuery("INSERT INTO log.dungeon_ranking VALUES('%d', '%s', '%d', '%d', '%d', '%d');",dungeonID, ch->GetName(), ch->GetLevel(), finished, fastestTime, highestDamage);
}

/*
int DungeonInfo::GetRank(LPCHARACTER ch, int dungeonID, int pointType)
{
	if (ch == NULL)
		return 0;

	if (!ch->IsPC())
		return 0;
	
	SQLMsg * pkMsg(DBManager::instance().DirectQuery("SELECT finished, fastest_time, highest_damage FROM log.dungeon_ranking WHERE dungeon_id = '%d' and name = '%s';",dungeonID, ch->GetName()));
	SQLResult * Res = pkMsg->Get();
	
	if (Res->uiNumRows > 0) {
		MYSQL_ROW row;
		if (pointType == 1)
			return atoi(row[0]);
		else if (pointType == 2)
			return atoi(row[1]);
		else if (pointType == 3)
			return atoi(row[2]);
		else
			return 0;
	}
	else
		return 0;
}
*/

int DungeonInfo::GetRank(LPCHARACTER ch, int dungeonID, int pointType)
{
    if (ch == NULL)
        return 0;
 
    if (!ch->IsPC())
        return 0;
 
    SQLMsg* pkMsg(DBManager::instance().DirectQuery("SELECT finished, fastest_time, highest_damage FROM log.dungeon_ranking WHERE dungeon_id = '%d' and name = '%s';", dungeonID, ch->GetName()));
    SQLResult* Res = pkMsg->Get();
 
    if (Res->uiNumRows > 0) {
        MYSQL_ROW row;
        if ((row = mysql_fetch_row(Res->pSQLResult)) != NULL) {
            if (pointType == 1)
                return atoi(row[0]);
            else if (pointType == 2)
                return atoi(row[1]);
            else if (pointType == 3)
                return atoi(row[2]);
            else
                return 0;
        }
    }
    else
        return 0;
}

void DungeonInfo::Update(LPCHARACTER ch)
{
	if (ch == NULL)
		return;

	if (!ch->IsPC())
		return;
	
	if (sizeof(dungeonTable) == 0)
		return;
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "CleanDungeonInfo");
	int size = *(&dungeonTable + 1) - dungeonTable;
	for (int i = 0; i <= size - 1; i++)
	{
		int dungeonFinished = GetRank(ch, i, 1);
		int dungeonFastestTime = GetRank(ch, i, 2);
		int dungeonHighestDamage = GetRank(ch, i, 3);
		
		std::string mapName(dungeonTable[i].map_name);
		boost::algorithm::replace_all(mapName, " ", "_");
		
		std::string mapEntrance(dungeonTable[i].map_entrance);
		boost::algorithm::replace_all(mapEntrance, " ", "_");
		
		ch->ChatPacket(CHAT_TYPE_COMMAND, "UpdateDungeonInfo %d %d %d %d %d %s %s %d %d %d %d %d %d %d %d %d %d %d %d",
		dungeonTable[i].type,
		dungeonTable[i].organization,
		dungeonTable[i].min_level,
		dungeonTable[i].party_members,
		dungeonTable[i].map_index,
		mapName.c_str(),
		mapEntrance.c_str(),
		dungeonTable[i].map_coords[0],
		dungeonTable[i].map_coords[1],
		dungeonTable[i].cooldown,
		dungeonTable[i].duration,
		dungeonTable[i].max_level,
		dungeonTable[i].strength_bonus,
		dungeonTable[i].resistance_bonus,
		dungeonTable[i].item_vnum,
		dungeonTable[i].boss_vnum,
		dungeonFinished,
		dungeonFastestTime,
		dungeonHighestDamage
		); 
	}
}