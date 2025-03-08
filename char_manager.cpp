#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "desc.h"
#include "char.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "party.h"
#include "regen.h"
#include "p2p.h"
#include "dungeon.h"
#include "db.h"
#include "config.h"
#include "xmas_event.h"
#include "questmanager.h"
#include "questlua.h"
#include "locale_service.h"
#include "XTrapManager.h"

#ifndef __GNUC__
#include <boost/bind.hpp>
#endif

CHARACTER_MANAGER::CHARACTER_MANAGER() :
	m_iVIDCount(0),
	m_pkChrSelectedStone(NULL),
	m_bUsePendingDestroy(false)
{
	RegisterRaceNum(xmas::MOB_XMAS_FIRWORK_SELLER_VNUM);
	RegisterRaceNum(xmas::MOB_SANTA_VNUM);
	RegisterRaceNum(xmas::MOB_XMAS_TREE_VNUM);

	m_iMobItemRate = 100;
	m_iMobDamageRate = 100;
	m_iMobGoldAmountRate = 100;
	m_iMobGoldDropRate = 100;
	m_iMobExpRate = 100;

	m_iMobItemRatePremium = 100;
	m_iMobGoldAmountRatePremium = 100;
	m_iMobGoldDropRatePremium = 100;
	m_iMobExpRatePremium = 100;
	
	m_iUserDamageRate = 100;
	m_iUserDamageRatePremium = 100;
#ifdef ENABLE_EVENT_MANAGER
	m_eventData.clear();
#endif
#ifdef ENABLE_REWARD_SYSTEM
	m_rewardData.clear();
#endif
}

CHARACTER_MANAGER::~CHARACTER_MANAGER()
{
	Destroy();
}

void CHARACTER_MANAGER::Destroy()
{
	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.begin();
	while (it != m_map_pkChrByVID.end()) {
		LPCHARACTER ch = it->second;
		M2_DESTROY_CHARACTER(ch); // m_map_pkChrByVID is changed here
#ifdef ENABLE_EVENT_MANAGER
	m_eventData.clear();
#endif
		it = m_map_pkChrByVID.begin();
	}
#ifdef ENABLE_REWARD_SYSTEM
	m_rewardData.clear();
#endif
}

void CHARACTER_MANAGER::GracefulShutdown()
{
	NAME_MAP::iterator it = m_map_pkPCChr.begin();

	while (it != m_map_pkPCChr.end())
		(it++)->second->Disconnect("GracefulShutdown");
}

DWORD CHARACTER_MANAGER::AllocVID()
{
	++m_iVIDCount;
	return m_iVIDCount;
}

LPCHARACTER CHARACTER_MANAGER::CreateCharacter(const char* name, DWORD dwPID, const char* accountname)
{
	DWORD dwVID = AllocVID();

#ifdef M2_USE_POOL
	LPCHARACTER ch = pool_.Construct();
#else
	LPCHARACTER ch = M2_NEW CHARACTER;
#endif
	ch->Create(name, dwVID, dwPID ? true : false);

	m_map_pkChrByVID.insert(std::make_pair(dwVID, ch));

	if (dwPID)
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];
		str_lower(name, szName, sizeof(szName));
		
		if (accountname != nullptr)
			m_map_AccCH.insert(NAME_MAP::value_type(accountname, ch));

		m_map_pkPCChr.insert(NAME_MAP::value_type(szName, ch));
		m_map_pkChrByPID.insert(std::make_pair(dwPID, ch));
	}

	return (ch);
}

#ifndef DEBUG_ALLOC
void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER ch)
#else
void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER ch, const char* file, size_t line)
#endif
{
	if (!ch)
		return;

	// <Factor> Check whether it has been already deleted or not.
	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.find(ch->GetVID());
	if (it == m_map_pkChrByVID.end()) {
		sys_err("[CHARACTER_MANAGER::DestroyCharacter] <Factor> %d not found", (long)(ch->GetVID()));
		return; // prevent duplicated destrunction
	}

	// 던전에 소속된 몬스터는 던전에서도 삭제하도록.
#ifdef ENABLE_NEW_PET_SYSTEM
	if (ch->IsNPC() && !ch->IsPet() && !ch->IsNewPet() && !ch->IsBuffNPC() && ch->GetRider() == NULL)
#else
	if (ch->IsNPC() && !ch->IsPet() && ch->GetRider() == NULL)
#endif
	{
		if (ch->GetDungeon())
		{
			ch->GetDungeon()->DeadCharacter(ch);
		}
	}

	if (m_bUsePendingDestroy)
	{
		m_set_pkChrPendingDestroy.insert(ch);
		return;
	}

	m_map_pkChrByVID.erase(it);

	if (true == ch->IsPC())
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];

		str_lower(ch->GetName(), szName, sizeof(szName));

		NAME_MAP::iterator it = m_map_pkPCChr.find(szName);

		if (m_map_pkPCChr.end() != it)
			m_map_pkPCChr.erase(it);

		NAME_MAP::iterator it2 = m_map_AccCH.find(ch->GetDesc()->GetAccountTable().login);
		
		if (m_map_AccCH.end() != it2)
			m_map_AccCH.erase(it2);
	}

	if (0 != ch->GetPlayerID())
	{
		itertype(m_map_pkChrByPID) it = m_map_pkChrByPID.find(ch->GetPlayerID());

		if (m_map_pkChrByPID.end() != it)
		{
			m_map_pkChrByPID.erase(it);
		}
	}

	UnregisterRaceNumMap(ch);

	RemoveFromStateList(ch);

#ifdef M2_USE_POOL
	pool_.Destroy(ch);
#else
#ifndef DEBUG_ALLOC
	M2_DELETE(ch);
#else
	M2_DELETE_EX(ch, file, line);
#endif
#endif
}

LPCHARACTER CHARACTER_MANAGER::FindPCByLogin(const char* login)
{
	char szLogin[LOGIN_MAX_LEN + 1];
	str_lower(login, szLogin, sizeof(szLogin));
	NAME_MAP::iterator it = m_map_AccCH.find(szLogin);

	if (it == m_map_AccCH.end())
		return NULL;

	LPCHARACTER found = it->second;

	return found;
}


LPCHARACTER CHARACTER_MANAGER::Find(DWORD dwVID)
{
	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.find(dwVID);

	if (m_map_pkChrByVID.end() == it)
		return NULL;
	
	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && dwVID != (DWORD)found->GetVID()) {
		sys_err("[CHARACTER_MANAGER::Find] <Factor> %u != %u", dwVID, (DWORD)found->GetVID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::Find(const VID & vid)
{
	LPCHARACTER tch = Find((DWORD) vid);

	if (!tch || tch->GetVID() != vid)
		return NULL;

	return tch;
}

LPCHARACTER CHARACTER_MANAGER::FindByPID(DWORD dwPID)
{
	itertype(m_map_pkChrByPID) it = m_map_pkChrByPID.find(dwPID);

	if (m_map_pkChrByPID.end() == it)
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && dwPID != found->GetPlayerID()) {
		sys_err("[CHARACTER_MANAGER::FindByPID] <Factor> %u != %u", dwPID, found->GetPlayerID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::FindPC(const char * name)
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	str_lower(name, szName, sizeof(szName));
	NAME_MAP::iterator it = m_map_pkPCChr.find(szName);

	if (it == m_map_pkPCChr.end())
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && strncasecmp(szName, found->GetName(), CHARACTER_NAME_MAX_LEN) != 0) {
		sys_err("[CHARACTER_MANAGER::FindPC] <Factor> %s != %s", name, found->GetName());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRandomPosition(DWORD dwVnum, long lMapIndex)
{
	// 왜구 스폰할지말지를 결정할 수 있게함
	{
		if (dwVnum == 5001 && !quest::CQuestManager::instance().GetEventFlag("japan_regen"))
		{
			sys_log(1, "WAEGU[5001] regen disabled.");
			return NULL;
		}
	}

	// 해태를 스폰할지 말지를 결정할 수 있게 함
	{
		if (dwVnum == 5002 && !quest::CQuestManager::instance().GetEventFlag("newyear_mob"))
		{
			sys_log(1, "HAETAE (new-year-mob) [5002] regen disabled.");
			return NULL;
		}
	}

	// 광복절 이벤트 
	{
		if (dwVnum == 5004 && !quest::CQuestManager::instance().GetEventFlag("independence_day"))
		{
			sys_log(1, "INDEPENDECE DAY [5004] regen disabled.");
			return NULL;
		}
	}

	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
	{
		sys_err("no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!map_allow_find(lMapIndex))
	{
		sys_err("not allowed map %u", lMapIndex);
		return NULL;
	}

	LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
	if (pkSectreeMap == NULL) {
		return NULL;
	}

	int i;
	long x, y;
	for (i=0; i<2000; i++)
	{
		x = number(1, (pkSectreeMap->m_setting.iWidth / 100)  - 1) * 100 + pkSectreeMap->m_setting.iBaseX;
		y = number(1, (pkSectreeMap->m_setting.iHeight / 100) - 1) * 100 + pkSectreeMap->m_setting.iBaseY;
		//LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);
		LPSECTREE tree = pkSectreeMap->Find(x, y);

		if (!tree)
			continue;

		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT))
			continue;

		if (IS_SET(dwAttr, ATTR_BANPK))
			continue;

		break;
	}

	if (i == 2000)
	{
		sys_err("cannot find valid location");
		return NULL;
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);

	if (!ch)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create new character");
		return NULL;
	}

	ch->SetProto(pkMob);

	// if mob is npc with no empire assigned, assign to empire of map
	if (pkMob->m_table.bType == CHAR_TYPE_NPC)
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));

	ch->SetRotation(number(0, 360));

	if (!ch->Show(lMapIndex, x, y, 0, false))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_err(0, "SpawnMobRandomPosition: cannot show monster");
		return NULL;
	}

	char buf[512+1];
	long local_x = x - pkSectreeMap->m_setting.iBaseX;
	long local_y = y - pkSectreeMap->m_setting.iBaseY;
	snprintf(buf, sizeof(buf), "spawn %s[%d] random position at %ld %ld %ld %ld (time: %d)", ch->GetName(), dwVnum, x, y, local_x, local_y, get_global_time());
	
	if (test_server)
		SendNotice(buf);

	sys_log(0, buf);
	return (ch);
}

LPCHARACTER CHARACTER_MANAGER::SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion, int iRot, bool bShow)
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);
	if (!pkMob)
	{
		sys_err("SpawnMob: no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!(pkMob->m_table.bType == CHAR_TYPE_NPC || pkMob->m_table.bType == CHAR_TYPE_WARP || pkMob->m_table.bType == CHAR_TYPE_GOTO) || mining::IsVeinOfOre (dwVnum))
	{
		LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

		if (!tree)
		{
			sys_log(0, "no sectree for spawn at %d %d mobvnum %d mapindex %d", x, y, dwVnum, lMapIndex);
			return NULL;
		}

		DWORD dwAttr = tree->GetAttribute(x, y);

		bool is_set = false;

		if ( mining::IsVeinOfOre (dwVnum) ) is_set = IS_SET(dwAttr, ATTR_BLOCK);
		else is_set = IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT);

		if ( is_set )
		{
			// SPAWN_BLOCK_LOG
			static bool s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");
			static DWORD s_nextTime=get_global_time()+10000;

			DWORD curTime=get_global_time();

			if (curTime>s_nextTime)
			{
				s_nextTime=curTime;
				s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");

			}

			if (s_isLog)
				sys_log(0, "SpawnMob: BLOCKED position for spawn %s %u at %d %d (attr %u)", pkMob->m_table.szName, dwVnum, x, y, dwAttr);
			// END_OF_SPAWN_BLOCK_LOG
			return NULL;
		}

		if (IS_SET(dwAttr, ATTR_BANPK))
		{
			sys_log(0, "SpawnMob: BAN_PK position for mob spawn %s %u at %d %d", pkMob->m_table.szName, dwVnum, x, y);
			return NULL;
		}
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "SpawnMob: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);

	if (!ch)
	{
		sys_log(0, "SpawnMob: cannot create new character");
		return NULL;
	}

	if (iRot == -1)
		iRot = number(0, 360);

	ch->SetProto(pkMob);

	// if mob is npc with no empire assigned, assign to empire of map
	if (pkMob->m_table.bType == CHAR_TYPE_NPC)
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));

	ch->SetRotation(iRot);

	if (bShow && !ch->Show(lMapIndex, x, y, z, bSpawnMotion))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_log(0, "SpawnMob: cannot show monster");
		return NULL;
	}

	return (ch);
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRange(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, bool bIsException, bool bSpawnMotion, bool bAggressive )
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
		return NULL;

	if (pkMob->m_table.bType == CHAR_TYPE_STONE)	// 돌은 무조건 SPAWN 모션이 있다.
		bSpawnMotion = true;

	int i = 16;

	while (i--)
	{
		int x = number(sx, ex);
		int y = number(sy, ey);
		/*
		   if (bIsException)
		   if (is_regen_exception(x, y))
		   continue;
		 */
		LPCHARACTER ch = SpawnMob(dwVnum, lMapIndex, x, y, 0, bSpawnMotion);

		if (ch)
		{
			sys_log(1, "MOB_SPAWN: %s(%d) %dx%d", ch->GetName(), (DWORD) ch->GetVID(), ch->GetX(), ch->GetY());
			if ( bAggressive )
				ch->SetAggressive();
			return (ch);
		}
	}

	return NULL;
}

void CHARACTER_MANAGER::SelectStone(LPCHARACTER pkChr)
{
	m_pkChrSelectedStone = pkChr;
}

bool CHARACTER_MANAGER::SpawnMoveGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, int tx, int ty, LPREGEN pkRegen, bool bAggressive_)
{
	if (!dwVnum)
		return false;
	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return false;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;
		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)	// 못만든 몬스터가 대장일 경우에는 그냥 실패
				return false;

			continue;
		}

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}
		if (bAggressive)
			tch->SetAggressive();

		if (tch->Goto(tx, ty))
			tch->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	}

	return true;
}

bool CHARACTER_MANAGER::SpawnGroupGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	const DWORD dwGroupID = CMobManager::Instance().GetGroupFromGroupGroup(dwVnum);

	if( dwGroupID != 0 )
	{
		return SpawnGroup(dwGroupID, lMapIndex, sx, sy, ex, ey, pkRegen, bAggressive_, pDungeon);
	}
	else
	{
		//sys_err( "NOT_EXIST_GROUP_GROUP_VNUM(%u) MAP(%ld)", dwVnum, lMapIndex );
		return false;
	}
}

LPCHARACTER CHARACTER_MANAGER::SpawnGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	if (!dwVnum)
		return NULL;
	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return NULL;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;

		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	LPCHARACTER chLeader = NULL;

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)	// 못만든 몬스터가 대장일 경우에는 그냥 실패
				return NULL;

			continue;
		}

		if (i == 0)
			chLeader = tch;

		tch->SetDungeon(pDungeon);

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}

		if (bAggressive)
			tch->SetAggressive();
	}

	return chLeader;
}

struct FuncUpdateAndResetChatCounter
{
	void operator () (LPCHARACTER ch)
	{
		ch->ResetChatCounter();
		ch->CFSM::Update();
	}
};

void CHARACTER_MANAGER::Update(int iPulse)
{
	using namespace std;
#ifdef __GNUC__
	using namespace __gnu_cxx;
#endif

	BeginPendingDestroy();

	// PC 캐릭터 업데이트
	{
		if (!m_map_pkPCChr.empty())
		{
			// 컨테이너 복사
			CHARACTER_VECTOR v;
			v.reserve(m_map_pkPCChr.size());
#ifdef __GNUC__
			transform(m_map_pkPCChr.begin(), m_map_pkPCChr.end(), back_inserter(v), select2nd<NAME_MAP::value_type>());
#else
			transform(m_map_pkPCChr.begin(), m_map_pkPCChr.end(), back_inserter(v), boost::bind(&NAME_MAP::value_type::second, _1));
#endif

			if (0 == (iPulse % PASSES_PER_SEC(5)))
			{
				FuncUpdateAndResetChatCounter f;
				for_each(v.begin(), v.end(), f);
			}
			else
			{
				//for_each(v.begin(), v.end(), mem_fun(&CFSM::Update));
				for_each(v.begin(), v.end(), bind2nd(mem_fun(&CHARACTER::UpdateCharacter), iPulse));
			}
		}

//		for_each_pc(bind2nd(mem_fun(&CHARACTER::UpdateCharacter), iPulse));
	}

	// 몬스터 업데이트
	{
		if (!m_set_pkChrState.empty())
		{
			CHARACTER_VECTOR v;
			v.reserve(m_set_pkChrState.size());
#ifdef __GNUC__
			transform(m_set_pkChrState.begin(), m_set_pkChrState.end(), back_inserter(v), identity<CHARACTER_SET::value_type>());
#else
			v.insert(v.end(), m_set_pkChrState.begin(), m_set_pkChrState.end());
#endif
			for_each(v.begin(), v.end(), bind2nd(mem_fun(&CHARACTER::UpdateStateMachine), iPulse));
		}
	}

	// 산타 따로 업데이트
	{
		CharacterVectorInteractor i;

		if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(xmas::MOB_SANTA_VNUM, i))
		{
			for_each(i.begin(), i.end(),
					bind2nd(mem_fun(&CHARACTER::UpdateStateMachine), iPulse));
		}
	}

	// 1시간에 한번씩 몹 사냥 개수 기록 
	if (0 == (iPulse % PASSES_PER_SEC(3600)))
	{
		for (itertype(m_map_dwMobKillCount) it = m_map_dwMobKillCount.begin(); it != m_map_dwMobKillCount.end(); ++it)
			DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER_KILL, it->first, it->second);

#ifdef _USE_SERVER_KEY_
		extern bool Metin2Server_IsInvalid();
		extern bool g_bShutdown;
		if (Metin2Server_IsInvalid())
		{
			g_bShutdown = true;
		}
#endif

		m_map_dwMobKillCount.clear();
	}

	// 테스트 서버에서는 60초마다 캐릭터 개수를 센다
	if (test_server && 0 == (iPulse % PASSES_PER_SEC(60)))
		sys_log(0, "CHARACTER COUNT vid %zu pid %zu", m_map_pkChrByVID.size(), m_map_pkChrByPID.size());

	// 지연된 DestroyCharacter 하기
	FlushPendingDestroy();
}

void CHARACTER_MANAGER::ProcessDelayedSave()
{
	CHARACTER_SET::iterator it = m_set_pkChrForDelayedSave.begin();

	while (it != m_set_pkChrForDelayedSave.end())
	{
		LPCHARACTER pkChr = *it++;
		pkChr->SaveReal();
	}

	m_set_pkChrForDelayedSave.clear();
}

bool CHARACTER_MANAGER::AddToStateList(LPCHARACTER ch)
{
	assert(ch != NULL);

	CHARACTER_SET::iterator it = m_set_pkChrState.find(ch);

	if (it == m_set_pkChrState.end())
	{
		m_set_pkChrState.insert(ch);
		return true;
	}

	return false;
}

void CHARACTER_MANAGER::RemoveFromStateList(LPCHARACTER ch)
{
	CHARACTER_SET::iterator it = m_set_pkChrState.find(ch);

	if (it != m_set_pkChrState.end())
	{
		//sys_log(0, "RemoveFromStateList %p", ch);
		m_set_pkChrState.erase(it);
	}
}

void CHARACTER_MANAGER::DelayedSave(LPCHARACTER ch)
{
	m_set_pkChrForDelayedSave.insert(ch);
}

bool CHARACTER_MANAGER::FlushDelayedSave(LPCHARACTER ch)
{
	CHARACTER_SET::iterator it = m_set_pkChrForDelayedSave.find(ch);

	if (it == m_set_pkChrForDelayedSave.end())
		return false;

	m_set_pkChrForDelayedSave.erase(it);
	ch->SaveReal();
	return true;
}

void CHARACTER_MANAGER::RegisterForMonsterLog(LPCHARACTER ch)
{
	m_set_pkChrMonsterLog.insert(ch);
}

void CHARACTER_MANAGER::UnregisterForMonsterLog(LPCHARACTER ch)
{
	m_set_pkChrMonsterLog.erase(ch);
}

void CHARACTER_MANAGER::PacketMonsterLog(LPCHARACTER ch, const void* buf, int size)
{
	itertype(m_set_pkChrMonsterLog) it;

	for (it = m_set_pkChrMonsterLog.begin(); it!=m_set_pkChrMonsterLog.end();++it)
	{
		LPCHARACTER c = *it;

		if (ch && DISTANCE_APPROX(c->GetX()-ch->GetX(), c->GetY()-ch->GetY())>6000)
			continue;

		LPDESC d = c->GetDesc();

		if (d)
			d->Packet(buf, size);
	}
}

void CHARACTER_MANAGER::KillLog(DWORD dwVnum)
{
	const DWORD SEND_LIMIT = 10000;

	itertype(m_map_dwMobKillCount) it = m_map_dwMobKillCount.find(dwVnum);

	if (it == m_map_dwMobKillCount.end())
		m_map_dwMobKillCount.insert(std::make_pair(dwVnum, 1));
	else
	{
		++it->second;

		if (it->second > SEND_LIMIT)
		{
			DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER_KILL, it->first, it->second);
			m_map_dwMobKillCount.erase(it);
		}
	}
}

void CHARACTER_MANAGER::RegisterRaceNum(DWORD dwVnum)
{
	m_set_dwRegisteredRaceNum.insert(dwVnum);
}

void CHARACTER_MANAGER::RegisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	if (m_set_dwRegisteredRaceNum.find(dwVnum) != m_set_dwRegisteredRaceNum.end()) // 등록된 번호 이면
	{
		sys_log(0, "RegisterRaceNumMap %s %u", ch->GetName(), dwVnum);
		m_map_pkChrByRaceNum[dwVnum].insert(ch);
	}
}

void CHARACTER_MANAGER::UnregisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	itertype(m_map_pkChrByRaceNum) it = m_map_pkChrByRaceNum.find(dwVnum);

	if (it != m_map_pkChrByRaceNum.end())
		it->second.erase(ch);
}

bool CHARACTER_MANAGER::GetCharactersByRaceNum(DWORD dwRaceNum, CharacterVectorInteractor & i)
{
	std::map<DWORD, CHARACTER_SET>::iterator it = m_map_pkChrByRaceNum.find(dwRaceNum);

	if (it == m_map_pkChrByRaceNum.end())
		return false;

	// 컨테이너 복사
	i = it->second;
	return true;
}

#define FIND_JOB_WARRIOR_0	(1 << 3)
#define FIND_JOB_WARRIOR_1	(1 << 4)
#define FIND_JOB_WARRIOR_2	(1 << 5)
#define FIND_JOB_WARRIOR	(FIND_JOB_WARRIOR_0 | FIND_JOB_WARRIOR_1 | FIND_JOB_WARRIOR_2)
#define FIND_JOB_ASSASSIN_0	(1 << 6)
#define FIND_JOB_ASSASSIN_1	(1 << 7)
#define FIND_JOB_ASSASSIN_2	(1 << 8)
#define FIND_JOB_ASSASSIN	(FIND_JOB_ASSASSIN_0 | FIND_JOB_ASSASSIN_1 | FIND_JOB_ASSASSIN_2)
#define FIND_JOB_SURA_0		(1 << 9)
#define FIND_JOB_SURA_1		(1 << 10)
#define FIND_JOB_SURA_2		(1 << 11)
#define FIND_JOB_SURA		(FIND_JOB_SURA_0 | FIND_JOB_SURA_1 | FIND_JOB_SURA_2)
#define FIND_JOB_SHAMAN_0	(1 << 12)
#define FIND_JOB_SHAMAN_1	(1 << 13)
#define FIND_JOB_SHAMAN_2	(1 << 14)
#define FIND_JOB_SHAMAN		(FIND_JOB_SHAMAN_0 | FIND_JOB_SHAMAN_1 | FIND_JOB_SHAMAN_2)

//
// (job+1)*3+(skill_group)
//
LPCHARACTER CHARACTER_MANAGER::FindSpecifyPC(unsigned int uiJobFlag, long lMapIndex, LPCHARACTER except, int iMinLevel, int iMaxLevel)
{
	LPCHARACTER chFind = NULL;
	itertype(m_map_pkChrByPID) it;
	int n = 0;

	for (it = m_map_pkChrByPID.begin(); it != m_map_pkChrByPID.end(); ++it)
	{
		LPCHARACTER ch = it->second;

		if (ch == except)
			continue;

		if (ch->GetLevel() < iMinLevel)
			continue;

		if (ch->GetLevel() > iMaxLevel)
			continue;

		if (ch->GetMapIndex() != lMapIndex)
			continue;

		if (uiJobFlag)
		{
			unsigned int uiChrJob = (1 << ((ch->GetJob() + 1) * 3 + ch->GetSkillGroup()));

			if (!IS_SET(uiJobFlag, uiChrJob))
				continue;
		}

		if (!chFind || number(1, ++n) == 1)
			chFind = ch;
	}

	return chFind;
}

int CHARACTER_MANAGER::GetMobItemRate(LPCHARACTER ch)	
{ 
	//PREVENT_TOXICATION_FOR_CHINA
	if ( LC_IsNewCIBN() )
	{
		if ( ch->IsOverTime( OT_3HOUR ) )
		{
			if (ch && ch->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0)
				return m_iMobItemRatePremium/2;
			return m_iMobItemRate/2; 
		}
		else if ( ch->IsOverTime( OT_5HOUR ) )
		{
			return 0;
		}
	}
	//END_PREVENT_TOXICATION_FOR_CHINA
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0)
		return m_iMobItemRatePremium;
	return m_iMobItemRate; 
}

int CHARACTER_MANAGER::GetMobDamageRate(LPCHARACTER ch)	
{ 
	return m_iMobDamageRate; 
}

int CHARACTER_MANAGER::GetMobGoldAmountRate(LPCHARACTER ch)
{ 
	if ( !ch )
		return m_iMobGoldAmountRate;

	//PREVENT_TOXICATION_FOR_CHINA
	if ( LC_IsNewCIBN() )
	{
		if ( ch->IsOverTime( OT_3HOUR ) )
		{
			if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
				return m_iMobGoldAmountRatePremium/2;
			return m_iMobGoldAmountRate/2; 
		}
		else if ( ch->IsOverTime( OT_5HOUR ) )
		{
			return 0;
		}
	}
	//END_PREVENT_TOXICATION_FOR_CHINA
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
		return m_iMobGoldAmountRatePremium;
	return m_iMobGoldAmountRate; 
}

int CHARACTER_MANAGER::GetMobGoldDropRate(LPCHARACTER ch)
{
	if ( !ch )
		return m_iMobGoldDropRate;

	//PREVENT_TOXICATION_FOR_CHINA
	if ( LC_IsNewCIBN() )
	{
		if ( ch->IsOverTime( OT_3HOUR ) )
		{
			if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
				return m_iMobGoldDropRatePremium/2;
			return m_iMobGoldDropRate/2;
		}
		else if ( ch->IsOverTime( OT_5HOUR ) )
		{
			return 0;
		}
	}
	//END_PREVENT_TOXICATION_FOR_CHINA
	
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
		return m_iMobGoldDropRatePremium;
	return m_iMobGoldDropRate;
}

int CHARACTER_MANAGER::GetMobExpRate(LPCHARACTER ch)
{ 
	if ( !ch )
		return m_iMobExpRate;

	if ( LC_IsNewCIBN() )
	{
		if ( ch->IsOverTime( OT_3HOUR ) )
		{
			if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
				return m_iMobExpRatePremium/2;
			return m_iMobExpRate/2; 
		}
		else if ( ch->IsOverTime( OT_5HOUR ) )
		{
			return 0;
		}
	}
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		return m_iMobExpRatePremium;
	return m_iMobExpRate; 
}

int	CHARACTER_MANAGER::GetUserDamageRate(LPCHARACTER ch)
{
	if (!ch)
		return m_iUserDamageRate;

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		return m_iUserDamageRatePremium;

	return m_iUserDamageRate;
}

void CHARACTER_MANAGER::SendScriptToMap(long lMapIndex, const std::string & s)
{
	LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);

	if (NULL == pSecMap)
		return;

	struct packet_script p;

	p.header = HEADER_GC_SCRIPT;
	p.skin = 1;
	p.src_size = s.size();

	quest::FSendPacket f;
	p.size = p.src_size + sizeof(struct packet_script);
	f.buf.write(&p, sizeof(struct packet_script));
	f.buf.write(&s[0], s.size());

	pSecMap->for_each(f);
}

bool CHARACTER_MANAGER::BeginPendingDestroy()
{
	// Begin 이 후에 Begin을 또 하는 경우에 Flush 하지 않는 기능 지원을 위해
	// 이미 시작되어있으면 false 리턴 처리
	if (m_bUsePendingDestroy)
		return false;

	m_bUsePendingDestroy = true;
	return true;
}

void CHARACTER_MANAGER::FlushPendingDestroy()
{
	using namespace std;

	m_bUsePendingDestroy = false; // 플래그를 먼저 설정해야 실제 Destroy 처리가 됨

	if (!m_set_pkChrPendingDestroy.empty())
	{
		sys_log(0, "FlushPendingDestroy size %d", m_set_pkChrPendingDestroy.size());
		
		CHARACTER_SET::iterator it = m_set_pkChrPendingDestroy.begin(),
			end = m_set_pkChrPendingDestroy.end();
		for ( ; it != end; ++it) {
			M2_DESTROY_CHARACTER(*it);
		}

		m_set_pkChrPendingDestroy.clear();
	}
}

#ifdef ENABLE_CLONE_SYSTEM
#include "skill.h"
void CHARACTER_MANAGER::CopyPC(LPCHARACTER src, LPCHARACTER ch)
{
	if (!src || !ch)
		return;

	ch->SetPet();
	ch->SetRace(src->GetJob());
	ch->SetLevel(src->GetLevel());
	ch->SetMapIndex(src->GetMapIndex());
	ch->SetHP(src->GetHP());
	ch->SetSP(src->GetSP());
	std::string stName = std::string(src->GetName());
	stName += "'s Dito";
	ch->SetName(stName.c_str());
	ch->SetCharType(CHAR_TYPE_NPC);
	ch->ComputePoints();
	for (int i = 0; i<POINT_MAX_NUM; i++)
	{
		ch->SetRealPoint(i, src->GetRealPoint(i));
		ch->SetPoint(i, src->GetPoint(i));
	}
	for (int x = 0; x<GUILD_SKILL_END; x++)
	{
		ch->SetSkillLevel(x, src->GetSkillLevel(x));
	}
	for (int j = 0; j<PART_MAX_NUM; j++)
	{
		ch->SetPart(j, src->GetPart(j));
	}
	ch->ComputePoints();
	ch->SetSkillGroup(src->GetSkillGroup());
}
#endif

CharacterVectorInteractor::CharacterVectorInteractor(const CHARACTER_SET & r)
{
	using namespace std;
#ifdef __GNUC__
	using namespace __gnu_cxx;
#endif

	reserve(r.size());
#ifdef __GNUC__
	transform(r.begin(), r.end(), back_inserter(*this), identity<CHARACTER_SET::value_type>());
#else
	insert(end(), r.begin(), r.end());
#endif

	if (CHARACTER_MANAGER::instance().BeginPendingDestroy())
		m_bMyBegin = true;
}

CharacterVectorInteractor::~CharacterVectorInteractor()
{
	if (m_bMyBegin)
		CHARACTER_MANAGER::instance().FlushPendingDestroy();
}

#ifdef ENABLE_EVENT_MANAGER
#include "item_manager.h"
#include "item.h"
#include "desc_client.h"
#include "desc_manager.h"
void CHARACTER_MANAGER::ClearEventData()
{
	m_eventData.clear();
}
/*
BONUS_EVENT = 1,////DONE
DOUBLE_BOSS_LOOT_EVENT = 2,//DONE
DOUBLE_METIN_LOOT_EVENT = 3,//DONE
DOUBLE_MISSION_BOOK_EVENT = 4,//DONE
DUNGEON_COOLDOWN_EVENT = 5,//DONE
DUNGEON_TICKET_LOOT_EVENT = 6,//DONE
EMPIRE_WAR_EVENT = 7,
MOONLIGHT_EVENT = 8,//DONE
TOURNAMENT_EVENT = 9,
WHELL_OF_FORTUNE_EVENT = 10,
HALLOWEEN_EVENT = 11,
NPC_SEARCH_EVENT = 12,
*/

void CHARACTER_MANAGER::CheckBonusEvent(LPCHARACTER ch)
{
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	auto it = m_eventData.find(vKey.tm_mday);
	if (it != m_eventData.end())
	{
		if (it->second.size())
		{
			for (BYTE j = 0; j < it->second.size(); ++j)
			{
				const TEventManagerData& eventData = it->second[j];
				if (cur_Time > eventData.startRealTime && cur_Time < eventData.endRealTime)
				{
					if (eventData.eventIndex == BONUS_EVENT)
					{
						if (eventData.channelFlag != 0)
						{
							if (eventData.channelFlag != ch->GetEmpire())
								continue;
						}
						ch->ApplyPoint(eventData.value[0], eventData.value[1]);
					}
				}
			}
		}
	}
}

bool CHARACTER_MANAGER::CheckEventIsActive(BYTE eventIndex)
{
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	auto it = m_eventData.find(vKey.tm_mday);
	if (it != m_eventData.end())
	{
		if (it->second.size())
		{
			for (BYTE j = 0; j < it->second.size(); ++j)
			{
				const TEventManagerData& eventData = it->second[j];
				if (eventData.eventIndex == eventIndex)
					if (cur_Time > eventData.startRealTime && cur_Time < eventData.endRealTime)
						return true;
			}
		}
	}
	return false;
}
void CHARACTER_MANAGER::CheckEventForDrop(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item)
{
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	auto it = m_eventData.find(vKey.tm_mday);
	if (it != m_eventData.end())
	{
		if (it->second.size())
		{
			for (BYTE j = 0; j < it->second.size(); ++j)
			{
				const TEventManagerData& eventData = it->second[j];
				if (cur_Time > eventData.startRealTime && cur_Time < eventData.endRealTime)
				{
					int prob = number(1, 100);
					int success_prob = eventData.value[3];
					if (prob > success_prob)
						return;
					if (eventData.eventIndex == DOUBLE_BOSS_LOOT_EVENT)
					{
						if (pkChr->GetMobRank() >= MOB_RANK_BOSS)
						{
							std::vector<LPITEM> m_cache;
							LPITEM item = NULL;
							for (DWORD j = 0; j < vec_item.size(); ++j)
							{
								item = ITEM_MANAGER::Instance().CreateItem(vec_item[j]->GetVnum(), 1, 0, true);
								if (item) m_cache.emplace_back(item);
							}
							for (DWORD j = 0; j < m_cache.size(); ++j)
								vec_item.emplace_back(m_cache[j]);
							m_cache.clear();
						}
					}
					else if (eventData.eventIndex == DOUBLE_METIN_LOOT_EVENT)
					{
						if (pkChr->IsStone())
						{
							std::vector<LPITEM> m_cache;
							LPITEM item = NULL;
							for (DWORD j = 0; j < vec_item.size(); ++j)
							{
								item = ITEM_MANAGER::Instance().CreateItem(vec_item[j]->GetVnum(), 1, 0, true);
								if (item) m_cache.emplace_back(item);
							}
							for (DWORD j = 0; j < m_cache.size(); ++j)
								vec_item.emplace_back(m_cache[j]);
							m_cache.clear();
						}
					}
					else if (eventData.eventIndex == DOUBLE_MISSION_BOOK_EVENT)
					{
						std::vector<LPITEM> m_cache;
						LPITEM item = NULL;
						std::vector<DWORD> m_missionbook = {
							50300,
							469,
						};
						for (DWORD j = 0; j < vec_item.size(); ++j)
						{
							LPITEM checkItem = vec_item[j];
							auto it = std::find(m_missionbook.begin(), m_missionbook.end(), checkItem->GetVnum());
							if (it != m_missionbook.end())
							{

								item = ITEM_MANAGER::Instance().CreateItem(checkItem->GetVnum(), 1, 0, true);
								if (item) m_cache.emplace_back(item);
							}
						}
						for (DWORD j = 0; j < m_cache.size(); ++j)
							vec_item.emplace_back(m_cache[j]);
						m_cache.clear();
					}
					else if (eventData.eventIndex == DUNGEON_TICKET_LOOT_EVENT)
					{
						std::vector<LPITEM> m_cache;
						LPITEM item = NULL;
						std::vector<DWORD> m_missionbook = {
							50300,
							469,
						};
						for (DWORD j = 0; j < vec_item.size(); ++j)
						{
							LPITEM checkItem = vec_item[j];
							auto it = std::find(m_missionbook.begin(), m_missionbook.end(), checkItem->GetVnum());
							if (it != m_missionbook.end())
							{
								item = ITEM_MANAGER::Instance().CreateItem(checkItem->GetVnum(), 1, 0, true);
								if (item) m_cache.emplace_back(item);
							}
						}
						for (DWORD j = 0; j < m_cache.size(); ++j)
							vec_item.emplace_back(m_cache[j]);
						m_cache.clear();
					}
					else if (eventData.eventIndex == MOONLIGHT_EVENT)
					{
						DWORD itemVnum = 50011;
						LPITEM item = ITEM_MANAGER::Instance().CreateItem(itemVnum, 1, 0, true);
						if (item) vec_item.emplace_back(item);
					}
				}
			}
		}
	}
}
void CHARACTER_MANAGER::SendDataPlayer(LPCHARACTER ch)
{
	TPacketGCEventManager p;
	p.header = HEADER_GC_EVENT_MANAGER;	

	p.size = sizeof(p);
	TEMP_BUFFER buf;
	BYTE dayCount = m_eventData.size();
	buf.write(&dayCount, sizeof(BYTE));
	p.size += sizeof(BYTE);

	if (m_eventData.size())
	{
		for (auto it = m_eventData.begin(); it != m_eventData.end(); ++it) {
			p.size += sizeof(BYTE) + sizeof(BYTE) + (sizeof(TEventManagerDataClient) * it->second.size());

			BYTE dayIndex = it->first;
			buf.write(&dayIndex, sizeof(BYTE));
			BYTE dayEventCount = it->second.size();
			buf.write(&dayEventCount, sizeof(BYTE));

			std::vector<TEventManagerDataClient> m_vec;
			for (BYTE j = 0; j < it->second.size(); ++j)
			{
				const TEventManagerData& eventData= it->second[j];
				TEventManagerDataClient p;
				p.eventIndex = eventData.eventIndex;
				p.startRealTime = eventData.startRealTime-get_global_time();
				p.endRealTime = eventData.endRealTime-get_global_time();
				p.isAlreadyStart = CheckEventIsActive(eventData.eventIndex);
				strlcpy(p.endText, eventData.endText, sizeof(p.endText));
				strlcpy(p.startText, eventData.startText, sizeof(p.startText));
				p.empireFlag = eventData.empireFlag;
				p.channelFlag = eventData.channelFlag;
				thecore_memcpy(&p.value, &eventData.value, sizeof(p.value));
				m_vec.emplace_back(p);
			}
			buf.write(m_vec.data(), sizeof(TEventManagerDataClient) * dayEventCount);
		}
	}
	ch->GetDesc()->BufferedPacket(&p, sizeof(p));
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}
void CHARACTER_MANAGER::SetEventStatus(const TEventManagerData& m_data, bool eventStatus)
{
	// eventStatus 1 - ACTIVE
	// eventStatus 0 - DEACTIVE
	std::map<BYTE, std::pair<std::string, std::string>> m_eventText = {
		{BONUS_EVENT,{"Bonus event is active!","Bonus event is done!"}},
		{DOUBLE_BOSS_LOOT_EVENT,{"Double Boss Loot event is active!","Double Boss Loot event is done!"}},
		{DOUBLE_METIN_LOOT_EVENT,{"Double Metin Loot event is active!","Double Metin Loot event is done!"}},
		{DOUBLE_MISSION_BOOK_EVENT,{"Double Mission Book event is active!","Double Mission Book event is done!"}},
		{DUNGEON_COOLDOWN_EVENT,{"Dungeon Cooldown event is active!","Dungeon Cooldown event is done!"}},
		{DUNGEON_TICKET_LOOT_EVENT,{"Dungeon Ticket event is active!","Dungeon Ticket event is done!"}},
		{MOONLIGHT_EVENT,{"MoonLight event is active!","MoonLight event is done!"}},
	};

	auto it = m_eventText.find(m_data.eventIndex);
	if (it != m_eventText.end())
	{
		if (m_data.channelFlag == 0)
		{
			if (eventStatus)
				SendNotice(it->second.first.c_str());
			else
				SendNotice(it->second.second.c_str());
		}
		else
		{
			if (eventStatus)
				BroadcastNotice(it->second.first.c_str());
			else
				BroadcastNotice(it->second.second.c_str());
		}
	}

	// CLEAR BONUS
	if (m_data.eventIndex == BONUS_EVENT)
	{
		const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
		if (eventStatus)
		{
			for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
			{
				LPCHARACTER ch = (*it)->GetCharacter();
				if (!ch)
					continue;

				if (m_data.empireFlag != 0)
					if (m_data.empireFlag != ch->GetEmpire())
						continue;

				ch->ComputePoints();
			}
		}
		else
		{
			for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
			{
				LPCHARACTER ch = (*it)->GetCharacter();
				if (!ch)
					continue;
				if (m_data.empireFlag != 0)
					if (m_data.empireFlag != ch->GetEmpire())
						continue;
				long value = m_data.value[1];
				ch->ApplyPoint(m_data.value[0], -value);
				ch->ComputePoints();
			}
		}
	}
}
void CHARACTER_MANAGER::SetEventData(BYTE dayIndex, const std::vector<TEventManagerData>& m_data)
{
	auto it = m_eventData.find(dayIndex);
	if (it != m_eventData.end())
	{
		it->second.clear();
		for (DWORD j = 0; j < m_data.size(); ++j)
			it->second.emplace_back(m_data[j]);
	}
	else
	{
		m_eventData.emplace(dayIndex, m_data);
	}
}
#endif


#ifdef ENABLE_REWARD_SYSTEM
const char* GetRewardIndexToString(BYTE rewardIndex)
{
	std::map<BYTE, std::string> rewardNames = {
		{REWARD_PET_115,"REWARD_PET_115"},
		{REWARD_LEGENDARY_SKILL,"REWARD_LEGENDARY_SKILL"},
		{REWARD_LEGENDARY_SKILL_SET,"REWARD_LEGENDARY_SKILL_SET"},
		{REWARD_THANDRUIL,"REWARD_THANDRUIL"},
		{REWARD_HYDRA,"REWARD_HYDRA"},
		{REWARD_CRYSTAL_DRAGON,"REWARD_CRYSTAL_DRAGON"},
		{REWARD_OFFLINESHOP_SLOT,"REWARD_OFFLINESHOP_SLOT"},
		{REWARD_INVENTORY_SLOT,"REWARD_INVENTORY_SLOT"},
		{REWARD_AVERAGE,"REWARD_AVERAGE"},
		{REWARD_ELEMENT,"REWARD_ELEMENT"},
		{REWARD_BATTLEPASS,"REWARD_BATTLEPASS"},
		{REWARD_CUSTOM_SASH,"REWARD_CUSTOM_SASH"},
		{REWARD_AURA,"REWARD_AURA"},
		{REWARD_ENERGY,"REWARD_ENERGY"},
		{REWARD_112_BIO,"REWARD_112_BIO"},
		{REWARD_120_BIO,"REWARD_120_BIO"},
		{REWARD_LEADER_SHIP,"REWARD_LEADER_SHIP"},
		{REWARD_BUFFI_LEGENDARY_SKILL,"REWARD_BUFFI_LEGENDARY_SKILL"},
		{REWARD_50,"REWARD_50"},
		{REWARD_75,"REWARD_75"},
		{REWARD_90,"REWARD_90"},
		{REWARD_100,"REWARD_100"},
		{REWARD_110,"REWARD_110"},
		{REWARD_120,"REWARD_120"},
	};
	auto it = rewardNames.find(rewardIndex);
	if (it != rewardNames.end())
		return it->second.c_str();
	return 0;
}
BYTE GetRewardIndex(const char* szRewardName)
{
	std::map<std::string, BYTE> rewardNames = {
		{"REWARD_PET_115",REWARD_PET_115},
		{"REWARD_LEGENDARY_SKILL",REWARD_LEGENDARY_SKILL},
		{"REWARD_LEGENDARY_SKILL_SET",REWARD_LEGENDARY_SKILL_SET},
		{"REWARD_THANDRUIL",REWARD_THANDRUIL},
		{"REWARD_HYDRA",REWARD_HYDRA},
		{"REWARD_CRYSTAL_DRAGON",REWARD_CRYSTAL_DRAGON},
		{"REWARD_OFFLINESHOP_SLOT",REWARD_OFFLINESHOP_SLOT},
		{"REWARD_INVENTORY_SLOT",REWARD_INVENTORY_SLOT},
		{"REWARD_AVERAGE",REWARD_AVERAGE},
		{"REWARD_ELEMENT",REWARD_ELEMENT},
		{"REWARD_BATTLEPASS",REWARD_BATTLEPASS},
		{"REWARD_CUSTOM_SASH",REWARD_CUSTOM_SASH},
		{"REWARD_AURA",REWARD_AURA},
		{"REWARD_ENERGY",REWARD_ENERGY},
		{"REWARD_112_BIO",REWARD_112_BIO},
		{"REWARD_120_BIO",REWARD_120_BIO},
		{"REWARD_LEADER_SHIP",REWARD_LEADER_SHIP},
		{"REWARD_BUFFI_LEGENDARY_SKILL",REWARD_BUFFI_LEGENDARY_SKILL},
		{"REWARD_50",REWARD_50},
		{"REWARD_75",REWARD_75},
		{"REWARD_90",REWARD_90},
		{"REWARD_100",REWARD_100},
		{"REWARD_110",REWARD_110},
		{"REWARD_120",REWARD_120},
	};
	auto it = rewardNames.find(szRewardName);
	if (it != rewardNames.end())
		return it->second;
	return 0;
}
void CHARACTER_MANAGER::LoadRewardData()
{
	m_rewardData.clear();

	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery), "SELECT rewardIndex, lc_text, playerName, itemVnum0, itemCount0, itemVnum1, itemCount1, itemVnum2, itemCount2 FROM player.reward_table");
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));

	if (pMsg->Get()->uiNumRows != 0)
	{
		MYSQL_ROW row = NULL;
		for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
		{
			int col = 0;
			TRewardInfo p;
			p.m_rewardItems.clear();

			char rewardName[50];
			DWORD rewardIndex;
			strlcpy(rewardName, row[col++], sizeof(rewardName));
			rewardIndex = GetRewardIndex(rewardName);

			strlcpy(p.lc_text, row[col++], sizeof(p.lc_text));
			strlcpy(p.playerName, row[col++], sizeof(p.playerName));
			for (BYTE j = 0; j < 3; ++j)
			{
				DWORD itemVnum, itemCount;
				str_to_number(itemVnum, row[col++]);
				str_to_number(itemCount, row[col++]);
				p.m_rewardItems.emplace_back(itemVnum, itemCount);
			}
			m_rewardData.emplace(rewardIndex, p);
		}
	}
}
bool CHARACTER_MANAGER::IsRewardEmpty(BYTE rewardIndex)
{
	auto it = m_rewardData.find(rewardIndex);
	if (it != m_rewardData.end())
	{
		if (strcmp(it->second.playerName, "") == 0)
			return true;
	}
	return false;
}
void CHARACTER_MANAGER::SendRewardInfo(LPCHARACTER ch)
{
	ch->SetProtectTime("RewardInfo", 1);
	std::string cmd="";//12
	if (m_rewardData.size())
	{
		for (auto it = m_rewardData.begin(); it != m_rewardData.end(); ++it)
		{
			if (strlen(it->second.playerName) > 1)
			{
				char text[60];
				snprintf(text, sizeof(text), "%d|%s#", it->first, it->second.playerName);
				cmd += text;
			}
			if (strlen(cmd.c_str()) + 12 > CHAT_MAX_LEN - 30)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "RewardInfo %s", cmd.c_str());
				cmd = "";
			}
		}
		if (strlen(cmd.c_str()) > 1)
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RewardInfo %s", cmd.c_str());
	}
}

struct RewardForEach
{
	void operator() (LPDESC d)
	{
		LPCHARACTER ch = d->GetCharacter();
		if (ch == NULL)
			return;
		else if (ch->GetProtectTime("RewardInfo") != 1)
			return;
		CHARACTER_MANAGER::Instance().SendRewardInfo(ch);
	}
};
void CHARACTER_MANAGER::SetRewardData(BYTE rewardIndex, const char* playerName, bool isP2P)
{
	if (!IsRewardEmpty(rewardIndex))
		return;

	auto it = m_rewardData.find(rewardIndex);
	if (it == m_rewardData.end())
		return;
	TRewardInfo& rewardInfo = it->second;
	strlcpy(rewardInfo.playerName, playerName, sizeof(rewardInfo.playerName));

	if (isP2P)
	{
		LPCHARACTER ch = FindPC(playerName);
		if (ch)
		{
			for (DWORD j = 0; j < rewardInfo.m_rewardItems.size(); ++j)
				ch->AutoGiveItem(rewardInfo.m_rewardItems[j].first, rewardInfo.m_rewardItems[j].second);
		}

		char msg[CHAT_MAX_LEN+1];
		snprintf(msg, sizeof(msg), LC_TEXT(it->second.lc_text), playerName);
		//snprintf(msg, sizeof(msg), rewardInfo.lc_text, playerName);
		BroadcastNotice(msg);

		TPacketGGRewardInfo p;
		p.bHeader = HEADER_GG_REWARD_INFO;
		p.rewardIndex = rewardIndex;
		strlcpy(p.playerName, playerName, sizeof(p.playerName));
		P2P_MANAGER::Instance().Send(&p, sizeof(p));

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "UPDATE player.reward_table SET playerName = '%s' WHERE lc_text = '%s'", playerName, rewardInfo.lc_text);
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	}
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), RewardForEach());
}
#endif

#ifdef __BACK_DUNGEON__
bool CHARACTER_MANAGER::CheckBackDungeon(LPCHARACTER ch, BYTE checkType, DWORD mapIdx)
{
	if (!ch && checkType != BACK_DUNGEON_REMOVE)
		return false;

	//sys_err("%s %u %u",ch ? ch->GetName() : "None", checkType, mapIdx);
	// std::map<DWORD, std::map<std::pair<DWORD, WORD>, std::vector<DWORD>>> m_mapbackDungeon;

	if (checkType == BACK_DUNGEON_SAVE)
	{
		sys_err("BACK_DUNGEON_SAVE 0");
		const DWORD dungeonIdx = ch->GetMapIndex();
		if (dungeonIdx < 10000)
			return false;
		//sys_err("BACK_DUNGEON_SAVE 1");
		LPDUNGEON dungeon = CDungeonManager::Instance().FindByMapIndex(dungeonIdx);
		if (!dungeon)
			return false;
		//sys_err("BACK_DUNGEON_SAVE 2");
		if (dungeon->GetFlag("can_back_dungeon") != 1)
			return false;
		//sys_err("BACK_DUNGEON_SAVE 3");
		const DWORD mapIdx = dungeonIdx / 10000;
		auto it = m_mapbackDungeon.find(mapIdx);
		if (it == m_mapbackDungeon.end())
		{
			std::map<std::pair<DWORD, WORD>, std::vector<DWORD>> m_mapList;
			m_mapbackDungeon.emplace(mapIdx, m_mapList);
			it = m_mapbackDungeon.find(mapIdx);
		}
		auto itDungeon = it->second.find(std::make_pair(dungeonIdx, mother_port));
		if (itDungeon == it->second.end())
		{
			std::vector<DWORD> m_pidList;
			it->second.emplace(std::make_pair(dungeonIdx, mother_port), m_pidList);
			itDungeon = it->second.find(std::make_pair(dungeonIdx, mother_port));
		}
		if(std::find(itDungeon->second.begin(), itDungeon->second.end(),ch->GetPlayerID()) == itDungeon->second.end())
			itDungeon->second.emplace_back(ch->GetPlayerID());
	}
	else if (checkType == BACK_DUNGEON_REMOVE)
	{
		if (mapIdx < 10000)
			return false;
		auto it = m_mapbackDungeon.find(mapIdx / 10000);
		if (it != m_mapbackDungeon.end())
		{
			for (auto& [dungeonData, playerList] : it->second)
			{
				if (mapIdx == dungeonData.first)
				{
					LPDUNGEON dungeon = CDungeonManager::Instance().FindByMapIndex(mapIdx);
					if (dungeon)
						dungeon->SetFlag("can_back_dungeon", 0);
					it->second.erase(dungeonData);
					return true;
				}
			}
		}
	}
	else if (checkType == BACK_DUNGEON_CHECK || checkType == BACK_DUNGEON_WARP)
	{
		const DWORD curretMapIdx = ch->GetMapIndex();
		if (curretMapIdx > 10000)
			return false;

		const std::map<DWORD, std::vector<DWORD>> m_baseToDungeonIdx = {

			
			{
				91, // baseMapidx
				{
					160,//Atlantis Dungeon
					161,//Bloddy Dungeon
					163,//Andun Temple
					157,//Seelen Dungeon
					75,//Pyramide Dungeon
					66,//Demonen Turm
					280,//Spinnen Baroness
					216,//Devils Catacomb
					409,//Drachenraum
					285,//Razador
					289,//Nemere
					293,//Jotun
				},
			},

		};

		const auto itBase = m_baseToDungeonIdx.find(curretMapIdx);
		if (itBase != m_baseToDungeonIdx.end())
		{
			for (const auto& dungeonIdx : itBase->second)
			{
				auto it = m_mapbackDungeon.find(dungeonIdx);
				if (it != m_mapbackDungeon.end())
				{
					for (auto& [dungeonData, playerList] : it->second)
					{
						LPDUNGEON dungeon = CDungeonManager::Instance().FindByMapIndex(dungeonData.first);
						if (std::find(playerList.begin(), playerList.end(), ch->GetPlayerID()) == playerList.end())
							continue;
						if (!dungeon)
							continue;
						if (dungeon->GetFlag("can_back_dungeon") != 1)
							continue;
						if (checkType == BACK_DUNGEON_WARP)
						{
							if (mapIdx == dungeonData.first)
							{
								ch->WarpSet(dungeon->GetFlag("LAST_X"), dungeon->GetFlag("LAST_Y"), dungeonData.first);
								return true;
							}
						}
						else
							ch->ChatPacket(CHAT_TYPE_COMMAND, "ReJoinDungeonDialog %d", dungeonData.first);
					}
				}
			}
		}
	}
	return false;
}
#endif
