#include "stdafx.h"

#include "../../common/teen_packet.h"
#include "../../common/VnumHelper.h"

#include "char.h"

#include "config.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "motion.h"
#include "vector.h"
#include "packet.h"
#include "cmd.h"
#include "fishing.h"
#include "exchange.h"
#include "battle.h"
#include "affect.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "pvp.h"
#include "party.h"
#include "start_position.h"
#include "questmanager.h"
#include "log.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "war_map.h"
#include "xmas_event.h"
#include "banword.h"
#include "target.h"
#include "wedding.h"
#include "mob_manager.h"
#include "mining.h"
#include "monarch.h"
#include "castle.h"
#include "arena.h"
#include "dev_log.h"
#include "horsename_manager.h"
#include "pcbang.h"
#include "gm.h"
#include "map_location.h"
#include "BlueDragon_Binder.h"
#include "HackShield.h"
#include "skill_power.h"
#include "XTrapManager.h"
#include "buff_on_attributes.h"
#ifdef ENABLE_CLONE_SYSTEM
#include "DitoSystem.h"
#endif
#ifdef ENABLE_PREMIUM_SYSTEM
	#include "premium_system.h"
#endif
#ifdef __MELEY_LAIR_DUNGEON__
#include "MeleyLair.h"
#endif
#ifdef __PET_SYSTEM__
#include "PetSystem.h"
#endif
#ifdef LWT_BUFFI_SYSTEM
	#include "lwt_buffi.h"
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
#include "buff_npc_system.h"
#endif
#include "DragonSoul.h"

#ifdef ENABLE_NEW_PET_SYSTEM
#include "char_petsystem.h"
#endif
#ifdef ENABLE_SWITCHBOT
#include "new_switchbot.h"
#endif
#ifdef ENABLE_EXTENDED_BATTLE_PASS
#include "battlepass_manager.h"
#endif
#ifdef __SEND_TARGET_INFO__
#include <algorithm>
#include <iterator>
using namespace std;
#endif
#include <boost/algorithm/string.hpp>

#ifdef ENABLE_OFFLINESHOP_SYSTEM
#include "offline_shop.h"
#include "offlineshop_manager.h"
#endif

#include "spamblock.h"

extern const BYTE g_aBuffOnAttrPoints;
extern bool RaceToJob(unsigned race, unsigned *ret_job);

extern int g_nPortalLimitTime;
extern int test_server;

extern bool IS_SUMMONABLE_ZONE(int map_index); // char_item.cpp
bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index);

bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index)
{
	switch (map_index)
	{
	case 301:
	case 302:
	case 303:
	case 304:
		if (ch->GetLevel() < 90)
			return false;
	}
	return true;
}

// <Factor> DynamicCharacterPtr member function definitions

LPCHARACTER DynamicCharacterPtr::Get() const {
	LPCHARACTER p = NULL;
	if (is_pc) {
		p = CHARACTER_MANAGER::instance().FindByPID(id);
	} else {
		p = CHARACTER_MANAGER::instance().Find(id);
	}
	return p;
}

DynamicCharacterPtr& DynamicCharacterPtr::operator=(LPCHARACTER character) {
	if (character == NULL) {
		Reset();
		return *this;
	}
	if (character->IsPC()) {
		is_pc = true;
		id = character->GetPlayerID();
	} else {
		is_pc = false;
		id = character->GetVID();
	}
	return *this;
}

CHARACTER::CHARACTER()
{
	m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateIdle, &CHARACTER::EndStateEmpty);
	m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
	m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateBattle, &CHARACTER::EndStateEmpty);

	Initialize();
}

CHARACTER::~CHARACTER()
{
	Destroy();
}

void CHARACTER::Initialize()
{
	CEntity::Initialize(ENTITY_CHARACTER);

	m_bNoOpenedShop = true;

	m_bOpeningSafebox = false;
#ifdef __RENEWAL_BRAVE_CAPE__
	m_pkBraveCapeEvent = NULL;
#endif

	m_fSyncTime = get_float_time()-3;
	m_dwPlayerID = 0;
	m_dwKillerPID = 0;
#ifdef __SEND_TARGET_INFO__
	dwLastTargetInfoPulse = 0;
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
	m_stImmortalSt = 0;

	m_newpetskillcd[0] = 0;
	m_newpetskillcd[1] = 0;
	m_newpetskillcd[2] = 0;
#endif

	m_iMoveCount = 0;

	m_pkRegen = NULL;
	regen_id_ = 0;
	m_posRegen.x = m_posRegen.y = m_posRegen.z = 0;
	m_posStart.x = m_posStart.y = 0;
	m_posDest.x = m_posDest.y = 0;
	m_fRegenAngle = 0.0f;

	m_pkMobData		= NULL;
	m_pkMobInst		= NULL;

	m_pkShop		= NULL;
	m_pkChrShopOwner	= NULL;

	m_pkExchange	= NULL;
	m_pkParty		= NULL;
	m_pkPartyRequestEvent = NULL;
#ifdef ENABLE_OFFLINESHOP_SYSTEM
	m_pkOfflineShop = NULL;
	isOfflineShopPanelOpen = false;
	m_protection_Time.clear();
#endif
	m_pGuild = NULL;

	m_pkChrTarget = NULL;

	m_pkMuyeongEvent = NULL;

	m_pkWarpNPCEvent = NULL;
	m_pkDeadEvent = NULL;
	m_pkStunEvent = NULL;
	m_pkSaveEvent = NULL;
	m_pkRecoveryEvent = NULL;
	m_pkTimedEvent = NULL;
	m_pkFishingEvent = NULL;
	m_pkWarpEvent = NULL;

	// MINING
	m_pkMiningEvent = NULL;
	// END_OF_MINING

	m_pkPoisonEvent = NULL;
	m_pkFireEvent = NULL;
	m_pkCheckSpeedHackEvent	= NULL;
	m_speed_hack_count	= 0;

	m_pkAffectEvent = NULL;
	m_afAffectFlag = TAffectFlag(0, 0);

	m_pkDestroyWhenIdleEvent = NULL;
	m_bFarmState = false;

	m_pkChrSyncOwner = NULL;

	memset(&m_points, 0, sizeof(m_points));
	memset(&m_pointsInstant, 0, sizeof(m_pointsInstant));
	memset(&m_quickslot, 0, sizeof(m_quickslot));
#ifdef ENABLE_KILL_STATISTICS
	memset(&m_killstatistics, 0, sizeof(m_killstatistics));
#endif
	memset(&character_cards, 0, sizeof(character_cards));
	memset(&randomized_cards, 0, sizeof(randomized_cards));

	m_bCharType = CHAR_TYPE_MONSTER;

	SetPosition(POS_STANDING);

	m_dwPlayStartTime = m_dwLastMoveTime = get_dword_time();

	GotoState(m_stateIdle);
	m_dwStateDuration = 1;

	m_dwLastAttackTime = get_dword_time() - 20000;

	m_bAddChrState = 0;

	m_pkChrStone = NULL;

	m_pkSafebox = NULL;
	m_iSafeboxSize = -1;
	m_iSafeboxLoadTime = 0;

	m_pkMall = NULL;
	m_iMallLoadTime = 0;

	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;
	m_lWarpMapIndex = 0;

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;

	m_pSkillLevels = NULL;

	m_dwMoveStartTime = 0;
	m_dwMoveDuration = 0;

	m_dwFlyTargetID = 0;

	m_dwNextStatePulse = 0;

	m_dwLastDeadTime = get_dword_time()-180000;

	m_bSkipSave = false;

	m_bItemLoaded = false;

	m_bHasPoisoned = false;

	m_pkDungeon = NULL;
	m_iEventAttr = 0;

	m_kAttackLog.dwVID = 0;
	m_kAttackLog.dwTime = 0;

	m_bNowWalking = m_bWalking = false;
	ResetChangeAttackPositionTime();

	m_bDetailLog = false;
	m_bMonsterLog = false;

	m_bDisableCooltime = false;

	m_iAlignment = 0;
	m_iRealAlignment = 0;
	
#ifdef ENABLE_REBORN_SYSTEM
	m_iReborn = 0;
	m_iRealReborn = 0;
#endif
	
#ifdef ENABLE_TITLE_SYSTEM
	m_iPrestige = 0;
	m_iRealPrestige = 0;
#endif

	m_iKillerModePulse = 0;
	m_bPKMode = PK_MODE_PEACE;

	m_dwQuestNPCVID = 0;
	m_dwQuestByVnum = 0;
	m_pQuestItem = NULL;

	m_szMobileAuth[0] = '\0';

	m_dwUnderGuildWarInfoMessageTime = get_dword_time()-60000;

	m_bUnderRefine = false;

	// REFINE_NPC
	m_dwRefineNPCVID = 0;
	// END_OF_REFINE_NPC

	m_dwPolymorphRace = 0;

	m_bStaminaConsume = false;

	ResetChainLightningIndex();

	m_dwMountVnum = 0;
	m_chHorse = NULL;
	m_chRider = NULL;

	m_pWarMap = NULL;
	m_pWeddingMap = NULL;
	m_bChatCounter = 0;

	ResetStopTime();

	m_dwLastVictimSetTime = get_dword_time() - 3000;
	m_iMaxAggro = -100;

	m_bSendHorseLevel = 0;
	m_bSendHorseHealthGrade = 0;
	m_bSendHorseStaminaGrade = 0;

	m_dwLoginPlayTime = 0;

	m_pkChrMarried = NULL;

	m_posSafeboxOpen.x = -1000;
	m_posSafeboxOpen.y = -1000;

	// EQUIP_LAST_SKILL_DELAY
	m_dwLastSkillTime = get_dword_time();
	// END_OF_EQUIP_LAST_SKILL_DELAY

	// MOB_SKILL_COOLTIME
	memset(m_adwMobSkillCooltime, 0, sizeof(m_adwMobSkillCooltime));
	// END_OF_MOB_SKILL_COOLTIME

	m_isinPCBang = false;

	// ARENA
	m_pArena = NULL;
	m_nPotionLimit = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
	// END_ARENA

	//PREVENT_TRADE_WINDOW
	m_isOpenSafebox = 0;
	//END_PREVENT_TRADE_WINDOW
	
	//PREVENT_REFINE_HACK
	m_iRefineTime = 0;
	//END_PREVENT_REFINE_HACK
	
	//RESTRICT_USE_SEED_OR_MOONBOTTLE
	m_iSeedTime = 0;
	//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
	//PREVENT_PORTAL_AFTER_EXCHANGE
	m_iExchangeTime = 0;
	//END_PREVENT_PORTAL_AFTER_EXCHANGE
	//
	m_iSafeboxLoadTime = 0;

	m_iMyShopTime = 0;

	InitMC();

	m_deposit_pulse = 0;

	SET_OVER_TIME(this, OT_NONE);

	m_strNewName = "";

	m_known_guild.clear();

	m_dwLogOffInterval = 0;

	m_bComboSequence = 0;
	m_dwLastComboTime = 0;
	m_bComboIndex = 0;
	m_iComboHackCount = 0;
	m_dwSkipComboAttackByTime = 0;
#ifdef ENABLE_CHECK_PICKUP_HACK
	m_dwLastPickupTime = 0;
#endif
#ifdef ENABLE_CSHIELD
	m_pkCShieldEvent = NULL;
	m_pkCShieldDataEvent = NULL;
	m_cshield = nullptr;
#endif
	bIndexLotteryReward = -1;

#ifdef ENABLE_CHECK_PICKUP_HACK
	m_dwLastPickupTime = 0;
#endif
#ifdef ENABLE_CHECK_WALLHACK
	m_dwCountWallhackPoint = 0;
#endif
#ifdef ENABLE_CHECK_GHOSTMODE
	m_dwCountGhostmodePoint = 0;
#endif
	
	m_dwMountTime = 0;

	m_dwLastGoldDropTime = 0;

	m_HackShieldCheckEvent = NULL;
	m_HackShieldCheckMode = false;

	m_bIsLoadedAffect = false;
	cannot_dead = false;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	m_buffNPCSystem = 0;
	//m_buffSkillHealHPValue = 0;
	m_buffSkillRecoverHPValue = 0;
#endif
#ifdef __HIDE_COSTUME_SYSTEM__
	m_bHideBodyCostume = false;
	m_bHideHairCostume = false;
	#ifdef __SASH_SYSTEM__
	m_bHideSashCostume = false;
	#endif
	#ifdef __WEAPON_COSTUME_SYSTEM__
	m_bHideWeaponCostume = false;
	#endif
#endif
#ifdef ENABLE_CLONE_SYSTEM
	m_pkDito=NULL;
#endif

#ifdef __PET_SYSTEM__
	m_petSystem = 0;
	m_bIsPet = false;
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	m_newpetSystem = 0;
	m_bIsNewPet = false;
	m_eggvid = 0;
#endif
#ifdef LWT_BUFFI_SYSTEM
	m_bIsSupport = false;
	m_supportSystem = 0;
	m_Owner = NULL;
#endif

	m_fAttMul = 1.0f;
	m_fDamMul = 1.0f;

	m_pointsInstant.iDragonSoulActiveDeck = -1;

	memset(&m_tvLastSyncTime, 0, sizeof(m_tvLastSyncTime));
	m_iSyncHackCount = 0;
#if defined(__DAILY_GIFT_SYSTEM__)
	m_bDailyGiftOpen = false;
	m_bDailyGiftLoad = false;
	m_dwDailyGiftRenewTime = quest::CQuestManager::instance().GetEventFlag("daily_gift_renew_time");
#endif
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	m_listExtBattlePass.clear();
	m_bIsLoadedExtBattlePass = false;
	m_dwLastReciveExtBattlePassInfoTime = 0;
#endif
#ifdef __CHANGELOOK_SYSTEM__
	m_bChangeLook = false;
#endif
#ifdef __SASH_SYSTEM__
	m_bSashCombination = false;
	m_bSashAbsorption = false;
#endif
#ifdef __AURA_SYSTEM__
	m_bAuraRefineWindowType = AURA_WINDOW_TYPE_MAX;
	m_bAuraRefineWindowOpen = false;
	for (BYTE i = AURA_SLOT_MAIN; i < AURA_SLOT_MAX; i++)
		m_pAuraRefineWindowItemSlot[i] = NPOS;

	memset(&m_bAuraRefineInfo, 0, AURA_REFINE_INFO_SLOT_MAX * sizeof(TAuraRefineInfo));
#endif
	m_dwBuffiTime = 0;
#ifdef __SKILL_COSTUME__
	memset(&m_bSkillMotion, 0, sizeof(m_bSkillMotion));
#endif		
#ifdef __SKILL_COLOR_SYSTEM__
	memset(&m_dwSkillColor, 0, sizeof(m_dwSkillColor));
#endif
#ifdef __FAKE_PLAYER__
	m_bIsFakePlayer = false;
	m_pFakePlayerDeadEvent = nullptr;
	m_stFakePlayerLanguage = "en";
	m_dwFakePlayerGuildID = 0;
	m_bFakePlayerBlockEQ = 0;
#endif
}

void CHARACTER::Create(const char * c_pszName, DWORD vid, bool isPC)
{
	static int s_crc = 172814;

	char crc_string[128+1];
	snprintf(crc_string, sizeof(crc_string), "%s%p%d", c_pszName, this, ++s_crc);
	m_vid = VID(vid, GetCRC32(crc_string, strlen(crc_string)));

	if (isPC)
	{
		m_stName = c_pszName;
#ifdef ENABLE_CSHIELD
		m_cshield = std::make_shared<CShield>();
#endif
	}
}

#ifdef ENABLE_CLONE_SYSTEM
bool CHARACTER::IsDito()
{
	DWORD race = GetRaceNum();
	bool bFound =false;
	for (int i = MAIN_RACE_WARRIOR_M; i<MAIN_RACE_MAX_NUM; i++)
	{
		if (race == i)
			bFound =true;
	}
	return IsNPC() && bFound;
}

LPCHARACTER CHARACTER::GetDito()
{
	return m_pkDito ? m_pkDito->GetActor() : NULL;
}
#endif

void CHARACTER::Destroy()
{
	if (m_pkRegen)
	{
		if (m_pkDungeon) {
			// Dungeon regen may not be valid at this point
			if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_)) {
				--m_pkRegen->count;
			}
		} else {
			// Is this really safe?
			--m_pkRegen->count;
		}
		m_pkRegen = NULL;
	}

	if (m_pkDungeon)
	{
		SetDungeon(NULL);
	}

#ifdef __PET_SYSTEM__
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;

		m_petSystem = 0;
	}
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (m_buffNPCSystem)
	{
		m_buffNPCSystem->Destroy();
		delete m_buffNPCSystem;

		m_buffNPCSystem = 0;
	}
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	if (m_newpetSystem)
	{
		m_newpetSystem->Destroy();
		delete m_newpetSystem;

		m_newpetSystem = 0;
	}
#endif
#ifdef LWT_BUFFI_SYSTEM
	if (m_supportSystem)
	{
		m_supportSystem->Destroy();
		delete m_supportSystem;

		m_supportSystem = 0;
	}
#endif
#ifdef ENABLE_CLONE_SYSTEM
	if (m_pkDito)
	{
		m_pkDito->Unsummon();
		
		delete m_pkDito;

		m_pkDito = 0;
	}
#endif

	HorseSummon(false);

	if (GetRider())
		GetRider()->ClearHorseInfo();

	if( IsPC() )
	{
		if (isHackShieldEnable)
		{
			CHackShieldManager::instance().DeleteClientHandle(GetPlayerID());
		}
	}

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
//		BindDesc(NULL);
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();
#ifdef ENABLE_OFFLINESHOP_SYSTEM
	if (GetOfflineShop())
		COfflineShopManager::Instance().StopShopping(this);
#endif
	SetVictim(NULL);

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	ClearStone();
	ClearSync();
	ClearTarget();

	if (NULL == m_pkMobData)
	{
		DragonSoul_CleanUp();
		ClearItem();
	}

	// <Factor> m_pkParty becomes NULL after CParty destructor call!
	LPPARTY party = m_pkParty;
	if (party)
	{
		if (party->GetLeaderPID() == GetVID() && !IsPC())
		{
			M2_DELETE(party);
		}
		else
		{
			party->Unlink(this); 

			if (!IsPC())
				party->Quit(GetVID());
		}

		SetParty(NULL); // 안해도 되지만 안전하게.
	}

	if (m_pkMobInst)
	{
		M2_DELETE(m_pkMobInst);
		m_pkMobInst = NULL;
	}

	m_pkMobData = NULL;

	if (m_pkSafebox)
	{
		M2_DELETE(m_pkSafebox);
		m_pkSafebox = NULL;
	}

	if (m_pkMall)
	{
		M2_DELETE(m_pkMall);
		m_pkMall = NULL;
	}

	m_set_pkChrSpawnedBy.clear();

	StopMuyeongEvent();
	event_cancel(&m_pkWarpNPCEvent);
	event_cancel(&m_pkRecoveryEvent);
	event_cancel(&m_pkDeadEvent);
	event_cancel(&m_pkSaveEvent);
	event_cancel(&m_pkTimedEvent);
	event_cancel(&m_pkStunEvent);
	event_cancel(&m_pkFishingEvent);
	event_cancel(&m_pkPoisonEvent);
	event_cancel(&m_pkFireEvent);
	event_cancel(&m_pkPartyRequestEvent);
	//DELAYED_WARP
	event_cancel(&m_pkWarpEvent);
	event_cancel(&m_pkCheckSpeedHackEvent);
	//END_DELAYED_WARP

	// RECALL_DELAY
	//event_cancel(&m_pkRecallEvent);
	// END_OF_RECALL_DELAY

	// MINING
	event_cancel(&m_pkMiningEvent);
#ifdef ENABLE_CSHIELD
	event_cancel(&m_pkCShieldEvent);
	event_cancel(&m_pkCShieldDataEvent);
#endif
#ifdef __RENEWAL_BRAVE_CAPE__
	event_cancel(&m_pkBraveCapeEvent);
#endif
	// END_OF_MINING
#ifdef __FAKE_PLAYER__
	event_cancel(&m_pFakePlayerDeadEvent);
#endif		


	StopHackShieldCheckCycle();

	for (itertype(m_mapMobSkillEvent) it = m_mapMobSkillEvent.begin(); it != m_mapMobSkillEvent.end(); ++it)
	{
		LPEVENT pkEvent = it->second;
		event_cancel(&pkEvent);
	}
	m_mapMobSkillEvent.clear();

	//event_cancel(&m_pkAffectEvent);
	ClearAffect();

	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin();  it != m_map_buff_on_attrs.end(); it++)
	{
		if (NULL != it->second)
		{
			M2_DELETE(it->second);
		}
	}
	m_map_buff_on_attrs.clear();

	event_cancel(&m_pkDestroyWhenIdleEvent);

	if (m_pSkillLevels)
	{
		M2_DELETE_ARRAY(m_pSkillLevels);
		m_pSkillLevels = NULL;
	}

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);

	if (m_bMonsterLog)
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
}

const char * CHARACTER::GetName() const
{
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName : "") : m_stName.c_str();
}

#ifdef ENABLE_PVP_ADVANCED
void CHARACTER::DestroyPvP()
{
	if (GetDesc() != NULL)
	{
		const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};
		
		int moneyBet = GetQuestFlag(szTableStaticPvP[8]);
		int isDuel = GetQuestFlag(szTableStaticPvP[9]);
			
		if (isDuel != 0)
		{
			if (moneyBet > 0)
			{
				PointChange(POINT_GOLD, moneyBet, true);	
			}
			
			char szBuf[CHAT_MAX_LEN + 1];
			snprintf(szBuf, sizeof(szBuf), "BINARY_Duel_Delete");
			ChatPacket(CHAT_TYPE_COMMAND, szBuf);	
		
			for (int i = 0; i < _countof(szTableStaticPvP); i++)
			{
				SetQuestFlag(szTableStaticPvP[i], 0);	
			}
		}
	}
}
#endif

void EncodeMovePacket(TPacketGCMove & pack, DWORD dwVID, BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, BYTE bRot)
{
	pack.bHeader = HEADER_GC_MOVE;
	pack.bFunc   = bFunc;
	pack.bArg    = bArg;
	pack.dwVID   = dwVID;
	pack.dwTime  = dwTime ? dwTime : get_dword_time();
	pack.bRot    = bRot;
	pack.lX		= x;
	pack.lY		= y;
	pack.dwDuration	= dwDuration;
}

void CHARACTER::RestartAtSamePos()
{
	if (m_bIsObserver)
		return;

	EncodeRemovePacket(this);
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
			EncodeInsertPacket(entity);

		if( entity->IsType(ENTITY_CHARACTER) )
		{
			LPCHARACTER lpChar = (LPCHARACTER)entity;
			if( lpChar->IsPC() || lpChar->IsNPC() || lpChar->IsMonster() )
			{
				if (!entity->IsObserverMode())
					entity->EncodeInsertPacket(this);
			}
		}
		else
		{
			if( !entity->IsObserverMode())
			{
				entity->EncodeInsertPacket(this);
			}
		}
	}
}


// Entity에 내가 나타났다고 패킷을 보낸다.
void CHARACTER::EncodeInsertPacket(LPENTITY entity)
{

	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	// 길드이름 버그 수정 코드
	LPCHARACTER ch = (LPCHARACTER) entity;
	ch->SendGuildName(GetGuild());

#ifdef __FAKE_PLAYER__
	if (IsFakePlayer())
	{	
		if (m_dwFakePlayerGuildID)
			ch->SendGuildName(m_dwFakePlayerGuildID);
	}
#endif	

	TPacketGCCharacterAdd pack;

	pack.header		= HEADER_GC_CHARACTER_ADD;
	pack.dwVID		= m_vid;
	pack.bType		= GetCharType();
	pack.angle		= GetRotation();
	pack.x		= GetX();
	pack.y		= GetY();
	pack.z		= GetZ();
	pack.wRaceNum	= GetRaceNum();
#ifdef ENABLE_NEW_PET_SYSTEM
	if (IsPet() || IsNewPet())
#else
	if (IsPet())
#endif
	{
		pack.bMovingSpeed	= 150;
	}
	else
	{
		pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	}
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];

	pack.bStateFlag = m_bAddChrState;

	int iDur = 0;

	if (m_posDest.x != pack.x || m_posDest.y != pack.y)
	{
		iDur = (m_dwMoveStartTime + m_dwMoveDuration) - get_dword_time();

		if (iDur <= 0)
		{
			pack.x = m_posDest.x;
			pack.y = m_posDest.y;
		}
	}

	d->Packet(&pack, sizeof(pack));

	if (IsPC()
		|| m_bCharType == CHAR_TYPE_NPC
#ifdef __FAKE_PLAYER__
		|| IsFakePlayer()
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		|| IsBuffNPC()
#endif
	)
	{
		TPacketGCCharacterAdditionalInfo addPacket;
		memset(&addPacket, 0, sizeof(TPacketGCCharacterAdditionalInfo));

		addPacket.header = HEADER_GC_CHAR_ADDITIONAL_INFO;
		addPacket.dwVID = m_vid;

		addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
		addPacket.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
		addPacket.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
		addPacket.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		if (IsBuffNPC() == true) {
			addPacket.awPart[CHR_EQUIPPART_WEAPON] = GetBuffWearWeapon();
			addPacket.awPart[CHR_EQUIPPART_HAIR] = GetBuffWearHead();
			addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetBuffWearArmor();
		}
#endif
#ifdef __SASH_SYSTEM__
		addPacket.awPart[CHR_EQUIPPART_SASH] = GetPart(PART_SASH);
#endif
#ifdef __ITEM_SHINING__
		addPacket.adwShining[CHR_SHINING_WEAPON_1] = GetWear(WEAR_SHINING_WEAPON_1) ? GetWear(WEAR_SHINING_WEAPON_1)->GetVnum() : 0;
		addPacket.adwShining[CHR_SHINING_WEAPON_2] = GetWear(WEAR_SHINING_WEAPON_2) ? GetWear(WEAR_SHINING_WEAPON_2)->GetVnum() : 0;
		addPacket.adwShining[CHR_SHINING_WEAPON_3] = GetWear(WEAR_SHINING_WEAPON_3) ? GetWear(WEAR_SHINING_WEAPON_3)->GetVnum() : 0;
		addPacket.adwShining[CHR_SHINING_ARMOR_1] = GetWear(WEAR_SHINING_ARMOR_1) ? GetWear(WEAR_SHINING_ARMOR_1)->GetVnum() : 0;
		addPacket.adwShining[CHR_SHINING_ARMOR_2] = GetWear(WEAR_SHINING_ARMOR_2) ? GetWear(WEAR_SHINING_ARMOR_2)->GetVnum() : 0;
		addPacket.adwShining[CHR_SHINING_SPECIAL] = GetWear(WEAR_SHINING_SPECIAL) ? GetWear(WEAR_SHINING_SPECIAL)->GetVnum() : 0;
#endif

#ifdef __AURA_SYSTEM__
		addPacket.awPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif

#ifdef LWT_BUFFI_SYSTEM
		addPacket.bPKMode = IsSupport() ? PK_MODE_PROTECT : m_bPKMode;
#else
		addPacket.bPKMode = m_bPKMode;
#endif
		addPacket.dwMountVnum = GetMountVnum();
		addPacket.bEmpire = m_bEmpire;
#ifdef LWT_BUFFI_SYSTEM
		if(IsSupport())
		{
			addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetBuffiArmor();
			addPacket.awPart[CHR_EQUIPPART_WEAPON] = SetBuffiWeapon();
			addPacket.awPart[CHR_EQUIPPART_HAIR] = GetBuffiHair();
			addPacket.adwShining[CHR_SHINING_ARMOR_1] = GetBuffiEffect();

			// addPacket.bBuffiEffect =  GetBuffiEffect();
		}
#endif
#ifdef MULTI_LANGUAGE_SYSTEM		
		strcpy(addPacket.szCountry, GetLang().c_str());
#endif

		if (IsPC() 
#ifdef ENABLE_NEW_PET_SYSTEM		
			|| IsNewPet() 
#endif	
			|| IsSupport()
#ifdef __FAKE_PLAYER__
			|| IsFakePlayer()
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
			|| IsBuffNPC()
#endif	
		)
		{
			addPacket.dwLevel = GetLevel();
#ifdef __SKILL_COSTUME__
			memcpy(addPacket.bSkillMotion, GetSkillMotion(), sizeof(addPacket.bSkillMotion));
#endif	
#ifdef __SKILL_COLOR_SYSTEM__
			memcpy(addPacket.dwSkillColor, GetSkillColor(), sizeof(addPacket.dwSkillColor));
#endif			
		}
		else
		{
			addPacket.dwLevel = 0;
		}

		if (false)
		{
			LPCHARACTER ch = (LPCHARACTER) entity;

			if (GetEmpire() == ch->GetEmpire() || ch->GetGMLevel() > GM_PLAYER || m_bCharType == CHAR_TYPE_NPC)
			{
				goto show_all_info;
			}
			else
			{
				memset(addPacket.name, 0, CHARACTER_NAME_MAX_LEN);
				addPacket.dwGuildID = 0;
				addPacket.sAlignment = 0;
#ifdef ENABLE_REBORN_SYSTEM
				addPacket.sReborn = 0;
#endif
#ifdef ENABLE_TITLE_SYSTEM
				addPacket.sPrestige = 0;
#endif
			}
		}
		else
		{
		show_all_info:
				strlcpy(addPacket.name, GetName(), sizeof(addPacket.name));
#ifdef ENABLE_LANG_AND_EMPIRE_FLAG
/*			
				Here you need to replace the "ro" with your function what get current language, like: GetLanguage() and should return a char type or string.
				Need to return like "de", "ro", "en", "fr", "tr" etc.
				If your language is not like this and function to get lang is another type of data, you need to contact me and i will do a converting and adaptable with your language system.
				Should look like: strlcpy(addPacket.language, GetLanguage(), sizeof(addPacket.language));
*/
/// PENIS AMK		
			if (IsPC())
			{

				TAccountTable& account = GetDesc()->GetAccountTable();
				const char* lang = account.lang;
				memset(addPacket.language, 0, sizeof(addPacket.language));
				strncpy(addPacket.language, lang, sizeof(addPacket.language));
			}
#ifdef __FAKE_PLAYER__
			else if (IsFakePlayer())
				strncpy(addPacket.language, m_stFakePlayerLanguage.c_str(), sizeof(addPacket.language));
#endif
			// strlcpy(addPacket.language, lang, sizeof(addPacket.language));

#endif
			if (GetGuild() != NULL)
			{	
				addPacket.dwGuildID = GetGuild()->GetID();
				CGuild* pGuild = this->GetGuild();
				
				if (pGuild->GetMasterPID() == GetPlayerID())
					addPacket.dwNewIsGuildName = 3;
				else if (pGuild->NewIsGuildGeneral(GetPlayerID()) == true)
					addPacket.dwNewIsGuildName = 2;
				else
					addPacket.dwNewIsGuildName = 1;
			}
#ifdef __FAKE_PLAYER__
			else if (IsFakePlayer())
			{
				addPacket.dwGuildID = m_dwFakePlayerGuildID;
				addPacket.dwNewIsGuildName = 0;
			}
#endif				
			else
			{
				addPacket.dwGuildID = 0;
				addPacket.dwNewIsGuildName = 0;
			}

			addPacket.sAlignment = m_iAlignment / 10;
#ifdef ENABLE_REBORN_SYSTEM
			addPacket.sReborn = m_iReborn;
#endif
#ifdef ENABLE_TITLE_SYSTEM
			addPacket.sPrestige = m_iPrestige;
#endif
		}

		d->Packet(&addPacket, sizeof(TPacketGCCharacterAdditionalInfo));
	}

	if (iDur)
	{
		TPacketGCMove pack;
		EncodeMovePacket(pack, GetVID(), FUNC_MOVE, 0, m_posDest.x, m_posDest.y, iDur, 0, (BYTE) (GetRotation() / 5));
		d->Packet(&pack, sizeof(pack));

		TPacketGCWalkMode p;
		p.vid = GetVID();
		p.header = HEADER_GC_WALK_MODE;
		p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

		d->Packet(&p, sizeof(p));
	}
#ifdef ENABLE_OFFLINESHOP_SYSTEM
	if (IsOfflineShopNPC())
	{
		LPOFFLINESHOP shop = GetOfflineShop();
		if (shop)
		{
			TPacketGCShopSign p;
			p.bHeader = HEADER_GC_OFFLINE_SHOP_SIGN;
			p.dwVID = GetVID();
			strlcpy(p.szSign, shop->GetShopSign(), sizeof(p.szSign));
			d->Packet(&p, sizeof(TPacketGCShopSign));
		}
	}
#endif
	if (entity->IsType(ENTITY_CHARACTER) && GetDesc())
	{
		LPCHARACTER ch = (LPCHARACTER) entity;
		if (ch->IsWalking())
		{
			TPacketGCWalkMode p;
			p.vid = ch->GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = ch->m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;
			GetDesc()->Packet(&p, sizeof(p));
		}
	}

	if (entity->IsType(ENTITY_CHARACTER))
	{
		sys_log(3, "EntityInsert %s (RaceNum %d) (%d %d) TO %s",
				GetName(), GetRaceNum(), GetX() / SECTREE_SIZE, GetY() / SECTREE_SIZE, ((LPCHARACTER)entity)->GetName());
	}
}

void CHARACTER::EncodeRemovePacket(LPENTITY entity)
{
	if (entity->GetType() != ENTITY_CHARACTER)
		return;

	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	TPacketGCCharacterDelete pack;

	pack.header	= HEADER_GC_CHARACTER_DEL;
	pack.id	= m_vid;

	d->Packet(&pack, sizeof(TPacketGCCharacterDelete));

	if (entity->IsType(ENTITY_CHARACTER))
		sys_log(3, "EntityRemove %s(%d) FROM %s", GetName(), (DWORD) m_vid, ((LPCHARACTER) entity)->GetName());
}

void CHARACTER::UpdatePacket()
{
	if (GetSectree() == NULL) return;

	TPacketGCCharacterUpdate pack;
	TPacketGCCharacterUpdate pack2;

	pack.header = HEADER_GC_CHARACTER_UPDATE;
	pack.dwVID = m_vid;

	pack.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
	pack.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
	pack.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
	pack.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (IsBuffNPC() == true) {
		pack.awPart[CHR_EQUIPPART_WEAPON] = GetBuffWearWeapon();
		pack.awPart[CHR_EQUIPPART_HAIR] = GetBuffWearHead();
		pack.awPart[CHR_EQUIPPART_ARMOR] = GetBuffWearArmor();
	}
#endif
#ifdef __SASH_SYSTEM__
	pack.awPart[CHR_EQUIPPART_SASH] = GetPart(PART_SASH);
#endif
#ifdef __AURA_SYSTEM__
	pack.awPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif
#ifdef __ITEM_SHINING__
	pack.adwShining[CHR_SHINING_WEAPON_1] = GetWear(WEAR_SHINING_WEAPON_1) ? GetWear(WEAR_SHINING_WEAPON_1)->GetVnum() : 0;
	pack.adwShining[CHR_SHINING_WEAPON_2] = GetWear(WEAR_SHINING_WEAPON_2) ? GetWear(WEAR_SHINING_WEAPON_2)->GetVnum() : 0;
	pack.adwShining[CHR_SHINING_WEAPON_3] = GetWear(WEAR_SHINING_WEAPON_3) ? GetWear(WEAR_SHINING_WEAPON_3)->GetVnum() : 0;
	pack.adwShining[CHR_SHINING_ARMOR_1] = GetWear(WEAR_SHINING_ARMOR_1) ? GetWear(WEAR_SHINING_ARMOR_1)->GetVnum() : 0;
	pack.adwShining[CHR_SHINING_ARMOR_2] = GetWear(WEAR_SHINING_ARMOR_2) ? GetWear(WEAR_SHINING_ARMOR_2)->GetVnum() : 0;
	pack.adwShining[CHR_SHINING_SPECIAL] = GetWear(WEAR_SHINING_SPECIAL) ? GetWear(WEAR_SHINING_SPECIAL)->GetVnum() : 0;
#endif
#ifdef __SKILL_COSTUME__
	memcpy(pack.bSkillMotion, GetSkillMotion(), sizeof(pack.bSkillMotion));
#endif
#ifdef __SKILL_COLOR_SYSTEM__
	memcpy(pack.dwSkillColor, GetSkillColor(), sizeof(pack.dwSkillColor));
#endif
	pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
	pack.bStateFlag	= m_bAddChrState;
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
	pack.dwGuildID	= 0;
	pack.sAlignment	= m_iAlignment / 10;
#ifdef ENABLE_REBORN_SYSTEM
	pack.sReborn	= m_iReborn;
#endif
	pack.dwLevel = GetLevel();
#ifdef ENABLE_TITLE_SYSTEM
	pack.sPrestige	= m_iPrestige;
#endif
#ifdef LWT_BUFFI_SYSTEM
	pack.bPKMode = IsSupport() ? PK_MODE_PROTECT : m_bPKMode;
#else
	pack.bPKMode	= m_bPKMode;
#endif
	if (GetGuild())
		pack.dwGuildID = GetGuild()->GetID();
#ifdef __FAKE_PLAYER__
	else if (IsFakePlayer())
		pack.dwGuildID = m_dwFakePlayerGuildID;
#endif

	pack.dwMountVnum	= GetMountVnum();
#ifdef LWT_BUFFI_SYSTEM
	if(IsSupport())
	{
		pack.awPart[CHR_EQUIPPART_ARMOR] = GetBuffiArmor();
		pack.awPart[CHR_EQUIPPART_WEAPON] = SetBuffiWeapon();
		pack.awPart[CHR_EQUIPPART_HAIR] = GetBuffiHair();
		pack.adwShining[CHR_SHINING_ARMOR_1] = GetBuffiEffect();
		// pack.bBuffiEffect =  GetBuffiEffect();
	}	
#endif
#ifdef MULTI_LANGUAGE_SYSTEM	
	strcpy(pack.szCountry, GetLang().c_str());
#endif
	
	CGuild* pGuild = this->GetGuild();
	if (pGuild)
	{
		if (pGuild->GetMasterPID() == GetPlayerID())
			pack.dwNewIsGuildName = 3;
		else if (pGuild->NewIsGuildGeneral(GetPlayerID()) == true)
			pack.dwNewIsGuildName = 2;
		else
			pack.dwNewIsGuildName = 1;
	}
	else
	{
		pack.dwNewIsGuildName = 0;
	}

	pack2 = pack;
	pack2.dwGuildID = 0;
	pack2.sAlignment = 0;
#ifdef ENABLE_REBORN_SYSTEM
	pack2.sReborn = 0;
#endif
#ifdef LWT_BUFFI_SYSTEM
	pack2.bBuffiEffect = 0;
#endif
#ifdef ENABLE_TITLE_SYSTEM
	pack2.sPrestige = 0;
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	pack2.dwLevel = 0;
#endif

	if (false)
	{
		if (m_bIsObserver != true)
		{
			for (ENTITY_MAP::iterator iter = m_map_view.begin(); iter != m_map_view.end(); iter++)
			{
				LPENTITY pEntity = iter->first;

				if (pEntity != NULL)
				{
					if (pEntity->IsType(ENTITY_CHARACTER) == true)
					{
						if (pEntity->GetDesc() != NULL)
						{
							LPCHARACTER pChar = (LPCHARACTER)pEntity;

							if (GetEmpire() == pChar->GetEmpire() || pChar->GetGMLevel() > GM_PLAYER)
							{
								pEntity->GetDesc()->Packet(&pack, sizeof(pack));
							}
							else
							{
								pEntity->GetDesc()->Packet(&pack2, sizeof(pack2));
							}
						}
					}
					else
					{
						if (pEntity->GetDesc() != NULL)
						{
							pEntity->GetDesc()->Packet(&pack, sizeof(pack));
						}
					}
				}
			}
		}

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		PacketAround(&pack, sizeof(pack));
	}
}

LPCHARACTER CHARACTER::FindCharacterInView(const char * c_pszName, bool bFindPCOnly)
{
	ENTITY_MAP::iterator it = m_map_view.begin();

	for (; it != m_map_view.end(); ++it)
	{
		if (!it->first->IsType(ENTITY_CHARACTER))
			continue;

		LPCHARACTER tch = (LPCHARACTER) it->first;

		if (bFindPCOnly && tch->IsNPC())
			continue;

		if (!strcasecmp(tch->GetName(), c_pszName))
			return (tch);
	}

	return NULL;
}

void CHARACTER::SetPosition(int pos)
{
	if (pos == POS_STANDING)
	{
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

		event_cancel(&m_pkDeadEvent);
		event_cancel(&m_pkStunEvent);
	}
	else if (pos == POS_DEAD)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);

	if (!IsStone())
	{
		switch (pos)
		{
			case POS_FIGHTING:
				if (!IsState(m_stateBattle))
					MonsterLog("[BATTLE] 싸우는 상태");

				GotoState(m_stateBattle);
				break;

			default:
				if (!IsState(m_stateIdle))
					MonsterLog("[IDLE] 쉬는 상태");

				GotoState(m_stateIdle);
				break;
		}
	}

	m_pointsInstant.position = pos;
}

void CHARACTER::Save()
{
	if (!m_bSkipSave)
		CHARACTER_MANAGER::instance().DelayedSave(this);
}

void CHARACTER::CreatePlayerProto(TPlayerTable & tab)
{
	memset(&tab, 0, sizeof(TPlayerTable));

	if (GetNewName().empty())
	{
		strlcpy(tab.name, GetName(), sizeof(tab.name));
	}
	else
	{
		strlcpy(tab.name, GetNewName().c_str(), sizeof(tab.name));
	}

	strlcpy(tab.ip, GetDesc()->GetHostName(), sizeof(tab.ip));

	tab.id			= m_dwPlayerID;
	tab.voice		= GetPoint(POINT_VOICE);
	tab.level		= GetLevel();
	tab.level_step	= GetPoint(POINT_LEVEL_STEP);
	tab.exp			= GetExp();
	tab.gold		= GetGold();
#ifdef __GEM_SYSTEM__
	tab.gem			= GetGem();
#endif
#ifdef ENABLE_ASLAN_LOTTERY
	tab.lotto_moneypool 				= GetLottoMoney();
	tab.lotto_totalmoneywin 		= GetLottoTotalMoney();
#endif
	tab.job			= m_points.job;
	tab.part_base	= m_pointsInstant.bBasePart;
	tab.skill_group	= m_points.skill_group;

	DWORD dwPlayedTime = (get_dword_time() - m_dwPlayStartTime);

	if (dwPlayedTime > 60000)
	{
		if (GetSectree() && !GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		{
			if (GetRealAlignment() < 0)
			{
				if (IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_TIME))
					UpdateAlignment(120 * (dwPlayedTime / 60000));
				else
					UpdateAlignment(60 * (dwPlayedTime / 60000));
			}
			else
				UpdateAlignment(5 * (dwPlayedTime / 60000));
		}

		SetRealPoint(POINT_PLAYTIME, GetRealPoint(POINT_PLAYTIME) + dwPlayedTime / 60000);
		ResetPlayTime(dwPlayedTime % 60000);
	}

	tab.playtime = GetRealPoint(POINT_PLAYTIME);
	tab.lAlignment = m_iRealAlignment;
#ifdef ENABLE_REBORN_SYSTEM
	tab.lReborn = m_iRealReborn;
#endif
#ifdef ENABLE_TITLE_SYSTEM
	tab.lPrestige = m_iRealPrestige;
#endif

	if (m_posWarp.x != 0 || m_posWarp.y != 0)
	{
		tab.x = m_posWarp.x;
		tab.y = m_posWarp.y;
		tab.z = 0;
		tab.lMapIndex = m_lWarpMapIndex;
	}
	else
	{
		tab.x = GetX();
		tab.y = GetY();
		tab.z = GetZ();
		tab.lMapIndex	= GetMapIndex();
	}

	if (m_lExitMapIndex == 0)
	{
		tab.lExitMapIndex	= tab.lMapIndex;
		tab.lExitX		= tab.x;
		tab.lExitY		= tab.y;
	}
	else
	{
		tab.lExitMapIndex	= m_lExitMapIndex;
		tab.lExitX		= m_posExit.x;
		tab.lExitY		= m_posExit.y;
	}

	sys_log(0, "SAVE: %s %dx%d", GetName(), tab.x, tab.y);

	tab.st = GetRealPoint(POINT_ST);
	tab.ht = GetRealPoint(POINT_HT);
	tab.dx = GetRealPoint(POINT_DX);
	tab.iq = GetRealPoint(POINT_IQ);

	tab.stat_point = GetPoint(POINT_STAT);
	tab.skill_point = GetPoint(POINT_SKILL);
	tab.sub_skill_point = GetPoint(POINT_SUB_SKILL);
	tab.horse_skill_point = GetPoint(POINT_HORSE_SKILL);

	tab.stat_reset_count = GetPoint(POINT_STAT_RESET_COUNT);

	tab.hp = GetHP();
	tab.sp = GetSP();

	tab.stamina = GetStamina();

	tab.sRandomHP = m_points.iRandomHP;
	tab.sRandomSP = m_points.iRandomSP;
#ifdef ENABLE_KILL_STATISTICS
	tab.iJinnoKills = GetJinnoKills();
	tab.iShinsooKills = GetShinsooKills();
	tab.iChunjoKills = GetChunjoKills();
	tab.iTotalKills = GetTotalKills();
	tab.iDuelsWon = GetDuelsWon();
	tab.iDuelsLost = GetDuelsLost();
	tab.iMonstersKills = GetMonstersKills();
	tab.iBossesKills = GetBossesKills();
	tab.iStonesKills = GetStonesKills();
#endif

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
		tab.quickslot[i] = m_quickslot[i];

	if (m_stMobile.length() && !*m_szMobileAuth)
		strlcpy(tab.szMobile, m_stMobile.c_str(), sizeof(tab.szMobile));

	thecore_memcpy(tab.parts, m_pointsInstant.parts, sizeof(tab.parts));

	// REMOVE_REAL_SKILL_LEVLES
	thecore_memcpy(tab.skills, m_pSkillLevels, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

	tab.horse = GetHorseData();
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	tab.battle_pass_premium_id = GetExtBattlePassPremiumID();
#endif
#ifdef __SKILL_COSTUME__
	memcpy(tab.bSkillMotion, GetSkillMotion(), sizeof(tab.bSkillMotion));
#endif	
#ifdef ENABLE_OFFLINESHOP_SYSTEM
	tab.shopFlag = GetOfflineShopFlag();
#endif
}


void CHARACTER::SaveReal()
{
	if (m_bSkipSave)
		return;

	if (!GetDesc())
	{
		sys_err("Character::Save : no descriptor when saving (name: %s)", GetName());
		return;
	}

	TPlayerTable table;
	CreatePlayerProto(table);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, GetDesc()->GetHandle(), &table, sizeof(TPlayerTable));

	quest::PC * pkQuestPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (!pkQuestPC)
		sys_err("CHARACTER::Save : null quest::PC pointer! (name %s)", GetName());
	else
	{
		pkQuestPC->Save();
	}

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
	if (pMarriage)
		pMarriage->Save();
}

void CHARACTER::FlushDelayedSaveItem()
{
	// 저장 안된 소지품을 전부 저장시킨다.
	LPITEM item;

	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		if ((item = GetInventoryItem(i)))
			ITEM_MANAGER::instance().FlushDelayedSave(item);
}

void CHARACTER::Disconnect(const char * c_pszReason)
{
	assert(GetDesc() != NULL);

	sys_log(0, "DISCONNECT: %s (%s)", GetName(), c_pszReason ? c_pszReason : "unset" );
#ifdef __BACK_DUNGEON__
	CHARACTER_MANAGER::Instance().CheckBackDungeon(this, BACK_DUNGEON_SAVE);
#endif

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	if (GetArena() != NULL)
	{
		GetArena()->OnDisconnect(GetPlayerID());
	}

	if (GetParty() != NULL)
	{
		GetParty()->UpdateOfflineState(GetPlayerID());
	}

#ifdef ENABLE_OFFLINESHOP_SYSTEM
	if (GetOfflineShop())
	{
		GetOfflineShop()->RemoveGuest(this);
		SetOfflineShop(NULL);
	}
#endif

	marriage::CManager::instance().Logout(this);

	// P2P Logout
	TPacketGGLogout p;
	p.bHeader = HEADER_GG_LOGOUT;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogout));
#ifdef ENABLE_TITLE_SYSTEM
	char buf[51];
	snprintf(buf, sizeof(buf), "%s %d %d %ld %d %d %d", 
		inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), g_bChannel, GetMapIndex(), GetAlignment(), GetTitle(), GetReborn());
#else		
	char buf[51];
	snprintf(buf, sizeof(buf), "%s %d %d %ld %d", 
		inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), g_bChannel, GetMapIndex(), GetAlignment());		
#endif

	LogManager::instance().CharLog(this, 0, "LOGOUT", buf);
	#ifdef __MELEY_LAIR_DUNGEON__
	if (MeleyLair::CMgr::instance().IsMeleyMap(GetMapIndex()))
		MeleyLair::CMgr::instance().Leave(GetGuild(), this, false);
	#endif

	if (LC_IsYMIR() || LC_IsKorea() || LC_IsBrazil())
	{
		long playTime = GetRealPoint(POINT_PLAYTIME) - m_dwLoginPlayTime;
		LogManager::instance().LoginLog(false, GetDesc()->GetAccountTable().id, GetPlayerID(), GetLevel(), GetJob(), playTime);

		if (LC_IsBrazil() != true)
			CPCBangManager::instance().Log(GetDesc()->GetHostName(), GetPlayerID(), playTime);
	}

	if (m_pWarMap)
		SetWarMap(NULL);

	if (m_pWeddingMap)
	{
		SetWeddingMap(NULL);
	}

	if (GetGuild())
		GetGuild()->LogoutMember(this);

	quest::CQuestManager::instance().LogoutPC(this);
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	ListExtBattlePassMap::iterator itext = m_listExtBattlePass.begin();
	while (itext != m_listExtBattlePass.end())
	{
		TPlayerExtBattlePassMission* pkMission = *itext++;

		if (!pkMission->bIsUpdated)
			continue;

		db_clientdesc->DBPacket(HEADER_GD_SAVE_EXT_BATTLE_PASS, 0, pkMission, sizeof(TPlayerExtBattlePassMission));
	}
	m_bIsLoadedExtBattlePass = false;
#endif
#ifdef ENABLE_PVP_ADVANCED
	DestroyPvP();
#endif
	if (GetParty())
		GetParty()->Unlink(this);

	// 죽었을 때 접속끊으면 경험치 줄게 하기
	if (IsStun() || IsDead())
	{
		DeathPenalty(0);
		PointChange(POINT_HP, 50 - GetHP());
	}


	if (!CHARACTER_MANAGER::instance().FlushDelayedSave(this))
	{
		SaveReal();
	}

	FlushDelayedSaveItem();

	SaveAffect();
	m_bIsLoadedAffect = false;

	m_bSkipSave = true; // 이 이후에는 더이상 저장하면 안된다.

	quest::CQuestManager::instance().DisconnectPC(this);

	CloseSafebox();

	CloseMall();
#ifdef __AURA_SYSTEM__
	if (IsAuraRefineWindowOpen())
		AuraRefineWindowClose();
#endif

	CPVPManager::instance().Disconnect(this);

	CTargetManager::instance().Logout(GetPlayerID());

	MessengerManager::instance().Logout(GetName());

	if (g_TeenDesc)
	{
		int		offset = 0;
		char	buf[245] = {0};

		buf[0] = HEADER_GT_LOGOUT;
		offset += 1;

		memset(buf+offset, 0x00, 2);
		offset += 2;

		TAccountTable	&acc_table = GetDesc()->GetAccountTable();
		memcpy(buf+offset, &acc_table.id, 4);
		offset += 4;

		g_TeenDesc->Packet(buf, offset);
	}

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
//		BindDesc(NULL);
	}

	CXTrapManager::instance().DestroyClientSession(this);

	M2_DESTROY_CHARACTER(this);
}

bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion/* = false */)
{
	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "cannot find sectree by %dx%d mapindex %d", x, y, lMapIndex);
		return false;
	}

	SetMapIndex(lMapIndex);

	bool bChangeTree = false;

	if (!GetSectree() || GetSectree() != sectree)
		bChangeTree = true;

	if (bChangeTree)
	{
		if (GetSectree())
			GetSectree()->RemoveEntity(this);

		ViewCleanup();
	}

	if (IsFakePlayer())
	{
		// nothing to do here
	}
	
#ifdef ENABLE_CHECK_MOVESPEED_HACK
	if (IsPC())
		GetCShield()->ResetMoveSpeedhack(get_dword_time());
#endif
	
	else if (!IsNPC())
	{
		sys_log(0, "SHOW: %s %dx%dx%d", GetName(), x, y, z);
		//GetCShield()->ResetMoveSpeedhack(get_dword_time()); // Deaktivieren DEV Server. Wieder AKtivieren bei Live CShiled Elendos V2
		if (GetStamina() < GetMaxStamina())
			StartAffectEvent();
	}
	else if (m_pkMobData)
	{
		m_pkMobInst->m_posLastAttacked.x = x;
		m_pkMobInst->m_posLastAttacked.y = y;
		m_pkMobInst->m_posLastAttacked.z = z;
	}

	if (bShowSpawnMotion || HasSpawnAnimation())
	{
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
		m_afAffectFlag.Set(AFF_SPAWN);
	}

	SetXYZ(x, y, z);

	m_posDest.x = x;
	m_posDest.y = y;
	m_posDest.z = z;

	m_posStart.x = x;
	m_posStart.y = y;
	m_posStart.z = z;

	if (bChangeTree)
	{
		EncodeInsertPacket(this);
		sectree->InsertEntity(this);

		UpdateSectree();
	}
	else
	{
		ViewReencode();
		sys_log(0, "      in same sectree");
	}

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	
	SetValidComboInterval(0);
	return true;
}

// BGM_INFO
struct BGMInfo
{
	std::string	name;
	float		vol;
};

typedef std::map<unsigned, BGMInfo> BGMInfoMap;

static BGMInfoMap 	gs_bgmInfoMap;
static bool		gs_bgmVolEnable = false;

void CHARACTER_SetBGMVolumeEnable()
{
	gs_bgmVolEnable = true;	
	sys_log(0, "bgm_info.set_bgm_volume_enable");
}

void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float vol)
{
	BGMInfo newInfo;
	newInfo.name = name;
	newInfo.vol = vol;

	gs_bgmInfoMap[mapIndex] = newInfo;

	sys_log(0, "bgm_info.add_info(%d, '%s', %f)", mapIndex, name, vol);
}

const BGMInfo& CHARACTER_GetBGMInfo(unsigned mapIndex)
{
	BGMInfoMap::iterator f = gs_bgmInfoMap.find(mapIndex);
	if (gs_bgmInfoMap.end() == f)
	{
		static BGMInfo s_empty = {"", 0.0f};
		return s_empty;
	}
	return f->second;
}

bool CHARACTER_IsBGMVolumeEnable()
{
	return gs_bgmVolEnable;
}
// END_OF_BGM_INFO

void CHARACTER::MainCharacterPacket()
{
	const unsigned mapIndex = GetMapIndex();
	const BGMInfo& bgmInfo = CHARACTER_GetBGMInfo(mapIndex);

	// SUPPORT_BGM
	// if (!bgmInfo.name.empty())
	// {
		// if (CHARACTER_IsBGMVolumeEnable())
		// {
			// sys_log(1, "bgm_info.play_bgm_vol(%d, name='%s', vol=%f)", mapIndex, bgmInfo.name.c_str(), bgmInfo.vol);
			// TPacketGCMainCharacter4_BGM_VOL mainChrPacket;
			// mainChrPacket.header = HEADER_GC_MAIN_CHARACTER4_BGM_VOL;
			// mainChrPacket.dwVID = m_vid;
			// mainChrPacket.wRaceNum = GetRaceNum();
			// mainChrPacket.lx = GetX();
			// mainChrPacket.ly = GetY();
			// mainChrPacket.lz = GetZ();
			// mainChrPacket.empire = GetDesc()->GetEmpire();
			// mainChrPacket.skill_group = GetSkillGroup();
			// strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));

			// mainChrPacket.fBGMVol = bgmInfo.vol;
			// strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			// GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter4_BGM_VOL));
		// }
		// else
		// {
			// sys_log(1, "bgm_info.play(%d, '%s')", mapIndex, bgmInfo.name.c_str());
			// TPacketGCMainCharacter3_BGM mainChrPacket;
			// mainChrPacket.header = HEADER_GC_MAIN_CHARACTER3_BGM;
			// mainChrPacket.dwVID = m_vid;
			// mainChrPacket.wRaceNum = GetRaceNum();
			// mainChrPacket.lx = GetX();
			// mainChrPacket.ly = GetY();
			// mainChrPacket.lz = GetZ();
			// mainChrPacket.empire = GetDesc()->GetEmpire();
			// mainChrPacket.skill_group = GetSkillGroup();
			// strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));
			// strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			// GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter3_BGM));
		// }
		// if (m_stMobile.length())
				// ChatPacket(CHAT_TYPE_COMMAND, "sms");
	// }
	// END_OF_SUPPORT_BGM
	// else
	// {
		sys_log(0, "bgm_info.play(%d, DEFAULT_BGM_NAME)", mapIndex);

		TPacketGCMainCharacter pack;
		pack.header = HEADER_GC_MAIN_CHARACTER;
		pack.dwVID = m_vid;
		pack.wRaceNum = GetRaceNum();
		pack.lx = GetX();
		pack.ly = GetY();
		pack.lz = GetZ();
		pack.empire = GetDesc()->GetEmpire();
		pack.skill_group = GetSkillGroup();
		strlcpy(pack.szName, GetName(), sizeof(pack.szName));
		GetDesc()->Packet(&pack, sizeof(TPacketGCMainCharacter));

		if (m_stMobile.length())
			ChatPacket(CHAT_TYPE_COMMAND, "sms");
	// }
}

void CHARACTER::PointsPacket()
{
	if (!GetDesc())
		return;

	TPacketGCPoints pack;

	pack.header	= HEADER_GC_CHARACTER_POINTS;

	pack.points[POINT_LEVEL]		= GetLevel();
	pack.points[POINT_EXP]		= GetExp();
	pack.points[POINT_NEXT_EXP]		= GetNextExp();
	pack.points[POINT_HP]		= GetHP();
	pack.points[POINT_MAX_HP]		= GetMaxHP();
	pack.points[POINT_SP]		= GetSP();
	pack.points[POINT_MAX_SP]		= GetMaxSP();
	pack.points[POINT_GOLD]		= GetGold();
#ifdef __GEM_SYSTEM__
	pack.points[POINT_GEM] = GetGem();
#endif
	pack.points[POINT_STAMINA]		= GetStamina();
	pack.points[POINT_MAX_STAMINA]	= GetMaxStamina();

	for (int i = POINT_ST; i < POINT_MAX_NUM; ++i)
		pack.points[i] = GetPoint(i);

#ifdef ENABLE_ASLAN_LOTTERY
	pack.points[POINT_LOTTO_MONEY] 	= GetLottoMoney();
	pack.points[POINT_LOTTO_TOTAL_MONEY] 	= GetLottoTotalMoney();
#endif
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	pack.points[POINT_BATTLE_PASS_PREMIUM_ID] = GetExtBattlePassPremiumID();
#endif
#ifdef __LIMIT_PRESTIGE__
	pack.points[POINT_PRESTIGE]		= GetReborn();
#endif
	GetDesc()->Packet(&pack, sizeof(TPacketGCPoints));
}

bool CHARACTER::ChangeSex()
{
	int src_race = GetRaceNum();

	switch (src_race)
	{
		case MAIN_RACE_WARRIOR_M:
			m_points.job = MAIN_RACE_WARRIOR_W;
			break;

		case MAIN_RACE_WARRIOR_W:
			m_points.job = MAIN_RACE_WARRIOR_M;
			break;

		case MAIN_RACE_ASSASSIN_M:
			m_points.job = MAIN_RACE_ASSASSIN_W;
			break;

		case MAIN_RACE_ASSASSIN_W:
			m_points.job = MAIN_RACE_ASSASSIN_M;
			break;

		case MAIN_RACE_SURA_M:
			m_points.job = MAIN_RACE_SURA_W;
			break;

		case MAIN_RACE_SURA_W:
			m_points.job = MAIN_RACE_SURA_M;
			break;

		case MAIN_RACE_SHAMAN_M:
			m_points.job = MAIN_RACE_SHAMAN_W;
			break;

		case MAIN_RACE_SHAMAN_W:
			m_points.job = MAIN_RACE_SHAMAN_M;
			break;

		default:
			sys_err("CHANGE_SEX: %s unknown race %d", GetName(), src_race);
			return false;
	}

	sys_log(0, "CHANGE_SEX: %s (%d -> %d)", GetName(), src_race, m_points.job);
#ifdef ENABLE_CLONE_SYSTEM
	if (m_pkDito)
	{
		if (m_pkDito->IsSummoned())
		{
			m_pkDito->Unsummon();
			m_pkDito->SetOwner(this);
			m_pkDito->Summon();
		
		}	
	}
#endif
	return true;
}

WORD CHARACTER::GetRaceNum() const
{
	if (m_dwPolymorphRace)
		return m_dwPolymorphRace;

	if (m_pkMobData)
		return m_pkMobData->m_table.dwVnum;

	return m_points.job;
}

void CHARACTER::SetRace(BYTE race)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("CHARACTER::SetRace(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
		return;
	}

	m_points.job = race;
}

BYTE CHARACTER::GetJob() const
{
	unsigned race = m_points.job;
	unsigned job;

	if (RaceToJob(race, &job))
		return job;

	sys_err("CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
	return JOB_WARRIOR;
}

void CHARACTER::SetLevel(BYTE level)
{
	m_points.level = level;

	if (IsPC())
	{
		if (level < PK_PROTECT_LEVEL)
			SetPKMode(PK_MODE_PROTECT);
		else if (GetGMLevel() != GM_PLAYER)
			SetPKMode(PK_MODE_PROTECT);
		else if (m_bPKMode == PK_MODE_PROTECT)
			SetPKMode(PK_MODE_PEACE);
	}
}

void CHARACTER::SetEmpire(BYTE bEmpire)
{
	m_bEmpire = bEmpire;
}

void CHARACTER::SetPlayerProto(const TPlayerTable * t)
{
	if (!GetDesc() || !*GetDesc()->GetHostName())
		sys_err("cannot get desc or hostname");
	else
		SetGMLevel();

	m_bCharType = CHAR_TYPE_PC;

	m_dwPlayerID = t->id;

	m_iAlignment = t->lAlignment;
	m_iRealAlignment = t->lAlignment;
#ifdef ENABLE_REBORN_SYSTEM
	m_iReborn = t->lReborn;
	m_iRealReborn = t->lReborn;
#endif
	
#ifdef ENABLE_TITLE_SYSTEM
	m_iPrestige = t->lPrestige;
	m_iRealPrestige = t->lPrestige;
#endif
#ifdef __SKILL_COSTUME__
	memcpy(m_bSkillMotion, t->bSkillMotion, sizeof(m_bSkillMotion)); 
#endif
	m_points.voice = t->voice;

	m_points.skill_group = t->skill_group; 

	m_pointsInstant.bBasePart = t->part_base;
	SetPart(PART_HAIR, t->parts[PART_HAIR]);
#ifdef __SASH_SYSTEM__
	SetPart(PART_SASH, t->parts[PART_SASH]);
#endif
#ifdef __AURA_SYSTEM__
	SetPart(PART_AURA, t->parts[PART_AURA]);
#endif

	m_points.iRandomHP = t->sRandomHP;
	m_points.iRandomSP = t->sRandomSP;
#ifdef ENABLE_KILL_STATISTICS
	m_killstatistics.iJinnoKills = t->iJinnoKills;
	m_killstatistics.iShinsooKills = t->iShinsooKills;
	m_killstatistics.iChunjoKills = t->iChunjoKills;
	m_killstatistics.iTotalKills = t->iTotalKills;
	m_killstatistics.iDuelsWon = t->iDuelsWon;
	m_killstatistics.iDuelsLost = t->iDuelsLost;
	m_killstatistics.iMonstersKills = t->iMonstersKills;
	m_killstatistics.iBossesKills = t->iBossesKills;
	m_killstatistics.iStonesKills = t->iStonesKills;
#endif

	// REMOVE_REAL_SKILL_LEVLES
	if (m_pSkillLevels)
		M2_DELETE_ARRAY(m_pSkillLevels);

	m_pSkillLevels = M2_NEW TPlayerSkill[SKILL_MAX_NUM];
	thecore_memcpy(m_pSkillLevels, t->skills, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

	if (t->lMapIndex >= 10000)
	{
		m_posWarp.x = t->lExitX;
		m_posWarp.y = t->lExitY;
		m_lWarpMapIndex = t->lExitMapIndex;
	}

	SetRealPoint(POINT_PLAYTIME, t->playtime);
	m_dwLoginPlayTime = t->playtime;
	SetRealPoint(POINT_ST, t->st);
	SetRealPoint(POINT_HT, t->ht);
	SetRealPoint(POINT_DX, t->dx);
	SetRealPoint(POINT_IQ, t->iq);

	SetPoint(POINT_ST, t->st);
	SetPoint(POINT_HT, t->ht);
	SetPoint(POINT_DX, t->dx);
	SetPoint(POINT_IQ, t->iq);

	SetPoint(POINT_STAT, t->stat_point);
	SetPoint(POINT_SKILL, t->skill_point);
	SetPoint(POINT_SUB_SKILL, t->sub_skill_point);
	SetPoint(POINT_HORSE_SKILL, t->horse_skill_point);

	SetPoint(POINT_STAT_RESET_COUNT, t->stat_reset_count);

	SetPoint(POINT_LEVEL_STEP, t->level_step);
	SetRealPoint(POINT_LEVEL_STEP, t->level_step);

	SetRace(t->job);

	SetLevel(t->level);
	SetExp(t->exp);
	SetGold(t->gold);
#ifdef __GEM_SYSTEM__
	SetGem(t->gem);
#endif
#ifdef ENABLE_ASLAN_LOTTERY
	SetLottoMoney(t->lotto_moneypool);
	SetLottoTotalMoney(t->lotto_totalmoneywin);
#endif

#ifdef ENABLE_EXTENDED_BATTLE_PASS
	SetExtBattlePassPremiumID(t->battle_pass_premium_id);
#endif
	SetMapIndex(t->lMapIndex);
	SetXYZ(t->x, t->y, t->z);
#ifdef ENABLE_OFFLINESHOP_SYSTEM
	SetOfflineShopFlag(t->shopFlag);
#endif
	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());
	SetStamina(t->stamina);

	//GM일때 보호모드  
	if (!test_server)
	{
		if (GetGMLevel() > GM_LOW_WIZARD)
		{
			m_afAffectFlag.Set(AFF_YMIR);
			m_bPKMode = PK_MODE_PROTECT;
		}
	}

#ifdef ENABLE_PREMIUM_SYSTEM
	#ifdef ENABLE_PREMIUM_DEBUG
	sys_log(0, "Player is premium checking.");
	#endif
	if(CPremiumSystem::instance().IsPremium(this))
	{
		#ifdef ENABLE_PREMIUM_DEBUG
		sys_log(0, "Player is premium, calling functions.");
		m_afAffectFlag.Set(AFF_YMIR);
		#endif
		m_afAffectFlag.Set(AFF_PREMIUM);
		CPremiumSystem::instance().ExpirationCheck(this);
		CPremiumSystem::instance().BoxCheck(this);
		#ifdef ENABLE_PREMIUM_DEBUG
		sys_log(0, "Player is premium, calling functions end.");
		#endif
	}
#endif

	if (GetLevel() < PK_PROTECT_LEVEL)
		m_bPKMode = PK_MODE_PROTECT;

	m_stMobile = t->szMobile;

	SetHorseData(t->horse);

	if (GetHorseLevel() > 0)
		UpdateHorseDataByLogoff(t->logoff_interval);

	thecore_memcpy(m_aiPremiumTimes, t->aiPremiumTimes, sizeof(t->aiPremiumTimes));

	m_dwLogOffInterval = t->logoff_interval;

	sys_log(0, "PLAYER_LOAD: %s PREMIUM %d %d, LOGGOFF_INTERVAL %u PTR: %p", t->name, m_aiPremiumTimes[0], m_aiPremiumTimes[1], t->logoff_interval, this);

	if (GetGMLevel() != GM_PLAYER) 
	{
		LogManager::instance().CharLog(this, GetGMLevel(), "GM_LOGIN", "");
		sys_log(0, "GM_LOGIN(gmlevel=%d, name=%s(%d), pos=(%d, %d)", GetGMLevel(), GetName(), GetPlayerID(), GetX(), GetY());
	}

#ifdef __PET_SYSTEM__
	// NOTE: 일단 캐릭터가 PC인 경우에만 PetSystem을 갖도록 함. 유럽 머신당 메모리 사용률때문에 NPC까지 하긴 좀..
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;
	}

	m_petSystem = M2_NEW CPetSystem(this);
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (m_buffNPCSystem)
	{
		m_buffNPCSystem->Destroy();
		delete m_buffNPCSystem;
	}

	m_buffNPCSystem = M2_NEW CBuffNPCActor(this);
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	if (m_newpetSystem)
	{
		m_newpetSystem->Destroy();
		delete m_newpetSystem;
	}

	m_newpetSystem = M2_NEW CNewPetSystem(this);
#endif
#ifdef LWT_BUFFI_SYSTEM
	if (m_supportSystem)
	{
		m_supportSystem->Destroy();
		delete m_supportSystem;
	}

	m_supportSystem = M2_NEW CSupportSystem(this);
#endif
#ifdef ENABLE_CLONE_SYSTEM
	m_pkDito=M2_NEW CDitoActor;
	m_pkDito->SetOwner(this);
#endif
}

EVENTFUNC(kill_ore_load_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "kill_ore_load_even> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}	

	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

#ifdef LWT_BUFFI_SYSTEM
void CHARACTER::SendSupportSkillPacket(DWORD skill_vnum, DWORD skillValue)
{
	TPacketGCSupportUseSkill pack;
	pack.bHeader = HEADER_GC_SUPPORT_SKILL;
	pack.dwVnum = skill_vnum;
	pack.dwVid = GetVID();
	pack.dwLevel = skillValue;
	
	PacketView(&pack, sizeof(TPacketGCSupportUseSkill), this);
}
#endif

void CHARACTER::SetProto(const CMob * pkMob)
{
	if (m_pkMobInst)
		M2_DELETE(m_pkMobInst);

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;

	m_bPKMode = PK_MODE_FREE;

	const TMobTable * t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	////////////////////
	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	if (IsStone())
	{
		DetermineDropMetinStone();
	}

	if (IsWarp() || IsGoto())
	{
		StartWarpNPCEvent();
	}

	CHARACTER_MANAGER::instance().RegisterRaceNumMap(this);

	// XXX X-mas santa hardcoding
	if (GetRaceNum() == xmas::MOB_SANTA_VNUM)
	{
		SetPoint(POINT_ATT_GRADE_BONUS, 10);
		if (g_iUseLocale)
			SetPoint(POINT_DEF_GRADE_BONUS, 6);
		else
			SetPoint(POINT_DEF_GRADE_BONUS, 15);

		//산타용
		//m_dwPlayStartTime = get_dword_time() + 10 * 60 * 1000;
		//신선자 노해 
		m_dwPlayStartTime = get_dword_time() + 30 * 1000;
		if (test_server)
			m_dwPlayStartTime = get_dword_time() + 30 * 1000;
	}

	// XXX CTF GuildWar hardcoding
	if (warmap::IsWarFlag(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
	}

	if (warmap::IsWarFlagBase(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
	}

	if (m_bCharType == CHAR_TYPE_HORSE || 
			GetRaceNum() == 20101 ||
			GetRaceNum() == 20102 ||
			GetRaceNum() == 20103 ||
			GetRaceNum() == 20104 ||
			GetRaceNum() == 20105 ||
			GetRaceNum() == 20106 ||
			GetRaceNum() == 20107 ||
			GetRaceNum() == 20108 ||
			GetRaceNum() == 20109
	  )
	{
		SetHorseAI();
	}

	// MINING
	if (mining::IsVeinOfOre (GetRaceNum()))
	{
		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = this;

		m_pkMiningEvent = event_create(kill_ore_load_event, info, PASSES_PER_SEC(number(7 * 60, 15 * 60)));
	}
	// END_OF_MINING
}

const TMobTable & CHARACTER::GetMobTable() const
{
	return m_pkMobData->m_table;
}

bool CHARACTER::IsRaceFlag(DWORD dwBit) const
{
	return m_pkMobData ? IS_SET(m_pkMobData->m_table.dwRaceFlag, dwBit) : 0;
}

DWORD CHARACTER::GetMobDamageMin() const
{
	return m_pkMobData->m_table.dwDamageRange[0];
}

DWORD CHARACTER::GetMobDamageMax() const
{
	return m_pkMobData->m_table.dwDamageRange[1];
}

float CHARACTER::GetMobDamageMultiply() const
{
	float fDamMultiply = GetMobTable().fDamMultiply;

	if (IsBerserk())
		fDamMultiply = fDamMultiply * 2.0f; // BALANCE: 광폭화 시 두배

	return fDamMultiply;
}

DWORD CHARACTER::GetMobDropItemVnum() const
{
	return m_pkMobData->m_table.dwDropItemVnum;
}

bool CHARACTER::IsSummonMonster() const
{
	return GetSummonVnum() != 0;
}

DWORD CHARACTER::GetSummonVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwSummonVnum : 0;
}

DWORD CHARACTER::GetPolymorphItemVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwPolymorphItemVnum : 0;
}

DWORD CHARACTER::GetMonsterDrainSPPoint() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDrainSP : 0;
}

BYTE CHARACTER::GetMobRank() const
{
	if (!m_pkMobData)
		return MOB_RANK_KNIGHT;	// PC일 경우 KNIGHT급

	return m_pkMobData->m_table.bRank;
}

BYTE CHARACTER::GetMobSize() const
{
	if (!m_pkMobData)
		return MOBSIZE_MEDIUM;

	return m_pkMobData->m_table.bSize;
}

WORD CHARACTER::GetMobAttackRange() const
{ 
	switch (GetMobBattleType())
	{
		case BATTLE_TYPE_RANGE:
		case BATTLE_TYPE_MAGIC:
			return m_pkMobData->m_table.wAttackRange + GetPoint(POINT_BOW_DISTANCE);  
		default:
			return m_pkMobData->m_table.wAttackRange; 
	}
}

BYTE CHARACTER::GetMobBattleType() const
{
	if (!m_pkMobData)
		return BATTLE_TYPE_MELEE;

	return (m_pkMobData->m_table.bBattleType);
}

void CHARACTER::ComputeBattlePoints()
{
	if (IsPolymorphed())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iAtt = 0;
		int iDef = 0;

		if (pMob)
		{
			iAtt = GetLevel() * 2 + GetPolymorphPoint(POINT_ST) * 2;
			// lev + con
			iDef = GetLevel() + GetPolymorphPoint(POINT_HT) + pMob->m_table.wDef;
		}

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE)); 
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
	else if (IsPC()
#ifdef ENABLE_CLONE_SYSTEM
		|| IsDito()
#endif
#ifdef __FAKE_PLAYER__
		|| IsFakePlayer()
 #endif
	)
	{
		SetPoint(POINT_ATT_GRADE, 0);
		SetPoint(POINT_DEF_GRADE, 0);
		SetPoint(POINT_CLIENT_DEF_GRADE, 0);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));

		//
		// 기본 ATK = 2lev + 2str, 직업에 마다 2str은 바뀔 수 있음
		//
		int iAtk = GetLevel() * 2;
		int iStatAtk = 0;

		switch (GetJob())
		{
			case JOB_WARRIOR:
			case JOB_SURA:
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;

			case JOB_ASSASSIN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_DX)) / 3;
				break;

			case JOB_SHAMAN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_IQ)) / 3;
				break;

			default:
				sys_err("invalid job %d", GetJob());
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;
		}

		// 말을 타고 있고, 스탯으로 인한 공격력이 ST*2 보다 낮으면 ST*2로 한다.
		// 스탯을 잘못 찍은 사람 공격력이 더 낮지 않게 하기 위해서다.
		if (GetMountVnum() && iStatAtk < 2 * GetPoint(POINT_ST))
			iStatAtk = (2 * GetPoint(POINT_ST));

		iAtk += iStatAtk;

		// 승마(말) : 검수라 데미지 감소  
		if (GetMountVnum())
		{
			if (GetJob() == JOB_SURA && GetSkillGroup() == 1)
			{
				iAtk += (iAtk * GetHorseLevel()) / 60;
			}
			else
			{
				iAtk += (iAtk * GetHorseLevel()) / 30;
			}
		}
		
		//
		// ATK Setting
		//
		iAtk += GetPoint(POINT_ATT_GRADE_BONUS);

		PointChange(POINT_ATT_GRADE, iAtk);

		// DEF = LEV + CON + ARMOR
		int iShowDef = GetLevel() + GetPoint(POINT_HT); // For Ymir(천마)
		int iDef = GetLevel() + (int) (GetPoint(POINT_HT) / 1.25); // For Other
		int iArmor = 0;

		LPITEM pkItem;

		for (int i = 0; i < WEAR_MAX_NUM; ++i)
			if ((pkItem = GetWear(i)) && pkItem->GetType() == ITEM_ARMOR)
			{
				if (pkItem->GetSubType() == ARMOR_BODY || pkItem->GetSubType() == ARMOR_HEAD || pkItem->GetSubType() == ARMOR_FOOTS || pkItem->GetSubType() == ARMOR_SHIELD)
				{
					iArmor += pkItem->GetValue(1);
					iArmor += (2 * pkItem->GetValue(5));
				}
			}
#ifdef __AURA_SYSTEM__
			else if (pkItem && pkItem->GetType() == ITEM_COSTUME && pkItem->GetSubType() == COSTUME_AURA)
			{
				const long c_lLevelSocket = pkItem->GetSocket(ITEM_SOCKET_AURA_CURRENT_LEVEL);
				const long c_lDrainSocket = pkItem->GetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM);
				const long c_lBoostSocket = pkItem->GetSocket(ITEM_SOCKET_AURA_BOOST);

				BYTE bCurLevel = (c_lLevelSocket / 100000) - 1000;
				BYTE bBoostIndex = c_lBoostSocket / 100000000;

				TItemTable* pBoosterProto = ITEM_MANAGER::instance().GetTable(ITEM_AURA_BOOST_ITEM_VNUM_BASE + bBoostIndex);
				float fAuraDrainPer = (1.0f * bCurLevel / 10.0f) / 100.0f;
				if (pBoosterProto)
					fAuraDrainPer += 1.0f * pBoosterProto->alValues[ITEM_AURA_BOOST_PERCENT_VALUE] / 100.0f;

				TItemTable* pDrainedItem = NULL;
				if (c_lDrainSocket != 0)
					pDrainedItem = ITEM_MANAGER::instance().GetTable(c_lDrainSocket);
				if (pDrainedItem != NULL && pDrainedItem->bType == ITEM_ARMOR && pDrainedItem->bSubType == ARMOR_SHIELD)
				{
					float fValue = (pDrainedItem->alValues[1] + (2 * pDrainedItem->alValues[5])) * fAuraDrainPer;
					iArmor += static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));;
				}
			}
#endif

		// 말 타고 있을 때 방어력이 말의 기준 방어력보다 낮으면 기준 방어력으로 설정
		if( true == IsHorseRiding() )
		{
			if (iArmor < GetHorseArmor())
				iArmor = GetHorseArmor();

			const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());

			if (pHorseName != NULL && strlen(pHorseName))
			{
				iArmor += 20;
			}
		}

		iArmor += GetPoint(POINT_DEF_GRADE_BONUS);
		iArmor += GetPoint(POINT_PARTY_DEFENDER_BONUS);

		// INTERNATIONAL_VERSION
		if (LC_IsYMIR())
		{
			PointChange(POINT_DEF_GRADE, iShowDef + iArmor);
		}
		else
		{
			PointChange(POINT_DEF_GRADE, iDef + iArmor);
			PointChange(POINT_CLIENT_DEF_GRADE, (iShowDef + iArmor) - GetPoint(POINT_DEF_GRADE));
		}
		// END_OF_INTERNATIONAL_VERSION

		PointChange(POINT_MAGIC_ATT_GRADE, GetLevel() * 2 + GetPoint(POINT_IQ) * 2 + GetPoint(POINT_MAGIC_ATT_GRADE_BONUS));
		PointChange(POINT_MAGIC_DEF_GRADE, GetLevel() + (GetPoint(POINT_IQ) * 3 + GetPoint(POINT_HT)) / 3 + iArmor / 2 + GetPoint(POINT_MAGIC_DEF_GRADE_BONUS));
	}
	else
	{
		// 2lev + str * 2
		int iAtt = GetLevel() * 2 + GetPoint(POINT_ST) * 2;
		// lev + con
		int iDef = GetLevel() + GetPoint(POINT_HT) + GetMobTable().wDef;

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE)); 
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
}

void CHARACTER::ComputePoints()
{
	long lStat = GetPoint(POINT_STAT);
	long lStatResetCount = GetPoint(POINT_STAT_RESET_COUNT);
	long lSkillActive = GetPoint(POINT_SKILL);
	long lSkillSub = GetPoint(POINT_SUB_SKILL);
	long lSkillHorse = GetPoint(POINT_HORSE_SKILL);
	long lLevelStep = GetPoint(POINT_LEVEL_STEP);

	long lAttackerBonus = GetPoint(POINT_PARTY_ATTACKER_BONUS);
	long lTankerBonus = GetPoint(POINT_PARTY_TANKER_BONUS);
	long lBufferBonus = GetPoint(POINT_PARTY_BUFFER_BONUS);
	long lSkillMasterBonus = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
	long lHasteBonus = GetPoint(POINT_PARTY_HASTE_BONUS);
	long lDefenderBonus = GetPoint(POINT_PARTY_DEFENDER_BONUS);

	long lHPRecovery = GetPoint(POINT_HP_RECOVERY);
	long lSPRecovery = GetPoint(POINT_SP_RECOVERY);

	memset(m_pointsInstant.points, 0, sizeof(m_pointsInstant.points));
	BuffOnAttr_ClearAll();
	m_SkillDamageBonus.clear();

	SetPoint(POINT_STAT, lStat);
	SetPoint(POINT_SKILL, lSkillActive);
	SetPoint(POINT_SUB_SKILL, lSkillSub);
	SetPoint(POINT_HORSE_SKILL, lSkillHorse);
	SetPoint(POINT_LEVEL_STEP, lLevelStep);
	SetPoint(POINT_STAT_RESET_COUNT, lStatResetCount);

	SetPoint(POINT_ST, GetRealPoint(POINT_ST));
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetPart(PART_MAIN, GetOriginalPart(PART_MAIN));
	SetPart(PART_WEAPON, GetOriginalPart(PART_WEAPON));
	SetPart(PART_HEAD, GetOriginalPart(PART_HEAD));
	SetPart(PART_HAIR, GetOriginalPart(PART_HAIR));
#ifdef __SASH_SYSTEM__
	SetPart(PART_SASH, GetOriginalPart(PART_SASH));
#endif
#ifdef __AURA_SYSTEM__
	SetPart(PART_AURA, GetOriginalPart(PART_AURA));
#endif

	SetPoint(POINT_PARTY_ATTACKER_BONUS, lAttackerBonus);
	SetPoint(POINT_PARTY_TANKER_BONUS, lTankerBonus);
	SetPoint(POINT_PARTY_BUFFER_BONUS, lBufferBonus);
	SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, lSkillMasterBonus);
	SetPoint(POINT_PARTY_HASTE_BONUS, lHasteBonus);
	SetPoint(POINT_PARTY_DEFENDER_BONUS, lDefenderBonus);

	SetPoint(POINT_HP_RECOVERY, lHPRecovery);
	SetPoint(POINT_SP_RECOVERY, lSPRecovery);

	// PC_BANG_ITEM_ADD
	SetPoint(POINT_PC_BANG_EXP_BONUS, 0);
	SetPoint(POINT_PC_BANG_DROP_BONUS, 0);
	// END_PC_BANG_ITEM_ADD

	int iMaxHP, iMaxSP;
	int iMaxStamina;

	if (IsPC()
#ifdef ENABLE_CLONE_SYSTEM
		|| IsDito()
#endif
#ifdef __FAKE_PLAYER__
		|| IsFakePlayer()
 #endif
	)
	{
		// 최대 생명력/정신력
		iMaxHP = JobInitialPoints[GetJob()].max_hp + m_points.iRandomHP + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].hp_per_ht;
		iMaxSP = JobInitialPoints[GetJob()].max_sp + m_points.iRandomSP + GetPoint(POINT_IQ) * JobInitialPoints[GetJob()].sp_per_iq;
		iMaxStamina = JobInitialPoints[GetJob()].max_stamina + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].stamina_per_con;

		{
			CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_ADD_HP);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_ADD_HP) / 100.0f);

				iMaxHP += static_cast<int>(pkSk->kPointPoly.Eval());
			}
		}

		// 기본 값들
		SetPoint(POINT_MOV_SPEED,	200);
		SetPoint(POINT_ATT_SPEED,	100);
		PointChange(POINT_ATT_SPEED, GetPoint(POINT_PARTY_HASTE_BONUS));
		SetPoint(POINT_CASTING_SPEED,	100);
#ifdef __GEM_SYSTEM__
		SetPoint(POINT_GEM, GetGem());
#endif
	}
	else
	{
		iMaxHP = m_pkMobData->m_table.dwMaxHP;
		iMaxSP = 0;
		iMaxStamina = 0;

		SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		SetPoint(POINT_MOV_SPEED, m_pkMobData->m_table.sMovingSpeed);
		SetPoint(POINT_CASTING_SPEED, m_pkMobData->m_table.sAttackSpeed);
	}

	if (IsPC()
#ifdef ENABLE_CLONE_SYSTEM
	|| IsDito()
#endif
	)
	{
		// 말 타고 있을 때는 기본 스탯이 말의 기준 스탯보다 낮으면 높게 만든다.
		// 따라서 말의 기준 스탯이 무사 기준이므로, 수라/무당은 전체 스탯 합이
		// 대채적으로 더 올라가게 될 것이다.
		if (GetMountVnum()) 
		{
			if (GetHorseST() > GetPoint(POINT_ST))
				PointChange(POINT_ST, GetHorseST() - GetPoint(POINT_ST));

			if (GetHorseDX() > GetPoint(POINT_DX))
				PointChange(POINT_DX, GetHorseDX() - GetPoint(POINT_DX));

			if (GetHorseHT() > GetPoint(POINT_HT))
				PointChange(POINT_HT, GetHorseHT() - GetPoint(POINT_HT));

			if (GetHorseIQ() > GetPoint(POINT_IQ))
				PointChange(POINT_IQ, GetHorseIQ() - GetPoint(POINT_IQ));
		}

	}

	ComputeBattlePoints();

	// 기본 HP/SP 설정
	if (iMaxHP != GetMaxHP())
	{
		SetRealPoint(POINT_MAX_HP, iMaxHP); // 기본HP를 RealPoint에 저장해 놓는다.
	}

	PointChange(POINT_MAX_HP, 0);

	if (iMaxSP != GetMaxSP())
	{
		SetRealPoint(POINT_MAX_SP, iMaxSP); // 기본SP를 RealPoint에 저장해 놓는다.
	}

	PointChange(POINT_MAX_SP, 0);

	SetMaxStamina(iMaxStamina);

	m_pointsInstant.dwImmuneFlag = 0;

	for (int i = 0 ; i < WEAR_MAX_NUM; i++) 
	{
		LPITEM pItem = GetWear(i);
		if (pItem)
		{
			pItem->ModifyPoints(true);
			SET_BIT(m_pointsInstant.dwImmuneFlag, GetWear(i)->GetImmuneFlag());
		}
	}

	// 용혼석 시스템
	// ComputePoints에서는 케릭터의 모든 속성값을 초기화하고,
	// 아이템, 버프 등에 관련된 모든 속성값을 재계산하기 때문에,
	// 용혼석 시스템도 ActiveDeck에 있는 모든 용혼석의 속성값을 다시 적용시켜야 한다.
#ifdef ENABLE_EVENT_MANAGER
		CHARACTER_MANAGER::Instance().CheckBonusEvent(this);
#endif
	if (DragonSoul_IsDeckActivated())
	{
		for (int i = WEAR_MAX_NUM + DS_SLOT_MAX * DragonSoul_GetActiveDeck(); 
			i < WEAR_MAX_NUM + DS_SLOT_MAX * (DragonSoul_GetActiveDeck() + 1); i++)	
		{
			LPITEM pItem = GetWear(i);
			if (pItem)
			{
				if (DSManager::instance().IsTimeLeftDragonSoul(pItem))
					pItem->ModifyPoints(true);
			}
		}
	}

	ComputeSkillPoints();

	RefreshAffect();
	// TP BUG FIX		
	CPetSystem* pPetSystem = GetPetSystem();
	if (NULL != pPetSystem)
	{
		pPetSystem->RefreshBuff();
	}

	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		it->second->GiveAllAttributes();
	}
	
	if (GetHP() > GetMaxHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());
	if (GetSP() > GetMaxSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());

	UpdatePacket();
}

// m_dwPlayStartTime의 단위는 milisecond다. 데이터베이스에는 분단위로 기록하기
// 때문에 플레이시간을 계산할 때 / 60000 으로 나눠서 하는데, 그 나머지 값이 남았
// 을 때 여기에 dwTimeRemain으로 넣어서 제대로 계산되도록 해주어야 한다.
void CHARACTER::ResetPlayTime(DWORD dwTimeRemain)
{
	m_dwPlayStartTime = get_dword_time() - dwTimeRemain;
}

const int aiRecoveryPercents[10] = { 1, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

EVENTFUNC(recovery_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "recovery_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}	

	if (!ch->IsPC()
#ifdef __FAKE_PLAYER__
		&& !ch->IsFakePlayer()
#endif			
	)
	{
		//
		// 몬스터 회복
		//
		if (ch->IsAffectFlag(AFF_POISON))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));

		if (2493 == ch->GetMobTable().dwVnum)
		{
			int regenPct = BlueDragon_GetRangeFactor("hp_regen", ch->GetHPPct());
			regenPct += ch->GetMobTable().bRegenPercent;

			for (int i=1 ; i <= 4 ; ++i)
			{
				if (REGEN_PECT_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					regenPct += (val*cnt);

					break;
				}
			}

			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * regenPct) / 100));
		}
		else if (!ch->IsDoor())
		{
			ch->MonsterLog("HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
		}

		if (ch->GetHP() >= ch->GetMaxHP())
		{
			ch->m_pkRecoveryEvent = NULL;
			return 0;
		}

		if (2493 == ch->GetMobTable().dwVnum)
		{
			for (int i=1 ; i <= 4 ; ++i)
			{
				if (REGEN_TIME_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					return PASSES_PER_SEC(MAX(1, (ch->GetMobTable().bRegenCycle - (val*cnt))));
				}
			}
		}

		return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
	}
	else
	{
		//
		// PC 회복
		//
		ch->CheckTarget();
		//ch->UpdateSectree(); // 여기서 이걸 왜하지?
		ch->UpdateKillerMode();

		if (ch->IsAffectFlag(AFF_POISON) == true)
		{
			// 중독인 경우 자동회복 금지
			// 파법술인 경우 자동회복 금지
			return 3;
		}

		int iSec = (get_dword_time() - ch->GetLastMoveTime()) / 3000;

		// SP 회복 루틴.
		// 왜 이걸로 해서 함수로 빼놨는가 ?!
		ch->DistributeSP(ch);

		if (ch->GetMaxHP() <= ch->GetHP())
			return PASSES_PER_SEC(3);

		int iPercent = 0;
		int iAmount = 0;
		
		{
			iPercent = aiRecoveryPercents[MIN(9, iSec)];
			iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
		}
		
		iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;

		sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%d", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);

		ch->PointChange(POINT_HP, iAmount, false);
		return PASSES_PER_SEC(3);
	}
}

void CHARACTER::StartRecoveryEvent()
{
	if (m_pkRecoveryEvent)
		return;

	if (IsDead() || IsStun())
		return;

	if (IsNPC() && GetHP() >= GetMaxHP()) // 몬스터는 체력이 다 차있으면 시작 안한다.
		return;
		
	#ifdef __MELEY_LAIR_DUNGEON__
	if ((MeleyLair::CMgr::instance().IsMeleyMap(GetMapIndex())) && ((GetRaceNum() == (WORD)(MeleyLair::STATUE_VNUM)) || ((GetRaceNum() == (WORD)(MeleyLair::BOSS_VNUM)))))
		return;
	#endif

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;
	
	int iSec = 0;
	if (IsPC()
#ifdef __FAKE_PLAYER__
		|| IsFakePlayer()
#endif		
	)
		iSec = 3; 
	else
		iSec = (MAX(1, GetMobTable().bRegenCycle));

	m_pkRecoveryEvent = event_create(recovery_event, info, PASSES_PER_SEC(iSec));
}

void CHARACTER::Standup()
{
	struct packet_position pack_position;

	if (!IsPosition(POS_SITTING))
		return;

	SetPosition(POS_STANDING);

	sys_log(1, "STANDUP: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_GENERAL;

	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::Sitdown(int is_ground)
{
	struct packet_position pack_position;

	if (IsPosition(POS_SITTING))
		return;

	SetPosition(POS_SITTING);
	sys_log(1, "SITDOWN: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_SITTING_GROUND;
	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::SetRotation(float fRot)
{
	m_pointsInstant.fRot = fRot;
}

// x, y 방향으로 보고 선다.
void CHARACTER::SetRotationToXY(long x, long y)
{
	SetRotation(GetDegreeFromPositionXY(GetX(), GetY(), x, y));
}

bool CHARACTER::CannotMoveByAffect() const
{
	return (IsAffectFlag(AFF_STUN));
}

bool CHARACTER::CanMove() const
{
	if (CannotMoveByAffect())
		return false;
	// 0.2초 전이라면 움직일 수 없다.
	/*
	   if (get_float_time() - m_fSyncTime < 0.2f)
	   return false;
	 */
	return true;
}

// 무조건 x, y 위치로 이동 시킨다.
bool CHARACTER::Sync(long x, long y)
{
	if (!GetSectree())
		return false;

	LPSECTREE new_tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), x, y);

	if (!new_tree)
	{
		if (GetDesc())
		{
			sys_err("cannot find tree at %d %d (name: %s)", x, y, GetName());
			x = GetX();
			y = GetY();
			new_tree = GetSectree();
		}
		else
		{
			sys_err("no tree: %s %d %d %d", GetName(), x, y, GetMapIndex());
			Dead();
		}

		return false;
	}
	
#ifdef ENABLE_CHECK_MOVESPEED_HACK
	if (IsPC())
	{
		if (GetCShield()->CheckMoveSpeedhack(x, y, get_dword_time(), IsRiding(), GetMoveSpeed()))
		{
			LogManager::instance().CShieldLog(GetAID(), GetName(), 10001);
			Show(GetMapIndex(), GetX(), GetY(), 0);
			Stop();
		}
	}
#endif
	
	SetRotationToXY(x, y);
	SetXYZ(x, y, 0);

	if (GetDungeon())
	{
		// 던젼용 이벤트 속성 변화
		int iLastEventAttr = m_iEventAttr;
		m_iEventAttr = new_tree->GetEventAttribute(x, y);

		if (m_iEventAttr != iLastEventAttr)
		{
			if (GetParty())
			{
				quest::CQuestManager::instance().AttrOut(GetParty()->GetLeaderPID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetParty()->GetLeaderPID(), this, m_iEventAttr);
			}
			else
			{
				quest::CQuestManager::instance().AttrOut(GetPlayerID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetPlayerID(), this, m_iEventAttr);
			}
		}
	}

	if (GetSectree() != new_tree)
	{
		if (!IsNPC())
		{
			SECTREEID id = new_tree->GetID();
			SECTREEID old_id = GetSectree()->GetID();

			sys_log(0, "SECTREE DIFFER: %s %dx%d was %dx%d",
					GetName(),
					id.coord.x,
					id.coord.y,
					old_id.coord.x,
					old_id.coord.y);
		}

		new_tree->InsertEntity(this);
	}

	return true;
}

void CHARACTER::Stop()
{
	if (!IsState(m_stateIdle))
		MonsterLog("[IDLE] 정지");

	GotoState(m_stateIdle);

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
}

bool CHARACTER::Goto(long x, long y)
{
	// TODO 거리체크 필요
	// 같은 위치면 이동할 필요 없음 (자동 성공)
	if (GetX() == x && GetY() == y)
		return false;

	if (m_posDest.x == x && m_posDest.y == y)
	{
		if (!IsState(m_stateMove))
		{
			m_dwStateDuration = 4;
			GotoState(m_stateMove);
		}
		return false;
	}

	m_posDest.x = x;
	m_posDest.y = y;

	CalculateMoveDuration();

	m_dwStateDuration = 4;

	
	if (!IsState(m_stateMove))
	{
		MonsterLog("[MOVE] %s", GetVictim() ? "대상추적" : "그냥이동");

		if (GetVictim())
		{
			//MonsterChat(MONSTER_CHAT_CHASE);
			//MonsterChat(MONSTER_CHAT_ATTACK);
		}
	}

	GotoState(m_stateMove);

	return true;
}


DWORD CHARACTER::GetMotionMode() const
{
	DWORD dwMode = MOTION_MODE_GENERAL;

	if (IsPolymorphed())
		return dwMode;

	LPITEM pkItem;

	if ((pkItem = GetWear(WEAR_WEAPON)))
	{
		switch (pkItem->GetProto()->bSubType)
		{
			case WEAPON_SWORD:
				dwMode = MOTION_MODE_ONEHAND_SWORD;
				break;

			case WEAPON_TWO_HANDED:
				dwMode = MOTION_MODE_TWOHAND_SWORD;
				break;

			case WEAPON_DAGGER:
				dwMode = MOTION_MODE_DUALHAND_SWORD;
				break;

			case WEAPON_BOW:
				dwMode = MOTION_MODE_BOW;
				break;

			case WEAPON_BELL:
				dwMode = MOTION_MODE_BELL;
				break;

			case WEAPON_FAN:
				dwMode = MOTION_MODE_FAN;
				break;
		}
	}
	return dwMode;
}

float CHARACTER::GetMoveMotionSpeed() const
{
	DWORD dwMode = GetMotionMode();

	const CMotion * pkMotion = NULL;

	if (!GetMountVnum())
		pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(dwMode, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	else
	{
		pkMotion = CMotionManager::instance().GetMotion(GetMountVnum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));

		if (!pkMotion)
			pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_HORSE, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	}

	if (pkMotion)
		return -pkMotion->GetAccumVector().y / pkMotion->GetDuration();
	else
	{
		sys_err("cannot find motion (name %s race %d mode %d)", GetName(), GetRaceNum(), dwMode);
		return 300.0f;
	}
}

float CHARACTER::GetMoveSpeed() const
{
	return GetMoveMotionSpeed() * 10000 / CalculateDuration(GetLimitPoint(POINT_MOV_SPEED), 10000);
}

void CHARACTER::CalculateMoveDuration()
{
	m_posStart.x = GetX();
	m_posStart.y = GetY();

	float fDist = DISTANCE_SQRT(m_posStart.x - m_posDest.x, m_posStart.y - m_posDest.y);

	float motionSpeed = GetMoveMotionSpeed();

	m_dwMoveDuration = CalculateDuration(GetLimitPoint(POINT_MOV_SPEED),
			(int) ((fDist / motionSpeed) * 1000.0f));

	if (IsNPC())
		sys_log(1, "%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",
				GetName(), fDist, GetLimitPoint(POINT_MOV_SPEED), m_dwMoveDuration, motionSpeed,
				m_posStart.x, m_posStart.y, m_posDest.x, m_posDest.y);

	m_dwMoveStartTime = get_dword_time();
}

// x y 위치로 이동 한다. (이동할 수 있는 가 없는 가를 확인 하고 Sync 메소드로 실제 이동 한다)
// 서버는 char의 x, y 값을 바로 바꾸지만,
// 클라에서는 이전 위치에서 바꾼 x, y까지 interpolation한다.
// 걷거나 뛰는 것은 char의 m_bNowWalking에 달려있다.
// Warp를 의도한 것이라면 Show를 사용할 것.
bool CHARACTER::Move(long x, long y)
{
	// 같은 위치면 이동할 필요 없음 (자동 성공)
	if (GetX() == x && GetY() == y)
		return true;
		
	if (x < 0 || y < 0) {
		sys_err("Got negative value in MOVE: x=%d, y=%d", x, y);
		return false;
	}

	if (test_server)
		if (m_bDetailLog)
			sys_log(0, "%s position %u %u", GetName(), x, y);

	OnMove();
	return Sync(x, y);
}

void CHARACTER::SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, int iRot)
{
	TPacketGCMove pack;

	if (bFunc == FUNC_WAIT)
	{
		x = m_posDest.x;
		y = m_posDest.y;
		dwDuration = m_dwMoveDuration;
	}

	EncodeMovePacket(pack, GetVID(), bFunc, bArg, x, y, dwDuration, dwTime, iRot == -1 ? (int) GetRotation() / 5 : iRot);
	PacketView(&pack, sizeof(TPacketGCMove), this);
}

int CHARACTER::GetRealPoint(BYTE type) const
{
	return m_points.points[type];
}

void CHARACTER::SetRealPoint(BYTE type, int val)
{
	m_points.points[type] = val;
}

int CHARACTER::GetPolymorphPoint(BYTE type) const
{
	if (IsPolymorphed() && !IsPolyMaintainStat())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iPower = GetPolymorphPower();

		if (pMob)
		{
			switch (type)
			{
				case POINT_ST:
					if (GetJob() == JOB_SHAMAN || GetJob() == JOB_SURA && GetSkillGroup() == 2)
						return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_IQ);
					return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_ST);

				case POINT_HT:
					return pMob->m_table.bCon * iPower / 100 + GetPoint(POINT_HT);

				case POINT_IQ:
					return pMob->m_table.bInt * iPower / 100 + GetPoint(POINT_IQ);

				case POINT_DX:
					return pMob->m_table.bDex * iPower / 100 + GetPoint(POINT_DX);
			}
		}
	}

	return GetPoint(type);
}

long long CHARACTER::GetPoint(BYTE type) const
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	long long val = m_pointsInstant.points[type];
	long long max_val = LLONG_MAX;

	switch (type)
	{
		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			max_val = 50;
			break;
		case POINT_GOLD:
			max_val = GOLD_MAX;
			break;		
	}

	if (val > max_val)
		sys_err("POINT_ERROR: %s type %d val %lld (max: %lld)", GetName(), val, max_val);

	return (val);
}

int CHARACTER::GetLimitPoint(BYTE type) const
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;
	int limit = INT_MAX;
	int min_limit = -INT_MAX;

	switch (type)
	{
		case POINT_ATT_SPEED:
			min_limit = 0;

			if (IsPC())
				limit = 170;
			else
				limit = 250;
			break;

		case POINT_MOV_SPEED:
			min_limit = 0;

			if (IsPC())
				limit = 200;
			else
				limit = 250;
			break;

		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			limit = 50;
			max_val = 50;
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
			limit = 20;
			max_val = 50;
			break;
	}

	if (val > max_val)
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);

	if (val > limit)
		val = limit;

	if (val < min_limit)
		val = min_limit;

	return (val);
}

void CHARACTER::SetPoint(BYTE type, long long val)
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return;
	}

	m_pointsInstant.points[type] = val;

	// 아직 이동이 다 안끝났다면 이동 시간 계산을 다시 해야 한다.
	if (type == POINT_MOV_SPEED && get_dword_time() < m_dwMoveStartTime + m_dwMoveDuration)
	{
		CalculateMoveDuration();
	}
}

long long CHARACTER::GetAllowedGold() const
{
	if (GetLevel() <= 10)
		return 100000;
	else if (GetLevel() <= 20)
		return 500000;
	else
		return 50000000;
}

void CHARACTER::CheckMaximumPoints()
{
	if (GetMaxHP() < GetHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetMaxSP() < GetSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());
}

void CHARACTER::PointChange(BYTE type, long long amount, bool bAmount, bool bBroadcast)
{
#ifdef ENABLE_CLONE_SYSTEM
	if (IsDito())
		return;
#endif
	long long val = 0;

	//sys_log(0, "PointChange %d %d | %d -> %d cHP %d mHP %d", type, amount, GetPoint(type), GetPoint(type)+amount, GetHP(), GetMaxHP());

	switch (type)
	{
		case POINT_NONE:
			return;

		case POINT_LEVEL:
			if ((GetLevel() + amount) > gPlayerMaxLevel)
				return;

			SetLevel(GetLevel() + amount);
			val = GetLevel();

			sys_log(0, "LEVELUP: %s %d NEXT EXP %d", GetName(), GetLevel(), GetNextExp());

			PointChange(POINT_NEXT_EXP,	GetNextExp(), false);

			if (amount)
			{
				quest::CQuestManager::instance().LevelUp(GetPlayerID());
#ifdef ENABLE_BIYOLOG
			CheckBio();
#endif

				LogManager::instance().LevelLog(this, val, GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000);

				if (GetGuild())
				{
					GetGuild()->LevelChange(GetPlayerID(), GetLevel());
				}
				// if (val == 5)
				// {
				// SetSkillLevel(143,20);
				// SetSkillLevel(144,20);
				// SetSkillLevel(145,20);
				// SetSkillLevel(146,20);
				// SetSkillLevel(147,20);
				// SkillLevelPacket();
				// }

				if (GetParty())
				{
					GetParty()->RequestSetMemberLevel(GetPlayerID(), GetLevel());
				}
			}
			break;

		case POINT_NEXT_EXP:
			val = GetNextExp();
			bAmount = false;	// 무조건 bAmount는 false 여야 한다.
			break;

		case POINT_EXP:
			{
				DWORD exp = GetExp();
				DWORD next_exp = GetNextExp();

				// 청소년보호
				if (LC_IsNewCIBN())
				{
					if (IsOverTime(OT_NONE))
					{
						dev_log(LOG_DEB0, "<EXP_LOG> %s = NONE", GetName());
					}
					else if (IsOverTime(OT_3HOUR))
					{
						amount = (amount / 2);
						dev_log(LOG_DEB0, "<EXP_LOG> %s = 3HOUR", GetName());
					}
					else if (IsOverTime(OT_5HOUR))
					{
						amount = 0;
						dev_log(LOG_DEB0, "<EXP_LOG> %s = 5HOUR", GetName());
					}
				}

				// exp가 0 이하로 가지 않도록 한다
				if (amount < 0 && exp < -amount)
				{
					sys_log(1, "%s AMOUNT < 0 %d, CUR EXP: %d", GetName(), -amount, exp);
					amount = -exp;

					SetExp(exp + amount);
					val = GetExp();
				}
				else
				{
					if (gPlayerMaxLevel <= GetLevel())
						return;

					if (block_exp && amount > 0)
					{
						return;
					} 

					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "You have gained %d exp.", amount);

					DWORD iExpBalance = 0;

					// 레벨 업!
					if (exp + amount >= next_exp)
					{
						iExpBalance = (exp + amount) - next_exp;
						amount = next_exp - exp;

						SetExp(0);
						exp = next_exp;
					}
					else
					{
						SetExp(exp + amount);
						exp = GetExp();
					}

					DWORD q = DWORD(next_exp / 4.0f);
					int iLevStep = GetRealPoint(POINT_LEVEL_STEP);

					// iLevStep이 4 이상이면 레벨이 올랐어야 하므로 여기에 올 수 없는 값이다.
					if (iLevStep >= 4)
					{
						sys_err("%s LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
						iLevStep = 4;
					}

					if (exp >= next_exp && iLevStep < 4)
					{
						for (int i = 0; i < 4 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 3 && iLevStep < 3)
					{
						for (int i = 0; i < 3 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 2 && iLevStep < 2)
					{
						for (int i = 0; i < 2 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q && iLevStep < 1)
						PointChange(POINT_LEVEL_STEP, 1);

					if (iExpBalance)
					{
						PointChange(POINT_EXP, iExpBalance);
					}

					val = GetExp();
				}
			}
			break;

		case POINT_LEVEL_STEP:
			if (amount > 0)
			{
				val = GetPoint(POINT_LEVEL_STEP) + amount;

				switch (val)
				{
					case 1:
					case 2:
					case 3:
						//if (GetLevel() < 100) PointChange(POINT_STAT, 1);
						if (GetLevel() < 91) PointChange(POINT_STAT, 1);
						break;

					case 4:
						{
							int iHP = number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end);
							int iSP = number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end);

							m_points.iRandomHP += iHP;
							m_points.iRandomSP += iSP;

							if (GetSkillGroup())
							{
								if (GetLevel() >= 5)
									PointChange(POINT_SKILL, 1);

								if (GetLevel() >= 9)
									PointChange(POINT_SUB_SKILL, 1);
							}

							PointChange(POINT_MAX_HP, iHP);
							PointChange(POINT_MAX_SP, iSP);
							PointChange(POINT_LEVEL, 1, false, true);

							val = 0;
						}
						break;
				}

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());

				SetPoint(POINT_LEVEL_STEP, val);
				SetRealPoint(POINT_LEVEL_STEP, val);

				Save();
			}
			else
				val = GetPoint(POINT_LEVEL_STEP);

			break;

		case POINT_HP:
			{
				if (IsDead() || IsStun())
					return;

				int prev_hp = GetHP();

				amount = MIN(GetMaxHP() - GetHP(), amount);
				SetHP(GetHP() + amount);
				val = GetHP();

				BroadcastTargetPacket();

				if (GetParty() && IsPC() && val != prev_hp)
					GetParty()->SendPartyInfoOneToAll(this);
			}
			break;

		case POINT_SP:
			{
				if (IsDead() || IsStun())
					return;

				amount = MIN(GetMaxSP() - GetSP(), amount);
				SetSP(GetSP() + amount);
				val = GetSP();
			}
			break;

		case POINT_STAMINA:
			{
				if (IsDead() || IsStun())
					return;

				int prev_val = GetStamina();
				amount = MIN(GetMaxStamina() - GetStamina(), amount);
				SetStamina(GetStamina() + amount);
				val = GetStamina();
				
				if (val == 0)
				{
					// Stamina가 없으니 걷자!
					SetNowWalking(true);
				}
				else if (prev_val == 0)
				{
					// 없던 스테미나가 생겼으니 이전 모드 복귀
					ResetWalking();
				}

				if (amount < 0 && val != 0) // 감소는 보내지않는다.
					return;
			}
			break;

		case POINT_MAX_HP:
			{
				SetPoint(type, GetPoint(type) + amount);

				//SetMaxHP(GetMaxHP() + amount);
				// 최대 생명력 = (기본 최대 생명력 + 추가) * 최대생명력%
				int hp = GetRealPoint(POINT_MAX_HP);
				int add_hp = MIN(3500, hp * GetPoint(POINT_MAX_HP_PCT) / 100);
				add_hp += GetPoint(POINT_MAX_HP);
				add_hp += GetPoint(POINT_PARTY_TANKER_BONUS);

				SetMaxHP(hp + add_hp);

				val = GetMaxHP();
			}
			break;

		case POINT_MAX_SP:
			{
				SetPoint(type, GetPoint(type) + amount);

				//SetMaxSP(GetMaxSP() + amount);
				// 최대 정신력 = (기본 최대 정신력 + 추가) * 최대정신력%
				int sp = GetRealPoint(POINT_MAX_SP);
				int add_sp = MIN(800, sp * GetPoint(POINT_MAX_SP_PCT) / 100);
				add_sp += GetPoint(POINT_MAX_SP);
				add_sp += GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);

				SetMaxSP(sp + add_sp);

				val = GetMaxSP();
			}
			break;

		case POINT_MAX_HP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_HP, 0);
			break;

		case POINT_MAX_SP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_SP, 0);
			break;

		case POINT_MAX_STAMINA:
			SetMaxStamina(GetMaxStamina() + amount);
			val = GetMaxStamina();
			break;
			
#ifdef __GEM_SYSTEM__
	case POINT_GEM:
	{
		const int64_t nTotalGem = static_cast<int64_t>(GetGem()) + static_cast<long long>(amount);
		if (GEM_MAX <= nTotalGem)
		{
			sys_err("[OVERFLOW_GEM] OriGem %d AddedGem %lld id %u Name %s ", GetGem(), amount, GetPlayerID(), GetName());
			return;
		}
		SetGem(GetGem() + amount);
		val = GetGem();
	}
	break;
#endif

		case POINT_GOLD:
			{
				const unsigned long long nTotalMoney = static_cast<unsigned long long>(GetGold()) + static_cast<unsigned long long>(amount);

				if (GOLD_MAX <= nTotalMoney)
				{
					sys_err("[OVERFLOW_GOLD] OriGold %lld AddedGold %lld id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
					LogManager::instance().CharLog(this, GetGold() + amount, "OVERFLOW_GOLD", "");
					return;
				}

				// 청소년보호
				if (LC_IsNewCIBN() && amount > 0)
				{
					if (IsOverTime(OT_NONE))
					{
						dev_log(LOG_DEB0, "<GOLD_LOG> %s = NONE", GetName());
					}
					else if (IsOverTime(OT_3HOUR))
					{
						amount = (amount / 2);
						dev_log(LOG_DEB0, "<GOLD_LOG> %s = 3HOUR", GetName());
					}
					else if (IsOverTime(OT_5HOUR))
					{
						amount = 0;
						dev_log(LOG_DEB0, "<GOLD_LOG> %s = 5HOUR", GetName());
					}
				}

				SetGold(GetGold() + amount);
				val = GetGold();
			}
			break;

#ifdef ENABLE_EXTENDED_BATTLE_PASS
		case POINT_BATTLE_PASS_PREMIUM_ID:
			{
				SetExtBattlePassPremiumID(amount);
				val = GetExtBattlePassPremiumID();
			}
			break;
#endif
#ifdef ENABLE_ASLAN_LOTTERY
		case POINT_LOTTO_MONEY:
			{
				SetLottoMoney(GetLottoMoney() + amount);
				val = GetLottoMoney();
			}
			break;
		case POINT_LOTTO_TOTAL_MONEY:
			{
				SetLottoTotalMoney(GetLottoTotalMoney() + amount);
				val = GetLottoTotalMoney();
			}
			break;
#endif
		case POINT_SKILL:
		case POINT_STAT:
		case POINT_SUB_SKILL:
		case POINT_STAT_RESET_COUNT:
		case POINT_HORSE_SKILL:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			SetRealPoint(type, val);
			break;

		case POINT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_CLIENT_DEF_GRADE, amount);
			break;

		case POINT_CLIENT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_ST:
		case POINT_HT:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HP_REGEN:
		case POINT_SP_REGEN:
		case POINT_ATT_SPEED:
		case POINT_ATT_GRADE:
		case POINT_MOV_SPEED:
		case POINT_CASTING_SPEED:
		case POINT_MAGIC_ATT_GRADE:
		case POINT_MAGIC_DEF_GRADE:
		case POINT_BOW_DISTANCE:
		case POINT_HP_RECOVERY:
		case POINT_SP_RECOVERY:

		case POINT_ATTBONUS_HUMAN:	// 42 인간에게 강함
		case POINT_ATTBONUS_ANIMAL:	// 43 동물에게 데미지 % 증가
		case POINT_ATTBONUS_ORC:		// 44 웅귀에게 데미지 % 증가
		case POINT_ATTBONUS_MILGYO:	// 45 밀교에게 데미지 % 증가
		case POINT_ATTBONUS_UNDEAD:	// 46 시체에게 데미지 % 증가
		case POINT_ATTBONUS_DEVIL:	// 47 마귀(악마)에게 데미지 % 증가
#ifdef ENABLE_SHADOW_BONUS
		case POINT_ATTBONUS_SHADOW:
#endif
		case POINT_ATTBONUS_MONSTER:
		case POINT_ATTBONUS_SURA:
		case POINT_ATTBONUS_ASSASSIN:
		case POINT_ATTBONUS_WARRIOR:
		case POINT_ATTBONUS_SHAMAN:

		case POINT_POISON_PCT:
		case POINT_STUN_PCT:
		case POINT_SLOW_PCT:

		case POINT_BLOCK:
		case POINT_DODGE:

		case POINT_CRITICAL_PCT:
		case POINT_RESIST_CRITICAL:
		case POINT_PENETRATE_PCT:
		case POINT_RESIST_PENETRATE:
		case POINT_CURSE_PCT:

		case POINT_STEAL_HP:		// 48 생명력 흡수
		case POINT_STEAL_SP:		// 49 정신력 흡수

		case POINT_MANA_BURN_PCT:	// 50 마나 번
		case POINT_DAMAGE_SP_RECOVER:	// 51 공격당할 시 정신력 회복 확률
		case POINT_RESIST_NORMAL_DAMAGE:
		case POINT_RESIST_SWORD:
		case POINT_RESIST_TWOHAND:
		case POINT_RESIST_DAGGER:
		case POINT_RESIST_BELL: 
		case POINT_RESIST_FAN: 
		case POINT_RESIST_BOW:
		case POINT_RESIST_FIRE:
		case POINT_RESIST_ELEC:
		case POINT_RESIST_MAGIC:
		case POINT_RESIST_WIND:
		case POINT_RESIST_ICE:
		case POINT_RESIST_EARTH:
		case POINT_RESIST_DARK:
		case POINT_REFLECT_MELEE:	// 67 공격 반사
		case POINT_REFLECT_CURSE:	// 68 저주 반사
		case POINT_POISON_REDUCE:	// 69 독데미지 감소
		case POINT_KILL_SP_RECOVER:	// 70 적 소멸시 MP 회복
		case POINT_KILL_HP_RECOVERY:	// 75  
		case POINT_HIT_HP_RECOVERY:
		case POINT_HIT_SP_RECOVERY:
		case POINT_MANASHIELD:
		case POINT_ATT_BONUS:
		case POINT_DEF_BONUS:
		case POINT_SKILL_DAMAGE_BONUS:
		case POINT_NORMAL_HIT_DAMAGE_BONUS:

			// DEPEND_BONUS_ATTRIBUTES 
		case POINT_SKILL_DEFEND_BONUS:
		case POINT_NORMAL_HIT_DEFEND_BONUS:
#ifdef ELEMENT_NEW_BONUSES
		case POINT_ATTBONUS_ELEC:
		case POINT_ATTBONUS_FIRE:
		case POINT_ATTBONUS_ICE:
		case POINT_ATTBONUS_WIND:
		case POINT_ATTBONUS_EARTH:
		case POINT_ATTBONUS_DARK:
#endif
#ifdef ENABLE_NEW_ATTR
		case POINT_ATTBONUS_METIN:
#endif
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case POINT_PARTY_ATTACKER_BONUS:
		case POINT_PARTY_TANKER_BONUS:
		case POINT_PARTY_BUFFER_BONUS:
		case POINT_PARTY_SKILL_MASTER_BONUS:
		case POINT_PARTY_HASTE_BONUS:
		case POINT_PARTY_DEFENDER_BONUS:

		case POINT_RESIST_WARRIOR :
		case POINT_RESIST_ASSASSIN :
		case POINT_RESIST_SURA :
		case POINT_RESIST_SHAMAN :

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
		case POINT_MALL_EXPBONUS:
		case POINT_MALL_ITEMBONUS:
		case POINT_MALL_GOLDBONUS:
		case POINT_MELEE_MAGIC_ATT_BONUS_PER:
			if (GetPoint(type) + amount > 100)
			{
				sys_err("MALL_BONUS exceeded over 100!! point type: %d name: %s amount %d", type, GetName(), amount);
				amount = 100 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

			// PC_BANG_ITEM_ADD		
		case POINT_PC_BANG_EXP_BONUS :
		case POINT_PC_BANG_DROP_BONUS :
		case POINT_RAMADAN_CANDY_BONUS_EXP:
			SetPoint(type, amount);
			val = GetPoint(type);
			break;
			// END_PC_BANG_ITEM_ADD		

		case POINT_EXP_DOUBLE_BONUS:	// 71  
		case POINT_GOLD_DOUBLE_BONUS:	// 72  
		case POINT_ITEM_DROP_BONUS:	// 73  
		case POINT_POTION_BONUS:	// 74
			if (GetPoint(type) + amount > 100)
			{
				sys_err("BONUS exceeded over 100!! point type: %d name: %s amount %d", type, GetName(), amount);
				amount = 100 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_IMMUNE_STUN:		// 76 
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			break;

		case POINT_IMMUNE_SLOW:		// 77  
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			break;

		case POINT_IMMUNE_FALL:	// 78   
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			break;

		case POINT_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_VOICE:
		case POINT_EMPIRE_POINT:
			//sys_err("CHARACTER::PointChange: %s: point cannot be changed. use SetPoint instead (type: %d)", GetName(), type);
			val = GetRealPoint(type);
			break;

		case POINT_POLYMORPH:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			SetPolymorph(val);
			break;

		case POINT_MOUNT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			MountVnum(val);
			break;

		case POINT_ENERGY:
		case POINT_COSTUME_ATTR_BONUS:
			{
				int old_val = GetPoint(type);
				SetPoint(type, old_val + amount);
				val = GetPoint(type);
				BuffOnAttr_ValueChange(type, old_val, val);
			}
			break;

#ifdef __NEW_ATTRIBUTES__
		case POINT_ATTBONUS_METIN:   
		case POINT_ATTBONUS_BOSS:     
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
#endif

		default:
			sys_err("CHARACTER::PointChange: %s: unknown point change type %d", GetName(), type);
			return;
	}

	switch (type)
	{
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HT:
			ComputeBattlePoints();
			break;
		case POINT_MAX_HP:
		case POINT_MAX_SP:
		case POINT_MAX_STAMINA:
			break;
	}

	if (type == POINT_HP && amount == 0)
		return;

	if (GetDesc())
	{
		struct packet_point_change pack;

		pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.type = type;
		pack.value = val;

		if (bAmount)
			pack.amount = amount;
		else
			pack.amount = 0;

		if (!bBroadcast)
			GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
		else
			PacketAround(&pack, sizeof(pack));
	}
}

#ifdef ENABLE_NEW_PET_SYSTEM
void CHARACTER::SendPetLevelUpEffect(int vid, int type, int value, int amount) 
{
	struct packet_point_change pack;

	pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
	pack.dwVID = vid;
	pack.type = type;
	pack.value = value;
	pack.amount = amount;
	PacketAround(&pack, sizeof(pack));
}
#endif

void CHARACTER::ApplyPoint(BYTE bApplyType, int iVal)
{
	switch (bApplyType)
	{
		case APPLY_NONE:			// 0
			break;;

		case APPLY_CON:
			PointChange(POINT_HT, iVal);
			PointChange(POINT_MAX_HP, (iVal * JobInitialPoints[GetJob()].hp_per_ht));
			PointChange(POINT_MAX_STAMINA, (iVal * JobInitialPoints[GetJob()].stamina_per_con));
			break;

		case APPLY_INT: 
			PointChange(POINT_IQ, iVal);
			PointChange(POINT_MAX_SP, (iVal * JobInitialPoints[GetJob()].sp_per_iq));
			break;

		case APPLY_SKILL:
			// SKILL_DAMAGE_BONUS
			{
				// 최상위 비트 기준으로 8비트 vnum, 9비트 add, 15비트 change
				// 00000000 00000000 00000000 00000000
				// ^^^^^^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
				// vnum     ^ add       change
				BYTE bSkillVnum = (BYTE) (((DWORD)iVal) >> 24);
				int iAdd = iVal & 0x00800000;
				int iChange = iVal & 0x007fffff;

				sys_log(1, "APPLY_SKILL skill %d add? %d change %d", bSkillVnum, iAdd ? 1 : 0, iChange);

				if (0 == iAdd)
					iChange = -iChange;

				boost::unordered_map<BYTE, int>::iterator iter = m_SkillDamageBonus.find(bSkillVnum);

				if (iter == m_SkillDamageBonus.end())
					m_SkillDamageBonus.insert(std::make_pair(bSkillVnum, iChange));
				else
					iter->second += iChange;
			}
			// END_OF_SKILL_DAMAGE_BONUS
			break;

		case APPLY_STR:
		case APPLY_DEX:
		case APPLY_MAX_HP:
		case APPLY_MAX_SP:
		case APPLY_MAX_HP_PCT:
		case APPLY_MAX_SP_PCT:
		case APPLY_ATT_SPEED:
		case APPLY_MOV_SPEED:
		case APPLY_CAST_SPEED:
		case APPLY_HP_REGEN:
		case APPLY_SP_REGEN:
		case APPLY_POISON_PCT:
		case APPLY_STUN_PCT:
		case APPLY_SLOW_PCT:
		case APPLY_CRITICAL_PCT:
		case APPLY_PENETRATE_PCT:
		case APPLY_ATTBONUS_HUMAN:
		case APPLY_ATTBONUS_ANIMAL:
		case APPLY_ATTBONUS_ORC:
		case APPLY_ATTBONUS_MILGYO:
		case APPLY_ATTBONUS_UNDEAD:
		case APPLY_ATTBONUS_DEVIL:
#ifdef ENABLE_SHADOW_BONUS
		case APPLY_ATTBONUS_SHADOW:
#endif
		case APPLY_ATTBONUS_WARRIOR:	// 59
		case APPLY_ATTBONUS_ASSASSIN:	// 60
		case APPLY_ATTBONUS_SURA:	// 61
		case APPLY_ATTBONUS_SHAMAN:	// 62
		case APPLY_ATTBONUS_MONSTER:	// 63
		case APPLY_STEAL_HP:
		case APPLY_STEAL_SP:
		case APPLY_MANA_BURN_PCT:
		case APPLY_DAMAGE_SP_RECOVER:
		case APPLY_BLOCK:
		case APPLY_DODGE:
		case APPLY_RESIST_SWORD:
		case APPLY_RESIST_TWOHAND:
		case APPLY_RESIST_DAGGER:
		case APPLY_RESIST_BELL:
		case APPLY_RESIST_FAN:
		case APPLY_RESIST_BOW:
		case APPLY_RESIST_FIRE:
		case APPLY_RESIST_ELEC:
		case APPLY_RESIST_MAGIC:
		case APPLY_RESIST_WIND:
		case APPLY_RESIST_ICE:
		case APPLY_RESIST_EARTH:
		case APPLY_RESIST_DARK:
		case APPLY_REFLECT_MELEE:
		case APPLY_REFLECT_CURSE:
		case APPLY_ANTI_CRITICAL_PCT:
		case APPLY_ANTI_PENETRATE_PCT:
		case APPLY_POISON_REDUCE:
		case APPLY_KILL_SP_RECOVER:
		case APPLY_EXP_DOUBLE_BONUS:
		case APPLY_GOLD_DOUBLE_BONUS:
		case APPLY_ITEM_DROP_BONUS:
		case APPLY_POTION_BONUS:
		case APPLY_KILL_HP_RECOVER:
		case APPLY_IMMUNE_STUN:	
		case APPLY_IMMUNE_SLOW:	
		case APPLY_IMMUNE_FALL:	
		case APPLY_BOW_DISTANCE:
		case APPLY_ATT_GRADE_BONUS:
		case APPLY_DEF_GRADE_BONUS:
		case APPLY_MAGIC_ATT_GRADE:
		case APPLY_MAGIC_DEF_GRADE:
		case APPLY_CURSE_PCT:
		case APPLY_MAX_STAMINA:
		case APPLY_MALL_ATTBONUS:
		case APPLY_MALL_DEFBONUS:
		case APPLY_MALL_EXPBONUS:
		case APPLY_MALL_ITEMBONUS:
		case APPLY_MALL_GOLDBONUS:
		case APPLY_SKILL_DAMAGE_BONUS:
		case APPLY_NORMAL_HIT_DAMAGE_BONUS:

			// DEPEND_BONUS_ATTRIBUTES
		case APPLY_SKILL_DEFEND_BONUS:
		case APPLY_NORMAL_HIT_DEFEND_BONUS:
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case APPLY_PC_BANG_EXP_BONUS :
		case APPLY_PC_BANG_DROP_BONUS :

		case APPLY_RESIST_WARRIOR :
		case APPLY_RESIST_ASSASSIN :
		case APPLY_RESIST_SURA :
		case APPLY_RESIST_SHAMAN :	
		case APPLY_ENERGY:					// 82 기력
		case APPLY_DEF_GRADE:				// 83 방어력. DEF_GRADE_BONUS는 클라에서 두배로 보여지는 의도된 버그(...)가 있다.
		case APPLY_COSTUME_ATTR_BONUS:		// 84 코스튬 아이템에 붙은 속성치 보너스
		case APPLY_MAGIC_ATTBONUS_PER:		// 85 마법 공격력 +x%
		case APPLY_MELEE_MAGIC_ATTBONUS_PER:			// 86 마법 + 밀리 공격력 +x%
#ifdef ELEMENT_NEW_BONUSES
		case APPLY_ATTBONUS_ELEC:
		case APPLY_ATTBONUS_FIRE:
		case APPLY_ATTBONUS_ICE:
		case APPLY_ATTBONUS_WIND:
		case APPLY_ATTBONUS_EARTH:
		case APPLY_ATTBONUS_DARK:
#endif
#ifdef ENABLE_NEW_ATTR
		case APPLY_ATTBONUS_METIN:
#endif
#ifdef __NEW_ATTRIBUTES__
		case APPLY_ATTBONUS_METIN:    
		case APPLY_ATTBONUS_BOSS:     
#endif	
			PointChange(aApplyInfo[bApplyType].bPointType, iVal);
			break;

		default:
			sys_err("Unknown apply type %d name %s", bApplyType, GetName());
			break;
	}
}

void CHARACTER::MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet)
{
	packet->header	= HEADER_GC_MOTION;
	packet->vid		= m_vid;
	packet->motion	= motion;

	if (victim)
		packet->victim_vid = victim->GetVID();
	else
		packet->victim_vid = 0;
}

void CHARACTER::Motion(BYTE motion, LPCHARACTER victim)
{
	struct packet_motion pack_motion;
	MotionPacketEncode(motion, victim, &pack_motion);
	PacketAround(&pack_motion, sizeof(struct packet_motion));
}

EVENTFUNC(save_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "save_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}	
	sys_log(1, "SAVE_EVENT: %s", ch->GetName());
	ch->Save();
	ch->FlushDelayedSaveItem();
	return (save_event_second_cycle);
}

void CHARACTER::StartSaveEvent()
{
	if (m_pkSaveEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;
	m_pkSaveEvent = event_create(save_event, info, save_event_second_cycle);
}

void CHARACTER::MonsterLog(const char* format, ...)
{
	if (!test_server)
		return;

	if (IsPC())
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%u)", (DWORD)GetVID());

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	va_list args;

	va_start(args, format);

	int len2 = vsnprintf(chatbuf + len, sizeof(chatbuf) - len, format, args);

	if (len2 < 0 || len2 >= (int) sizeof(chatbuf) - len)
		len += (sizeof(chatbuf) - len) - 1;
	else
		len += len2;

	// \0 문자 포함
	++len;

	va_end(args);

	TPacketGCChat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size		= sizeof(TPacketGCChat) + len;
	pack_chat.type      = CHAT_TYPE_TALKING;
	pack_chat.id        = (DWORD)GetVID();
	pack_chat.bEmpire	= 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	CHARACTER_MANAGER::instance().PacketMonsterLog(this, buf.read_peek(), buf.size());
}

void CHARACTER::ChatPacket(BYTE type, const char * format, ...)
{
	LPDESC d = GetDesc();

	if (!d || !format)
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	struct packet_chat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size      = sizeof(struct packet_chat) + len;
	pack_chat.type      = type;
	pack_chat.id        = 0;
	pack_chat.bEmpire   = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());

	if (type == CHAT_TYPE_COMMAND && test_server)
		sys_log(0, "SEND_COMMAND %s %s", GetName(), chatbuf);
}

#ifdef MULTI_LANGUAGE_SYSTEM
void CHARACTER::ChatInfoTrans(const char * format, ...)
{
	LPDESC d = GetDesc();

	if (!d || !format)
		return;

	std::string transText = LC_TEXT_TRANS(format, GetLang());

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

	va_start(args, format);
	int32_t len = vsnprintf(chatbuf, sizeof(chatbuf), transText.c_str(), args);
	va_end(args);

	struct packet_chat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(struct packet_chat) + len;
	pack_chat.type = CHAT_TYPE_INFO;
	pack_chat.id = 0;
	pack_chat.bEmpire = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());
}


#endif

std::string CHARACTER::GetLang()
{
	if (GetDesc() != NULL)
	{
		return GetDesc()->GetAccountTable().lang;
	}

	return "de";
}

void CHARACTER::SetLang(std::string lang)
{
	if (GetDesc() != NULL)
	{
		strlcpy(GetDesc()->GetAccountTable().lang, lang.c_str(), sizeof(GetDesc()->GetAccountTable().lang));
	}
}

// MINING
void CHARACTER::mining_take()
{
	m_pkMiningEvent = NULL;
}

void CHARACTER::mining_cancel()
{
	if (m_pkMiningEvent)
	{
		sys_log(0, "XXX MINING CANCEL");
		event_cancel(&m_pkMiningEvent);
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("채광을 중단하였습니다."));
	}
}

void CHARACTER::mining(LPCHARACTER chLoad)
{
	if (m_pkMiningEvent)
	{
		mining_cancel();
		return;
	}

	if (!chLoad)
		return;
	
#ifdef ENABLE_CSHIELD
	if (GetMapIndex() != chLoad->GetMapIndex() || DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 1000)
		return;
#endif
	

	if (mining::GetRawOreFromLoad(chLoad->GetRaceNum()) == 0)
		return;
		
	if (GetMapIndex() != chLoad->GetMapIndex() || DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 1000)
		return;

	LPITEM pick = GetWear(WEAR_WEAPON);

	if (!pick || pick->GetType() != ITEM_PICK)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("곡괭이를 장착하세요."));
		return;
	}

	int count = number(5, 15); // 동작 횟수, 한 동작당 2초

	// 채광 동작을 보여줌
	TPacketGCDigMotion p;
	p.header = HEADER_GC_DIG_MOTION;
	p.vid = GetVID();
	p.target_vid = chLoad->GetVID();
	p.count = count;

	PacketAround(&p, sizeof(p));

	m_pkMiningEvent = mining::CreateMiningEvent(this, chLoad, count);
}
// END_OF_MINING

bool CHARACTER::IsNearWater() const
{
	if (!GetSectree())
		return false;

	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			if (IS_SET(GetSectree()->GetAttribute(GetX() + x * 100, GetY() + y * 100), ATTR_WATER))
				return true;
		}
	}

	return false;
}

void CHARACTER::fishing()
{
	
	if (m_pkFishingEvent)
	{
		fishing_take();
		return;
	}
	if (!IsNearWater())
		return;

	// 못감 속성에서 낚시를 시도한다?
	{
		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

		int	x = GetX();
		int y = GetY();

		LPSECTREE tree = pkSectreeMap->Find(x, y);
		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("낚시를 할 수 있는 곳이 아닙니다"));
			return;
		}
	}

	LPITEM rod = GetWear(WEAR_WEAPON);

	// 낚시대 장착
	if (!rod || rod->GetType() != ITEM_ROD)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("낚시대를 장착 하세요."));
		return;
	}

	if (0 == rod->GetSocket(2))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("미끼를 끼고 던져 주세요."));
		return;
	}

	float fx, fy;
	GetDeltaByDegree(GetRotation(), 400.0f, &fx, &fy);

	m_pkFishingEvent = fishing::CreateFishingEvent(this);
}

void CHARACTER::fishing_take()
{
	LPITEM rod = GetWear(WEAR_WEAPON);
	if (rod && rod->GetType() == ITEM_ROD)
	{
		using fishing::fishing_event_info;
		if (m_pkFishingEvent)
		{
			struct fishing_event_info* info = dynamic_cast<struct fishing_event_info*>(m_pkFishingEvent->info);

			if (info)
				fishing::Take(info, this);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("낚시대가 아닌 물건으로 낚시를 할 수 없습니다!"));
	}

	event_cancel(&m_pkFishingEvent);
}

bool CHARACTER::StartStateMachine(int iNextPulse)
{
	if (CHARACTER_MANAGER::instance().AddToStateList(this))
	{
		m_dwNextStatePulse = thecore_heart->pulse + iNextPulse;
		return true;
	}

	return false;
}

void CHARACTER::StopStateMachine()
{
	CHARACTER_MANAGER::instance().RemoveFromStateList(this);
}

void CHARACTER::UpdateStateMachine(DWORD dwPulse)
{
	if (dwPulse < m_dwNextStatePulse)
		return;

	if (IsDead())
		return;

	Update();
	m_dwNextStatePulse = dwPulse + m_dwStateDuration;
}

void CHARACTER::SetNextStatePulse(int iNextPulse)
{
	CHARACTER_MANAGER::instance().AddToStateList(this);
	m_dwNextStatePulse = iNextPulse;

	if (iNextPulse < 10)
		MonsterLog("다음상태로어서가자");
}


// 캐릭터 인스턴스 업데이트 함수.
void CHARACTER::UpdateCharacter(DWORD dwPulse)
{
	CFSM::Update();
}

void CHARACTER::SetShop(LPSHOP pkShop)
{
	if ((m_pkShop = pkShop))
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
	else
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP); 
		SetShopOwner(NULL);
	}
}

void CHARACTER::SetExchange(CExchange * pkExchange)
{
	m_pkExchange = pkExchange;
}

void CHARACTER::SetPart(BYTE bPartPos, WORD wVal)
{
	assert(bPartPos < PART_MAX_NUM);
	m_pointsInstant.parts[bPartPos] = wVal;
}

WORD CHARACTER::GetPart(BYTE bPartPos) const
{
	assert(bPartPos < PART_MAX_NUM);
#ifdef __HIDE_COSTUME_SYSTEM__
	if (bPartPos == PART_MAIN && GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true)
	{
		if (const LPITEM pArmor = GetWear(WEAR_BODY))
	#ifdef __CHANGE_LOOK_SYSTEM__
			return pArmor->GetTransmutation() != 0 ? pArmor->GetTransmutation() : pArmor->GetVnum();
	#else
			return pArmor->GetVnum();
	#endif
		else
			return 0;
	}
	else if (bPartPos == PART_HAIR && GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
		return 0;
	#ifdef __SASH_SYSTEM__
	else if (bPartPos == PART_SASH && GetWear(WEAR_COSTUME_SASH) && IsSashCostumeHidden() == true)
		return 0;
	#endif
	#ifdef __WEAPON_COSTUME_SYSTEM__
	else if (bPartPos == PART_WEAPON && GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden() == true)
	{
		if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
		#ifdef __CHANGE_LOOK_SYSTEM__
			return pWeapon->GetTransmutation() != 0 ? pWeapon->GetTransmutation() : pWeapon->GetVnum();
		#else
			return pWeapon->GetVnum();
		#endif
		else
			return 0;
	}
	#endif
#endif
	return m_pointsInstant.parts[bPartPos];
}

WORD CHARACTER::GetOriginalPart(BYTE bPartPos) const
{
	switch (bPartPos)
	{
		case PART_MAIN:
#ifdef __HIDE_COSTUME_SYSTEM__
			if (GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true)
				if (const LPITEM pArmor = GetWear(WEAR_BODY))
					return pArmor->GetVnum();
#endif
			if (!IsPC()) // PC가 아닌 경우 현재 파트를 그대로 리턴
				return GetPart(PART_MAIN);
			else
				return m_pointsInstant.bBasePart;

		case PART_HAIR:
#ifdef __HIDE_COSTUME_SYSTEM__
			if (GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
				return 0;
#endif
			return GetPart(PART_HAIR);

#ifdef __SASH_SYSTEM__
		case PART_SASH:
	#ifdef __HIDE_COSTUME_SYSTEM__
			if (GetWear(WEAR_COSTUME_SASH) && IsSashCostumeHidden() == true)
				return 0;
	#endif
			return GetPart(PART_SASH);
#endif

#ifdef __WEAPON_COSTUME_SYSTEM__
		case PART_WEAPON:
	#ifdef __HIDE_COSTUME_SYSTEM__
			if (GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden() == true)
				if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
					return pWeapon->GetVnum();
	#endif
			return GetWear(WEAR_COSTUME_WEAPON) ? GetPart(PART_WEAPON) : 0;
#endif

		default:
			return 0;
	}
}

BYTE CHARACTER::GetCharType() const
{
	return m_bCharType;
}

bool CHARACTER::SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList)
{
	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
		return false;
	// END_OF_TRENT_MONSTER

	if (ch == this)
	{
		sys_err("SetSyncOwner owner == this (%p)", this);
		return false;
	}

	if (!ch)
	{
		if (bRemoveFromList && m_pkChrSyncOwner)
		{
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
		}

		if (m_pkChrSyncOwner)
			sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());

		// 리스트에서 제거하지 않더라도 포인터는 NULL로 셋팅되어야 한다.
		m_pkChrSyncOwner = NULL;
	}
	else
	{
		if (!IsSyncOwner(ch))
			return false;

		// 거리가 200 이상이면 SyncOwner가 될 수 없다.
		if (DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY()) > 250)
		{
			sys_log(1, "SetSyncOwner distance over than 250 %s %s", GetName(), ch->GetName());

			// SyncOwner일 경우 Owner로 표시한다.
			if (m_pkChrSyncOwner == ch)
				return true;

			return false;
		}

		if (m_pkChrSyncOwner != ch)
		{
			if (m_pkChrSyncOwner)
			{
				sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());
				m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
			}

			m_pkChrSyncOwner = ch;
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.push_back(this);

			// SyncOwner가 바뀌면 LastSyncTime을 초기화한다.
			static const timeval zero_tv = {0, 0};
			SetLastSyncTime(zero_tv);

			sys_log(1, "SetSyncOwner set %s %p to %s", GetName(), this, ch->GetName());
		}

		m_fSyncTime = get_float_time();
	}

	// TODO: Sync Owner가 같더라도 계속 패킷을 보내고 있으므로,
	//       동기화 된 시간이 3초 이상 지났을 때 풀어주는 패킷을
	//       보내는 방식으로 하면 패킷을 줄일 수 있다.
	TPacketGCOwnership pack;

	pack.bHeader	= HEADER_GC_OWNERSHIP;
	pack.dwOwnerVID	= ch ? ch->GetVID() : 0;
	pack.dwVictimVID	= GetVID();

	PacketAround(&pack, sizeof(TPacketGCOwnership));
	return true;
}

struct FuncClearSync
{
	void operator () (LPCHARACTER ch)
	{
		assert(ch != NULL);
		ch->SetSyncOwner(NULL, false);	// false 플래그로 해야 for_each 가 제대로 돈다.
	}
};

void CHARACTER::ClearSync()
{
	SetSyncOwner(NULL);

	// 아래 for_each에서 나를 m_pkChrSyncOwner로 가진 자들의 포인터를 NULL로 한다.
	std::for_each(m_kLst_pkChrSyncOwned.begin(), m_kLst_pkChrSyncOwned.end(), FuncClearSync());
	m_kLst_pkChrSyncOwned.clear();
}

bool CHARACTER::IsSyncOwner(LPCHARACTER ch) const
{
	if (m_pkChrSyncOwner == ch)
		return true;

	// 마지막으로 동기화 된 시간이 3초 이상 지났다면 소유권이 아무에게도
	// 없다. 따라서 아무나 SyncOwner이므로 true 리턴
	if (get_float_time() - m_fSyncTime >= 3.0f)
		return true;

	return false;
}

void CHARACTER::SetParty(LPPARTY pkParty)
{
	if (pkParty == m_pkParty)
		return;

	if (pkParty && m_pkParty)
		sys_err("%s is trying to reassigning party (current %p, new party %p)", GetName(), get_pointer(m_pkParty), get_pointer(pkParty));

	sys_log(1, "PARTY set to %p", get_pointer(pkParty));

	//if (m_pkDungeon && IsPC())
	//SetDungeon(NULL);
	m_pkParty = pkParty;

	if (IsPC())
	{
		if (m_pkParty)
			SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);
		else
			REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);

		UpdatePacket();
	}
}

// PARTY_JOIN_BUG_FIX
/// 파티 가입 event 정보
EVENTINFO(TPartyJoinEventInfo)
{
	DWORD	dwGuestPID;		///< 파티에 참여할 캐릭터의 PID
	DWORD	dwLeaderPID;		///< 파티 리더의 PID

	TPartyJoinEventInfo() 
	: dwGuestPID( 0 )
	, dwLeaderPID( 0 )
	{
	}
} ;

EVENTFUNC(party_request_event)
{
	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( info == NULL )
	{
		sys_err( "party_request_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->dwGuestPID);

	if (ch)
	{
		sys_log(0, "PartyRequestEvent %s", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
		ch->SetPartyRequestEvent(NULL);
	}

	return 0;
}

bool CHARACTER::RequestToParty(LPCHARACTER leader)
{
	if (leader->GetParty())
		leader = leader->GetParty()->GetLeaderCharacter();

	if (!leader)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("파티장이 접속 상태가 아니라서 요청을 할 수 없습니다."));
		return false;
	}

	if (m_pkPartyRequestEvent)
		return false; 

	if (!IsPC() || !leader->IsPC())
		return false;

	if (leader->IsBlockMode(BLOCK_PARTY_REQUEST))
		return false;

	PartyJoinErrCode errcode = IsPartyJoinableCondition(leader, this);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
			return false;

		case PERR_DIFFEMPIRE:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 다른 제국과 파티를 이룰 수 없습니다."));
			return false;

		case PERR_DUNGEON:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 던전 안에서는 파티 초대를 할 수 없습니다.")); 
			return false;

		case PERR_OBSERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 관전 모드에선 파티 초대를 할 수 없습니다.")); 
			return false;

		case PERR_LVBOUNDARY:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> -30 ~ +30 레벨 이내의 상대방만 초대할 수 있습니다.")); 
			return false;

		case PERR_LOWLEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티내 최고 레벨 보다 30레벨이 낮아 초대할 수 없습니다."));
			return false;

		case PERR_HILEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티내 최저 레벨 보다 30레벨이 높아 초대할 수 없습니다.")); 
			return false;

		case PERR_ALREADYJOIN: 	
			return false;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 더 이상 파티원을 초대할 수 없습니다.")); 
			return false;

		default:
			sys_err("Do not process party join error(%d)", errcode); 
			return false;
	}

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = GetPlayerID();
	info->dwLeaderPID = leader->GetPlayerID();

	SetPartyRequestEvent(event_create(party_request_event, info, PASSES_PER_SEC(10)));

	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u", (DWORD) GetVID());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님에게 파티가입 신청을 했습니다."), leader->GetName());
	return true;
}

void CHARACTER::DenyToParty(LPCHARACTER member)
{
	sys_log(1, "DenyToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::DenyToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

void CHARACTER::AcceptToParty(LPCHARACTER member)
{
	sys_log(1, "AcceptToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::AcceptToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	if (!GetParty())
		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 파티에 속해있지 않습니다."));
	else 
	{
		if (GetPlayerID() != GetParty()->GetLeaderPID())
			return;

		PartyJoinErrCode errcode = IsPartyJoinableCondition(this, member);
		switch (errcode) 
		{
			case PERR_NONE: 		member->PartyJoin(this); return;
			case PERR_SERVER:		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다.")); break;
			case PERR_DUNGEON:		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 던전 안에서는 파티 초대를 할 수 없습니다.")); break;
			case PERR_OBSERVER: 	member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 관전 모드에선 파티 초대를 할 수 없습니다.")); break;
			case PERR_LVBOUNDARY:	member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> -30 ~ +30 레벨 이내의 상대방만 초대할 수 있습니다.")); break;
			case PERR_LOWLEVEL: 	member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티내 최고 레벨 보다 30레벨이 낮아 초대할 수 없습니다.")); break;
			case PERR_HILEVEL: 		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티내 최저 레벨 보다 30레벨이 높아 초대할 수 없습니다.")); break;
			case PERR_ALREADYJOIN: 	break;
			case PERR_PARTYISFULL: {
									   ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 더 이상 파티원을 초대할 수 없습니다."));
									   member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티의 인원제한이 초과하여 파티에 참가할 수 없습니다."));
									   break;
								   }
			default: sys_err("Do not process party join error(%d)", errcode);
		}
	}

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

/**
 * 파티 초대 event callback 함수.
 * event 가 발동하면 초대 거절로 처리한다.
 */
EVENTFUNC(party_invite_event)
{
	TPartyJoinEventInfo * pInfo = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( pInfo == NULL )
	{
		sys_err( "party_invite_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER pchInviter = CHARACTER_MANAGER::instance().FindByPID(pInfo->dwLeaderPID);

	if (pchInviter)
	{
		sys_log(1, "PartyInviteEvent %s", pchInviter->GetName());
		pchInviter->PartyInviteDeny(pInfo->dwGuestPID);
	}

	return 0;
}

void CHARACTER::PartyInvite(LPCHARACTER pchInvitee)
{
	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티원을 초대할 수 있는 권한이 없습니다."));
		return;
	}
	else if (pchInvitee->IsBlockMode(BLOCK_PARTY_INVITE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> %s 님이 파티 거부 상태입니다."), pchInvitee->GetName());
		return;
	}
#ifdef ENABLE_PVP_ADVANCED
	else if ((GetDuel("BlockParty")))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("duel_block_function"));
		return;
	}
	
	else if ((pchInvitee->GetDuel("BlockParty")))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("duel_block_victim"), pchInvitee->GetName());
		return;
	}
#endif	

	PartyJoinErrCode errcode = IsPartyJoinableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
			return;

		case PERR_DIFFEMPIRE:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 다른 제국과 파티를 이룰 수 없습니다."));
			return;

		case PERR_DUNGEON:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 던전 안에서는 파티 초대를 할 수 없습니다."));
			return;

		case PERR_OBSERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 관전 모드에선 파티 초대를 할 수 없습니다."));
			return;

		case PERR_LVBOUNDARY:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> -30 ~ +30 레벨 이내의 상대방만 초대할 수 있습니다."));
			return;

		case PERR_LOWLEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티내 최고 레벨 보다 30레벨이 낮아 초대할 수 없습니다."));
			return;

		case PERR_HILEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티내 최저 레벨 보다 30레벨이 높아 초대할 수 없습니다."));
			return;

		case PERR_ALREADYJOIN:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 이미 %s님은 파티에 속해 있습니다."), pchInvitee->GetName());
			return;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 더 이상 파티원을 초대할 수 없습니다."));
			return;

		default:
			sys_err("Do not process party join error(%d)", errcode);
			return;
	}

	if (m_PartyInviteEventMap.end() != m_PartyInviteEventMap.find(pchInvitee->GetPlayerID()))
		return;

	//
	// EventMap 에 이벤트 추가
	// 
	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = pchInvitee->GetPlayerID();
	info->dwLeaderPID = GetPlayerID();

	m_PartyInviteEventMap.insert(EventMap::value_type(pchInvitee->GetPlayerID(), event_create(party_invite_event, info, PASSES_PER_SEC(10))));

	//
	// 초대 받는 character 에게 초대 패킷 전송
	// 

	TPacketGCPartyInvite p;
	p.header = HEADER_GC_PARTY_INVITE;
	p.leader_vid = GetVID();
	pchInvitee->GetDesc()->Packet(&p, sizeof(p));
}

void CHARACTER::PartyInviteAccept(LPCHARACTER pchInvitee)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(pchInvitee->GetPlayerID());

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteAccept from not invited character(%s)", pchInvitee->GetName());
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티원을 초대할 수 있는 권한이 없습니다."));
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableMutableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
			return;

		case PERR_DUNGEON:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 던전 안에서는 파티 초대에 응할 수 없습니다."));
			return;

		case PERR_OBSERVER:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 관전 모드에선 파티 초대를 할 수 없습니다."));
			return;

		case PERR_LVBOUNDARY:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> -30 ~ +30 레벨 이내의 상대방만 초대할 수 있습니다."));
			return;

		case PERR_LOWLEVEL:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티내 최고 레벨 보다 30레벨이 낮아 초대할 수 없습니다."));
			return;

		case PERR_HILEVEL:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티내 최저 레벨 보다 30레벨이 높아 초대할 수 없습니다."));
			return;

		case PERR_ALREADYJOIN:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티 초대에 응할 수 없습니다."));
			return;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 더 이상 파티원을 초대할 수 없습니다."));
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티의 인원제한이 초과하여 파티에 참가할 수 없습니다."));
			return;

		default:
			sys_err("ignore party join error(%d)", errcode);
			return;
	}

	//
	// 파티 가입 처리
	// 

	if (GetParty())
		pchInvitee->PartyJoin(this);
	else
	{
		LPPARTY pParty = CPartyManager::instance().CreateParty(this);

		pParty->Join(pchInvitee->GetPlayerID());
		pParty->Link(pchInvitee);
		pParty->SendPartyInfoAllToOne(this);
	}
}

void CHARACTER::PartyInviteDeny(DWORD dwPID)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(dwPID);

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteDeny to not exist event(inviter PID: %d, invitee PID: %d)", GetPlayerID(), dwPID);
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	LPCHARACTER pchInvitee = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pchInvitee)
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> %s님이 파티 초대를 거절하셨습니다."), pchInvitee->GetName());
}

void CHARACTER::PartyJoin(LPCHARACTER pLeader)
{
	pLeader->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> %s님이 파티에 참가하셨습니다."), GetName());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> %s님의 파티에 참가하셨습니다."), pLeader->GetName());

	pLeader->GetParty()->Join(GetPlayerID());
	pLeader->GetParty()->Link(this);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (pchLeader->GetEmpire() != pchGuest->GetEmpire())
		return PERR_DIFFEMPIRE;

	return IsPartyJoinableMutableCondition(pchLeader, pchGuest);
}

static bool __party_can_join_by_level(LPCHARACTER leader, LPCHARACTER quest)
{
    return true;
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (!CPartyManager::instance().IsEnablePCParty())
		return PERR_SERVER;
	else if (pchLeader->GetDungeon())
		return PERR_DUNGEON;
	else if (pchGuest->IsObserverMode())
		return PERR_OBSERVER;
	else if (false == __party_can_join_by_level(pchLeader, pchGuest))
		return PERR_LVBOUNDARY;
	else if (pchGuest->GetParty())
		return PERR_ALREADYJOIN;
	else if (pchLeader->GetParty())
   	{
	   	if (pchLeader->GetParty()->GetMemberCount() == PARTY_MAX_MEMBER)
			return PERR_PARTYISFULL;
	}

	return PERR_NONE;
}
// END_OF_PARTY_JOIN_BUG_FIX

void CHARACTER::SetDungeon(LPDUNGEON pkDungeon)
{
	if (pkDungeon && m_pkDungeon)
		sys_err("%s is trying to reassigning dungeon (current %p, new party %p)", GetName(), get_pointer(m_pkDungeon), get_pointer(pkDungeon));

	if (m_pkDungeon == pkDungeon) {
		return;
	}

	if (m_pkDungeon)
	{
		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->DecPartyMember(GetParty(), this);
			else
				m_pkDungeon->DecMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->DecMonster();
		}
	}

	m_pkDungeon = pkDungeon;

	if (pkDungeon)
	{
		sys_log(0, "%s DUNGEON set to %p, PARTY is %p", GetName(), get_pointer(pkDungeon), get_pointer(m_pkParty));

		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->IncPartyMember(GetParty(), this);
			else
				m_pkDungeon->IncMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->IncMonster();
		}
	}
}

void CHARACTER::SetWarMap(CWarMap * pWarMap)
{
	if (m_pWarMap)
		m_pWarMap->DecMember(this);

	m_pWarMap = pWarMap;

	if (m_pWarMap)
		m_pWarMap->IncMember(this);
}

void CHARACTER::SetWeddingMap(marriage::WeddingMap* pMap)
{
	if (m_pWeddingMap)
		m_pWeddingMap->DecMember(this);

	m_pWeddingMap = pMap;

	if (m_pWeddingMap)
		m_pWeddingMap->IncMember(this);
}

void CHARACTER::SetRegen(LPREGEN pkRegen)
{
	m_pkRegen = pkRegen;
	if (pkRegen != NULL) {
		regen_id_ = pkRegen->id;
	}
	m_fRegenAngle = GetRotation();
	m_posRegen = GetXYZ();
}

bool CHARACTER::OnIdle()
{
	return false;
}

void CHARACTER::OnMove(bool bIsAttack)
{
	m_dwLastMoveTime = get_dword_time();

	if (bIsAttack)
	{
		m_dwLastAttackTime = m_dwLastMoveTime;

		if (IsAffectFlag(AFF_REVIVE_INVISIBLE))
			RemoveAffect(AFFECT_REVIVE_INVISIBLE);

		if (IsAffectFlag(AFF_EUNHYUNG))
		{
			RemoveAffect(SKILL_EUNHYUNG);
			SetAffectedEunhyung();
		}
		else
		{
			ClearAffectedEunhyung();
		}

		/*if (IsAffectFlag(AFF_JEONSIN))
		  RemoveAffect(SKILL_JEONSINBANGEO);*/
	}

	/*if (IsAffectFlag(AFF_GUNGON))
	  RemoveAffect(SKILL_GUNGON);*/

	// MINING
	mining_cancel();
	// END_OF_MINING
}

void CHARACTER::OnClick(LPCHARACTER pkChrCauser)
{
	if (!pkChrCauser)
	{
		sys_err("OnClick %s by NULL", GetName());
		return;
	}

	DWORD vid = GetVID();
	sys_log(0, "OnClick %s[vnum %d ServerUniqueID %d, pid %d] by %s", GetName(), GetRaceNum(), vid, GetPlayerID(), pkChrCauser->GetName());

	#ifdef __MELEY_LAIR_DUNGEON__
	if ((IsNPC()) && (GetRaceNum() == (WORD)(MeleyLair::GATE_VNUM)) && (MeleyLair::CMgr::instance().IsMeleyMap(pkChrCauser->GetMapIndex())))
	{
		MeleyLair::CMgr::instance().Start(pkChrCauser, pkChrCauser->GetGuild());
		return;
	}
	#endif

	{
		if (pkChrCauser->GetExchange())
		{
			if (test_server)
				sys_err("OnClick Fail (%s->%s) - pc is exchanging", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	if (IsPC())
	{
		if (!CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID, GetVID()))
		{
			if (test_server)
				sys_err("%s.OnClickFailure(%s) - target is PC", pkChrCauser->GetName(), GetName());
			return;
		}
	}
	pkChrCauser->SetQuestNPCID(GetVID());
#ifdef ENABLE_OFFLINESHOP_SYSTEM
	if (IsOfflineShopNPC())
	{
		if (pkChrCauser->CanOpenOfflineShop())
			return;
		GetOfflineShop()->AddGuest(pkChrCauser, this);
		return;
	}
#endif
	if (quest::CQuestManager::instance().Click(pkChrCauser->GetPlayerID(), this))
	{
		return;
	}

	if (!IsPC())
	{
		if (!m_triggerOnClick.pFunc)
		{
			return;
		}

		m_triggerOnClick.pFunc(this, pkChrCauser);
	}
}

BYTE CHARACTER::GetGMLevel() const
{
	if (test_server)
		return GM_IMPLEMENTOR;
	return m_pointsInstant.gm_level;
}

void CHARACTER::SetGMLevel()
{
	if (GetDesc())
	{
	    m_pointsInstant.gm_level =  gm_get_level(GetName(), GetDesc()->GetHostName(), GetDesc()->GetAccountTable().login);
	}
	else
	{
	    m_pointsInstant.gm_level = GM_PLAYER;
	}
}

BOOL CHARACTER::IsGM() const
{
	if (m_pointsInstant.gm_level != GM_PLAYER)
		return true;
	if (test_server)
		return true;
	return false;
}

void CHARACTER::SetStone(LPCHARACTER pkChrStone)
{
	m_pkChrStone = pkChrStone;

	if (m_pkChrStone)
	{
		if (pkChrStone->m_set_pkChrSpawnedBy.find(this) == pkChrStone->m_set_pkChrSpawnedBy.end())
			pkChrStone->m_set_pkChrSpawnedBy.insert(this);
	}
}

struct FuncDeadSpawnedByStone
{
	void operator () (LPCHARACTER ch)
	{
		ch->Dead(NULL);
		ch->SetStone(NULL);
	}
};

void CHARACTER::ClearStone()
{
	if (!m_set_pkChrSpawnedBy.empty())
	{
		// 내가 스폰시킨 몬스터들을 모두 죽인다.
		FuncDeadSpawnedByStone f;
		std::for_each(m_set_pkChrSpawnedBy.begin(), m_set_pkChrSpawnedBy.end(), f);
		m_set_pkChrSpawnedBy.clear();
	}

	if (!m_pkChrStone)
		return;

	m_pkChrStone->m_set_pkChrSpawnedBy.erase(this);
	m_pkChrStone = NULL;
}

void CHARACTER::ClearTarget()
{
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
		m_pkChrTarget = NULL;
	}
	
	TPacketGCTarget p;
	p.header = HEADER_GC_TARGET;
	p.dwVID = 0;
	p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
	p.iMinHP = 0;
	p.iMaxHP = 0;
#endif
	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();
	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *(it++);
		pkChr->m_pkChrTarget = NULL;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}
		
		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
	
	m_set_pkChrTargetedBy.clear();
}

void CHARACTER::SetTarget(LPCHARACTER pkChrTarget)
{
	if (m_pkChrTarget == pkChrTarget)
		return;
	
	if (IS_CASTLE_MAP(GetMapIndex()) && !IsGM())
		return;
	
	if (m_pkChrTarget)
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
	
	m_pkChrTarget = pkChrTarget;
	
	TPacketGCTarget p;
	p.header = HEADER_GC_TARGET;
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.insert(this);
		p.dwVID	= m_pkChrTarget->GetVID();
#ifdef __VIEW_TARGET_PLAYER_HP__
		if ((m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
		else if (m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed())
		{
			p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = m_pkChrTarget->GetHP();
			p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
#else
		if ((m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed()) || (m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
#endif
		}
		else
		{
			if (m_pkChrTarget->GetRaceNum() == 20101 || m_pkChrTarget->GetRaceNum() == 20102 || m_pkChrTarget->GetRaceNum() == 20103 || m_pkChrTarget->GetRaceNum() == 20104 || m_pkChrTarget->GetRaceNum() == 20105 || m_pkChrTarget->GetRaceNum() == 20106 || m_pkChrTarget->GetRaceNum() == 20107 || m_pkChrTarget->GetRaceNum() == 20108 || m_pkChrTarget->GetRaceNum() == 20109)
			{
				LPCHARACTER owner = m_pkChrTarget->GetVictim();
				if (owner)
				{
					int iHorseHealth = owner->GetHorseHealth();
					int iHorseMaxHealth = owner->GetHorseMaxHealth();
					if (iHorseMaxHealth)
					{
						p.bHPPercent = MINMAX(0,  iHorseHealth * 100 / iHorseMaxHealth, 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
					else
					{
						p.bHPPercent = 100;
#ifdef __VIEW_TARGET_DECIMAL_HP__
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
				}
				else
				{
					p.bHPPercent = 100;
#ifdef __VIEW_TARGET_DECIMAL_HP__
					p.iMinHP = 100;
					p.iMaxHP = 100;
#endif
				}
			}
			else
			{
				if (m_pkChrTarget->GetMaxHP() <= 0)
				{
					p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
					p.iMinHP = 0;
					p.iMaxHP = 0;
#endif
				}
				else
				{
					p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
					p.iMinHP = m_pkChrTarget->GetHP();
					p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
				}
			}
		}
	}
	else
	{
		p.dwVID = 0;
		p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}
	
	GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
}

void CHARACTER::BroadcastTargetPacket()
{
	if (m_set_pkChrTargetedBy.empty())
		return;
	
	TPacketGCTarget p;
	p.header = HEADER_GC_TARGET;
	p.dwVID = GetVID();
	if (GetMaxHP() <= 0)
	{
		p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}
	else
	{
#ifdef __VIEW_TARGET_PLAYER_HP__
		p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
		p.iMinHP = GetHP();
		p.iMaxHP = GetMaxHP();
#endif
#else
		if (IsPC())
		{
			p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
		else
		{
			p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
#ifdef __VIEW_TARGET_DECIMAL_HP__
			p.iMinHP = GetHP();
			p.iMaxHP = GetMaxHP();
#endif
		}
#endif
	}
	
	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();
	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *it++;
		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}
		
		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
}

void CHARACTER::CheckTarget()
{
	if (!m_pkChrTarget)
		return;

	if (DISTANCE_APPROX(GetX() - m_pkChrTarget->GetX(), GetY() - m_pkChrTarget->GetY()) >= 4800)
		SetTarget(NULL);
}

void CHARACTER::SetWarpLocation(long lMapIndex, long x, long y)
{
	m_posWarp.x = x * 100;
	m_posWarp.y = y * 100;
	m_lWarpMapIndex = lMapIndex;
}

void CHARACTER::SaveExitLocation()
{
	m_posExit = GetXYZ();
	m_lExitMapIndex = GetMapIndex();
}

void CHARACTER::ExitToSavedLocation()
{
	sys_log (0, "ExitToSavedLocation");
	WarpSet(m_posWarp.x, m_posWarp.y, m_lWarpMapIndex);

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;
}

// fixme 
// 지금까진 privateMapIndex 가 현재 맵 인덱스와 같은지 체크 하는 것을 외부에서 하고,
// 다르면 warpset을 불렀는데
// 이를 warpset 안으로 넣자.
bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex)
{
	if (!IsPC())
		return false;

	long lAddr;
	long lMapIndex;
	WORD wPort;

	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return false;
	}

	//Send Supplementary Data Block if new map requires security packages in loading this map
	{
		long lCurAddr;
		long lCurMapIndex = 0;
		WORD wCurPort;

		CMapLocation::instance().Get(GetX(), GetY(), lCurMapIndex, lCurAddr, wCurPort);

		//do not send SDB files if char is in the same map
		if( lCurMapIndex != lMapIndex )
		{
			const TMapRegion * rMapRgn = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);
			{
				DESC_MANAGER::instance().SendClientPackageSDBToLoadMap( GetDesc(), rMapRgn->strMapName.c_str() );	
			}
		}
	}

	if (lPrivateMapIndex >= 10000)
	{
		if (lPrivateMapIndex / 10000 != lMapIndex)
		{
			sys_err("Invalid map inedx %d, must be child of %d", lPrivateMapIndex, lMapIndex);
			return false;
		}

		lMapIndex = lPrivateMapIndex;
	}

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "WarpSet %s %d %d current map %d target map %d", GetName(), x, y, GetMapIndex(), lMapIndex);

	TPacketGCWarp p;

	p.bHeader	= HEADER_GC_WARP;
	p.lX	= x;
	p.lY	= y;
	p.lAddr	= lAddr;
	p.wPort	= wPort;
#ifdef ENABLE_SWITCHBOT
	CSwitchbotManager::Instance().SetIsWarping(GetPlayerID(), true);

	if (p.wPort != mother_port)
	{
		CSwitchbotManager::Instance().P2PSendSwitchbot(GetPlayerID(), p.wPort);
	}
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));

	//if (!LC_IsNewCIBN())
	{
		char buf[256];
		snprintf(buf, sizeof(buf), "%s MapIdx %ld DestMapIdx%ld DestX%ld DestY%ld Empire%d", GetName(), GetMapIndex(), lPrivateMapIndex, x, y, GetEmpire());
		LogManager::instance().CharLog(this, 0, "WARP", buf);
	}

	return true;
}

void CHARACTER::WarpEnd()
{
	if (test_server)
		sys_log(0, "WarpEnd %s", GetName());

	if (m_posWarp.x == 0 && m_posWarp.y == 0)
		return;

	int index = m_lWarpMapIndex;

	if (index > 10000)
		index /= 10000;

	if (!map_allow_find(index))
	{
		// 이 곳으로 워프할 수 없으므로 워프하기 전 좌표로 되돌리자.
		sys_err("location %d %d not allowed to login this server", m_posWarp.x, m_posWarp.y);
		GetDesc()->SetPhase(PHASE_CLOSE);
		return;
	}

	sys_log(0, "WarpEnd %s %d %u %u", GetName(), m_lWarpMapIndex, m_posWarp.x, m_posWarp.y);

	Show(m_lWarpMapIndex, m_posWarp.x, m_posWarp.y, 0);
	Stop();

	m_lWarpMapIndex = 0;
	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;

	{
		// P2P Login
		TPacketGGLogin p;

		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, GetName(), sizeof(p.szName));
		p.dwPID = GetPlayerID();
		p.bEmpire = GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(GetX(), GetY());
		p.bChannel = g_bChannel;

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));
	}
}

bool CHARACTER::Return()
{
	if (!IsNPC())
		return false;

	int x, y;
	/*
	   float fDist = DISTANCE_SQRT(m_pkMobData->m_posLastAttacked.x - GetX(), m_pkMobData->m_posLastAttacked.y - GetY());
	   float fx, fy;
	   GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);
	   x = GetX() + (int) fx;
	   y = GetY() + (int) fy;
	 */
	SetVictim(NULL);

	x = m_pkMobInst->m_posLastAttacked.x;
	y = m_pkMobInst->m_posLastAttacked.y;

	SetRotationToXY(x, y);

	if (!Goto(x, y))
		return false;

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	if (test_server)
		sys_log(0, "%s %p 포기하고 돌아가자! %d %d", GetName(), this, x, y);

	if (GetParty())
		GetParty()->SendMessage(this, PM_RETURN, x, y);

	return true;
}

bool CHARACTER::Follow(LPCHARACTER pkChr, float fMinDistance)
{
	if (IsPC()
#ifdef __FAKE_PLAYER__
		|| IsFakePlayer()	
#endif			
	)
	{
		sys_err("CHARACTER::Follow : PC cannot use this method", GetName());
		return false;
	}

	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
	{
		if (pkChr->IsPC()) // 쫓아가는 상대가 PC일 때
		{
			// If i'm in a party. I must obey party leader's AI.
			if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
			{
				if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // 마지막으로 공격받은지 15초가 지났고
				{
					// 마지막 맞은 곳으로 부터 50미터 이상 차이나면 포기하고 돌아간다.
					if (m_pkMobData->m_table.wAttackRange < DISTANCE_APPROX(pkChr->GetX() - GetX(), pkChr->GetY() - GetY()))
						if (Return())
							return true;
				}
			}
		}
		return false;
	}
	// END_OF_TRENT_MONSTER

	long x = pkChr->GetX();
	long y = pkChr->GetY();

	if (pkChr->IsPC()) // 쫓아가는 상대가 PC일 때
	{
		// If i'm in a party. I must obey party leader's AI.
		if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
		{
			if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // 마지막으로 공격받은지 15초가 지났고
			{
				// 마지막 맞은 곳으로 부터 50미터 이상 차이나면 포기하고 돌아간다.
				if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
					if (Return())
						return true;
			}
		}
	}

	if (IsGuardNPC())
	{
		if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
			if (Return())
				return true;
	}

#ifdef ENABLE_NEW_PET_SYSTEM
	if (pkChr->IsState(pkChr->m_stateMove) &&
		GetMobBattleType() != BATTLE_TYPE_RANGE &&
		GetMobBattleType() != BATTLE_TYPE_MAGIC &&
		false == IsPet() && false == IsNewPet() && false == IsSupport())
#else
	if (pkChr->IsState(pkChr->m_stateMove) && 
		GetMobBattleType() != BATTLE_TYPE_RANGE && 
		GetMobBattleType() != BATTLE_TYPE_MAGIC &&
		false == IsPet())
#endif
	{
		float rot = pkChr->GetRotation();
		float rot_delta = GetDegreeDelta(rot, GetDegreeFromPositionXY(GetX(), GetY(), pkChr->GetX(), pkChr->GetY()));

		float yourSpeed = pkChr->GetMoveSpeed();
		float mySpeed = GetMoveSpeed();

		float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());
		float fFollowSpeed = mySpeed - yourSpeed * cos(rot_delta * M_PI / 180);

		if (fFollowSpeed >= 0.1f)
		{
			float fMeetTime = fDist / fFollowSpeed;
			float fYourMoveEstimateX, fYourMoveEstimateY;

			if( fMeetTime * yourSpeed <= 100000.0f )
			{
				GetDeltaByDegree(pkChr->GetRotation(), fMeetTime * yourSpeed, &fYourMoveEstimateX, &fYourMoveEstimateY);

				x += (long) fYourMoveEstimateX;
				y += (long) fYourMoveEstimateY;

				float fDistNew = sqrt(((double)x - GetX())*(x-GetX())+((double)y - GetY())*(y-GetY()));
				if (fDist < fDistNew)
				{
					x = (long)(GetX() + (x - GetX()) * fDist / fDistNew);
					y = (long)(GetY() + (y - GetY()) * fDist / fDistNew);
				}
			}
		}
	}

	// 가려는 위치를 바라봐야 한다.
	SetRotationToXY(x, y);

	float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());

	if (fDist <= fMinDistance)
		return false;

	float fx, fy;

	if (IsChangeAttackPosition(pkChr) && GetMobRank() < MOB_RANK_BOSS)
	{
		// 상대방 주변 랜덤한 곳으로 이동
		SetChangeAttackPositionTime();

		int retry = 16;
		int dx, dy;
		int rot = (int) GetDegreeFromPositionXY(x, y, GetX(), GetY());

		while (--retry)
		{
			if (fDist < 500.0f)
				GetDeltaByDegree((rot + number(-90, 90) + number(-90, 90)) % 360, fMinDistance, &fx, &fy);
			else
				GetDeltaByDegree(number(0, 359), fMinDistance, &fx, &fy);

			dx = x + (int) fx;
			dy = y + (int) fy;

			LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), dx, dy);

			if (NULL == tree)
				break;

			if (0 == (tree->GetAttribute(dx, dy) & (ATTR_BLOCK | ATTR_OBJECT)))
				break;
		}

		//sys_log(0, "근처 어딘가로 이동 %s retry %d", GetName(), retry);
		if (!Goto(dx, dy))
			return false;
	}
	else
	{
		// 직선 따라가기
		float fDistToGo = fDist - fMinDistance;
		GetDeltaByDegree(GetRotation(), fDistToGo, &fx, &fy);

		//sys_log(0, "직선으로 이동 %s", GetName());
		if (!Goto(GetX() + (int) fx, GetY() + (int) fy))
			return false;
	}

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	//MonsterLog("쫓아가기; %s", pkChr->GetName());
	return true;
}

float CHARACTER::GetDistanceFromSafeboxOpen() const
{
	return DISTANCE_APPROX(GetX() - m_posSafeboxOpen.x, GetY() - m_posSafeboxOpen.y);
}

void CHARACTER::SetSafeboxOpenPosition()
{
	m_posSafeboxOpen = GetXYZ();
}

CSafebox * CHARACTER::GetSafebox() const
{
	return m_pkSafebox;
}

void CHARACTER::ReqSafeboxLoad(const char* pszPassword)
{
	if (!*pszPassword || strlen(pszPassword) > SAFEBOX_PASSWORD_MAX_LEN)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}
	else if (m_pkSafebox)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 창고가 이미 열려있습니다."));
		return;
	}

	int iPulse = thecore_pulse();

	if (iPulse - GetSafeboxLoadTime()  < PASSES_PER_SEC(10))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 창고를 닫은지 10초 안에는 열 수 없습니다."));
		return;
	}
	else if (GetDistanceFromSafeboxOpen() > 1000)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 거리가 멀어서 창고를 열 수 없습니다."));
		return;
	}
	else if (m_bOpeningSafebox)
	{
		sys_log(0, "Overlapped safebox load request from %s", GetName());
		return;
	}

	SetSafeboxLoadTime();
	m_bOpeningSafebox = true;

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, pszPassword, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}

void CHARACTER::LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(true);
	//END_PREVENT_TRADE_WINDOW

	if (m_pkSafebox)
		bLoaded = true;

	if (!m_pkSafebox)
		m_pkSafebox = M2_NEW CSafebox(this, iSize, dwGold);
	else
		m_pkSafebox->ChangeSize(iSize);

	m_iSafeboxSize = iSize;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = iSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkSafebox->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);

#ifdef __CHANGELOOK_SYSTEM__
			item->SetTransmutation(pItems->transmutation);
#endif

			if (!m_pkSafebox->Add(pItems->pos, item))
			{
				M2_DESTROY_ITEM(item);
			}
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::ChangeSafeboxSize(BYTE bSize)
{
	//if (!m_pkSafebox)
	//return;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = bSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (m_pkSafebox)
		m_pkSafebox->ChangeSize(bSize);

	m_iSafeboxSize = bSize;
}

void CHARACTER::CloseSafebox()
{
	if (!m_pkSafebox)
		return;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(false);
	//END_PREVENT_TRADE_WINDOW

	m_pkSafebox->Save();

	M2_DELETE(m_pkSafebox);
	m_pkSafebox = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseSafebox");

	SetSafeboxLoadTime();
	m_bOpeningSafebox = false;

	Save();
}

CSafebox * CHARACTER::GetMall() const
{
	return m_pkMall;
}

void CHARACTER::LoadMall(int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	if (m_pkMall)
		bLoaded = true;

	if (!m_pkMall)
		m_pkMall = M2_NEW CSafebox(this, 3 * SAFEBOX_PAGE_SIZE, 0);
	else
		m_pkMall->ChangeSize(3 * SAFEBOX_PAGE_SIZE);

	m_pkMall->SetWindowMode(MALL);

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_MALL_OPEN;
	p.bSize = 3 * SAFEBOX_PAGE_SIZE;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkMall->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
#ifdef __CHANGELOOK_SYSTEM__
			item->SetTransmutation(pItems->transmutation);
#endif
			item->SetAttributes(pItems->aAttr);

			if (!m_pkMall->Add(pItems->pos, item))
				M2_DESTROY_ITEM(item);
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::CloseMall()
{
	if (!m_pkMall)
		return;

	m_pkMall->Save();

	M2_DELETE(m_pkMall);
	m_pkMall = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseMall");
}

bool CHARACTER::BuildUpdatePartyPacket(TPacketGCPartyUpdate & out)
{
	if (!GetParty())
		return false;

	memset(&out, 0, sizeof(out));

	out.header		= HEADER_GC_PARTY_UPDATE;
	out.pid		= GetPlayerID();
	out.percent_hp	= MINMAX(0, GetHP() * 100 / GetMaxHP(), 100);
	out.role		= GetParty()->GetRole(GetPlayerID());

	sys_log(1, "PARTY %s role is %d", GetName(), out.role);

	LPCHARACTER l = GetParty()->GetLeaderCharacter();

	if (l && DISTANCE_APPROX(GetX() - l->GetX(), GetY() - l->GetY()) < PARTY_DEFAULT_RANGE)
	{
		if (g_iUseLocale)
			out.affects[0] = GetParty()->GetPartyBonusExpPercent();
		else
			out.affects[0] = GetParty()->GetExpBonusPercent();
		out.affects[1] = GetPoint(POINT_PARTY_ATTACKER_BONUS);
		out.affects[2] = GetPoint(POINT_PARTY_TANKER_BONUS);
		out.affects[3] = GetPoint(POINT_PARTY_BUFFER_BONUS);
		out.affects[4] = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
		out.affects[5] = GetPoint(POINT_PARTY_HASTE_BONUS);
		out.affects[6] = GetPoint(POINT_PARTY_DEFENDER_BONUS);
	}

	return true;
}

int CHARACTER::GetLeadershipSkillLevel() const
{ 
	return GetSkillLevel(SKILL_LEADERSHIP);
}

void CHARACTER::QuerySafeboxSize()
{
	if (m_iSafeboxSize == -1)
	{
		DBManager::instance().ReturnQuery(QID_SAFEBOX_SIZE,
				GetPlayerID(),
				NULL, 
				"SELECT size FROM safebox%s WHERE account_id = %u",
				get_table_postfix(),
				GetDesc()->GetAccountTable().id);
	}
}

void CHARACTER::SetSafeboxSize(int iSize)
{
	sys_log(1, "SetSafeboxSize: %s %d", GetName(), iSize);
	m_iSafeboxSize = iSize;
	DBManager::instance().Query("UPDATE safebox%s SET size = %d WHERE account_id = %u", get_table_postfix(), iSize / SAFEBOX_PAGE_SIZE, GetDesc()->GetAccountTable().id);
}

int CHARACTER::GetSafeboxSize() const
{
	return m_iSafeboxSize;
}

void CHARACTER::SetNowWalking(bool bWalkFlag)
{
	//if (m_bNowWalking != bWalkFlag || IsNPC())
	if (m_bNowWalking != bWalkFlag)
	{
		if (bWalkFlag)
		{
			m_bNowWalking = true;
			m_dwWalkStartTime = get_dword_time();
		}
		else
		{
			m_bNowWalking = false;
		}

		//if (m_bNowWalking)
		{
			TPacketGCWalkMode p;
			p.vid = GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

			PacketView(&p, sizeof(p));
		}

		if (IsNPC())
		{
			if (m_bNowWalking)
				MonsterLog("걷는다");
			else
				MonsterLog("뛴다");
		}

		//sys_log(0, "%s is now %s", GetName(), m_bNowWalking?"walking.":"running.");
	}
}

void CHARACTER::StartStaminaConsume()
{
	if (m_bStaminaConsume)
		return;
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = true;
	//ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
	if (IsStaminaHalfConsume())
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec / 2, GetStamina());
	else
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
}

void CHARACTER::StopStaminaConsume()
{
	if (!m_bStaminaConsume)
		return;
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = false;
	ChatPacket(CHAT_TYPE_COMMAND, "StopStaminaConsume %d", GetStamina());
}

bool CHARACTER::IsStaminaConsume() const
{
	return m_bStaminaConsume;
}

bool CHARACTER::IsStaminaHalfConsume() const
{
	return IsEquipUniqueItem(UNIQUE_ITEM_HALF_STAMINA);
}

void CHARACTER::ResetStopTime()
{
	m_dwStopTime = get_dword_time();
}

DWORD CHARACTER::GetStopTime() const
{
	return m_dwStopTime;
}

void CHARACTER::ResetPoint(int iLv)
{
	BYTE bJob = GetJob();

	PointChange(POINT_LEVEL, iLv - GetLevel());

	SetRealPoint(POINT_ST, JobInitialPoints[bJob].st);
	SetPoint(POINT_ST, GetRealPoint(POINT_ST));

	SetRealPoint(POINT_HT, JobInitialPoints[bJob].ht);
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));

	SetRealPoint(POINT_DX, JobInitialPoints[bJob].dx);
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));

	SetRealPoint(POINT_IQ, JobInitialPoints[bJob].iq);
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetRandomHP((iLv - 1) * number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end));
	SetRandomSP((iLv - 1) * number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end));

	//PointChange(POINT_STAT, ((MINMAX(1, iLv, 99) - 1) * 3) + GetPoint(POINT_LEVEL_STEP) - GetPoint(POINT_STAT));
	PointChange(POINT_STAT, ((MINMAX(1, iLv, 90) - 1) * 3) + GetPoint(POINT_LEVEL_STEP) - GetPoint(POINT_STAT));

	ComputePoints();

	// 회복
	PointChange(POINT_HP, GetMaxHP() - GetHP());
	PointChange(POINT_SP, GetMaxSP() - GetSP());

	PointsPacket();

	LogManager::instance().CharLog(this, 0, "RESET_POINT", "");
}

bool CHARACTER::IsChangeAttackPosition(LPCHARACTER target) const
{ 
	if (!IsNPC())
		return true;

	DWORD dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_NEAR;

	if (DISTANCE_APPROX(GetX() - target->GetX(), GetY() - target->GetY()) > 
		AI_CHANGE_ATTACK_POISITION_DISTANCE + GetMobAttackRange())
		dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_FAR;

	return get_dword_time() - m_dwLastChangeAttackPositionTime > dwChangeTime; 
}

void CHARACTER::GiveRandomSkillBook()
{
	LPITEM item = AutoGiveItem(50300);

	if (NULL != item)
	{
		BYTE bJob = 0;

		if (!number(0, 1))
			bJob = GetJob() + 1;

		DWORD dwSkillVnum = 0;

		do
		{
			dwSkillVnum = number(1, 111);
			const CSkillProto* pkSk = CSkillManager::instance().Get(dwSkillVnum);

			if (NULL == pkSk)
				continue;

			if (bJob && bJob != pkSk->dwType)
				continue;

			break;
		} while (true);

		item->SetSocket(0, dwSkillVnum);
	}
}

void CHARACTER::ReviveInvisible(int iDur)
{
	AddAffect(AFFECT_REVIVE_INVISIBLE, POINT_NONE, 0, AFF_REVIVE_INVISIBLE, iDur, 0, true);

	if (IsPC())
	{
		CheckMount();
	}
}

#ifdef WJ_EXTENDED_PET_SYSTEM
void CHARACTER::CheckPet()
{
	LPITEM item = GetWear(WEAR_PET);
	if (item)
	{
		if (!item->IsPetItem())
		{
			return;
		}
		else
		{
			CPetSystem* petSystem = GetPetSystem();
			if (petSystem)
				petSystem->Summon(item->GetValue(3), item, 0, false);
		}
	}
	else
		return;
}
#endif

void CHARACTER::ToggleMonsterLog()
{
	m_bMonsterLog = !m_bMonsterLog;

	if (m_bMonsterLog)
	{
		CHARACTER_MANAGER::instance().RegisterForMonsterLog(this);
	}
	else
	{
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
	}
}

void CHARACTER::SetGuild(CGuild* pGuild)
{
	if (m_pGuild != pGuild)
	{
		m_pGuild = pGuild;
		UpdatePacket();
	}
}

void CHARACTER::SendGreetMessage()
{
	typeof(DBManager::instance().GetGreetMessage()) v = DBManager::instance().GetGreetMessage();

	for (itertype(v) it = v.begin(); it != v.end(); ++it)
	{
		ChatPacket(CHAT_TYPE_NOTICE, it->c_str());
	}
}

void CHARACTER::BeginStateEmpty()
{
	MonsterLog("!");
}

void CHARACTER::EffectPacket(int enumEffectType)
{
	TPacketGCSpecialEffect p;

	p.header = HEADER_GC_SEPCIAL_EFFECT;
	p.type = enumEffectType;
	p.vid = GetVID();

	PacketAround(&p, sizeof(TPacketGCSpecialEffect));
}

void CHARACTER::SpecificEffectPacket(const char filename[MAX_EFFECT_FILE_NAME])
{
	TPacketGCSpecificEffect p;

	p.header = HEADER_GC_SPECIFIC_EFFECT;
	p.vid = GetVID();
	memcpy (p.effect_file, filename, MAX_EFFECT_FILE_NAME);

	PacketAround(&p, sizeof(TPacketGCSpecificEffect));
}

//void CHARACTER::MonsterChat(BYTE bMonsterChatType)
//{
//	if (IsPC())
//		return;
//
//	char sbuf[256+1];
//
//	if (IsMonster())
//	{
//		if (number(0, 60))
//			return;
//
//		snprintf(sbuf, sizeof(sbuf), 
//				"(locale.monster_chat[%i] and locale.monster_chat[%i][%d] or '')",
//				GetRaceNum(), GetRaceNum(), bMonsterChatType*3 + number(1, 3));
//	}
//	else
//	{
//		if (bMonsterChatType != MONSTER_CHAT_WAIT)
//			return;
//
//		if (IsGuardNPC())
//		{
//			if (number(0, 6))
//				return;
//		}
//		else
//		{
//			if (number(0, 30))
//				return;
//		}
//
//		snprintf(sbuf, sizeof(sbuf), "(locale.monster_chat[%i] and locale.monster_chat[%i][number(1, table.getn(locale.monster_chat[%i]))] or '')", GetRaceNum(), GetRaceNum(), GetRaceNum());
//	}
//
//	std::string text = quest::ScriptToString(sbuf);
//
//	if (text.empty())
//		return;
//
//	struct packet_chat pack_chat;
//
//	pack_chat.header    = HEADER_GC_CHAT;
//	pack_chat.size	= sizeof(struct packet_chat) + text.size() + 1;
//	pack_chat.type      = CHAT_TYPE_TALKING;
//	pack_chat.id        = GetVID();
//	pack_chat.bEmpire	= 0;
//
//	TEMP_BUFFER buf;
//	buf.write(&pack_chat, sizeof(struct packet_chat));
//	buf.write(text.c_str(), text.size() + 1);
//
//	PacketAround(buf.read_peek(), buf.size());
//}

void CHARACTER::SetQuestNPCID(DWORD vid)
{
	m_dwQuestNPCVID = vid;
}

LPCHARACTER CHARACTER::GetQuestNPC() const
{
	return CHARACTER_MANAGER::instance().Find(m_dwQuestNPCVID);
}

void CHARACTER::SetQuestItemPtr(LPITEM item)
{
	m_pQuestItem = item;
}

void CHARACTER::ClearQuestItemPtr()
{
	m_pQuestItem = NULL;
}

LPITEM CHARACTER::GetQuestItemPtr() const
{
	return m_pQuestItem;
}

LPDUNGEON CHARACTER::GetDungeonForce() const
{ 
	if (m_lWarpMapIndex > 10000)
		return CDungeonManager::instance().FindByMapIndex(m_lWarpMapIndex);

	return m_pkDungeon;
}

void CHARACTER::SetBlockMode(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;

	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);

	SetQuestFlag("game_option.block_exchange", bFlag & BLOCK_EXCHANGE ? 1 : 0);
	SetQuestFlag("game_option.block_party_invite", bFlag & BLOCK_PARTY_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_guild_invite", bFlag & BLOCK_GUILD_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_whisper", bFlag & BLOCK_WHISPER ? 1 : 0);
	SetQuestFlag("game_option.block_messenger_invite", bFlag & BLOCK_MESSENGER_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_party_request", bFlag & BLOCK_PARTY_REQUEST ? 1 : 0);
}

void CHARACTER::SetBlockModeForce(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;
	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);
}

bool CHARACTER::IsGuardNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

int CHARACTER::GetPolymorphPower() const
{
	if (test_server)
	{
		int value = quest::CQuestManager::instance().GetEventFlag("poly");
		if (value)
			return value;
	}
	return aiPolymorphPowerByLevel[MINMAX(0, GetSkillLevel(SKILL_POLYMORPH), 40)];
}

void CHARACTER::SetPolymorph(DWORD dwRaceNum, bool bMaintainStat)
{
	if (dwRaceNum < JOB_MAX_NUM)
	{
		dwRaceNum = 0;
		bMaintainStat = false;
	}

	if (m_dwPolymorphRace == dwRaceNum)
		return;

	m_bPolyMaintainStat = bMaintainStat;
	m_dwPolymorphRace = dwRaceNum;

	sys_log(0, "POLYMORPH: %s race %u ", GetName(), dwRaceNum);

	if (dwRaceNum != 0)
		StopRiding();

	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);

	ViewReencode();

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	if (!bMaintainStat)
	{
		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_IQ, 0);
		PointChange(POINT_HT, 0);
	}

	// 폴리모프 상태에서 죽는 경우, 폴리모프가 풀리게 되는데
	// 폴리 모프 전후로 valid combo interval이 다르기 때문에
	// Combo 핵 또는 Hacker로 인식하는 경우가 있다.
	// 따라서 폴리모프를 풀거나 폴리모프 하게 되면,
	// valid combo interval을 reset한다.
	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputeBattlePoints();
}

int CHARACTER::GetQuestFlag(const std::string& flag) const
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());
	if(!pPC) {
		sys_err("Nullpointer in CHARACTER::GetQuestFlag %lu", GetPlayerID());
		return 0;
	}
	return pPC->GetFlag(flag);
}

void CHARACTER::SetQuestFlag(const std::string& flag, int value)
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());
	if(!pPC) {
		sys_err("Nullpointer in CHARACTER::SettQuestFlag %lu", GetPlayerID());
		return;
	}
	pPC->SetFlag(flag, value);
}

void CHARACTER::DetermineDropMetinStone()
{
	const int METIN_STONE_NUM = 14;
	static DWORD c_adwMetin[METIN_STONE_NUM] = 
	{
		28030,
		28031,
		28032,
		28033,
		28034,
		28035,
		28036,
		28037,
		28038,
		28039,
		28040,
		28041,
		28042,
		28043,
	};
	DWORD stone_num = GetRaceNum();
	int idx = std::lower_bound(aStoneDrop, aStoneDrop+STONE_INFO_MAX_NUM, stone_num) - aStoneDrop;
	if (idx >= STONE_INFO_MAX_NUM || aStoneDrop[idx].dwMobVnum != stone_num)
	{
		m_dwDropMetinStone = 0;
	}
	else
	{
		const SStoneDropInfo & info = aStoneDrop[idx];
		m_bDropMetinStonePct = info.iDropPct;
		{
			m_dwDropMetinStone = c_adwMetin[number(0, METIN_STONE_NUM - 1)];
			int iGradePct = number(1, 100);
			for (int iStoneLevel = 0; iStoneLevel < STONE_LEVEL_MAX_NUM; iStoneLevel ++)
			{
				int iLevelGradePortion = info.iLevelPct[iStoneLevel];
				if (iGradePct <= iLevelGradePortion)
				{
					break;
				}
				else
				{
					iGradePct -= iLevelGradePortion;
					m_dwDropMetinStone += 100; // 돌 +a -> +(a+1)이 될때마다 100씩 증가
				}
			}
		}
	}
}

void CHARACTER::SendEquipment(LPCHARACTER ch)
{
	TPacketViewEquip p;
	p.header = HEADER_GC_VIEW_EQUIP;
	p.vid    = GetVID();

#ifdef EQUIP_ENABLE_VIEW_SASH
	int pos[17] = {	WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR,	WEAR_UNIQUE1,
					WEAR_UNIQUE2, WEAR_ARROW, WEAR_SHIELD, WEAR_COSTUME_BODY, WEAR_COSTUME_HAIR, WEAR_RING1, WEAR_RING2, WEAR_BELT, WEAR_COSTUME_SASH};
	for (int i = 0; i < 17; i++)
#else
	int pos[16] = {	WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_UNIQUE1,
					WEAR_UNIQUE2, WEAR_ARROW, WEAR_SHIELD, WEAR_COSTUME_BODY, WEAR_COSTUME_HAIR, WEAR_RING1, WEAR_RING2, WEAR_BELT};
	for (int i = 0; i < 16; i++)	
#endif
	{
		LPITEM item = GetWear(pos[i]);
		if (item) {
			p.equips[i].vnum = item->GetVnum();
			p.equips[i].count = item->GetCount();

			thecore_memcpy(p.equips[i].alSockets, item->GetSockets(), sizeof(p.equips[i].alSockets));
			thecore_memcpy(p.equips[i].aAttr, item->GetAttributes(), sizeof(p.equips[i].aAttr));
		}
		else {
			p.equips[i].vnum = 0;
		}
	}
	ch->GetDesc()->Packet(&p, sizeof(p));
}

bool CHARACTER::CanSummon(int iLeaderShip)
{
	return (iLeaderShip >= 20 || iLeaderShip >= 12 && m_dwLastDeadTime + 180 > get_dword_time());
}

#ifdef ENABLE_PVP_ADVANCED	
int CHARACTER::GetDuel(const char* type) const
{
	const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};
	
	int m_nDuelTable[] = {(GetQuestFlag(szTableStaticPvP[0])), (GetQuestFlag(szTableStaticPvP[1])), (GetQuestFlag(szTableStaticPvP[2])), (GetQuestFlag(szTableStaticPvP[3])), (GetQuestFlag(szTableStaticPvP[4])), (GetQuestFlag(szTableStaticPvP[5])), (GetQuestFlag(szTableStaticPvP[6])), (GetQuestFlag(szTableStaticPvP[7])), (GetQuestFlag(szTableStaticPvP[8])), (GetQuestFlag(szTableStaticPvP[9]))};
	
	if (!strcmp(type, "BlockChangeItem") && m_nDuelTable[0] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockBuff") && m_nDuelTable[1] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockPotion") && m_nDuelTable[2] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockRide") && m_nDuelTable[3] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockPet") && m_nDuelTable[4] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockPoly") && m_nDuelTable[5] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockParty") && m_nDuelTable[6] > 0) {
		return true;
	}	
	if (!strcmp(type, "BlockExchange") && m_nDuelTable[7] > 0) {
		return true;
	}	
	if (!strcmp(type, "BetMoney") && m_nDuelTable[8] > 0) {
		return true;
	}	
	if (!strcmp(type, "IsFight") && m_nDuelTable[9] > 0) {
		return true;
	}	
	return false;
}

void CHARACTER::SetDuel(const char* type, int value)
{	
	const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};

	if (!strcmp(type, "BlockChangeItem")) {
		SetQuestFlag(szTableStaticPvP[0], value);
	}	
	if (!strcmp(type, "BlockBuff")) {
		SetQuestFlag(szTableStaticPvP[1], value);
	}	
	if (!strcmp(type, "BlockPotion")) {
		SetQuestFlag(szTableStaticPvP[2], value);
	}	
	if (!strcmp(type, "BlockRide")) {
		SetQuestFlag(szTableStaticPvP[3], value);
	}	
	if (!strcmp(type, "BlockPet")) {
		SetQuestFlag(szTableStaticPvP[4], value);
	}	
	if (!strcmp(type, "BlockPoly")) {
		SetQuestFlag(szTableStaticPvP[5], value);
	}	
	if (!strcmp(type, "BlockParty")) {
		SetQuestFlag(szTableStaticPvP[6], value);
	}	
	if (!strcmp(type, "BlockExchange")) {
		SetQuestFlag(szTableStaticPvP[7], value);
	}	
	if (!strcmp(type, "BetMoney")) {
		SetQuestFlag(szTableStaticPvP[8], 0);
	}	
	if (!strcmp(type, "IsFight")) {
		SetQuestFlag(szTableStaticPvP[9], value);
	}			
}
#endif


void CHARACTER::MountVnum(DWORD vnum)
{
	if (m_dwMountVnum == vnum)
		return;

	m_dwMountVnum = vnum;
	m_dwMountTime = get_dword_time();

	if (m_bIsObserver)
		return;

	//NOTE : Mount한다고 해서 Client Side의 객체를 삭제하진 않는다.
	//그리고 서버Side에서 탔을때 위치 이동은 하지 않는다. 왜냐하면 Client Side에서 Coliision Adjust를 할수 있는데
	//객체를 소멸시켰다가 서버위치로 이동시키면 이때 collision check를 하지는 않으므로 배경에 끼거나 뚫고 나가는 문제가 존재한다.
	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
	//EncodeRemovePacket(this);
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		//Mount한다고 해서 Client Side의 객체를 삭제하진 않는다.
		//EncodeRemovePacket(entity);
		//if (!m_bIsObserver)
		EncodeInsertPacket(entity);

		//if (!entity->IsObserverMode())
		//	entity->EncodeInsertPacket(this);
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputePoints();
}

namespace {
	class FuncCheckWarp
	{
		public:
			FuncCheckWarp(LPCHARACTER pkWarp)
			{
				m_lTargetY = 0;
				m_lTargetX = 0;

				m_lX = pkWarp->GetX();
				m_lY = pkWarp->GetY();

				m_bInvalid = false;
				m_bEmpire = pkWarp->GetEmpire();

				char szTmp[64];

				if (3 != sscanf(pkWarp->GetName(), " %s %ld %ld ", szTmp, &m_lTargetX, &m_lTargetY))
				{
					if (number(1, 100) < 5)
						sys_err("Warp NPC name wrong : vnum(%d) name(%s)", pkWarp->GetRaceNum(), pkWarp->GetName());

					m_bInvalid = true;

					return;
				}

				m_lTargetX *= 100;
				m_lTargetY *= 100;

				m_bUseWarp = true;

				if (pkWarp->IsGoto())
				{
					LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pkWarp->GetMapIndex());
					m_lTargetX += pkSectreeMap->m_setting.iBaseX;
					m_lTargetY += pkSectreeMap->m_setting.iBaseY;
					m_bUseWarp = false;
				}
			}

			bool Valid()
			{
				return !m_bInvalid;
			}

			void operator () (LPENTITY ent)
			{
				if (!Valid())
					return;

				if (!ent->IsType(ENTITY_CHARACTER))
					return;

				LPCHARACTER pkChr = (LPCHARACTER) ent;

				if (!pkChr->IsPC())
					return;

				int iDist = DISTANCE_APPROX(pkChr->GetX() - m_lX, pkChr->GetY() - m_lY);

				if (iDist > 300)
					return;

				if (m_bEmpire && pkChr->GetEmpire() && m_bEmpire != pkChr->GetEmpire())
					return;

				if (pkChr->IsHack())
					return;

				if (!pkChr->CanHandleItem(false, true))
					return;	
				
				if (m_bUseWarp)
					pkChr->WarpSet(m_lTargetX, m_lTargetY);
				else
				{
					pkChr->Show(pkChr->GetMapIndex(), m_lTargetX, m_lTargetY);
					pkChr->Stop();
				}
			}

			bool m_bInvalid;
			bool m_bUseWarp;

			long m_lX;
			long m_lY;
			long m_lTargetX;
			long m_lTargetY;

			BYTE m_bEmpire;
	};
}

EVENTFUNC(warp_npc_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "warp_npc_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}	

	if (!ch->GetSectree())
	{
		ch->m_pkWarpNPCEvent = NULL;
		return 0;
	}

	FuncCheckWarp f(ch);
	if (f.Valid())
		ch->GetSectree()->ForEachAround(f);

	return passes_per_sec / 2;
}

void CHARACTER::ChannelSwitch(int new_ch){
	long lAddr;
	long lMapIndex;
	WORD wPort;
	long x = this->GetX();
	long y = this->GetY();

	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
			sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
			return;
	}
	if(lMapIndex >= 10000){
		return;
	}
	if (lMapIndex == 217) {
		ChatPacket(CHAT_TYPE_INFO, "You can't change the channel while you're in a dungeon!");
		return;
	}
	if (wPort == 13099) {
		ChatPacket(CHAT_TYPE_INFO, "You can't change the channel in a global map!");
		return;	
	}
	
	/*std::map<WORD, int>micha;
	
	for(int i = 1; i < 4; i++){ //replace with maximum channels -1   actual 1300x - 1330x
		for(int i2 = 1; i2 < 9; i2++){ //replace with your core values actual  13x02 - 13x08
			micha[13*1000 + i*100 + i2] = i+1;
		}        
	}
	//micha[13002] = 1;
	int chan;
	if(micha.find(wPort) != micha.end()){
		chan = micha[wPort];
	}else{return;}*/
	
	//wegen kevs kack corenummerierung
	int chan;
	int core;
	if (wPort >= 13111 && wPort <= 13118){
		chan = 1;
		core = wPort - 13110 - 1;
	} else if (wPort >= 14080 && wPort <= 14087){
		chan = 2;
		core = wPort - 14080;
	} else if (wPort >= 15080 && wPort <= 15087){
		chan = 3;
		core = wPort - 15080;
	} else if (wPort >= 16080 && wPort <= 16088){
		chan = 4;
		core = wPort - 16080;
		if (core >= 4)
			core--;
	} else if (wPort >= 17080 && wPort <= 17088){
		chan = 5;
		core = wPort - 17080;
	}
	
	if (new_ch == chan){
		ChatPacket(CHAT_TYPE_INFO, "You are already in Channel %d.", new_ch);
		return;
	}
		
	
	int newPort;
	switch (new_ch){
		case 1:
			newPort = 13110 + core + 1;
			break;
		case 2:
			newPort = 14080 + core;
			break;
		case 3:
			newPort = 15080 + core;
			break;
		case 4:
			newPort = 16080 + core;			
			if (core >= 3)
				newPort++;
			break;
		case 5:
			newPort = 17080 + core;
			break;
		default:
			return;
	}
	
	Stop();
	Save();
	
	if(GetSectree()){
		GetSectree()->RemoveEntity(this);
		ViewCleanup();
		EncodeRemovePacket(this);
	}
	TPacketGCWarp p;

	p.bHeader	= HEADER_GC_WARP;
	p.lX	= x;
	p.lY	= y;
	p.lAddr	= lAddr;
	p.wPort	= newPort;
	

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));
}

void CHARACTER::StartWarpNPCEvent()
{
	if (m_pkWarpNPCEvent)
		return;

	if (!IsWarp() && !IsGoto())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkWarpNPCEvent = event_create(warp_npc_event, info, passes_per_sec / 2);
}

void CHARACTER::SyncPacket()
{
	TEMP_BUFFER buf;

	TPacketCGSyncPositionElement elem;

	elem.dwVID = GetVID();
	elem.lX = GetX();
	elem.lY = GetY();

	TPacketGCSyncPosition pack;

	pack.bHeader = HEADER_GC_SYNC_POSITION;
	pack.wSize = sizeof(TPacketGCSyncPosition) + sizeof(elem);

	buf.write(&pack, sizeof(pack));
	buf.write(&elem, sizeof(elem));

	PacketAround(buf.read_peek(), buf.size());
}

LPCHARACTER CHARACTER::GetMarryPartner() const
{
	return m_pkChrMarried;
}

void CHARACTER::SetMarryPartner(LPCHARACTER ch)
{
	m_pkChrMarried = ch;
}

int CHARACTER::GetMarriageBonus(DWORD dwItemVnum, bool bSum)
{
	if (IsNPC())
		return 0;

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());

	if (!pMarriage)
		return 0;

	return pMarriage->GetBonus(dwItemVnum, bSum, this);
}

void CHARACTER::ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID)
{
	if (!IsPC())
		return;

	TPacketGCQuestConfirm p;

	p.header = HEADER_GC_QUEST_CONFIRM;
	p.requestPID = dwRequestPID;
	p.timeout = iTimeout;
	strlcpy(p.msg, szMsg, sizeof(p.msg));

	GetDesc()->Packet(&p, sizeof(p));
}

int CHARACTER::GetPremiumRemainSeconds(BYTE bType) const
{
	if (bType >= PREMIUM_MAX_NUM)
		return 0;

	return m_aiPremiumTimes[bType] - get_global_time();
}

bool CHARACTER::WarpToPID(DWORD dwPID)
{
	LPCHARACTER victim;
	if ((victim = (CHARACTER_MANAGER::instance().FindByPID(dwPID))))
	{
		int mapIdx = victim->GetMapIndex();
		if (IS_SUMMONABLE_ZONE(mapIdx))
		{
			if (CAN_ENTER_ZONE(this, mapIdx))
			{
				WarpSet(victim->GetX(), victim->GetY());
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 있는 곳으로 워프할 수 없습니다."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 있는 곳으로 워프할 수 없습니다."));
			return false;
		}
	}
	else
	{
		// 다른 서버에 로그인된 사람이 있음 -> 메시지 보내 좌표를 받아오자
		// 1. A.pid, B.pid 를 뿌림
		// 2. B.pid를 가진 서버가 뿌린서버에게 A.pid, 좌표 를 보냄
		// 3. 워프
		CCI * pcci = P2P_MANAGER::instance().FindByPID(dwPID);

		if (!pcci)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 온라인 상태가 아닙니다."));
			return false;
		}

		if (pcci->bChannel != g_bChannel)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 %d 채널에 있습니다. (현재 채널 %d)"), pcci->bChannel, g_bChannel);
			return false;
		}
		else if (false == IS_SUMMONABLE_ZONE(pcci->lMapIndex))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 있는 곳으로 워프할 수 없습니다."));
			return false;
		}
		else
		{
			if (!CAN_ENTER_ZONE(this, pcci->lMapIndex))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 있는 곳으로 워프할 수 없습니다."));
				return false;
			}

			TPacketGGFindPosition p;
			p.header = HEADER_GG_FIND_POSITION;
			p.dwFromPID = GetPlayerID();
			p.dwTargetPID = dwPID;
			pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));

			if (test_server) 
				ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for teleport");
		}
	}
	return true;
}

// ADD_REFINE_BUILDING
CGuild* CHARACTER::GetRefineGuild() const
{
	LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);

	return (chRefineNPC ? chRefineNPC->GetGuild() : NULL);
}

bool CHARACTER::IsRefineThroughGuild() const
{
	return GetRefineGuild() != NULL;
}

int CHARACTER::ComputeRefineFee(int iCost, int iMultiply) const
{
	CGuild* pGuild = GetRefineGuild();
	if (pGuild)
	{
		if (pGuild == GetGuild())
			return iCost * iMultiply * 9 / 10;

		// 다른 제국 사람이 시도하는 경우 추가로 3배 더
		LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
		if (chRefineNPC && chRefineNPC->GetEmpire() != GetEmpire())
			return iCost * iMultiply * 3;

		return iCost * iMultiply;
	}
	else
		return iCost;
}

void CHARACTER::PayRefineFee(int iTotalMoney)
{
	int iFee = iTotalMoney / 10;
	CGuild* pGuild = GetRefineGuild();

	int iRemain = iTotalMoney;

	if (pGuild)
	{
		// 자기 길드이면 iTotalMoney에 이미 10%가 제외되어있다
		if (pGuild != GetGuild())
		{
			pGuild->RequestDepositMoney(this, iFee);
			iRemain -= iFee;
		}
	}

	PointChange(POINT_GOLD, -iRemain);
}
// END_OF_ADD_REFINE_BUILDING

//Hack 방지를 위한 체크.
bool CHARACTER::IsHack(bool bSendMsg, bool bCheckShopOwner, int limittime)
{
	const int iPulse = thecore_pulse();

	if (test_server)
		bSendMsg = true;

	//창고 연후 체크
	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("창고를 연후 %d초 이내에는 다른곳으로 이동할수 없습니다."), limittime);

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(limittime));
		return true; 
	}

	/*
	//거래관련 창 체크
	if (bCheckShopOwner)
	{
		if (GetExchange() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef __AURA_SYSTEM__
			|| IsAuraRefineWindowOpen()
#endif
		)
		{
			if (bSendMsg) {
				//sys_err("exchange %d, shopowner %d, opensafebox %d, cubeopen %d", GetExchange(), GetShopOwner(), IsOpenSafebox(), IsCubeOpen());
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래창,창고 등을 연 상태에서는 다른곳으로 이동,종료 할수 없습니다"));
			}

			return true;
		}
	}
	else
	{
		if (GetExchange() || IsOpenSafebox() || IsCubeOpen()
#ifdef __AURA_SYSTEM__
			|| IsAuraRefineWindowOpen()
#endif
		)
		{
			if (bSendMsg) {
				//sys_err("exchange %d, opensafebox %d, cubeopen %d", GetExchange(), IsOpenSafebox(), IsCubeOpen());
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래창,창고 등을 연 상태에서는 다른곳으로 이동,종료 할수 없습니다"));
			}

			return true;
		}
	}
	*/

	//PREVENT_PORTAL_AFTER_EXCHANGE
	//교환 후 시간체크
	if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래 후 %d초 이내에는 다른지역으로 이동 할 수 없습니다."), limittime );
		return true;
	}
	//END_PREVENT_PORTAL_AFTER_EXCHANGE

	//PREVENT_ITEM_COPY
	if (iPulse - GetMyShopTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래 후 %d초 이내에는 다른지역으로 이동 할 수 없습니다."), limittime);
		return true;
	}

	if (iPulse - GetRefineTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 개량후 %d초 이내에는 귀환부,귀환기억부를 사용할 수 없습니다."), limittime);
		return true; 
	}
	//END_PREVENT_ITEM_COPY

	return false;
}

BOOL CHARACTER::IsMonarch() const
{
	//MONARCH_LIMIT
	if (CMonarch::instance().IsMonarch(GetPlayerID(), GetEmpire()))
		return true;

	return false;

	//END_MONARCH_LIMIT
}
void CHARACTER::Say(const std::string & s)
{
	struct ::packet_script packet_script;

	packet_script.header = HEADER_GC_SCRIPT;
	packet_script.skin = 1;
	packet_script.src_size = s.size();
	packet_script.size = packet_script.src_size + sizeof(struct packet_script);
	
	TEMP_BUFFER buf;

	buf.write(&packet_script, sizeof(struct packet_script));
	buf.write(&s[0], s.size());

	if (IsPC())
	{
		GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}

//
// Monarch
//
void CHARACTER::InitMC()
{
	for (int n = 0; n < MI_MAX; ++n)
	{
		m_dwMonarchCooltime[n] = thecore_pulse(); 
	}

	m_dwMonarchCooltimelimit[MI_HEAL] = PASSES_PER_SEC(MC_HEAL);
	m_dwMonarchCooltimelimit[MI_WARP] = PASSES_PER_SEC(MC_WARP);
	m_dwMonarchCooltimelimit[MI_TRANSFER] = PASSES_PER_SEC(MC_TRANSFER);
	m_dwMonarchCooltimelimit[MI_TAX] = PASSES_PER_SEC(MC_TAX);
	m_dwMonarchCooltimelimit[MI_SUMMON] = PASSES_PER_SEC(MC_SUMMON);

	m_dwMonarchCooltime[MI_HEAL] -= PASSES_PER_SEC(GetMCL(MI_HEAL));
	m_dwMonarchCooltime[MI_WARP] -= PASSES_PER_SEC(GetMCL(MI_WARP));
	m_dwMonarchCooltime[MI_TRANSFER] -= PASSES_PER_SEC(GetMCL(MI_TRANSFER));
	m_dwMonarchCooltime[MI_TAX] -= PASSES_PER_SEC(GetMCL(MI_TAX));
	m_dwMonarchCooltime[MI_SUMMON] -= PASSES_PER_SEC(GetMCL(MI_SUMMON));
}

DWORD CHARACTER::GetMC(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltime[e];
}

void CHARACTER::SetMC(enum MONARCH_INDEX e)
{
	m_dwMonarchCooltime[e] = thecore_pulse();
}

bool CHARACTER::IsMCOK(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if ((iPulse -  GetMC(e)) <  GetMCL(e))
	{
		if (test_server)
			sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));
		
		return false;
	}
	
	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return true;
}

DWORD CHARACTER::GetMCL(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltimelimit[e];
}

DWORD CHARACTER::GetMCLTime(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return  (GetMCL(e)) / passes_per_sec   -  (iPulse - GetMC(e)) / passes_per_sec;
}

bool CHARACTER::IsSiegeNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

//------------------------------------------------
void CHARACTER::UpdateDepositPulse()
{
	m_deposit_pulse = thecore_pulse() + PASSES_PER_SEC(60*5);	// 5분
}

bool CHARACTER::CanDeposit() const
{
	return (m_deposit_pulse == 0 || (m_deposit_pulse < thecore_pulse()));
}
//------------------------------------------------

ESex GET_SEX(LPCHARACTER ch)
{
	switch (ch->GetRaceNum())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_SURA_M:
		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_SHAMAN_M:
			return SEX_MALE;

		case MAIN_RACE_ASSASSIN_W:
		case MAIN_RACE_SHAMAN_W:
		case MAIN_RACE_WARRIOR_W:
		case MAIN_RACE_SURA_W:
			return SEX_FEMALE;
	}

	/* default sex = male */
	return SEX_MALE;
}

int CHARACTER::GetHPPct() const
{
	return (GetHP() * 100) / GetMaxHP();
}

bool CHARACTER::IsBerserk() const
{
	if (m_pkMobInst != NULL)
		return m_pkMobInst->m_IsBerserk;
	else
		return false;
}

void CHARACTER::SetBerserk(bool mode)
{
	if (m_pkMobInst != NULL)
		m_pkMobInst->m_IsBerserk = mode;
}

bool CHARACTER::IsGodSpeed() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsGodSpeed;
	}
	else
	{
		return false;
	}
}

void CHARACTER::SetGodSpeed(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsGodSpeed = mode;

		if (mode == true)
		{
			SetPoint(POINT_ATT_SPEED, 250);
		}
		else
		{
			SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		}
	}
}

bool CHARACTER::IsDeathBlow() const
{
	if (number(1, 100) <= m_pkMobData->m_table.bDeathBlowPoint)
	{
		return true;
	}
	else
	{
		return false;
	}
}

struct FFindReviver
{
	FFindReviver()
	{
		pChar = NULL;
		HasReviver = false;
	}
	
	void operator() (LPCHARACTER ch)
	{
		if (ch->IsMonster() != true)
		{
			return;
		}

		if (ch->IsReviver() == true && pChar != ch && ch->IsDead() != true)
		{
			if (number(1, 100) <= ch->GetMobTable().bRevivePoint)
			{
				HasReviver = true;
				pChar = ch;
			}
		}
	}

	LPCHARACTER pChar;
	bool HasReviver;
};

bool CHARACTER::HasReviverInParty() const
{
	LPPARTY party = GetParty();

	if (party != NULL)
	{
		if (party->GetMemberCount() == 1) return false;

		FFindReviver f;
		party->ForEachMemberPtr(f);
		return f.HasReviver;
	}

	return false;
}

bool CHARACTER::IsRevive() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsRevive;
	}

	return false;
}

void CHARACTER::SetRevive(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsRevive = mode;
	}
}

#define IS_SPEED_HACK_PLAYER(ch) (ch->m_speed_hack_count > SPEEDHACK_LIMIT_COUNT)

EVENTFUNC(check_speedhack_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (NULL == ch || ch->IsNPC())
		return 0;

	if (IS_SPEED_HACK_PLAYER(ch))
	{
		// write hack log
		LogManager::instance().SpeedHackLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), ch->m_speed_hack_count);

		if (false == LC_IsEurope())
		{
			// close connection
			LPDESC desc = ch->GetDesc();

			if (desc)
			{
				DESC_MANAGER::instance().DestroyDesc(desc);
				return 0;
			}
		}
	}

	ch->m_speed_hack_count = 0;

	ch->ResetComboHackCount();
	return PASSES_PER_SEC(60);
}

void CHARACTER::StartCheckSpeedHackEvent()
{
	if (m_pkCheckSpeedHackEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCheckSpeedHackEvent = event_create(check_speedhack_event, info, PASSES_PER_SEC(60));	// 1분
}

void CHARACTER::GoHome()
{
	WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
}

void CHARACTER::SendGuildName(CGuild* pGuild)
{
	if (NULL == pGuild) return;

	DESC	*desc = GetDesc();

	if (NULL == desc) return;
	if (m_known_guild.find(pGuild->GetID()) != m_known_guild.end()) return;

	m_known_guild.insert(pGuild->GetID());

	TPacketGCGuildName	pack;
	memset(&pack, 0x00, sizeof(pack));

	pack.header		= HEADER_GC_GUILD;
	pack.subheader	= GUILD_SUBHEADER_GC_GUILD_NAME;
	pack.size		= sizeof(TPacketGCGuildName);
	pack.guildID	= pGuild->GetID();
	memcpy(pack.guildName, pGuild->GetName(), GUILD_NAME_MAX_LEN);

	desc->Packet(&pack, sizeof(pack));
}

void CHARACTER::SendGuildName(DWORD dwGuildID)
{
	SendGuildName(CGuildManager::instance().FindGuild(dwGuildID));
}

EVENTFUNC(destroy_when_idle_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "destroy_when_idle_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}	

	if (ch->GetVictim())
	{
		return PASSES_PER_SEC(300);
	}

	sys_log(1, "DESTROY_WHEN_IDLE: %s", ch->GetName());

	ch->m_pkDestroyWhenIdleEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::StartDestroyWhenIdleEvent()
{
	if (m_pkDestroyWhenIdleEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkDestroyWhenIdleEvent = event_create(destroy_when_idle_event, info, PASSES_PER_SEC(300));
}

void CHARACTER::SetComboSequence(BYTE seq)
{
	m_bComboSequence = seq;
}

BYTE CHARACTER::GetComboSequence() const
{
	return m_bComboSequence;
}

void CHARACTER::SetLastComboTime(DWORD time)
{
	m_dwLastComboTime = time;
}

DWORD CHARACTER::GetLastComboTime() const
{
	return m_dwLastComboTime;
}

void CHARACTER::SetValidComboInterval(int interval)
{
	m_iValidComboInterval = interval;
}

int CHARACTER::GetValidComboInterval() const
{
	return m_iValidComboInterval;
}

BYTE CHARACTER::GetComboIndex() const
{
	return m_bComboIndex;
}

void CHARACTER::IncreaseComboHackCount(int k)
{
	m_iComboHackCount += k;

	if (m_iComboHackCount >= 10)
	{
		if (GetDesc())
			if (GetDesc()->DelayedDisconnect(number(2, 7)))
			{
				sys_log(0, "COMBO_HACK_DISCONNECT: %s count: %d", GetName(), m_iComboHackCount);
				LogManager::instance().HackLog("Combo", this);
			}
	}
}

void CHARACTER::ResetComboHackCount()
{
	m_iComboHackCount = 0;
}

void CHARACTER::SkipComboAttackByTime(int interval)
{
	m_dwSkipComboAttackByTime = get_dword_time() + interval;
}

DWORD CHARACTER::GetSkipComboAttackByTime() const
{
	return m_dwSkipComboAttackByTime;
}

void CHARACTER::ResetChatCounter()
{
	m_bChatCounter = 0;
}

BYTE CHARACTER::IncreaseChatCounter()
{
	return ++m_bChatCounter;
}

BYTE CHARACTER::GetChatCounter() const
{
	return m_bChatCounter;
}

// 말이나 다른것을 타고 있나?
bool CHARACTER::IsRiding() const
{
	return IsHorseRiding() || GetMountVnum();
}

bool CHARACTER::CanWarp() const
{
	const int iPulse = thecore_pulse();
	const int limit_time = PASSES_PER_SEC(g_nPortalLimitTime);

	if ((iPulse - GetSafeboxLoadTime()) < limit_time)
		return false;

	if ((iPulse - GetExchangeTime()) < limit_time)
		return false;

	if ((iPulse - GetMyShopTime()) < limit_time)
		return false;

	if ((iPulse - GetRefineTime()) < limit_time)
		return false;

	if (GetExchange() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef __AURA_SYSTEM__
		|| IsAuraRefineWindowOpen()
#endif
	)
		return false;

	return true;
}

DWORD CHARACTER::GetNextExp() const
{
	if (PLAYER_EXP_TABLE_MAX < GetLevel())
		return 2500000000;
	else
		return exp_table[GetLevel()];
}

int	CHARACTER::GetSkillPowerByLevel(int level, bool bMob) const
{
	return CTableBySkill::instance().GetSkillPowerByLevelFromType(GetJob(), GetSkillGroup(), MINMAX(0, level, SKILL_MAX_LEVEL), bMob); 
}

#ifdef ENABLE_NEW_PET_SYSTEM
DWORD CHARACTER::PetGetNextExp() const
{
	if (IsNewPet()) {
		if (120 < GetLevel())
			return 2500000000;
		else
			return exppet_table[GetLevel()];
	}
}
#endif

#ifdef __SASH_SYSTEM__
void CHARACTER::OpenSash(bool bCombination)
{
	if (isSashOpened(bCombination))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The sash window it's already opened."));
		return;
	}
	
	if (bCombination)
	{
		if (m_bSashAbsorption)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you may close the sash absorption window."));
			return;
		}
		
		m_bSashCombination = true;
	}
	else
	{
		if (m_bSashCombination)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you may close the sash combine window."));
			return;
		}
		
		m_bSashAbsorption = true;
	}
	
	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;
	
	TPacketSash sPacket;
	sPacket.header = HEADER_GC_SASH;
	sPacket.subheader = SASH_SUBHEADER_GC_OPEN;
	sPacket.bWindow = bCombination;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketSash));
	
	ClearSashMaterials();
}

void CHARACTER::CloseSash()
{
	if ((!m_bSashCombination) && (!m_bSashAbsorption))
		return;
	
	bool bWindow = (m_bSashCombination == true ? true : false);
	
	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;
	
	TPacketSash sPacket;
	sPacket.header = HEADER_GC_SASH;
	sPacket.subheader = SASH_SUBHEADER_GC_CLOSE;
	sPacket.bWindow = bWindow;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketSash));
	
	if (bWindow)
		m_bSashCombination = false;
	else
		m_bSashAbsorption = false;
	
	ClearSashMaterials();
}

void CHARACTER::ClearSashMaterials()
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetSashMaterials();
	for (int i = 0; i < SASH_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			continue;
		
		pkItemMaterial[i]->Lock(false);
		pkItemMaterial[i] = NULL;
	}
}

bool CHARACTER::SashIsSameGrade(long lGrade)
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetSashMaterials();
	if (!pkItemMaterial[0])
		return false;
	
	bool bReturn = (pkItemMaterial[0]->GetValue(SASH_GRADE_VALUE_FIELD) == lGrade ? true : false);
	return bReturn;
}

long long CHARACTER::GetSashCombinePrice(long lGrade)
{
	long long dwPrice = 0;
	switch (lGrade)
	{
		case 2:
			{
				dwPrice = SASH_GRADE_2_PRICE;
			}
			break;
		case 3:
			{
				dwPrice = SASH_GRADE_3_PRICE;
			}
			break;
		case 4:
			{
				dwPrice = SASH_GRADE_4_PRICE;
			}
			break;
		default:
			{
				dwPrice = SASH_GRADE_1_PRICE;
			}
			break;
	}
	
	return dwPrice;
}

BYTE CHARACTER::CheckEmptyMaterialSlot()
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetSashMaterials();
	for (int i = 0; i < SASH_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			return i;
	}
	
	return 255;
}

void CHARACTER::GetSashCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs)
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetSashMaterials();
	
	if (m_bSashCombination)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			long lVal = pkItemMaterial[0]->GetValue(SASH_GRADE_VALUE_FIELD);
			if (lVal == 4)
			{
				dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
				dwMinAbs = pkItemMaterial[0]->GetSocket(SASH_ABSORPTION_SOCKET);
				DWORD dwMaxAbsCalc = (dwMinAbs + SASH_GRADE_4_ABS_RANGE > SASH_GRADE_4_ABS_MAX ? SASH_GRADE_4_ABS_MAX : (dwMinAbs + SASH_GRADE_4_ABS_RANGE));
				dwMaxAbs = dwMaxAbsCalc;
			}
			else
			{
				DWORD dwMaskVnum = pkItemMaterial[0]->GetOriginalVnum();
				TItemTable * pTable = ITEM_MANAGER::instance().GetTable(dwMaskVnum + 1);
				if (pTable)
					dwMaskVnum += 1;
				
				dwItemVnum = dwMaskVnum;
				switch (lVal)
				{
					case 2:
						{
							dwMinAbs = SASH_GRADE_3_ABS;
							dwMaxAbs = SASH_GRADE_3_ABS;
						}
						break;
					case 3:
						{
							dwMinAbs = SASH_GRADE_4_ABS_MIN;
							dwMaxAbs = SASH_GRADE_4_ABS_MAX_COMB;
						}
						break;
					default:
						{
							dwMinAbs = SASH_GRADE_2_ABS;
							dwMaxAbs = SASH_GRADE_2_ABS;
						}
						break;
				}
			}
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
	else
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
			dwMinAbs = pkItemMaterial[0]->GetSocket(SASH_ABSORPTION_SOCKET);
			dwMaxAbs = dwMinAbs;
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
}

void CHARACTER::AddSashMaterial(TItemPos tPos, BYTE bPos)
{
	if (bPos >= SASH_WINDOW_MAX_MATERIALS)
	{
		if (bPos == 255)
		{
			bPos = CheckEmptyMaterialSlot();
			if (bPos >= SASH_WINDOW_MAX_MATERIALS)
				return;
		}
		else
			return;
	}
	
	LPITEM pkItem = GetItem(tPos);
	if (!pkItem)
		return;
	else if ((pkItem->GetCell() >= INVENTORY_MAX_NUM) || (pkItem->IsEquipped()) || (tPos.IsBeltInventoryPosition()) || (pkItem->IsDragonSoul()))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (m_bSashCombination))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (bPos == 0) && (m_bSashAbsorption))
		return;
	else if (pkItem->isLocked())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't add locked items."));
		return;
	}
	#ifdef __SOULBINDING_SYSTEM__
	else if ((pkItem->IsBind()) || (pkItem->IsUntilBind()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't add binded items."));
		return;
	}
	#endif
	else if ((m_bSashCombination) && (bPos == 1) && (!SashIsSameGrade(pkItem->GetValue(SASH_GRADE_VALUE_FIELD))))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can combine just sashes of same grade."));
		return;
	}
	else if ((m_bSashCombination) && (pkItem->GetSocket(SASH_ABSORPTION_SOCKET) >= SASH_GRADE_4_ABS_MAX))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This sash got already maximum absorption chance."));
		return;
	}
	else if ((bPos == 1) && (m_bSashAbsorption))
	{
		if ((pkItem->GetType() != ITEM_WEAPON) && (pkItem->GetType() != ITEM_ARMOR))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can absorb just the bonuses from armors and weapons."));
			return;
		}
		else if ((pkItem->GetType() == ITEM_ARMOR) && (pkItem->GetSubType() != ARMOR_BODY))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can absorb just the bonuses from armors and weapons."));
			return;
		}
	}
	else if ((pkItem->GetSubType() != COSTUME_SASH) && (m_bSashCombination))
		return;
	else if ((pkItem->GetSubType() != COSTUME_SASH) && (bPos == 0) && (m_bSashAbsorption))
		return;
	else if ((pkItem->GetSocket(SASH_ABSORBED_SOCKET) > 0) && (bPos == 0) && (m_bSashAbsorption))
		return;
	
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetSashMaterials();
	if ((bPos == 1) && (!pkItemMaterial[0]))
		return;
	
	if (pkItemMaterial[bPos])
		return;
	
	pkItemMaterial[bPos] = pkItem;
	pkItemMaterial[bPos]->Lock(true);
	
	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetSashCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);
	
	TPacketSash sPacket;
	sPacket.header = HEADER_GC_SASH;
	sPacket.subheader = SASH_SUBHEADER_GC_ADDED;
	sPacket.bWindow = m_bSashCombination == true ? true : false;
	sPacket.dwPrice = GetSashCombinePrice(pkItem->GetValue(SASH_GRADE_VALUE_FIELD));
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = dwItemVnum;
	sPacket.dwMinAbs = dwMinAbs;
	sPacket.dwMaxAbs = dwMaxAbs;
	GetDesc()->Packet(&sPacket, sizeof(TPacketSash));
}

void CHARACTER::RemoveSashMaterial(BYTE bPos)
{
	if (bPos >= SASH_WINDOW_MAX_MATERIALS)
		return;
	
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetSashMaterials();
	
	long long dwPrice = 0;
	
	if (bPos == 1)
	{
		if (pkItemMaterial[bPos])
		{
			pkItemMaterial[bPos]->Lock(false);
			pkItemMaterial[bPos] = NULL;
		}
		
		if (pkItemMaterial[0])
			dwPrice = GetSashCombinePrice(pkItemMaterial[0]->GetValue(SASH_GRADE_VALUE_FIELD));
	}
	else
		ClearSashMaterials();
	
	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;
	
	TPacketSash sPacket;
	sPacket.header = HEADER_GC_SASH;
	sPacket.subheader = SASH_SUBHEADER_GC_REMOVED;
	sPacket.bWindow = m_bSashCombination == true ? true : false;
	sPacket.dwPrice = dwPrice;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketSash));
}

BYTE CHARACTER::CanRefineSashMaterials()
{
	BYTE bReturn = 0;
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetSashMaterials();
	if (m_bSashCombination)
	{
		for (int i = 0; i < SASH_WINDOW_MAX_MATERIALS; ++i)
		{
			if (pkItemMaterial[i])
			{
				if ((pkItemMaterial[i]->GetType() == ITEM_COSTUME) && (pkItemMaterial[i]->GetSubType() == COSTUME_SASH))
					bReturn = 1;
				else
				{
					bReturn = 0;
					break;
				}
			}
			else
			{
				bReturn = 0;
				break;
			}
		}
	}
	else if (m_bSashAbsorption)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			if ((pkItemMaterial[0]->GetType() == ITEM_COSTUME) && (pkItemMaterial[0]->GetSubType() == COSTUME_SASH))
				bReturn = 2;
			else
				bReturn = 0;
			
			if ((pkItemMaterial[1]->GetType() == ITEM_WEAPON) || ((pkItemMaterial[1]->GetType() == ITEM_ARMOR) && (pkItemMaterial[1]->GetSubType() == ARMOR_BODY)))
				bReturn = 2;
			else
				bReturn = 0;
			
			if (pkItemMaterial[0]->GetSocket(SASH_ABSORBED_SOCKET) > 0)
				bReturn = 0;
		}
		else
			bReturn = 0;
	}
	
	return bReturn;
}

void CHARACTER::RefineSashMaterials()
{
	BYTE bCan = CanRefineSashMaterials();
	if (bCan == 0)
		return;
	
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetSashMaterials();
	
	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetSashCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);
	long long dwPrice = GetSashCombinePrice(pkItemMaterial[0]->GetValue(SASH_GRADE_VALUE_FIELD));
	
	if (bCan == 1)
	{
		int iSuccessChance = 0;
		long lVal = pkItemMaterial[0]->GetValue(SASH_GRADE_VALUE_FIELD);
		switch (lVal)
		{
			case 2:
				{
					iSuccessChance = SASH_COMBINE_GRADE_2;
				}
				break;
			case 3:
				{
					iSuccessChance = SASH_COMBINE_GRADE_3;
				}
				break;
			case 4:
				{
					iSuccessChance = SASH_COMBINE_GRADE_4;
				}
				break;
			default:
				{
					iSuccessChance = SASH_COMBINE_GRADE_1;
				}
				break;
		}
		
		if (GetGold() < dwPrice)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Yang."));
			return;
		}
		
		int iChance = number(1, 100);
		bool bSucces = (iChance <= iSuccessChance ? true : false);
		if (bSucces)
		{
			LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(dwItemVnum, 1, 0, false);
			if (!pkItem)
			{
				sys_err("%d can't be created.", dwItemVnum);
				return;
			}
			
			ITEM_MANAGER::CopyAllAttrTo(pkItemMaterial[0], pkItem);
			LogManager::instance().ItemLog(this, pkItem, "COMBINE SUCCESS", pkItem->GetName());
			DWORD dwAbs = (dwMinAbs == dwMaxAbs ? dwMinAbs : number(dwMinAbs + 1, dwMaxAbs));
			pkItem->SetSocket(SASH_ABSORPTION_SOCKET, dwAbs);
			pkItem->SetSocket(SASH_ABSORBED_SOCKET, pkItemMaterial[0]->GetSocket(SASH_ABSORBED_SOCKET));
			
			PointChange(POINT_GOLD, -dwPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);
			
			WORD wCell = pkItemMaterial[0]->GetCell();
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[0], "COMBINE (REFINE SUCCESS)");
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE SUCCESS)");
			
			pkItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
			pkItem->AttrLog();
			
			if (lVal == 4)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("New absorption rate: %d%"), dwAbs);
			else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success."));
			
			EffectPacket(SE_EFFECT_SASH_SUCCEDED);
			LogManager::instance().SashLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, pkItem->GetID(), 1, dwAbs, 1);
			
			ClearSashMaterials();
		}
		else
		{
			PointChange(POINT_GOLD, -dwPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);
			
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE FAIL)");
			
			if (lVal == 4)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("New absorption rate: %d%"), pkItemMaterial[0]->GetSocket(SASH_ABSORPTION_SOCKET));
			else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Failed."));
			
			LogManager::instance().SashLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, 0, 0, 0, 0);
			
			pkItemMaterial[1] = NULL;
		}
		
		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;
		
		TPacketSash sPacket;
		sPacket.header = HEADER_GC_SASH;
		sPacket.subheader = SASH_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bSashCombination == true ? true : false;
		sPacket.dwPrice = dwPrice;
		sPacket.bPos = 0;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		if (bSucces)
			sPacket.dwMaxAbs = 100;
		else
			sPacket.dwMaxAbs = 0;
		
		GetDesc()->Packet(&sPacket, sizeof(TPacketSash));
	}
	else
	{
		pkItemMaterial[1]->CopyAttributeTo(pkItemMaterial[0]);
		LogManager::instance().ItemLog(this, pkItemMaterial[0], "ABSORB (REFINE SUCCESS)", pkItemMaterial[0]->GetName());
		pkItemMaterial[0]->SetSocket(SASH_ABSORBED_SOCKET, pkItemMaterial[1]->GetOriginalVnum());
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pkItemMaterial[0]->GetAttributeValue(i) < 0)
				pkItemMaterial[0]->SetForceAttribute(i, pkItemMaterial[0]->GetAttributeType(i), 0);
		}
		
		ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "ABSORBED (REFINE SUCCESS)");
		
		ITEM_MANAGER::instance().FlushDelayedSave(pkItemMaterial[0]);
		pkItemMaterial[0]->AttrLog();
		
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success."));
		
		ClearSashMaterials();
		
		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;
		
		TPacketSash sPacket;
		sPacket.header = HEADER_GC_SASH;
		sPacket.subheader = SASH_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bSashCombination == true ? true : false;
		sPacket.dwPrice = dwPrice;
		sPacket.bPos = 255;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		sPacket.dwMaxAbs = 1;
		GetDesc()->Packet(&sPacket, sizeof(TPacketSash));
	}
}

bool CHARACTER::CleanSashAttr(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;
	else if ((!pkItem) || (!pkTarget))
		return false;
	else if ((pkTarget->GetType() != ITEM_COSTUME) && (pkTarget->GetSubType() != COSTUME_SASH))
		return false;
	
	if (pkTarget->GetSocket(SASH_ABSORBED_SOCKET) <= 0)
		return false;
	
	pkTarget->SetSocket(SASH_ABSORBED_SOCKET, 0);
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		pkTarget->SetForceAttribute(i, 0, 0);
	
	pkItem->SetCount(pkItem->GetCount() - 1);
	LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT (CLEAN ATTR)", pkTarget->GetName());
	return true;
}
#endif

#ifdef __CHANGELOOK_SYSTEM__
void CHARACTER::ChangeLookWindow(bool bOpen, bool bRequest)
{
	if ((bOpen) && (isChangeLookOpened()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] The window is already opened."));
		return;
	}
	
	if ((!bOpen) && (!isChangeLookOpened()))
	{
		if (!bRequest)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] The window is not opened."));
		
		return;
	}
	
	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;
	
	TPacketChangeLook sPacket;
	sPacket.header = HEADER_GC_CL;
	sPacket.subheader = CL_SUBHEADER_OPEN;
	sPacket.subheader = bOpen == true ? CL_SUBHEADER_OPEN : CL_SUBHEADER_CLOSE;
	sPacket.dwCost = bOpen == true ? CL_TRANSMUTATION_PRICE : 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	GetDesc()->Packet(&sPacket, sizeof(TPacketChangeLook));
	
	m_bChangeLook = bOpen;
	ClearClWindowMaterials();
}

void CHARACTER::ClearClWindowMaterials()
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetClWindowMaterials();
	for (int i = 0; i < CL_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			continue;
		
		pkItemMaterial[i]->Lock(false);
		pkItemMaterial[i] = NULL;
	}
}

BYTE CHARACTER::CheckClEmptyMaterialSlot()
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetClWindowMaterials();
	for (int i = 0; i < CL_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			return i;
	}
	
	return 255;
}

void CHARACTER::AddClMaterial(TItemPos tPos, BYTE bPos)
{
	if (!isChangeLookOpened())
		return;
	else if (bPos >= CL_WINDOW_MAX_MATERIALS)
	{
		if (bPos != 255)
			return;
		
		bPos = CheckClEmptyMaterialSlot();
		if (bPos >= CL_WINDOW_MAX_MATERIALS)
			return;
	}
	
	LPITEM pkItem = GetItem(tPos);
	if (!pkItem)
		return;
	else if ((pkItem->GetCell() >= INVENTORY_MAX_NUM) || (tPos.IsBeltInventoryPosition()))
		return;
	else if (pkItem->IsEquipped())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] You cannot transmute an item while it is equipped."));
		return;
	}
	else if ((pkItem->GetType() != ITEM_WEAPON) && (pkItem->GetType() != ITEM_ARMOR) && (pkItem->GetType() != ITEM_COSTUME))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] This item cannot be transmuted."));
		return;
	}
#ifdef __NEW_ARROW_SYSTEM__
	else if ((pkItem->GetType() == ITEM_WEAPON) && ((pkItem->GetSubType() == WEAPON_ARROW) || (pkItem->GetSubType() == WEAPON_MOUNT_SPEAR) || (pkItem->GetSubType() == WEAPON_UNLIMITED_ARROW)))
#else
	else if ((pkItem->GetType() == ITEM_WEAPON) && ((pkItem->GetSubType() == WEAPON_ARROW) || (pkItem->GetSubType() == WEAPON_MOUNT_SPEAR)))
#endif
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] This item cannot be transmuted."));
		return;
	}
	else if ((pkItem->GetType() == ITEM_ARMOR) && (pkItem->GetSubType() != ARMOR_BODY))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] This item cannot be transmuted."));
		return;
	}
#ifdef __WEAPON_COSTUME_SYSTEM__
	else if ((pkItem->GetType() == ITEM_COSTUME) && (pkItem->GetSubType() != COSTUME_BODY) && (pkItem->GetSubType() != COSTUME_HAIR) && (pkItem->GetSubType() != COSTUME_WEAPON))
#else
	else if ((pkItem->GetType() == ITEM_COSTUME) && (pkItem->GetSubType() != COSTUME_BODY) && (pkItem->GetSubType() != COSTUME_HAIR))
#endif
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] This item cannot be transmuted."));
		return;
	}
#ifdef __SOULBINDING_SYSTEM__
	else if ((pkItem->IsBind()) || (pkItem->IsUntilBind()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] You can't add binded items."));
		return;
	}
#endif
	else if (pkItem->isLocked())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] You can't add locked items."));
		return;
	}
	else if (pkItem->GetTransmutation() != 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] You can't add items which are transmuted yet."));
		return;
	}
	
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetClWindowMaterials();
	if ((bPos == 1) && (!pkItemMaterial[0]))
		return;
	
	if (pkItemMaterial[bPos])
		return;
	
	if (bPos == 1)
	{
		bool bStop = false;
		if (pkItemMaterial[0]->GetType() != pkItem->GetType())
			bStop = true;
		else if (pkItemMaterial[0]->GetSubType() != pkItem->GetSubType())
			bStop = true;
		else if (pkItemMaterial[0]->GetOriginalVnum() == pkItem->GetOriginalVnum())
			bStop = true;
		else if (((IS_SET(pkItemMaterial[0]->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE)) && (!IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE))) || ((IS_SET(pkItemMaterial[0]->GetAntiFlag(), ITEM_ANTIFLAG_MALE)) && (!IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_MALE))))
			bStop = true;
		else if ((pkItem->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR) && (!IS_SET(pkItemMaterial[0]->GetAntiFlag(), ITEM_ANTIFLAG_WARRIOR)))
			bStop = true;
		else if ((pkItem->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN) && (!IS_SET(pkItemMaterial[0]->GetAntiFlag(), ITEM_ANTIFLAG_ASSASSIN)))
			bStop = true;
		else if ((pkItem->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN) && (!IS_SET(pkItemMaterial[0]->GetAntiFlag(), ITEM_ANTIFLAG_SHAMAN)))
			bStop = true;
		else if ((pkItem->GetAntiFlag() & ITEM_ANTIFLAG_SURA) && (!IS_SET(pkItemMaterial[0]->GetAntiFlag(), ITEM_ANTIFLAG_SURA)))
			bStop = true;
#ifdef __WOLFMAN_CHARACTER__
		else if ((pkItem->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN) && (!IS_SET(pkItemMaterial[0]->GetAntiFlag(), ITEM_ANTIFLAG_WOLFMAN)))
			bStop = true;
#endif
		
		if (bStop)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] You cannot submit this item."));
			return;
		}
	}
	
	pkItemMaterial[bPos] = pkItem;
	pkItemMaterial[bPos]->Lock(true);
	
	TPacketChangeLook sPacket;
	sPacket.header = HEADER_GC_CL;
	sPacket.subheader = CL_SUBHEADER_ADD;
	sPacket.dwCost = 0;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	GetDesc()->Packet(&sPacket, sizeof(TPacketChangeLook));
}

void CHARACTER::RemoveClMaterial(BYTE bPos)
{
	if (bPos >= CL_WINDOW_MAX_MATERIALS)
		return;
	
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetClWindowMaterials();
	
	if (!pkItemMaterial[bPos])
		return;
	
	if (bPos == 1)
	{
		pkItemMaterial[bPos]->Lock(false);
		pkItemMaterial[bPos] = NULL;
	}
	else
		ClearClWindowMaterials();
	
	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;
	
	TPacketChangeLook sPacket;
	sPacket.header = HEADER_GC_CL;
	sPacket.subheader = CL_SUBHEADER_REMOVE;
	sPacket.dwCost = 0;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	GetDesc()->Packet(&sPacket, sizeof(TPacketChangeLook));
}

void CHARACTER::RefineClMaterials()
{
	LPITEM * pkItemMaterial;
	pkItemMaterial = GetClWindowMaterials();
	if (!pkItemMaterial[0])
		return;
	else if (!pkItemMaterial[1])
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] Please submit the item you want to transmute."));
		return;
	}
	
	long long dwPrice = CL_TRANSMUTATION_PRICE;
	if (GetGold() < dwPrice)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] You don't have enough Yang."));
		return;
	}
	
	DWORD dwVnum = pkItemMaterial[1]->GetVnum();
	PointChange(POINT_GOLD, -dwPrice);
	DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);
	ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "TRANSMUTED (SUCCESSFULLY)");
	
	pkItemMaterial[0]->SetTransmutation(dwVnum, true);
	ClearClWindowMaterials();
	
	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;
	
	TPacketChangeLook sPacket;
	sPacket.header = HEADER_GC_CL;
	sPacket.subheader = CL_SUBHEADER_REFINE;
	sPacket.dwCost = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	GetDesc()->Packet(&sPacket, sizeof(TPacketChangeLook));
}

bool CHARACTER::CleanTransmutation(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;
	else if ((!pkItem) || (!pkTarget))
		return false;
	else if ((pkTarget->GetType() != ITEM_WEAPON) && (pkTarget->GetType() != ITEM_ARMOR) && (pkTarget->GetType() != ITEM_COSTUME))
		return false;
#ifdef __SOULBINDING_SYSTEM__
	else if ((pkTarget->IsBind()) || (pkTarget->IsUntilBind()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't remove the transmute because item is binded."));
		return false;
	}
#endif
	else if (pkTarget->isLocked())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't remove the transmute because item is locked."));
		return false;
	}
	
	if (pkTarget->GetTransmutation() == 0)
		return false;
	
	pkTarget->SetTransmutation(0);
	pkItem->SetCount(pkItem->GetCount() - 1);
	LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT (CLEAN TRANSMUTED)", pkTarget->GetName());
	return true;
}
#endif

void CHARACTER::MountSummon()
{
	if (GetMapIndex() == OXEVENT_MAP_INDEX)
		return;

	uint32_t mobVnum = GetMyHorseVnum();
	HorseSummon(true, false, mobVnum);
}

void CHARACTER::MountUnsummon()
{
	LPITEM mountItem = GetWear(WEAR_COSTUME_MOUNT);

	if (mountItem)
	{
		UnequipItem(mountItem);
	}
	else if (GetHorse())
	{
		if (IsRiding())
			StopRiding();

		HorseSummon(false);
	}
}

void CHARACTER::CheckMount()
{
	if (GetMapIndex() == OXEVENT_MAP_INDEX)
		return;

	LPITEM mountItem = GetWear(WEAR_COSTUME_MOUNT);

	if (!mountItem)
	{
		sys_err("no mount item");
		return;
	}
	
	EnterHorse(this);
}

#ifdef __SKILL_COSTUME__
void CHARACTER::SetSkillMotion(BYTE * bSkillMotion)
{
	memcpy(m_bSkillMotion, bSkillMotion, sizeof(m_bSkillMotion));
	UpdatePacket();
}

BYTE* CHARACTER::GetSkillMotion() 
{ 
#ifdef __FAKE_PC__
	if (FakePC_Check())
		return FakePC_GetOwner()->GetSkillMotion();
#endif	
	return m_bSkillMotion; 
}

void CHARACTER::UseSkillCostumeItem(LPITEM item, bool bEquip)
{
	BYTE bSkillEffect = item->GetValue(4);

	if (bEquip)
		m_bSkillMotion[item->GetSubType()] = bSkillEffect;
	else
		m_bSkillMotion[item->GetSubType()] = 0;
	
	UpdatePacket();
}
#endif


#ifdef ENABLE_EXTENDED_BATTLE_PASS
void CHARACTER::SetLastReciveExtBattlePassInfoTime(DWORD time)
{
	m_dwLastReciveExtBattlePassInfoTime = time;
}

void CHARACTER::SetLastReciveExtBattlePassOpenRanking(DWORD time)
{
	m_dwLastExtBattlePassOpenRankingTime = time;
}

void CHARACTER::LoadExtBattlePass(DWORD dwCount, TPlayerExtBattlePassMission* data)
{
	m_bIsLoadedExtBattlePass = false;

	for (int i = 0; i < dwCount; ++i, ++data)
	{
		TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
		newMission->dwPlayerId = data->dwPlayerId;
		newMission->dwBattlePassType = data->dwBattlePassType;
		newMission->dwMissionIndex = data->dwMissionIndex;
		newMission->dwMissionType = data->dwMissionType;
		newMission->dwBattlePassId = data->dwBattlePassId;
		newMission->dwExtraInfo = data->dwExtraInfo;
		newMission->bCompleted = data->bCompleted;
		newMission->bIsUpdated = data->bIsUpdated;

		m_listExtBattlePass.push_back(newMission);
	}

	m_bIsLoadedExtBattlePass = true;
}

DWORD CHARACTER::GetExtBattlePassMissionProgress(DWORD dwBattlePassType, BYTE bMissionIndex, BYTE bMissionType)
{
	DWORD BattlePassID;
	if (dwBattlePassType == 1)
		BattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (dwBattlePassType == 2)
		BattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else if (dwBattlePassType == 3)
		BattlePassID = CBattlePassManager::instance().GetEventBattlePassID();
	else {
		sys_err("Unknown BattlePassType (%d)", dwBattlePassType);
		return 0;
	}
	
	ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
	while (it != m_listExtBattlePass.end())
	{
		TPlayerExtBattlePassMission* pkMission = *it++;
		if (pkMission->dwBattlePassType == dwBattlePassType && pkMission->dwMissionIndex == bMissionIndex && pkMission->dwMissionType == bMissionType && pkMission->dwBattlePassId == BattlePassID)
			return pkMission->dwExtraInfo;
	}
	return 0;
}

bool CHARACTER::IsExtBattlePassCompletedMission(DWORD dwBattlePassType, BYTE bMissionIndex, BYTE bMissionType)
{
	DWORD BattlePassID;
	if (dwBattlePassType == 1)
		BattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (dwBattlePassType == 2)
		BattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else if (dwBattlePassType == 3)
		BattlePassID = CBattlePassManager::instance().GetEventBattlePassID();
	else {
		sys_err("Unknown BattlePassType (%d)", dwBattlePassType);
		return false;
	}
	ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
	while (it != m_listExtBattlePass.end())
	{
		TPlayerExtBattlePassMission* pkMission = *it++;
		if (pkMission->dwBattlePassType == dwBattlePassType && pkMission->dwMissionIndex == bMissionIndex && pkMission->dwMissionType == bMissionType && pkMission->dwBattlePassId == BattlePassID)
			return (pkMission->bCompleted ? true : false);
	}
	return false;
}

bool CHARACTER::IsExtBattlePassRegistered(BYTE bBattlePassType, DWORD dwBattlePassID)
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT * FROM player.battlepass_playerindex WHERE player_id = %d and battlepass_type = %d and battlepass_id = %d", GetPlayerID(), bBattlePassType, dwBattlePassID));
	if (pMsg->Get()->uiNumRows != 0)
		return true;

	return false;
}

void CHARACTER::UpdateExtBattlePassMissionProgress(DWORD dwMissionType, DWORD dwUpdateValue, DWORD dwCondition, bool isOverride)
{
	if (!GetDesc())
		return;

	if (!m_bIsLoadedExtBattlePass)
		return;

	DWORD dwSafeCondition = dwCondition;
	for (BYTE bBattlePassType = 1; bBattlePassType <= 3 ; ++bBattlePassType)
	{
		bool foundMission = false;
		DWORD dwSaveProgress = 0;
		dwCondition = dwSafeCondition;
		
		BYTE bBattlePassID;
		BYTE bMissionIndex = CBattlePassManager::instance().GetMissionIndex(bBattlePassType, dwMissionType, dwCondition);

		if (bBattlePassType == 1)
			bBattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
		if (bBattlePassType == 2){
			bBattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
			if (bBattlePassID != GetExtBattlePassPremiumID())
				continue;
		}
		if (bBattlePassType == 3)
			bBattlePassID = CBattlePassManager::instance().GetEventBattlePassID();

		DWORD dwFirstInfo, dwSecondInfo;
		if (CBattlePassManager::instance().BattlePassMissionGetInfo(bBattlePassType, bMissionIndex, bBattlePassID, dwMissionType, &dwFirstInfo, &dwSecondInfo))
		{
			if (dwFirstInfo == 0)
				dwCondition = 0;
			
			if (dwMissionType == 2 and dwFirstInfo <= dwCondition or dwMissionType == 4 and dwFirstInfo <= dwCondition or dwMissionType == 20 and dwFirstInfo <= dwCondition)
				dwCondition = dwFirstInfo;

			if (dwFirstInfo == dwCondition && GetExtBattlePassMissionProgress(bBattlePassType, bMissionIndex, dwMissionType) < dwSecondInfo)
			{
				ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
				while (it != m_listExtBattlePass.end())
				{
					TPlayerExtBattlePassMission* pkMission = *it++;

					if (pkMission->dwBattlePassType == bBattlePassType && pkMission->dwMissionIndex == bMissionIndex && pkMission->dwMissionType == dwMissionType && pkMission->dwBattlePassId == bBattlePassID)
					{
						pkMission->bIsUpdated = 1;

						if (isOverride)
							pkMission->dwExtraInfo = dwUpdateValue;
						else
							pkMission->dwExtraInfo += dwUpdateValue;

						if (pkMission->dwExtraInfo >= dwSecondInfo)
						{
							pkMission->dwExtraInfo = dwSecondInfo;
							pkMission->bCompleted = 1;

							std::string stMissionName = CBattlePassManager::instance().GetMissionNameByType(pkMission->dwMissionType);
							std::string stBattlePassName = CBattlePassManager::instance().GetNormalBattlePassNameByID(pkMission->dwBattlePassId);

							CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, bBattlePassID, bMissionIndex);
							if (bBattlePassType == 1) {
								EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
								ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
							}
							if (bBattlePassType == 2) {
								EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
								ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
							}
							if (bBattlePassType == 3) {
								EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
								ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
							}
							
							TPacketGCExtBattlePassMissionUpdate packet;
							packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
							packet.bBattlePassType = bBattlePassType;
							packet.bMissionIndex = bMissionIndex;
							packet.dwNewProgress = pkMission->dwExtraInfo;
							GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
						}

						dwSaveProgress = pkMission->dwExtraInfo;
						foundMission = true;

						if (pkMission->bCompleted != 1) {
							TPacketGCExtBattlePassMissionUpdate packet;
							packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
							packet.bBattlePassType = bBattlePassType;
							packet.bMissionIndex = bMissionIndex;
							packet.dwNewProgress = dwSaveProgress;
							GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
						}
						break;
					}
					
				}

				if (!foundMission)
				{
					if (!IsExtBattlePassRegistered(bBattlePassType, bBattlePassID))
						DBManager::instance().DirectQuery("INSERT INTO player.battlepass_playerindex SET player_id=%d, player_name='%s', battlepass_type=%d, battlepass_id=%d, start_time=NOW()", GetPlayerID(), GetName(), bBattlePassType, bBattlePassID);
					
					TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
					newMission->dwPlayerId = GetPlayerID();
					newMission->dwBattlePassType = bBattlePassType;
					newMission->dwMissionType = dwMissionType;
					newMission->dwBattlePassId = bBattlePassID;

					if (dwUpdateValue >= dwSecondInfo)
					{
						newMission->dwMissionIndex = CBattlePassManager::instance().GetMissionIndex(bBattlePassType, dwMissionType, dwCondition);
						newMission->dwExtraInfo = dwSecondInfo;
						newMission->bCompleted = 1;

						CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, bBattlePassID, bMissionIndex);
						if (bBattlePassType == 1) {
							EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
							ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
						}
						if (bBattlePassType == 2) {
							EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
							ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
						}
						if (bBattlePassType == 3) {
							EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
							ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
						}

						dwSaveProgress = dwSecondInfo;
					}
					else
					{
						newMission->dwMissionIndex = CBattlePassManager::instance().GetMissionIndex(bBattlePassType, dwMissionType, dwCondition);
						newMission->dwExtraInfo = dwUpdateValue;
						newMission->bCompleted = 0;

						dwSaveProgress = dwUpdateValue;
					}

					newMission->bIsUpdated = 1;

					m_listExtBattlePass.push_back(newMission);

					TPacketGCExtBattlePassMissionUpdate packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
					packet.bBattlePassType = bBattlePassType;
					packet.bMissionIndex = bMissionIndex;
					packet.dwNewProgress = dwSaveProgress;
					GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				}
			}
		}
	}
}

void CHARACTER::SetExtBattlePassMissionProgress(BYTE bBattlePassType, DWORD dwMissionIndex, DWORD dwMissionType, DWORD dwUpdateValue)
{
	if (!GetDesc())
		return;

	if (!m_bIsLoadedExtBattlePass)
		return;

	bool foundMission = false;
	DWORD dwSaveProgress = 0;
	
	BYTE bBattlePassID;
	if (bBattlePassType == 1)
		bBattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (bBattlePassType == 2)
		bBattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else if (bBattlePassType == 3)
		bBattlePassID = CBattlePassManager::instance().GetEventBattlePassID();
	else {
		sys_err("Unknown BattlePassType (%d)", bBattlePassType);
		return;
	}
	
	DWORD dwFirstInfo, dwSecondInfo;
	if (CBattlePassManager::instance().BattlePassMissionGetInfo(bBattlePassType, dwMissionIndex, bBattlePassID, dwMissionType, &dwFirstInfo, &dwSecondInfo))
	{
		ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
		while (it != m_listExtBattlePass.end())
		{
			TPlayerExtBattlePassMission* pkMission = *it++;

			if (pkMission->dwBattlePassType == bBattlePassType && pkMission->dwMissionIndex == dwMissionIndex && pkMission->dwMissionType == dwMissionType && pkMission->dwBattlePassId == bBattlePassID)
			{
				pkMission->bIsUpdated = 1;
				pkMission->bCompleted = 0;
				
				pkMission->dwExtraInfo = dwUpdateValue;

				if (pkMission->dwExtraInfo >= dwSecondInfo)
				{
					pkMission->dwExtraInfo = dwSecondInfo;
					pkMission->bCompleted = 1;

					std::string stMissionName = CBattlePassManager::instance().GetMissionNameByType(pkMission->dwMissionType);
					std::string stBattlePassName = CBattlePassManager::instance().GetNormalBattlePassNameByID(pkMission->dwBattlePassId);
					//ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("New Value : %d"), pkMission->dwExtraInfo);

					CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, bBattlePassID, dwMissionIndex);
					if (bBattlePassType == 1) {
						EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
						ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
					}
					if (bBattlePassType == 2) {
						EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
						ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
					}
					if (bBattlePassType == 3) {
						EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
						ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
					}
					
					TPacketGCExtBattlePassMissionUpdate packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
					packet.bBattlePassType = bBattlePassType;
					packet.bMissionIndex = dwMissionIndex;
					packet.dwNewProgress = pkMission->dwExtraInfo;
					GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				}

				dwSaveProgress = pkMission->dwExtraInfo;
				foundMission = true;

				if (pkMission->bCompleted != 1) {
					TPacketGCExtBattlePassMissionUpdate packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
					packet.bBattlePassType = bBattlePassType;
					packet.bMissionIndex = dwMissionIndex;
					packet.dwNewProgress = dwSaveProgress;
					GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				}
				break;
			}
		}

		if (!foundMission)
		{
			if (!IsExtBattlePassRegistered(bBattlePassType, bBattlePassID))
				DBManager::instance().DirectQuery("INSERT INTO player.battlepass_playerindex SET player_id=%d, player_name='%s', battlepass_type=%d, battlepass_id=%d, start_time=NOW()", GetPlayerID(), GetName(), bBattlePassType, bBattlePassID);

			TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
			newMission->dwPlayerId = GetPlayerID();
			newMission->dwBattlePassType = bBattlePassType;
			newMission->dwMissionType = dwMissionType;
			newMission->dwBattlePassId = bBattlePassID;

			if (dwUpdateValue >= dwSecondInfo)
			{
				newMission->dwMissionIndex = dwMissionIndex;
				newMission->dwExtraInfo = dwSecondInfo;
				newMission->bCompleted = 1;

				CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, bBattlePassID, dwMissionIndex);
				if (bBattlePassType == 1) {
					EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
					ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_NORMAL_MISSION"));
				}
				if (bBattlePassType == 2) {
					EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
					ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_PREMIUM_MISSION"));
				}
				if (bBattlePassType == 3) {
					EffectPacket(SE_EFFECT_BP_EVENT_MISSION_COMPLETED);
					ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("BATTLEPASS_COMPLETE_EVENT_MISSION"));
				}

				dwSaveProgress = dwSecondInfo;
			}
			else
			{
				newMission->dwMissionIndex = dwMissionIndex;
				newMission->dwExtraInfo = dwUpdateValue;
				newMission->bCompleted = 0;

				dwSaveProgress = dwUpdateValue;
			}

			newMission->bIsUpdated = 1;

			m_listExtBattlePass.push_back(newMission);

			TPacketGCExtBattlePassMissionUpdate packet;
			packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
			packet.bBattlePassType = bBattlePassType;
			packet.bMissionIndex = dwMissionIndex;
			packet.dwNewProgress = dwSaveProgress;
			GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
		}
	}
}
#endif

#ifdef ENABLE_BIYOLOG
void CHARACTER::CheckBio()
{
	if (!IsPC())
		return;
	
	int level = GetQuestFlag("bio.level");
	if (level == 0)
	{
		level += 1;
		SetQuestFlag("bio.level", level);
	}
	else if (level >= bio_max)
		return;

	int count = GetQuestFlag("bio.count");
	if (count == bio_data[level][2])
	{
		if (bio_data[level][5] != 0)
			ChatPacket(CHAT_TYPE_COMMAND, "biostone %d", level);
		else
		{
			if (bio_data[level][14] == 1)
				ChatPacket(CHAT_TYPE_COMMAND, "bioodul %d", level);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", level, count, GetQuestFlag("bio.time"));
	}
}
#endif

#ifdef __HIDE_COSTUME_SYSTEM__
void CHARACTER::SetBodyCostumeHidden(bool hidden)
{
	m_bHideBodyCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetBodyCostumeHidden %d", m_bHideBodyCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_body", m_bHideBodyCostume ? 1 : 0);
}

void CHARACTER::SetHairCostumeHidden(bool hidden)
{
	m_bHideHairCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetHairCostumeHidden %d", m_bHideHairCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_hair", m_bHideHairCostume ? 1 : 0);
}

#ifdef __SASH_SYSTEM__
void CHARACTER::SetSashCostumeHidden(bool hidden)
{
	m_bHideSashCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetSashCostumeHidden %d", m_bHideSashCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_sash", m_bHideSashCostume ? 1 : 0);
}
#endif

#ifdef __WEAPON_COSTUME_SYSTEM__
void CHARACTER::SetWeaponCostumeHidden(bool hidden)
{
	m_bHideWeaponCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetWeaponCostumeHidden %d", m_bHideWeaponCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_weapon", m_bHideWeaponCostume ? 1 : 0);
}
#endif
#endif

#ifdef __PICKUP_FILTER__
bool CHARACTER::IsFilteredPickupItem(uint32_t dwVnum, uint16_t wPickupFilterFlag)
{
	const TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(dwVnum);
	if (!pItemTable)
		return false;

	switch (pItemTable->bType)
	{
		case EItemTypes::ITEM_WEAPON:
			return (wPickupFilterFlag & EPickupFilterBlockModes::PICKUP_BLOCK_MODE_WEAPON) ? true : false; 
			
		case EItemTypes::ITEM_ARMOR:
			{
				switch (pItemTable->bSubType)
				{
					case EArmorSubTypes::ARMOR_BODY:
						return (wPickupFilterFlag & EPickupFilterBlockModes::PICKUP_BLOCK_MODE_ARMOR) ? true : false; 
					case EArmorSubTypes::ARMOR_HEAD:
						return (wPickupFilterFlag & EPickupFilterBlockModes::PICKUP_BLOCK_MODE_HEAD) ? true : false; 
					case EArmorSubTypes::ARMOR_SHIELD:
						return (wPickupFilterFlag & EPickupFilterBlockModes::PICKUP_BLOCK_MODE_SHIELD) ? true : false; 
					case EArmorSubTypes::ARMOR_WRIST:
						return (wPickupFilterFlag & EPickupFilterBlockModes::PICKUP_BLOCK_MODE_WRIST) ? true : false; 
					case EArmorSubTypes::ARMOR_FOOTS:
						return (wPickupFilterFlag & EPickupFilterBlockModes::PICKUP_BLOCK_MODE_FOOTS) ? true : false; 
					case EArmorSubTypes::ARMOR_NECK:
						return (wPickupFilterFlag & EPickupFilterBlockModes::PICKUP_BLOCK_MODE_NECK) ? true : false; 
					case EArmorSubTypes::ARMOR_EAR:
						return (wPickupFilterFlag & EPickupFilterBlockModes::PICKUP_BLOCK_MODE_EAR) ? true : false; 
					default:
						return false;
				}
			}
			break;
			
		default:
			return (wPickupFilterFlag & EPickupFilterBlockModes::PICKUP_BLOCK_MODE_ETC) ? true : false; 
	}
	
	return false;
}
#endif

#ifdef __SKILL_COLOR_SYSTEM__
void CHARACTER::SetSkillColor(DWORD * dwSkillColor)
{
	memcpy(m_dwSkillColor, dwSkillColor, sizeof(m_dwSkillColor));
	UpdatePacket();
}
#endif

#ifdef __VIP_SYSTEM__
bool CHARACTER::IsVIP() const
{
	auto pAffect = FindAffect(EAffectTypes::AFFECT_VIP); 
	if (nullptr == pAffect)
		return false;
	
	return true;
}

bool CHARACTER::ItemProcessVIP(LPITEM item)
{
	if (IsVIP()) 
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are already a VIP member."));
		return false;
	}
	
	const long lDuration = item->GetSocket(0);
	if (lDuration <= 0)
		return false;
	
	AddAffect(EAffectTypes::AFFECT_VIP, POINT_NONE, 0, EAffectBits::AFF_VIP, lDuration, 0, false, false); 
	item->SetCount(item->GetCount() - 1);
	return true;
}

bool CHARACTER::RemoveVIP()
{
	const auto pAffect = FindAffect(EAffectTypes::AFFECT_VIP); 
	if (nullptr == pAffect)
		return false;
	
	RemoveAffect(const_cast<CAffect*>(pAffect));
	return true;
}
#endif

//void CHARACTER::SetProtectTime(const std::string& flagname, int value)
//{
//	auto it = m_protection_Time.find(flagname);
//	if (it != m_protection_Time.end())
//		it->second = value;
//	else
//		m_protection_Time.insert(make_pair(flagname, value));
//}
//int CHARACTER::GetProtectTime(const std::string& flagname) const
//{
//	auto it = m_protection_Time.find(flagname);
//	if (it != m_protection_Time.end())
//		return it->second;
//	return 0;
//}

#ifdef __HWID_BAN__
std::string CHARACTER::GetHardwareID()
{
	std::string hwid = GetDesc()->GetAccountTable().hwid;
	return hwid;
}
#endif

#ifdef __PROMO_CODE__
enum EPromotionRewardTypes
{
	PROMOTION_REWARD_NONE,
	PROMOTION_REWARD_ITEM,
	PROMOTION_REWARD_YANG,
	PROMOTION_REWARD_LEVEL,
	PROMOTION_REWARD_COINS
};

void CHARACTER::SetPromotionCodeRestriction(const char* szEscapePromotionCode)
{
	DBManager::instance().DirectQuery("INSERT INTO player.promotion_code_restriction("
		"promotion_code, account_id, ip"
#ifdef __HWID_BAN__	
		", hwid"
#endif	
		") VALUES("
		"'%s', %d, '%s'"
#ifdef __HWID_BAN__		
		", '%s'"
#endif	
		")", 
		szEscapePromotionCode, 
		GetDesc()->GetAccountTable().id, 
		GetDesc()->GetHostName()
#ifdef __HWID_BAN__		
		,GetHardwareID().c_str()
#endif
		);
}

bool CHARACTER::CanUsePromotionCode(const char* szPromotionCode)
{
	char szEscapePromotionCode[PROMOTION_CODE_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szEscapePromotionCode, sizeof(szEscapePromotionCode), szPromotionCode, strlen(szPromotionCode));

	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT * FROM player.promotion_code_restriction WHERE "
		"(account_id = %d "
		"OR ip = '%s' "
#ifdef __HWID_BAN__
		"OR hwid = '%s' "
#endif
		")"
		"AND promotion_code ='%s'", 
		GetDesc()->GetAccountTable().id, 
		GetDesc()->GetHostName(),
#ifdef __HWID_BAN__
		GetHardwareID().c_str(),
#endif		
		szEscapePromotionCode);
		
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows == 0)
		return true;
			
	return false;
}

void CHARACTER::RewardPromotionCode(const char* szPromotionCode)
{
	char szEscapePromotionCode[PROMOTION_CODE_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szEscapePromotionCode, sizeof(szEscapePromotionCode), szPromotionCode, strlen(szPromotionCode));
	
	//checking promo code
	{
		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery),"SELECT level, promotion_code FROM player.promotion_code WHERE count > 0 and promotion_code ='%s'", szEscapePromotionCode);
		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		if (pMsg->Get()->uiNumRows == 0)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This promotion code dont exist's or is already used up."));
			return;
		}
			
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		DWORD dwLevel = 0;
		str_to_number(dwLevel, row[0]);
		
		if (GetLevel() < dwLevel)	
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need level %d to use this promotion code."), dwLevel);
			return;
		}			
	}
	
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),"SELECT category+0, vnum, count FROM player.promotion_code_items WHERE promotion_code ='%s'", szEscapePromotionCode);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows == 0)
		return;
			
	BYTE bCategory = 0;
	DWORD dwVnum = 0;
	long long count = 0;
	
	for (size_t i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		
		str_to_number(bCategory, row[0]);
		str_to_number(dwVnum, row[1]);
		str_to_number(count, row[2]);
				
		switch (bCategory)
		{
			case PROMOTION_REWARD_ITEM:
				AutoGiveItem(dwVnum, count);
				break;
				
			case PROMOTION_REWARD_YANG:
				PointChange(POINT_GOLD, count, true);
				//ChangeGold(gold);
				break;

			case PROMOTION_REWARD_LEVEL:
				PointChange(POINT_LEVEL, count, false, true);
				break;

			case PROMOTION_REWARD_COINS:
				DBManager::instance().DirectQuery("UPDATE account.account set coins = coins+%d WHERE id=%d", count, GetDesc()->GetAccountTable().id);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You received %d itemshop coins."), count);
				break;				
		}
	}
	
	DBManager::instance().DirectQuery("UPDATE player.promotion_code SET count = count-1 WHERE promotion_code ='%s'", szEscapePromotionCode);
	SetPromotionCodeRestriction(szEscapePromotionCode);
}
#endif

#ifdef __FAKE_PLAYER__
void CHARACTER::FakePlayerCreatePetSystem()
{
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;
	}

	m_petSystem = M2_NEW CPetSystem(this);
}
#endif

#ifdef ENABLE_SORT_INVEN
void CHARACTER::SortInven(BYTE option, uint8_t bInventoryType)
{
	if (IsDead()) 
		return;

	if (!CanWarp())
		return;
	
	if (GetLastSortTime() > get_global_time()) 
	{
		ChatPacket(CHAT_TYPE_INFO, "You need to wait %d sec.", GetLastSortTime() - get_global_time());
		return;
	}

	std::vector<LPITEM> all;
	LPITEM myitems;
	const auto size = static_cast<WORD>(INVENTORY_MAX_NUM);

	switch (bInventoryType)
	{
		case ESpecialInventoryTypes::INVENTORY_TYPE_SKILLBOOK:
			{
				for (int i = SPECIAL_INVENTORY_SKILLBOOK_START; i < SPECIAL_INVENTORY_SKILLBOOK_END; ++i)
				{
					if (myitems = GetInventoryItem(i)) 
					{
						all.emplace_back(myitems);
						myitems->RemoveFromCharacter();
						SyncQuickslot(QUICKSLOT_TYPE_ITEM, static_cast<BYTE>(i), 255);
					}
				}
			}
			break;
			
		case ESpecialInventoryTypes::INVENTORY_TYPE_STONE:
			{
				for (int i = SPECIAL_INVENTORY_STONE_START; i < SPECIAL_INVENTORY_STONE_END; ++i)
				{
					if (myitems = GetInventoryItem(i)) 
					{
						all.emplace_back(myitems);
						myitems->RemoveFromCharacter();
						SyncQuickslot(QUICKSLOT_TYPE_ITEM, static_cast<BYTE>(i), 255);
					}
				}
			}
			break;
			
		case ESpecialInventoryTypes::INVENTORY_TYPE_MATERIAL:
			{
				for (int i = SPECIAL_INVENTORY_MATERIAL_START; i < SPECIAL_INVENTORY_MATERIAL_END; ++i)
				{
					if (myitems = GetInventoryItem(i)) 
					{
						all.emplace_back(myitems);
						myitems->RemoveFromCharacter();
						SyncQuickslot(QUICKSLOT_TYPE_ITEM, static_cast<BYTE>(i), 255);
					}
				}
			}
			break;
			
		case ESpecialInventoryTypes::INVENTORY_TYPE_INVENTORY:
		default:
			{
				for (WORD i = 0; i < size; ++i) 
				{
					if (myitems = GetInventoryItem(i)) 
					{
						all.emplace_back(myitems);
						myitems->RemoveFromCharacter();
						SyncQuickslot(QUICKSLOT_TYPE_ITEM, static_cast<BYTE>(i), 255);
					}
				}
			}
			break;
	}
	
	if (all.empty())
		return;
	
	std::sort(all.begin(), all.end(), [option](const LPITEM i1, const LPITEM i2) 
	{
		switch (option) 
		{
		case 2:
			return i1->CustomSort() == i2->CustomSort() ? i1->GetSubType() < i2->GetSubType() : i1->CustomSort() < i2->CustomSort();
		case 3:
			return i1->GetLevelLimit() == i2->GetLevelLimit() ? i1->GetSubType() < i2->GetSubType() : i1->GetLevelLimit() > i2->GetLevelLimit();
		default:
			return std::strcmp(i1->GetName(), i2->GetName()) < 0;
		}
	});
	
	for (const auto& getitem : all) 
	{
		const auto table = ITEM_MANAGER::instance().GetTable(getitem->GetVnum());
		if (!table)
			continue;
		static const std::initializer_list<DWORD> out = { ITEM_AUTO_HP_RECOVERY_S, ITEM_AUTO_HP_RECOVERY_M, ITEM_AUTO_HP_RECOVERY_L, ITEM_AUTO_HP_RECOVERY_X, ITEM_AUTO_SP_RECOVERY_S, ITEM_AUTO_SP_RECOVERY_M, ITEM_AUTO_SP_RECOVERY_L, ITEM_AUTO_SP_RECOVERY_X };
		if (table->dwFlags & ITEM_FLAG_STACKABLE && table->bType != ITEM_BLEND && std::find(out.begin(), out.end(), getitem->GetVnum()) == out.end()) 
		{
			AutoGiveItem(getitem->GetVnum(), getitem->GetCount(), -1, false);
			M2_DESTROY_ITEM(getitem);
		}
		else
			AutoGiveItem(getitem);
	};
	
	SetLastSortTime(get_global_time() + 15); // 15 sec
}
#endif

#ifdef ENABLE_KILL_STATISTICS
void CHARACTER::SendKillStatisticsPacket()
{
	if (!GetDesc())
	{
		return;
	}

	TPacketGCKillStatistics KillStatisticsGC;

	KillStatisticsGC.bHeader = HEADER_GC_KILL_STATISTICS;
	KillStatisticsGC.iJinnoKills = GetJinnoKills();
	KillStatisticsGC.iShinsooKills = GetShinsooKills();
	KillStatisticsGC.iChunjoKills = GetChunjoKills();
	KillStatisticsGC.iTotalKills = GetTotalKills();
	KillStatisticsGC.iDuelsWon = GetDuelsWon();
	KillStatisticsGC.iDuelsLost = GetDuelsLost();
	KillStatisticsGC.iMonstersKills = GetMonstersKills();
	KillStatisticsGC.iBossesKills = GetBossesKills();
	KillStatisticsGC.iStonesKills = GetStonesKills();
	
	GetDesc()->Packet(&KillStatisticsGC, sizeof(TPacketGCKillStatistics));
}
#endif


#ifdef ENABLE_REWARD_SYSTEM
void CHARACTER::SetProtectTime(const std::string& flagname, int value)
{
	itertype(m_protection_Time) it = m_protection_Time.find(flagname);
	if (it != m_protection_Time.end())
		it->second = value;
	else
		m_protection_Time.insert(make_pair(flagname, value));
}
int CHARACTER::GetProtectTime(const std::string& flagname) const
{
	itertype(m_protection_Time) it = m_protection_Time.find(flagname);
	if (it != m_protection_Time.end())
		return it->second;
	return 0;
}
#endif


#ifdef __RENEWAL_BRAVE_CAPE__
struct FuncAggregateMonsterNew
{
	LPCHARACTER m_ch;
	int m_dwRange;
	FuncAggregateMonsterNew(LPCHARACTER ch, int range) : m_ch(ch), m_dwRange(range){}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (!ch->IsMonster() || ch->GetVictim())
				return;
			if (DISTANCE_APPROX(ch->GetX() - m_ch->GetX(), ch->GetY() - m_ch->GetY()) < m_dwRange)
			{
				if (ch->CanBeginFight())
					ch->BeginFight(m_ch);
			}
		}
	}
};
EVENTFUNC(brave_cape_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("<brave_cape_event> <Factor> Null pointer");
		return 0;
	}
	LPCHARACTER	ch = info->ch;
	if (ch == NULL)
		return 0;
	if (!ch->FindAffect(AFFECT_BRAVE_CAPE))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Brave affect is done loop event closed.");
		ch->m_pkBraveCapeEvent = NULL;
		return 0;
	}
	LPSECTREE pSec = ch->GetSectree();
	if (pSec)
	{
		FuncAggregateMonsterNew f(ch, ch->GetProtectTime("brave_cape_range"));
		pSec->ForEachAround(f);
	}
	return PASSES_PER_SEC(ch->GetProtectTime("brave_cape_time"));
}
void CHARACTER::GetBraveCapeCMDCompare(const char* c_pszArgument)
{
	std::vector<std::string> vecArgs;
	split_argument(c_pszArgument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "active")
	{
		if (vecArgs.size() < 4) { return; }
		if (!FindAffect(AFFECT_BRAVE_CAPE))
		{
			ChatPacket(CHAT_TYPE_INFO, "You don't has brave cape affect.");
			return;
		}
		if (m_pkBraveCapeEvent)
		{
			ChatPacket(CHAT_TYPE_INFO, "Already brave cape working..");
			return;
		}
		const std::vector<WORD> m_dwBlockedMapIdx = { 1, 21, 41 };
		if (std::find(m_dwBlockedMapIdx.begin(), m_dwBlockedMapIdx.end(), GetMapIndex()) != m_dwBlockedMapIdx.end())
		{
			ChatPacket(CHAT_TYPE_INFO, "You can't use brave cape on this map!");
			return;
		}

		int loopTime, eventRange;
		if (!str_to_number(loopTime, vecArgs[2].c_str()) || !str_to_number(eventRange, vecArgs[3].c_str()))
			return;

		if (loopTime < 2 || loopTime > 60 || eventRange < 1000 || eventRange > 15000)
			return;

		SetProtectTime("brave_cape_time", loopTime);
		SetProtectTime("brave_cape_range", eventRange);

		char_event_info* info = AllocEventInfo<char_event_info>();
		info->ch = this;
		m_pkBraveCapeEvent = event_create(brave_cape_event, info, 0);
		ChatPacket(CHAT_TYPE_COMMAND, "SetBraveCapeStatus 1");
		ChatPacket(CHAT_TYPE_INFO, "Brave cape event started..");

	}
	else if (vecArgs[1] == "deactive")
	{
		if (!m_pkBraveCapeEvent)
		{
			ChatPacket(CHAT_TYPE_INFO, "You don't has brave cape loop event!");
			return;
		}
		SetProtectTime("brave_cape_time", 0);
		SetProtectTime("brave_cape_range", 0);
		event_cancel(&m_pkBraveCapeEvent);
		m_pkBraveCapeEvent = NULL;
		ChatPacket(CHAT_TYPE_INFO, "Brave cape event closed..");
		ChatPacket(CHAT_TYPE_COMMAND, "SetBraveCapeStatus 0");
	}
}
#endif

#ifdef __GEM_SYSTEM__
void CHARACTER::OpenGemSlot()
{
	if (CountSpecifyItem(OPEN_GEM_SLOT_ITEM) < 1)
	{
		ChatPacket(CHAT_TYPE_INFO, "You don't has enought open gem slot item!");
		return;
	}
	const int openSlotFlag = GetQuestFlag("gem.open_slot");
	if (openSlotFlag < 19)
	{
		SetQuestFlag("gem.open_slot", openSlotFlag + 1);
		ChatPacket(CHAT_TYPE_COMMAND, "GemUpdateSlotCount %d", openSlotFlag + 1);
		RemoveSpecifyItem(OPEN_GEM_SLOT_ITEM, 1);
	}
}
void CHARACTER::RefreshGemPlayer()
{
	if (!m_mapGemItems.size())
	{
		LoadGemItems();
		if (!m_mapGemItems.size())
			RefreshGemItems();
	}

	ChatPacket(CHAT_TYPE_COMMAND, "GemClear");
	ChatPacket(CHAT_TYPE_COMMAND, "GemUpdateSlotCount %d", GetQuestFlag("gem.open_slot"));
	ChatPacket(CHAT_TYPE_COMMAND, "GemSetRefreshLeftTime %d", GetQuestFlag("gem.left_time") - time(0));
	std::string cmdString("");
#if __cplusplus < 199711L
	for (itertype(m_mapGemItems) it = m_mapGemItems.begin(); it != m_mapGemItems.end(); ++it)
	{
		const BYTE& slotPos = it->first;
		const TGemItem& itemData = it->second;
#else
	for (const auto& [slotPos, itemData] : m_mapGemItems)
	{
#endif
		cmdString += std::to_string(slotPos).c_str();
		cmdString += "#";
		cmdString += std::to_string(itemData.itemVnum).c_str();
		cmdString += "#";
		cmdString += std::to_string(itemData.itemCount).c_str();
		cmdString += "#";
		cmdString += std::to_string(itemData.itemPrice).c_str();
		cmdString += "#";
		cmdString += std::to_string(itemData.itemBuyed).c_str();
		cmdString += "|";
		if (cmdString.size() + 50 > CHAT_MAX_LEN)
		{
			ChatPacket(CHAT_TYPE_COMMAND, "GemSetItemsWithString %s", cmdString.c_str());
			cmdString.clear();
		}
	}
	if(cmdString.size())
		ChatPacket(CHAT_TYPE_COMMAND, "GemSetItemsWithString %s", cmdString.c_str());
}

void CHARACTER::RefreshGemALL(bool withRealTime)
{
	int leftTime = 0;
	if (withRealTime && GetQuestFlag("gem.left_time") != 0)
	{
		leftTime = time(0) - GetQuestFlag("gem.left_time");
		if (leftTime < 1)
			leftTime = 0;
	}
	SetQuestFlag("gem.left_time", (time(0) + REFRESH_TIME) - leftTime);
	RefreshGemItems();
	RefreshGemPlayer();
}
void CHARACTER::BuyGemItem(BYTE slotIndex)
{
#if __cplusplus < 199711L
	itertype(m_mapGemItems) it = m_mapGemItems.find(slotIndex);
#else
	auto it = m_mapGemItems.find(slotIndex);
#endif
	if (it != m_mapGemItems.end())
	{
		if (slotIndex >= 9)
		{
			if (GetQuestFlag("gem.open_slot") < (slotIndex - 9) + 1)
				return;
		}

		if (it->second.itemBuyed)
			return;
		else if (it->second.itemPrice > GetGem())
			return;
		it->second.itemBuyed = true;
		PointChange(POINT_GEM, -it->second.itemPrice);
		AutoGiveItem(it->second.itemVnum, it->second.itemCount);
		ChatPacket(CHAT_TYPE_COMMAND, "GemSetBuyedSlot %d 1", slotIndex);
		SaveGemItems();
	}
}
void CHARACTER::RefreshGemItems(bool withItem)
{
	m_mapGemItems.clear();

	// u should set here for gaya items.
#if __cplusplus < 199711L
	const DWORD gemItems[][3] = {
#else
	constexpr DWORD gemItems[][4] = {
#endif
	//	{item,count,price,lucky}
		{30410,1,75,30},  // Gaya Lolli (3 Tage)
		{30411,1,150,20}, // Gaya Lolli (5 Tage)
		{30412,1,200,15}, // Gaya Lolli (7 Tage)
		{30413,1,75,30},  // Gaya Mondring (3 Tage)
		{30414,1,150,20}, // Gaya Mondring (5 Tage)
		{30415,1,200,15}, // Gaya Mondring (7 Tage)
		{25040,5,25,50},  // Segenschriftrolle
		{25041,1,200,10}, // Magischer Stein
		{71032,1,50,45},  // Schriftrolle des Drachen
		{71035,1,150,40},  // Elixir des Forschers
		{30402,1,1,50},   // Gaya Beutel (1x Gaya)
		{30403,1,5,45},   // Gaya Beutel (5x Gaya)
		{30404,1,10,40},  // Gaya Beutel (10x Gaya)
		{30405,1,15,35},  // Gaya Beutel (15x Gaya)
		{30406,1,25,30},  // Gaya Beutel (25x Gaya)
		{30407,1,50,25},  // Gaya Beutel (50x Gaya)
		{30408,1,100,20}, // Gaya Beutel (100x Gaya)
		{39064,1,200,25}, // Gayahandelerweiterung
		{39063,1,200,30}, // Gayahandel zurucksetzen
		{70039,1,75,50},  // Schmiedehandbuch
		{71020,1,200,50}, // Segen des Lebens
		{27992,1,20,75},  // Weiße Perle
		{27993,1,30,75},  // Blaue Perle
		{27994,1,50,50},  // Rote Perle
		{27987,5,30,100}, // Muschel
		{50513,1,100,50}, // Seelenstein
		{50517,1,50,50},  // Techstein
		{73923,1,200,20}, // Skill Kostum Truhe
		{70102,5,30,75},  // Zen-Bohne
		{71012,1,30,75},  // Buch des Anfuhrers
		{71021,1,40,50},  // Schriftrolle des Krieges
		{27799,5,25,50},  // Graten
		{50621,1,70,40},  // Diamant
		{18900,1,100,25}, // Rotflammestein
		{50516,1,250,10}, // Buch der Dunkelheit ( G1-G10 auf Pskill)
		{77289,1,500,10}, // DSS Switcher
		{77290,1,500,10}, // FKS Switcher
		{71051,1,200,35}, // Gegenstand Verhexen (6/7) Bonus
		{71052,1,180,15}, // Gegenstand Segnen (6/7) Bonus
		{82148,1,100,30}, // Truhe der Weißheit
		{38057,1,100,20}, // Perlen Truhe
		{71015,1,75,40}, // Erfahrungsring
		{85001,1,100,15}, // Herrscherband (Einfach)
		{71001,1,10,60}, // Exorzismusschridtrolle
		{71094,1,10,40}, // Konzentrierteslesen
		{71221,1,1000,10}, // Offline Shop Dekoration Paket
		{72320,1,100,15}, // Offline Shop Inventar Erweiterung
		{73275,1,200,13}, // InGame Ringe 7 Tage Version
		{73276,1,200,13}, // InGame Ringe 7 Tage Version
		{73271,1,200,13}, // InGame Ringe 7 Tage Version
		{73274,1,200,13}, // InGame Ringe 7 Tage Version
		{73270,1,200,13}, // InGame Ringe 7 Tage Version
		{73277 ,1,200,13}, // InGame Ringe 7 Tage Version
	};

	const size_t listLen = _countof(gemItems);
	std::vector<BYTE> m_vecListIndex;
	while (1)
	{
		if (m_vecListIndex.size() >= listLen)
			break;
		if (m_vecListIndex.size() >= 27)
			break;
		for (WORD i = 0; i < listLen; ++i)
		{
			if (number(1, 100) < gemItems[i][3])
				continue;
			if (std::find(m_vecListIndex.begin(), m_vecListIndex.end(), i) != m_vecListIndex.end())
				continue;
			if (m_vecListIndex.size() >= 27)
				break;
#if __cplusplus < 199711L
			m_vecListIndex.push_back(i);
#else
			m_vecListIndex.emplace_back(i);
#endif
		}
	}

#if __cplusplus < 199711L
	for (auto it = m_vecListIndex.begin(); it != m_vecListIndex.end(); ++it)
	{
		const BYTE& listIndex = *it;
#else
	for(const auto& listIndex : m_vecListIndex)
	{
#endif
		TGemItem gemItem;
		gemItem.itemBuyed = false;
		gemItem.itemVnum = gemItems[listIndex][0];
		gemItem.itemCount = gemItems[listIndex][1];
		gemItem.itemPrice = gemItems[listIndex][2];
#if __cplusplus < 199711L
		m_mapGemItems.insert(std::make_pair(m_mapGemItems.size(), gemItem));
#else
		m_mapGemItems.emplace(m_mapGemItems.size(), gemItem);
#endif
	}
	SaveGemItems();
}
void CHARACTER::SaveGemItems()
{
	char filename[45];
	snprintf(filename, sizeof(filename), "locale/germany/gaya/%u", GetPlayerID());

	FILE* fp = fopen(filename, "w+");
	if (fp)
	{
		fprintf(fp, "index-vnum-count-price-buyed\n");
#if __cplusplus < 199711L
		for (itertype(m_mapGemItems) it = m_mapGemItems.begin(); it != m_mapGemItems.end(); ++it)
		{
			const BYTE& slotPos = it->first;
			const TGemItem& itemData = it->second;
			fprintf(fp, "%u %u %u %d %u\n", slotPos, itemData.itemVnum, itemData.itemCount, itemData.itemPrice, itemData.itemBuyed);
		}
#else
		for (const auto& [slotPos, itemData] : m_mapGemItems)
			fprintf(fp, "%u %u %u %d %u\n", slotPos, itemData.itemVnum, itemData.itemCount, itemData.itemPrice, itemData.itemBuyed);
#endif
		fclose(fp);
	}

}
void CHARACTER::LoadGemItems()
{
	m_mapGemItems.clear();
	char filename[45];
	snprintf(filename, sizeof(filename), "locale/germany/gaya/%u", GetPlayerID());
	FILE* fp;
	if ((fp = fopen(filename, "r")) != NULL)
	{
		char	one_line[256];
		while (fgets(one_line, 256, fp))
		{
			std::vector<std::string> m_vec;
			split_argument(one_line, m_vec);
			if (m_vec.size() != 5)
				continue;
			TGemItem gemItem;
			BYTE slotIndex;
			str_to_number(slotIndex, m_vec[0].c_str());
			str_to_number(gemItem.itemVnum, m_vec[1].c_str());
			str_to_number(gemItem.itemCount, m_vec[2].c_str());
			str_to_number(gemItem.itemPrice, m_vec[3].c_str());
			str_to_number(gemItem.itemBuyed, m_vec[4].c_str());
#if __cplusplus < 199711L
			m_mapGemItems.insert(std::make_pair(slotIndex, gemItem));
#else
			m_mapGemItems.emplace(slotIndex, gemItem);
#endif
		}
		fclose(fp);
	}
}
#endif


#ifdef ENABLE_NEW_KICK_HACK_FIX
bool CHARACTER::CheckAntiFlood()
{
	const int iLimitPerSec = 10; // 10 clicks per 1 second
	if (thecore_pulse() > GetAntiFloodPulse() + PASSES_PER_SEC(1))
	{
		SetAntiFloodCount(0);
		SetAntiFloodPulse(thecore_pulse());
	}
	return IncreaseAntiFloodCount() >= iLimitPerSec;
}
#endif


#ifdef ENABLE_CSHIELD
EVENTFUNC(cshield_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("cshield_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (NULL == ch || ch->IsNPC())
		return 0;

	DWORD cShield = ch->GetCShield()->VerifyCShield(ch->GetName());
	DWORD aid = ch->GetAID();

	if (cShield > 1)
	{
		if (cShield == 2)
			LogManager::instance().CShieldLog(aid, ch->GetName(), 10002);
		else
			LogManager::instance().CShieldLog(aid, ch->GetName(), 10003);

#ifdef ENABLE_AUTOBAN
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT count(*) FROM log.cshield_log WHERE accountid = '%d' AND errorcode != 10001 AND time BETWEEN DATE_ADD(NOW(), INTERVAL -20 MINUTE) AND NOW()", aid));
		if (pMsg->Get()->uiNumRows == 0)
			return 0;

		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		if (atoi(row[0]) >= 5)
		{
			DBManager::instance().DirectQuery("UPDATE account.account set status='BLOCK' where id='%d'", aid);
			LogManager::instance().CShieldLog(aid, ch->GetName(), 10004);
		}
#endif
		LPDESC d = ch->GetDesc();
		if (d)
			d->SetPhase(PHASE_CLOSE);
	}

	return PASSES_PER_SEC(20);
}

EVENTFUNC(cshield_data_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("cshield_data_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (NULL == ch || ch->IsNPC())
		return 0;

	if (!ch->GetCShield()->GetCShieldInit())
	{
		ch->GetCShield()->SetCShieldTime();
		ch->SendCShieldPacket(true);
	}
	else
		ch->SendCShieldPacket(false);

	return PASSES_PER_SEC(3);
}

void CHARACTER::StartCShieldEvent()
{
	if (m_pkCShieldEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCShieldEvent = event_create(cshield_event, info, PASSES_PER_SEC(25));
}

void CHARACTER::StartCShieldDataEvent()
{
	if (m_pkCShieldDataEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCShieldDataEvent = event_create(cshield_data_event, info, PASSES_PER_SEC(3));
}

void CHARACTER::SendCShieldPacket(bool start)
{
	LPDESC d = GetDesc();
	if (!d)
		return;

	SPacketGCCShield packet;
	packet.header = HEADER_GC_CSHIELD;
	packet.start = start;
	packet.num = GetCShield()->GetCShieldNumber();
	d->Packet(&packet, sizeof(SPacketGCCShield));
}
#endif


#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
DWORD CHARACTER::BuffGetNextExp() const
{
	if (IsBuffNPC()) {
		if (120 < GetLevel())
			return 2500000000;
		else
			return exp_table[GetLevel()];
	}
}

DWORD CHARACTER::BuffGetNextExpByLevel(int level) const
{
	if (120 < level)
		return 2500000000;
	else
		return exp_table[level];
}

void CHARACTER::SendBuffLevelUpEffect(int vid, int type, int value, int amount) {
	struct packet_point_change pack;

	pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
	pack.dwVID = vid;
	pack.type = type;
	pack.value = value;
	pack.amount = amount;
	PacketAround(&pack, sizeof(pack));
}

void CHARACTER::SendBuffNPCUseSkillPacket(int skill_vnum, int skill_level)
{
	TPacketGCBuffNPCUseSkill pack;
	pack.bHeader = HEADER_GC_BUFF_NPC_USE_SKILL;
	pack.dSkillVnum = skill_vnum;
	pack.dVid = GetVID();
	pack.dSkillLevel = skill_level;
	
	PacketView(&pack, sizeof(TPacketGCBuffNPCUseSkill), this);
}

void CHARACTER::LoadBuffNPC()
{
	if (GetBuffNPCSystem() != NULL) {
		GetBuffNPCSystem()->LoadBuffNPC();
		if (GetQuestFlag("buff_npc.is_summon") == 1)
			GetBuffNPCSystem()->Summon();
	}
}
#endif

#ifdef ENABLE_OFFLINESHOP_SYSTEM
bool CHARACTER::CheckWindows(bool bChat)
{
	if (GetExchange() || GetShop() || GetSafebox() || GetOfflineShop() || GetOfflineShopPanel())
	{
		if(bChat)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOWS"));
		return true;
	}
	return false;
}

bool CHARACTER::CanAddItemShop()
{
	time_t now = get_global_time();
	if (GetProtectTime("offlineshop.additem") > get_global_time())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.additem") - now);
		return false;
	}
	SetProtectTime("offlineshop.additem", now + 2);
	return true;
	
	
	
	

	//if (m_pkTimedEvent)
	//{
	//	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_BUG_WITH_TIMED_EVENT"));
	//	event_cancel(&m_pkTimedEvent);
	//	sys_log(0, "COfflineShopManager::AddItem: adding item with timed event xD %s:%d", GetName(), GetPlayerID());
	//	return true;
	//}
	//else if (!GetOfflineShop())
	//{
	//	sys_log(0, "COfflineShopManager::AddItem: adding item without panel <?> %s:%d", GetName(), GetPlayerID());
	//	return true;
	//}
	//return false;
}

bool CHARACTER::CanDestroyShop()
{
	time_t now = get_global_time();
	if (GetProtectTime("offlineshop.destroy") > get_global_time())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.destroy") - now);
		return true;
	}
	
	SetProtectTime("offlineshop.destroy", now + 3);
	if (!GetOfflineShop())
		return true;
	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_BUG_WITH_TIMED_EVENT"));
		event_cancel(&m_pkTimedEvent);
		return true;
	}
	return false;
}

bool CHARACTER::CanRemoveItemShop()
{
	time_t now = get_global_time();
	if (GetProtectTime("offlineshop.remove") > get_global_time())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.remove")-now);
		return true;
	}
	SetProtectTime("offlineshop.remove", now + 1);
	if (!GetOfflineShop())
		return true;
	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_BUG_WITH_TIMED_EVENT"));
		event_cancel(&m_pkTimedEvent);
		return true;
	}
	return false;
}


bool CHARACTER::CanCreateShop()
{
	if (!GetOfflineShopPanel())
		return true;
	SetOfflineShopPanel(false);
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.create") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.create")-time);
		return true;
	}
	SetProtectTime("offlineshop.create", time + 10);
	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_BUG_WITH_TIMED_EVENT"));
		event_cancel(&m_pkTimedEvent);
		return true;
	}
	if (GetGold() < 2000000)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CREATE_YANG"),2000000);
		return true;
	}
	else if (GetDungeon())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CANT_CREATE_IN_DUNGEON"));
		return true;
	}
	else if (COfflineShopManager::Instance().HasOfflineShop(this) == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_ALREADY_HAVE_SHOP"));
		return true;
	}
	return false;
}

bool CHARACTER::CanRemoveLogShop()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.log") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.log") - time);
		return true;
	}
	SetProtectTime("offlineshop.log", time + 30);

	return false;
}

bool CHARACTER::CanWithdrawMoney()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.money") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.money") - time);
		return true;
	}
	SetProtectTime("offlineshop.money", time + 5);
	return false;
}

bool CHARACTER::CanOpenShopPanel()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.panel") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.panel") - time);
		return true;
	}
	SetProtectTime("offlineshop.panel", time + 5);
	if (CheckWindows(true))
		return true;
	return false;
}

bool CHARACTER::CanOpenOfflineShop()
{
	if (m_pkTimedEvent)
		return true;
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.offlineshop") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.offlineshop") - time);
		return true;
	}
	SetProtectTime("offlineshop.offlineshop", time + 1);
	if (CheckWindows(true))
		return true;
	return false;
}

bool CHARACTER::CanChangeTitle()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.title") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.title") - time);
		return true;
	}
	SetProtectTime("offlineshop.title", time + 10);
	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_BUG_WITH_TIMED_EVENT"));
		event_cancel(&m_pkTimedEvent);
		return true;
	}
	if (!FindAffect(AFFECT_DECORATION))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_DECORATION"));
		return true;
	}
		
	return false;
}

bool CHARACTER::CanBuyItemOfflineShop()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.buy") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.buy") - time);
		return true;
	}
	SetProtectTime("offlineshop.buy", time + 1);
	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_BUG_WITH_TIMED_EVENT"));
		event_cancel(&m_pkTimedEvent);
		return true;
	}
	return false;
}
bool CHARACTER::CanChangeDecoration()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.decoration") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.decoration") - time);
		return true;
	}
	SetProtectTime("offlineshop.decoration", time + 10);
	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_BUG_WITH_TIMED_EVENT"));
		event_cancel(&m_pkTimedEvent);
		return true;
	}
	if (!FindAffect(AFFECT_DECORATION))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_DECORATION"));
		return true;
	}
	return false;
}

bool CHARACTER::CanGetBackItems()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.back") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.back") - time);
		return true;
	}
	SetProtectTime("offlineshop.back", time + 5);
	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_BUG_WITH_TIMED_EVENT"));
		event_cancel(&m_pkTimedEvent);
		return true;
	}
	return false;
}

bool CHARACTER::CanAddTimeShop()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.time") > time)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.time") - time);
		return true;
	}
	SetProtectTime("offlineshop.time", time + 10);
	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_BUG_WITH_TIMED_EVENT"));
		event_cancel(&m_pkTimedEvent);
		return true;
	}
	if (GetGold() < 20000000)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_CREATE_YANG"),2000000);
		return true;
	}
		
	return false;
}

#ifdef ENABLE_SHOP_SEARCH_SYSTEM
bool CHARACTER::CanSearch()
{
	time_t now = get_global_time();
	if (GetProtectTime("offlineshop.search") > get_global_time())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OFFLINE_SHOP_OLD_TIME"), GetProtectTime("offlineshop.search")-now);
		return true;
	}
	SetProtectTime("offlineshop.search", now + 1);
	return false;
}
#endif


void CHARACTER::PlayerPunish(bool PowerPunish, int Duration)
{
	if (!PowerPunish)
	{
		AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, Duration, 0, true);
		sys_log(0, "%s[%d] has been chatbanned because of spamming/writing words which are in the spamlist.txt", GetName(), GetPlayerID());
	}
	else
	{
		std::auto_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE account.account SET availDt = FROM_UNIXTIME(UNIX_TIMESTAMP(CURRENT_TIMESTAMP()) + %i) WHERE id = %d", Duration, GetAID()));
		sys_log(0, "%s[%d] has been banned because of saying blacklisted word", GetName(), GetPlayerID());
		GetDesc()->DelayedDisconnect(5);
	}
}

bool CHARACTER::SpamAllowBuf(const char *Message)
{
	if (!strcmp(Message, "(?˛´c)") || !strcmp(Message, "(μ·)") || !strcmp(Message, "(±a≫Y)") || !strcmp(Message, "(AA??)") || !strcmp(Message, "(≫c¶?)") || !strcmp(Message, "(?đłe)") || !strcmp(Message, "(??C?)") || !strcmp(Message, "(????)") || !strcmp(Message, "(AE??)"))
	{
		return true;
	}

	return false;
}

bool CHARACTER::SpamListCheck(const char *Message)
{
	for (int i = 0; i < SpamBlockListArray.size(); i++)
	{
		if (!strcmp(Message, SpamBlockListArray[i].c_str()))
		{
			return false;
		}
	}

	return true;
}

bool CHARACTER::BannListCheck(const char *Message)
{
	for (int i = 0; i < SpamBannListArray.size(); i++)
	{
		if (!strcmp(Message, SpamBannListArray[i].c_str()))
		{
			return false;
		}
	}

	return true;
}

#endif
