#ifndef __INC_METIN_II_CHAR_H__
#define __INC_METIN_II_CHAR_H__

#include <boost/unordered_map.hpp>

#include "../../common/stl.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "vid.h"
#include "constants.h"
#include "affect.h"
#include "affect_flag.h"
#include "packet.h"
#ifndef ENABLE_CUBE_RENEWAL
	#include "cube.h"
#else
	#include "cuberenewal.h"
#endif
#include "mining.h"



class CBuffOnAttributes;
class CPetSystem;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
class CBuffNPCActor;
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
class CNewPetSystem;
#endif
#ifdef ENABLE_CLONE_SYSTEM
class CDitoActor;
#endif

#ifdef ENABLE_OFFLINESHOP_SYSTEM
class COfflineShop;
typedef class COfflineShop* LPOFFLINESHOP;
#endif

#define INSTANT_FLAG_DEATH_PENALTY		(1 << 0)
#define INSTANT_FLAG_SHOP			(1 << 1)
#define INSTANT_FLAG_EXCHANGE			(1 << 2)
#define INSTANT_FLAG_STUN			(1 << 3)
#define INSTANT_FLAG_NO_REWARD			(1 << 4)

#define AI_FLAG_NPC				(1 << 0)
#define AI_FLAG_AGGRESSIVE			(1 << 1)
#define AI_FLAG_HELPER				(1 << 2)
#define AI_FLAG_STAYZONE			(1 << 3)
#define MAX_CARDS_IN_HAND	5
#define MAX_CARDS_IN_FIELD	3


#define SET_OVER_TIME(ch, time)	(ch)->SetOverTime(time)

extern int g_nPortalLimitTime;

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
	MAIN_RACE_MAX_NUM,
};

enum
{
	POISON_LENGTH = 30,
	STAMINA_PER_STEP = 1,
	SAFEBOX_PAGE_SIZE = 9,
	AI_CHANGE_ATTACK_POISITION_TIME_NEAR = 10000,
	AI_CHANGE_ATTACK_POISITION_TIME_FAR = 1000,
	AI_CHANGE_ATTACK_POISITION_DISTANCE = 100,
	SUMMON_MONSTER_COUNT = 3,
};

enum
{
	FLY_NONE,
	FLY_EXP,
	FLY_HP_MEDIUM,
	FLY_HP_BIG,
	FLY_SP_SMALL,
	FLY_SP_MEDIUM,
	FLY_SP_BIG,
	FLY_FIREWORK1,
	FLY_FIREWORK2,
	FLY_FIREWORK3,
	FLY_FIREWORK4,
	FLY_FIREWORK5,
	FLY_FIREWORK6,
	FLY_FIREWORK_CHRISTMAS,
	FLY_CHAIN_LIGHTNING,
	FLY_HP_SMALL,
	FLY_SKILL_MUYEONG,
};

enum EDamageType
{
	DAMAGE_TYPE_NONE,
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_NORMAL_RANGE,
	//스킬
	DAMAGE_TYPE_MELEE,
	DAMAGE_TYPE_RANGE,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_ELEC,
	DAMAGE_TYPE_MAGIC,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_SPECIAL,
};

enum EPointTypes
{
	POINT_NONE,                 // 0
	POINT_LEVEL,                // 1
	POINT_VOICE,                // 2
	POINT_EXP,                  // 3
	POINT_NEXT_EXP,             // 4
	POINT_HP,                   // 5
	POINT_MAX_HP,               // 6
	POINT_SP,                   // 7
	POINT_MAX_SP,               // 8  
	POINT_STAMINA,              // 9  스테미너
	POINT_MAX_STAMINA,          // 10 최대 스테미너

	POINT_GOLD,                 // 11
	POINT_ST,                   // 12 근력
	POINT_HT,                   // 13 체력
	POINT_DX,                   // 14 민첩성
	POINT_IQ,                   // 15 정신력
	POINT_DEF_GRADE,		// 16 ...
	POINT_ATT_SPEED,            // 17 공격속도
	POINT_ATT_GRADE,		// 18 공격력 MAX
	POINT_MOV_SPEED,            // 19 이동속도
	POINT_CLIENT_DEF_GRADE,	// 20 방어등급
	POINT_CASTING_SPEED,        // 21 주문속도 (쿨다운타임*100) / (100 + 이값) = 최종 쿨다운 타임
	POINT_MAGIC_ATT_GRADE,      // 22 마법공격력
	POINT_MAGIC_DEF_GRADE,      // 23 마법방어력
	POINT_EMPIRE_POINT,         // 24 제국점수
	POINT_LEVEL_STEP,           // 25 한 레벨에서의 단계.. (1 2 3 될 때 보상, 4 되면 레벨 업)
	POINT_STAT,                 // 26 능력치 올릴 수 있는 개수
	POINT_SUB_SKILL,		// 27 보조 스킬 포인트
	POINT_SKILL,		// 28 액티브 스킬 포인트
	POINT_WEAPON_MIN,		// 29 무기 최소 데미지
	POINT_WEAPON_MAX,		// 30 무기 최대 데미지
	POINT_PLAYTIME,             // 31 플레이시간
	POINT_HP_REGEN,             // 32 HP 회복률
	POINT_SP_REGEN,             // 33 SP 회복률

	POINT_BOW_DISTANCE,         // 34 활 사정거리 증가치 (meter)

	POINT_HP_RECOVERY,          // 35 체력 회복 증가량
	POINT_SP_RECOVERY,          // 36 정신력 회복 증가량

	POINT_POISON_PCT,           // 37 독 확률
	POINT_STUN_PCT,             // 38 기절 확률
	POINT_SLOW_PCT,             // 39 슬로우 확률
	POINT_CRITICAL_PCT,         // 40 크리티컬 확률
	POINT_PENETRATE_PCT,        // 41 관통타격 확률
	POINT_CURSE_PCT,            // 42 저주 확률

	POINT_ATTBONUS_HUMAN,       // 43 인간에게 강함
	POINT_ATTBONUS_ANIMAL,      // 44 동물에게 데미지 % 증가
	POINT_ATTBONUS_ORC,         // 45 웅귀에게 데미지 % 증가
	POINT_ATTBONUS_MILGYO,      // 46 밀교에게 데미지 % 증가
	POINT_ATTBONUS_UNDEAD,      // 47 시체에게 데미지 % 증가
	POINT_ATTBONUS_DEVIL,       // 48 마귀(악마)에게 데미지 % 증가
	POINT_ATTBONUS_INSECT,      // 49 벌레족
	POINT_ATTBONUS_FIRE,        // 50 화염족
	POINT_ATTBONUS_ICE,         // 51 빙설족
	POINT_ATTBONUS_DESERT,      // 52 사막족
	POINT_ATTBONUS_MONSTER,     // 53 모든 몬스터에게 강함
	POINT_ATTBONUS_WARRIOR,     // 54 무사에게 강함
	POINT_ATTBONUS_ASSASSIN,	// 55 자객에게 강함
	POINT_ATTBONUS_SURA,		// 56 수라에게 강함
	POINT_ATTBONUS_SHAMAN,		// 57 무당에게 강함
	POINT_ATTBONUS_TREE,     	// 58 나무에게 강함 20050729.myevan UNUSED5 

	POINT_RESIST_WARRIOR,		// 59 무사에게 저항
	POINT_RESIST_ASSASSIN,		// 60 자객에게 저항
	POINT_RESIST_SURA,			// 61 수라에게 저항
	POINT_RESIST_SHAMAN,		// 62 무당에게 저항

	POINT_STEAL_HP,             // 63 생명력 흡수
	POINT_STEAL_SP,             // 64 정신력 흡수

	POINT_MANA_BURN_PCT,        // 65 마나 번

	/// 피해시 보너스 ///

	POINT_DAMAGE_SP_RECOVER,    // 66 공격당할 시 정신력 회복 확률

	POINT_BLOCK,                // 67 블럭율
	POINT_DODGE,                // 68 회피율

	POINT_RESIST_SWORD,         // 69
	POINT_RESIST_TWOHAND,       // 70
	POINT_RESIST_DAGGER,        // 71
	POINT_RESIST_BELL,          // 72
	POINT_RESIST_FAN,           // 73
	POINT_RESIST_BOW,           // 74  화살   저항   : 대미지 감소
	POINT_RESIST_FIRE,          // 75  화염   저항   : 화염공격에 대한 대미지 감소
	POINT_RESIST_ELEC,          // 76  전기   저항   : 전기공격에 대한 대미지 감소
	POINT_RESIST_MAGIC,         // 77  술법   저항   : 모든술법에 대한 대미지 감소
	POINT_RESIST_WIND,          // 78  바람   저항   : 바람공격에 대한 대미지 감소

	POINT_REFLECT_MELEE,        // 79 공격 반사

	/// 특수 피해시 ///
	POINT_REFLECT_CURSE,		// 80 저주 반사
	POINT_POISON_REDUCE,		// 81 독데미지 감소

	/// 적 소멸시 ///
	POINT_KILL_SP_RECOVER,		// 82 적 소멸시 MP 회복
	POINT_EXP_DOUBLE_BONUS,		// 83
	POINT_GOLD_DOUBLE_BONUS,		// 84
	POINT_ITEM_DROP_BONUS,		// 85

	/// 회복 관련 ///
	POINT_POTION_BONUS,			// 86
	POINT_KILL_HP_RECOVERY,		// 87

	POINT_IMMUNE_STUN,			// 88
	POINT_IMMUNE_SLOW,			// 89
	POINT_IMMUNE_FALL,			// 90
	//////////////////

	POINT_PARTY_ATTACKER_BONUS,		// 91
	POINT_PARTY_TANKER_BONUS,		// 92

	POINT_ATT_BONUS,			// 93
	POINT_DEF_BONUS,			// 94

	POINT_ATT_GRADE_BONUS,		// 95
	POINT_DEF_GRADE_BONUS,		// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98

	POINT_RESIST_NORMAL_DAMAGE,		// 99

	POINT_HIT_HP_RECOVERY,		// 100
	POINT_HIT_SP_RECOVERY, 		// 101
	POINT_MANASHIELD,			// 102 흑신수호 스킬에 의한 마나쉴드 효과 정도

	POINT_PARTY_BUFFER_BONUS,		// 103
	POINT_PARTY_SKILL_MASTER_BONUS,	// 104

	POINT_HP_RECOVER_CONTINUE,		// 105
	POINT_SP_RECOVER_CONTINUE,		// 106

	POINT_STEAL_GOLD,			// 107 
	POINT_POLYMORPH,			// 108 변신한 몬스터 번호
	POINT_MOUNT,			// 109 타고있는 몬스터 번호

	POINT_PARTY_HASTE_BONUS,		// 110
	POINT_PARTY_DEFENDER_BONUS,		// 111
	POINT_STAT_RESET_COUNT,		// 112 피의 단약 사용을 통한 스텟 리셋 포인트 (1당 1포인트 리셋가능)

	POINT_HORSE_SKILL,			// 113

	POINT_MALL_ATTBONUS,		// 114 공격력 +x%
	POINT_MALL_DEFBONUS,		// 115 방어력 +x%
	POINT_MALL_EXPBONUS,		// 116 경험치 +x%
	POINT_MALL_ITEMBONUS,		// 117 아이템 드롭율 x/10배
	POINT_MALL_GOLDBONUS,		// 118 돈 드롭율 x/10배

	POINT_MAX_HP_PCT,			// 119 최대생명력 +x%
	POINT_MAX_SP_PCT,			// 120 최대정신력 +x%

	POINT_SKILL_DAMAGE_BONUS,		// 121 스킬 데미지 *(100+x)%
	POINT_NORMAL_HIT_DAMAGE_BONUS,	// 122 평타 데미지 *(100+x)%

	// DEFEND_BONUS_ATTRIBUTES
	POINT_SKILL_DEFEND_BONUS,		// 123 스킬 방어 데미지
	POINT_NORMAL_HIT_DEFEND_BONUS,	// 124 평타 방어 데미지
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	// PC_BANG_ITEM_ADD 
	POINT_PC_BANG_EXP_BONUS,		// 125 PC방 전용 경험치 보너스
	POINT_PC_BANG_DROP_BONUS,		// 126 PC방 전용 드롭률 보너스
	// END_PC_BANG_ITEM_ADD
	POINT_RAMADAN_CANDY_BONUS_EXP,			// 라마단 사탕 경험치 증가용

	POINT_ENERGY = 128,					// 128 기력

	// 기력 ui 용.
	// 서버에서 쓰지 않기만, 클라이언트에서 기력의 끝 시간을 POINT로 관리하기 때문에 이렇게 한다.
	// 아 부끄럽다
	POINT_ENERGY_END_TIME = 129,					// 129 기력 종료 시간

	POINT_COSTUME_ATTR_BONUS = 130,
	POINT_MAGIC_ATT_BONUS_PER = 131,
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,

	// 추가 속성 저항
	POINT_RESIST_ICE = 133,          //   냉기 저항   : 얼음공격에 대한 대미지 감소
	POINT_RESIST_EARTH = 134,        //   대지 저항   : 얼음공격에 대한 대미지 감소
	POINT_RESIST_DARK = 135,         //   어둠 저항   : 얼음공격에 대한 대미지 감소

	POINT_RESIST_CRITICAL = 136,		// 크리티컬 저항	: 상대의 크리티컬 확률을 감소
	POINT_RESIST_PENETRATE = 137,		// 관통타격 저항	: 상대의 관통타격 확률을 감소

	//POINT_MAX_NUM = 129	common/length.h
#ifdef ELEMENT_NEW_BONUSES
	POINT_ATTBONUS_ELEC,
	POINT_ATTBONUS_WIND,
	POINT_ATTBONUS_EARTH,
	POINT_ATTBONUS_DARK,
#endif
#ifdef ENABLE_NEW_ATTR
	POINT_ATTBONUS_METIN,
#endif
#ifdef ENABLE_SHADOW_BONUS
	POINT_ATTBONUS_SHADOW = 145,
#endif
#ifdef ENABLE_ASLAN_LOTTERY
	POINT_LOTTO_MONEY			= 148,
	POINT_LOTTO_TOTAL_MONEY		= 149,
#endif
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	POINT_BATTLE_PASS_PREMIUM_ID = 160,
#endif
#ifdef __NEW_ATTRIBUTES__
	POINT_ATTBONUS_METIN			= 161,
	POINT_ATTBONUS_BOSS     		= 162,
#endif	
#ifdef __LIMIT_PRESTIGE__
	POINT_PRESTIGE     		= 163,
#endif
#ifdef __GEM_SYSTEM__
	POINT_GEM = 191,
#endif
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM
};

enum EPositions
{
	POS_DEAD,
	POS_SLEEPING,
	POS_RESTING,
	POS_SITTING,
	POS_FISHING,
	POS_FIGHTING,
	POS_MOUNTING,
	POS_STANDING
};

enum EBlockAction
{
	BLOCK_EXCHANGE		= (1 << 0),
	BLOCK_PARTY_INVITE		= (1 << 1),
	BLOCK_GUILD_INVITE		= (1 << 2),
	BLOCK_WHISPER		= (1 << 3),
	BLOCK_MESSENGER_INVITE	= (1 << 4),
	BLOCK_PARTY_REQUEST		= (1 << 5),
};

#ifdef __PICKUP_FILTER__
enum EPickupFilterPickupModes
{
	PICKUP_FILTER_MODE_SINGLE,
	PICKUP_FILTER_MODE_ALL,
};

enum EPickupFilterBlockModes
{
	PICKUP_BLOCK_MODE_WEAPON	= (1 << 0),
	PICKUP_BLOCK_MODE_ARMOR		= (1 << 1),
	PICKUP_BLOCK_MODE_HEAD		= (1 << 2),
	PICKUP_BLOCK_MODE_SHIELD 	= (1 << 3),
	PICKUP_BLOCK_MODE_WRIST		= (1 << 4),
	PICKUP_BLOCK_MODE_FOOTS		= (1 << 5),
	PICKUP_BLOCK_MODE_NECK		= (1 << 6),
	PICKUP_BLOCK_MODE_EAR		= (1 << 7),
	PICKUP_BLOCK_MODE_ETC		= (1 << 8),
};
#endif 	

// <Factor> Dynamically evaluated CHARACTER* equivalent.
// Referring to SCharDeadEventInfo.
struct DynamicCharacterPtr {
	DynamicCharacterPtr() : is_pc(false), id(0) {}
	DynamicCharacterPtr(const DynamicCharacterPtr& o)
		: is_pc(o.is_pc), id(o.id) {}

	// Returns the LPCHARACTER found in CHARACTER_MANAGER.
	LPCHARACTER Get() const; 
	// Clears the current settings.
	void Reset() {
		is_pc = false;
		id = 0;
	}

	// Basic assignment operator.
	DynamicCharacterPtr& operator=(const DynamicCharacterPtr& rhs) {
		is_pc = rhs.is_pc;
		id = rhs.id;
		return *this;
	}
	// Supports assignment with LPCHARACTER type.
	DynamicCharacterPtr& operator=(LPCHARACTER character);
	// Supports type casting to LPCHARACTER.
	operator LPCHARACTER() const {
		return Get();
	}

	bool is_pc;
	uint32_t id;
};

/* 저장하는 데이터 */
typedef struct character_point
{
	long long		points[POINT_MAX_NUM];

	BYTE			job;
	BYTE			voice;

	BYTE			level;
	DWORD			exp;
	long long		gold;
#ifdef __GEM_SYSTEM__
	int				gem;
#endif

#ifdef ENABLE_ASLAN_LOTTERY
	long long		lotto_moneypool;
	long long		lotto_totalmoneywin;
#endif

	int				hp;
	int				sp;

	int				iRandomHP;
	int				iRandomSP;

	int				stamina;

	BYTE			skill_group;
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	int				battle_pass_premium_id;
#endif
} CHARACTER_POINT;

#ifdef ENABLE_KILL_STATISTICS
typedef struct character_kill_statistics
{
	int		iJinnoKills;
	int		iShinsooKills;
	int		iChunjoKills;
	int		iTotalKills;
	int		iDuelsWon;
	int		iDuelsLost;
	int		iMonstersKills;
	int		iBossesKills;
	int		iStonesKills;
} CHARACTER_KILL_STATISTICS;
#endif

/* 저장되지 않는 캐릭터 데이터 */
typedef struct character_point_instant
{
	long long		points[POINT_MAX_NUM];

	float			fRot;

	int				iMaxHP;
	int				iMaxSP;

	long			position;

	long			instant_flag;
	DWORD			dwAIFlag;
	DWORD			dwImmuneFlag;
	DWORD			dwLastShoutPulse;

	WORD			parts[PART_MAX_NUM];

	LPITEM			pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
	UINT			bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];

	// 용혼석 인벤토리.
	LPITEM			pDSItems[DRAGON_SOUL_INVENTORY_MAX_NUM];
	WORD			wDSItemGrid[DRAGON_SOUL_INVENTORY_MAX_NUM];
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	LPITEM 			pBuffEquipmentItem[BUFF_WINDOW_SLOT_MAX_NUM];
#endif
#ifdef ENABLE_SWITCHBOT
	LPITEM			pSwitchbotItems[SWITCHBOT_SLOT_COUNT];
#endif
	
	// by mhh
	LPITEM			pCubeItems[CUBE_MAX_NUM];
	LPCHARACTER		pCubeNpc;
#ifdef __CHANGELOOK_SYSTEM__
	LPITEM			pClMaterials[CL_WINDOW_MAX_MATERIALS];
#endif
#ifdef __SASH_SYSTEM__
	LPITEM			pSashMaterials[SASH_WINDOW_MAX_MATERIALS];
#endif

	LPCHARACTER			battle_victim;

	BYTE			gm_level;

	BYTE			bBasePart;	// 평상복 번호

	int				iMaxStamina;

	BYTE			bBlockMode;

	int				iDragonSoulActiveDeck;
	LPENTITY		m_pDragonSoulRefineWindowOpener;
#ifdef __AURA_SYSTEM__
	LPENTITY		m_pAuraRefineWindowOpener;
#endif
} CHARACTER_POINT_INSTANT;

#define TRIGGERPARAM		LPCHARACTER ch, LPCHARACTER causer

typedef struct trigger
{
	BYTE	type;
	int		(*func) (TRIGGERPARAM);
	long	value;
} TRIGGER;

class CTrigger
{
	public:
		CTrigger() : bType(0), pFunc(NULL)
		{
		}

		BYTE	bType;
		int	(*pFunc) (TRIGGERPARAM);
};

EVENTINFO(char_event_info)
{
	DynamicCharacterPtr ch;
};

typedef std::map<VID, size_t> targetMap;
struct TSkillUseInfo
{
	int	    iHitCount;
	int	    iMaxHitCount;
	int	    iSplashCount;
	DWORD   dwNextSkillUsableTime;
	int	    iRange;
	bool    bUsed;
    bool    bSkillCD;
    DWORD   dwHitCount;
	DWORD   dwVID;
	bool    isGrandMaster;

	targetMap TargetVIDMap;

	TSkillUseInfo()
		: iHitCount(0), iMaxHitCount(0), iSplashCount(0), dwNextSkillUsableTime(0), iRange(0), bUsed(false), bSkillCD(false), dwHitCount(0),
		dwVID(0), isGrandMaster(false)
   	{}

	bool    HitOnce(DWORD dwVnum = 0);
	bool    IsSkillCooldown(DWORD dwVnum, float fSkillPower);

	bool    UseSkill(bool isGrandMaster, DWORD vid, DWORD dwCooltime, int splashcount = 1, int hitcount = -1, int range = -1);
	DWORD   GetMainTargetVID() const	{ return dwVID; }
	void    SetMainTargetVID(DWORD vid) { dwVID=vid; }
	void    ResetHitCount() { if (iSplashCount) { iHitCount = iMaxHitCount; iSplashCount--; } }
};

typedef struct packet_party_update TPacketGCPartyUpdate;
class CExchange;
class CSkillProto;
class CParty;
class CDungeon;
class CWarMap;
class CAffect;
class CGuild;
class CSafebox;
class CArena;
#ifdef LWT_BUFFI_SYSTEM
class CSupportSystem;
#endif

class CShop;
typedef class CShop * LPSHOP;

class CMob;
class CMobInstance;
typedef struct SMobSkillInfo TMobSkillInfo;

//SKILL_POWER_BY_LEVEL
extern int GetSkillPowerByLevelFromType(int job, int skillgroup, int skilllevel);
//END_SKILL_POWER_BY_LEVEL

namespace marriage
{
	class WeddingMap;
}
enum e_overtime
{
	OT_NONE,
	OT_3HOUR,
	OT_5HOUR,
};

class CHARACTER : public CEntity, public CFSM, public CHorseRider
{
	protected:
		//////////////////////////////////////////////////////////////////////////////////
		// Entity 관련
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);
		//////////////////////////////////////////////////////////////////////////////////

	public:
		LPCHARACTER			FindCharacterInView(const char * name, bool bFindPCOnly);
		void				UpdatePacket();
#ifdef MULTI_LANGUAGE_SYSTEM		
		std::string GetLang();
		void SetLang(std::string lang);
#endif
#ifdef ENABLE_BIYOLOG
		void			CheckBio();
#endif
#ifdef ENABLE_KILL_STATISTICS
		void			SendKillStatisticsPacket();
		
		void			AddToJinnoKills()	{ m_killstatistics.iJinnoKills += 1; }
		int				GetJinnoKills() const		{ return m_killstatistics.iJinnoKills; }
		
		void			AddToShinsooKills()	{ m_killstatistics.iShinsooKills += 1; }
		int				GetShinsooKills() const		{ return m_killstatistics.iShinsooKills; }
		
		void			AddToChunjoKills()	{ m_killstatistics.iChunjoKills += 1; }
		int				GetChunjoKills() const		{ return m_killstatistics.iChunjoKills; }
		
		void			AddToTotalKills()	{ m_killstatistics.iTotalKills += 1; }
		int				GetTotalKills() const		{ return m_killstatistics.iTotalKills; }
		
		void			AddToDuelsWon()	{ m_killstatistics.iDuelsWon += 1; }
		int				GetDuelsWon() const		{ return m_killstatistics.iDuelsWon; }
		
		void			AddToDuelsLost()	{ m_killstatistics.iDuelsLost += 1; }
		int				GetDuelsLost() const		{ return m_killstatistics.iDuelsLost; }

		void			AddToMonstersKills()	{ m_killstatistics.iMonstersKills += 1; }
		int				GetMonstersKills() const		{ return m_killstatistics.iMonstersKills; }
		
		void			AddToBossesKills()	{ m_killstatistics.iBossesKills += 1; }
		int				GetBossesKills() const		{ return m_killstatistics.iBossesKills; }
		
		void			AddToStonesKills()	{ m_killstatistics.iStonesKills += 1; }
		int				GetStonesKills() const		{ return m_killstatistics.iStonesKills; }
#endif

		//////////////////////////////////////////////////////////////////////////////////
		// FSM (Finite State Machine) 관련
	protected:
		CStateTemplate<CHARACTER>	m_stateMove;
		CStateTemplate<CHARACTER>	m_stateBattle;
		CStateTemplate<CHARACTER>	m_stateIdle;

	public:
		virtual void		StateMove();
		virtual void		StateBattle();
		virtual void		StateIdle();
		virtual void		StateFlag();
		virtual void		StateFlagBase();
		void				StateHorse();

	protected:
		// STATE_IDLE_REFACTORING
		void				__StateIdle_Monster();
		void				__StateIdle_Stone();
		void				__StateIdle_NPC();
		// END_OF_STATE_IDLE_REFACTORING

	public:
		DWORD GetAIFlag() const	{ return m_pointsInstant.dwAIFlag; }
	
		void				SetAggressive();
		bool				IsAggressive() const;

		void				SetCoward();
		bool				IsCoward() const;
		void				CowardEscape();

		void				SetNoAttackShinsu();
		bool				IsNoAttackShinsu() const;

		void				SetNoAttackChunjo();
		bool				IsNoAttackChunjo() const;

		void				SetNoAttackJinno();
		bool				IsNoAttackJinno() const;

		void				SetAttackMob();
		bool				IsAttackMob() const;

		virtual void			BeginStateEmpty();
		virtual void			EndStateEmpty() {}

		void				RestartAtSamePos();

	protected:
		DWORD				m_dwStateDuration;
		//////////////////////////////////////////////////////////////////////////////////

	public:
		CHARACTER();
		virtual ~CHARACTER();

		void			Create(const char * c_pszName, DWORD vid, bool isPC);
		void			Destroy();

		void			Disconnect(const char * c_pszReason);

	protected:
		void			Initialize();

		//////////////////////////////////////////////////////////////////////////////////
		// Basic Points
#ifdef __SEND_TARGET_INFO__
	private:
		DWORD			dwLastTargetInfoPulse;

	public:
		DWORD			GetLastTargetInfoPulse() const	{ return dwLastTargetInfoPulse; }
		void			SetLastTargetInfoPulse(DWORD pulse) { dwLastTargetInfoPulse = pulse; }
#endif

	public:
		DWORD			GetPlayerID() const	{ return m_dwPlayerID; }

		void			SetPlayerProto(const TPlayerTable * table);
		void			CreatePlayerProto(TPlayerTable & tab);	// 저장 시 사용

		void			SetProto(const CMob * c_pkMob);
		WORD			GetRaceNum() const;

		void			Save();		// DelayedSave
		void			SaveReal();	// 실제 저장
		void			FlushDelayedSaveItem();
#ifdef __EXTENDED_BLEND_AFFECT__
		bool			UseExtendedBlendAffect(LPITEM item, int affect_type, int apply_type, int apply_value, int apply_duration);
#endif
#ifdef __NEW_BLEND_AFFECT__
		bool			SetBlendAffect(LPITEM item);
#endif
#ifdef ENABLE_NEW_AFFECT_POTION	
		void			SetAffectPotion(LPITEM item);
#endif	

		const char *	GetName() const;
		const VID &		GetVID() const		{ return m_vid;		}

		void			SetName(const std::string& name) { m_stName = name; }

		void			SetRace(BYTE race);
		bool			ChangeSex();

		DWORD			GetAID() const;
		int				GetChangeEmpireCount() const;
		void			SetChangeEmpireCount();
		int				ChangeEmpire(BYTE empire);

		BYTE			GetJob() const;
		BYTE			GetCharType() const;
#ifdef ENABLE_CLONE_SYSTEM
		void			SetCharType(BYTE type)  {m_bCharType = type; };
		TPlayerSkill*		GetSkills() { return m_pSkillLevels; }
		void			SetSkills(TPlayerSkill* skill) 
		{
			if (m_pSkillLevels != NULL)
			{
				for (int i = 0; i<SKILL_MAX_NUM; i++)
				{
					m_pSkillLevels[i].bMasterType = skill[i].bMasterType;
					m_pSkillLevels[i].bLevel= skill[i].bLevel;
					m_pSkillLevels[i].tNextRead= skill[i].tNextRead;
				}
			}
			else
			{
				m_pSkillLevels = M2_NEW TPlayerSkill[SKILL_MAX_NUM];
				for (int i = 0; i<SKILL_MAX_NUM; i++)
				{
					m_pSkillLevels[i].bMasterType = skill[i].bMasterType;
					m_pSkillLevels[i].bLevel= skill[i].bLevel;
					m_pSkillLevels[i].tNextRead= skill[i].tNextRead;
				}
			}
		}
		CDitoActor* m_pkDito;
		LPCHARACTER GetDito();
		bool	IsDito();
		void	RemoveAffectFlag(DWORD dwAff);
#endif

		bool			IsPC() const		{ return GetDesc() ? true : false; }
		bool			IsNPC()	const		{ return m_bCharType != CHAR_TYPE_PC; }
		bool			IsMonster()	const	{ return m_bCharType == CHAR_TYPE_MONSTER; }
		bool			IsStone() const		{ return m_bCharType == CHAR_TYPE_STONE; }
		bool			IsDoor() const		{ return m_bCharType == CHAR_TYPE_DOOR; } 
		bool			IsBuilding() const	{ return m_bCharType == CHAR_TYPE_BUILDING;  }
		bool			IsWarp() const		{ return m_bCharType == CHAR_TYPE_WARP; }
		bool			IsGoto() const		{ return m_bCharType == CHAR_TYPE_GOTO; }
//		bool			IsPet() const		{ return m_bCharType == CHAR_TYPE_PET; }

		DWORD			GetLastShoutPulse() const	{ return m_pointsInstant.dwLastShoutPulse; }
		void			SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
		int				GetLevel() const		{ return m_points.level;	}
		void			SetLevel(BYTE level);

		BYTE			GetGMLevel() const;
		BOOL 			IsGM() const;
		void			SetGMLevel(); 

		DWORD			GetExp() const		{ return m_points.exp;	}
		void			SetExp(DWORD exp)	{ m_points.exp = exp;	}
		bool            block_exp;
		DWORD			GetNextExp() const;
#ifdef ENABLE_NEW_PET_SYSTEM
		DWORD			PetGetNextExp() const;
#endif
		LPCHARACTER		DistributeExp();	// 제일 많이 때린 사람을 리턴한다.
		void			DistributeHP(LPCHARACTER pkKiller);
		void			DistributeSP(LPCHARACTER pkKiller, int iMethod=0);

		void			SetPosition(int pos);
		bool			IsPosition(int pos) const	{ return m_pointsInstant.position == pos ? true : false; }
		int				GetPosition() const		{ return m_pointsInstant.position; }

		void			SetPart(BYTE bPartPos, WORD wVal);
		WORD			GetPart(BYTE bPartPos) const;
		WORD			GetOriginalPart(BYTE bPartPos) const;

		void			SetHP(int hp)		{ m_points.hp = hp; }
		int				GetHP() const		{ return m_points.hp; }

		void			SetSP(int sp)		{ m_points.sp = sp; }
		int				GetSP() const		{ return m_points.sp; }

		void			SetStamina(int stamina)	{ m_points.stamina = stamina; }
		int				GetStamina() const		{ return m_points.stamina; }

		void			SetMaxHP(int iVal)	{ m_pointsInstant.iMaxHP = iVal; }
		int				GetMaxHP() const	{ return m_pointsInstant.iMaxHP; }

		void			SetMaxSP(int iVal)	{ m_pointsInstant.iMaxSP = iVal; }
		int				GetMaxSP() const	{ return m_pointsInstant.iMaxSP; }

		void			SetMaxStamina(int iVal)	{ m_pointsInstant.iMaxStamina = iVal; }
		int				GetMaxStamina() const	{ return m_pointsInstant.iMaxStamina; }

		void			SetRandomHP(int v)	{ m_points.iRandomHP = v; }
		void			SetRandomSP(int v)	{ m_points.iRandomSP = v; }

		int				GetRandomHP() const	{ return m_points.iRandomHP; }
		int				GetRandomSP() const	{ return m_points.iRandomSP; }

		int				GetHPPct() const;

		void			SetRealPoint(BYTE idx, int val);
		int				GetRealPoint(BYTE idx) const;

		void			SetPoint(BYTE idx, long long val);
		long long		GetPoint(BYTE idx) const;
		int				GetLimitPoint(BYTE idx) const;
		int				GetPolymorphPoint(BYTE idx) const;
#ifdef ENABLE_NEW_PET_SYSTEM
		void			SendPetLevelUpEffect(int vid, int type, int value, int amount);
#endif

		const TMobTable &	GetMobTable() const;
		BYTE				GetMobRank() const;
		BYTE				GetMobBattleType() const;
		BYTE				GetMobSize() const;
		DWORD				GetMobDamageMin() const;
		DWORD				GetMobDamageMax() const;
		WORD				GetMobAttackRange() const;
		DWORD				GetMobDropItemVnum() const;
		float				GetMobDamageMultiply() const;

		// NEWAI
		bool			IsBerserker() const;
		bool			IsBerserk() const;
		void			SetBerserk(bool mode);

		bool			IsStoneSkinner() const;

		bool			IsGodSpeeder() const;
		bool			IsGodSpeed() const;
		void			SetGodSpeed(bool mode);

		bool			IsDeathBlower() const;
		bool			IsDeathBlow() const;
		
		bool			HasSpawnAnimation() const { return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_SPAWN_ANIMATION); };
		bool			IsReviver() const;
		bool			HasReviverInParty() const;
		bool			IsRevive() const;
		void			SetRevive(bool mode);
		// NEWAI END

		bool			IsRaceFlag(DWORD dwBit) const;
		bool			IsSummonMonster() const;
		DWORD			GetSummonVnum() const;

		DWORD			GetPolymorphItemVnum() const;
		DWORD			GetMonsterDrainSPPoint() const;

		void			MainCharacterPacket();	// 내가 메인캐릭터라고 보내준다.

		void			ComputePoints();
		void			ComputeBattlePoints();
		void			PointChange(BYTE type, long long amount, bool bAmount = false, bool bBroadcast = false);
		void			PointsPacket();
		void			ApplyPoint(BYTE bApplyType, int iVal);
		void			CheckMaximumPoints();	// HP, SP 등의 현재 값이 최대값 보다 높은지 검사하고 높다면 낮춘다.

		bool			Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false);

		void			Sitdown(int is_ground);
		void			Standup();

		void			SetRotation(float fRot);
		void			SetRotationToXY(long x, long y);
		float			GetRotation() const	{ return m_pointsInstant.fRot; }

		void			MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet);
		void			Motion(BYTE motion, LPCHARACTER victim = NULL);

		void			ChatPacket(BYTE type, const char *format, ...);
		std::string		GetLang();
		void 			SetLang(std::string lang);
#ifdef MULTI_LANGUAGE_SYSTEM
		void ChatInfoTrans(const char * format, ...);
#endif
		//void			MonsterChat(BYTE bMonsterChatType);
		void			SendGreetMessage();

		void			ResetPoint(int iLv);

		void			SetBlockMode(BYTE bFlag);
		void			SetBlockModeForce(BYTE bFlag);
		bool			IsBlockMode(BYTE bFlag) const	{ return (m_pointsInstant.bBlockMode & bFlag)?true:false; }

		bool			IsPolymorphed() const		{ return m_dwPolymorphRace>0; }
		bool			IsPolyMaintainStat() const	{ return m_bPolyMaintainStat; } // 이전 스텟을 유지하는 폴리모프.
		void			SetPolymorph(DWORD dwRaceNum, bool bMaintainStat = false);
		DWORD			GetPolymorphVnum() const	{ return m_dwPolymorphRace; }
		int				GetPolymorphPower() const;

		// FISING	
		void			fishing();
		bool			IsNearWater() const;
		void			fishing_take();
		// END_OF_FISHING

		// MINING
		void			mining(LPCHARACTER chLoad);
		void			mining_cancel();
		void			mining_take();
		// END_OF_MINING

		void			ResetPlayTime(DWORD dwTimeRemain = 0);

		void			CreateFly(BYTE bType, LPCHARACTER pkVictim);

		void			ResetChatCounter();
		BYTE			IncreaseChatCounter();
		BYTE			GetChatCounter() const;
		int				LastMessageAt;
		int				BlockChatAfter;
		char			LastPlayerMessage[CHAT_MAX_LEN + 1];
		void			PlayerPunish(bool PowerPunish, int Duration);
		bool			SpamListCheck(const char *Message);
		bool			BannListCheck(const char *Message);
		bool			SpamAllowBuf(const char *Message);

	protected:
		DWORD			m_dwPolymorphRace;
		bool			m_bPolyMaintainStat;
		DWORD			m_dwLoginPlayTime;
		DWORD			m_dwPlayerID;
		VID				m_vid;
		std::string		m_stName;
		BYTE			m_bCharType;
#ifdef ENABLE_NEW_PET_SYSTEM
		BYTE			m_stImmortalSt;
		DWORD			m_newpetskillcd[3];
#endif

		CHARACTER_POINT		m_points;
		CHARACTER_POINT_INSTANT	m_pointsInstant;
#ifdef ENABLE_KILL_STATISTICS
		CHARACTER_KILL_STATISTICS	m_killstatistics;
#endif

		int				m_iMoveCount;
		DWORD			m_dwPlayStartTime;
		BYTE			m_bAddChrState;
		bool			m_bSkipSave;
		std::string		m_stMobile;
		char			m_szMobileAuth[5];
		BYTE			m_bChatCounter;

		// End of Basic Points

		//////////////////////////////////////////////////////////////////////////////////
		// Move & Synchronize Positions
		//////////////////////////////////////////////////////////////////////////////////
	public:
		bool			IsStateMove() const			{ return IsState((CState&)m_stateMove); }
		bool			IsStateIdle() const			{ return IsState((CState&)m_stateIdle); }
		bool			IsWalking() const			{ return m_bNowWalking || GetStamina()<=0; }
		void			SetWalking(bool bWalkFlag)	{ m_bWalking=bWalkFlag; }
		void			SetNowWalking(bool bWalkFlag);	
		void			ResetWalking()			{ SetNowWalking(m_bWalking); }

		bool			Goto(long x, long y);	// 바로 이동 시키지 않고 목표 위치로 BLENDING 시킨다.
		void			Stop();

		bool			CanMove() const;		// 이동할 수 있는가?

		void			SyncPacket();
		bool			Sync(long x, long y);	// 실제 이 메소드로 이동 한다 (각 종 조건에 의한 이동 불가가 없음)
		bool			Move(long x, long y);	// 조건을 검사하고 Sync 메소드를 통해 이동 한다.
		void			OnMove(bool bIsAttack = false);	// 움직일때 불린다. Move() 메소드 이외에서도 불릴 수 있다.
		DWORD			GetMotionMode() const;
		float			GetMoveMotionSpeed() const;
		float			GetMoveSpeed() const;
		void			CalculateMoveDuration();
		void			SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime=0, int iRot=-1);
		DWORD			GetCurrentMoveDuration() const	{ return m_dwMoveDuration; }
		DWORD			GetWalkStartTime() const	{ return m_dwWalkStartTime; }
		DWORD			GetLastMoveTime() const		{ return m_dwLastMoveTime; }
		DWORD			GetLastAttackTime() const	{ return m_dwLastAttackTime; }

		void			SetLastAttacked(DWORD time);	// 마지막으로 공격받은 시간 및 위치를 저장함

		bool			SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList = true);
		bool			IsSyncOwner(LPCHARACTER ch) const;

		bool			WarpSet(long x, long y, long lRealMapIndex = 0);
		void			SetWarpLocation(long lMapIndex, long x, long y);
		void			WarpEnd();
		const PIXEL_POSITION & GetWarpPosition() const { return m_posWarp; }
		bool			WarpToPID(DWORD dwPID);

		void			SaveExitLocation();
		void			ExitToSavedLocation();

		void			StartStaminaConsume();
		void			StopStaminaConsume();
		bool			IsStaminaConsume() const;
		bool			IsStaminaHalfConsume() const;

		void			ResetStopTime();
		DWORD			GetStopTime() const;

	protected:
		void			ClearSync();

		float			m_fSyncTime;
		LPCHARACTER		m_pkChrSyncOwner;
		CHARACTER_LIST	m_kLst_pkChrSyncOwned;	// 내가 SyncOwner인 자들

		PIXEL_POSITION	m_posDest;
		PIXEL_POSITION	m_posStart;
		PIXEL_POSITION	m_posWarp;
		long			m_lWarpMapIndex;

		PIXEL_POSITION	m_posExit;
		long			m_lExitMapIndex;

		DWORD			m_dwMoveStartTime;
		DWORD			m_dwMoveDuration;

		DWORD			m_dwLastMoveTime;
		DWORD			m_dwLastAttackTime;
		DWORD			m_dwWalkStartTime;
		DWORD			m_dwStopTime;

		bool			m_bWalking;
		bool			m_bNowWalking;
		bool			m_bStaminaConsume;
		// End

		// Quickslot 관련
	public:
		void			SyncQuickslot(BYTE bType, UINT bOldPos, UINT bNewPos);
		bool			GetQuickslot(UINT pos, TQuickslot ** ppSlot);
		bool			SetQuickslot(UINT pos, TQuickslot & rSlot);
		bool			DelQuickslot(UINT pos);
		bool			SwapQuickslot(UINT a, UINT b);
		void			ChainQuickslotItem(LPITEM pItem, BYTE bType, UINT bOldPos);

	protected:
		TQuickslot		m_quickslot[QUICKSLOT_MAX_NUM];

		////////////////////////////////////////////////////////////////////////////////////////
		// Affect
	public:
		void			StartAffectEvent();
		void			ClearAffect(bool bSave=false, bool isPcKill=false);
		void			ComputeAffect(CAffect * pkAff, bool bAdd);
		bool			AddAffect(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube = false);
		void			RefreshAffect();
		bool			RemoveAffect(DWORD dwType);
		bool			IsAffectFlag(DWORD dwAff) const;
#ifdef ENABLE_PREMIUM_SYSTEM
		void			SetAffectFlag(DWORD dwAff);
		void			ResetAffectFlag(DWORD dwAff);
#endif


		bool			UpdateAffect();	// called from EVENT
		int				ProcessAffect();

		void			LoadAffect(DWORD dwCount, TPacketAffectElement * pElements);
		void			SaveAffect();

		// Affect loading이 끝난 상태인가?
		bool			IsLoadedAffect() const	{ return m_bIsLoadedAffect; }		

		bool			IsGoodAffect(BYTE bAffectType) const;

		void			RemoveGoodAffect();
		void			RemoveBadAffect();

		CAffect *		FindAffect(DWORD dwType, BYTE bApply=APPLY_NONE) const;
		const std::list<CAffect *> & GetAffectContainer() const	{ return m_list_pkAffect; }
		bool			RemoveAffect(CAffect * pkAff);

	protected:
		bool			m_bIsLoadedAffect;
		TAffectFlag		m_afAffectFlag;
		std::list<CAffect *>	m_list_pkAffect;

	public:
		// PARTY_JOIN_BUG_FIX
		void			SetParty(LPPARTY pkParty);
		LPPARTY			GetParty() const	{ return m_pkParty; }

		bool			RequestToParty(LPCHARACTER leader);
		void			DenyToParty(LPCHARACTER member);
		void			AcceptToParty(LPCHARACTER member);

		/// 자신의 파티에 다른 character 를 초대한다.
		/**
		 * @param	pchInvitee 초대할 대상 character. 파티에 참여 가능한 상태이어야 한다.
		 *
		 * 양측 character 의 상태가 파티에 초대하고 초대받을 수 있는 상태가 아니라면 초대하는 캐릭터에게 해당하는 채팅 메세지를 전송한다.
		 */
		void			PartyInvite(LPCHARACTER pchInvitee);

		/// 초대했던 character 의 수락을 처리한다.
		/**
		 * @param	pchInvitee 파티에 참여할 character. 파티에 참여가능한 상태이어야 한다.
		 *
		 * pchInvitee 가 파티에 가입할 수 있는 상황이 아니라면 해당하는 채팅 메세지를 전송한다.
		 */
		void			PartyInviteAccept(LPCHARACTER pchInvitee);

		/// 초대했던 character 의 초대 거부를 처리한다.
		/**
		 * @param [in]	dwPID 초대 했던 character 의 PID
		 */
		void			PartyInviteDeny(DWORD dwPID);

		bool			BuildUpdatePartyPacket(TPacketGCPartyUpdate & out);
		int				GetLeadershipSkillLevel() const;

		bool			CanSummon(int iLeaderShip);

		void			SetPartyRequestEvent(LPEVENT pkEvent) { m_pkPartyRequestEvent = pkEvent; }

	protected:

		/// 파티에 가입한다.
		/**
		 * @param	pkLeader 가입할 파티의 리더
		 */
		void			PartyJoin(LPCHARACTER pkLeader);

		/**
		 * 파티 가입을 할 수 없을 경우의 에러코드.
		 * Error code 는 시간에 의존적인가에 따라 변경가능한(mutable) type 과 정적(static) type 으로 나뉜다.
		 * Error code 의 값이 PERR_SEPARATOR 보다 낮으면 변경가능한 type 이고 높으면 정적 type 이다.
		 */
		enum PartyJoinErrCode {
			PERR_NONE		= 0,	///< 처리성공
			PERR_SERVER,			///< 서버문제로 파티관련 처리 불가
			PERR_DUNGEON,			///< 캐릭터가 던전에 있음
			PERR_OBSERVER,			///< 관전모드임
			PERR_LVBOUNDARY,		///< 상대 캐릭터와 레벨차이가 남
			PERR_LOWLEVEL,			///< 상대파티의 최고레벨보다 30레벨 낮음
			PERR_HILEVEL,			///< 상대파티의 최저레벨보다 30레벨 높음
			PERR_ALREADYJOIN,		///< 파티가입 대상 캐릭터가 이미 파티중
			PERR_PARTYISFULL,		///< 파티인원 제한 초과
			PERR_SEPARATOR,			///< Error type separator.
			PERR_DIFFEMPIRE,		///< 상대 캐릭터와 다른 제국임
			PERR_MAX				///< Error code 최고치. 이 앞에 Error code 를 추가한다.
		};

		/// 파티 가입이나 결성 가능한 조건을 검사한다.
		/**
		 * @param 	pchLeader 파티의 leader 이거나 초대한 character
		 * @param	pchGuest 초대받는 character
		 * @return	모든 PartyJoinErrCode 가 반환될 수 있다.
		 */
		static PartyJoinErrCode	IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		/// 파티 가입이나 결성 가능한 동적인 조건을 검사한다.
		/**
		 * @param 	pchLeader 파티의 leader 이거나 초대한 character
		 * @param	pchGuest 초대받는 character
		 * @return	mutable type 의 code 만 반환한다.
		 */
		static PartyJoinErrCode	IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		LPPARTY			m_pkParty;
		DWORD			m_dwLastDeadTime;
		LPEVENT			m_pkPartyRequestEvent;

		/**
		 * 파티초청 Event map.
		 * key: 초대받은 캐릭터의 PID
		 * value: event의 pointer
		 *
		 * 초대한 캐릭터들에 대한 event map.
		 */
		typedef std::map< DWORD, LPEVENT >	EventMap;
		EventMap		m_PartyInviteEventMap;

		// END_OF_PARTY_JOIN_BUG_FIX

		////////////////////////////////////////////////////////////////////////////////////////
		// Dungeon
	public:
		void			SetDungeon(LPDUNGEON pkDungeon);
		LPDUNGEON		GetDungeon() const	{ return m_pkDungeon; }
		LPDUNGEON		GetDungeonForce() const;
	protected:
		LPDUNGEON	m_pkDungeon;
		int			m_iEventAttr;

		////////////////////////////////////////////////////////////////////////////////////////
		// Guild
	public:
		void			SetGuild(CGuild * pGuild);
		CGuild*			GetGuild() const	{ return m_pGuild; }

		void			SetWarMap(CWarMap* pWarMap);
		CWarMap*		GetWarMap() const	{ return m_pWarMap; }

	protected:
		CGuild *		m_pGuild;
		DWORD			m_dwUnderGuildWarInfoMessageTime;
		CWarMap *		m_pWarMap;

		////////////////////////////////////////////////////////////////////////////////////////
		// Item related
	public:
		bool			CanHandleItem(bool bSkipRefineCheck = false, bool bSkipObserver = false); // 아이템 관련 행위를 할 수 있는가?

		bool			IsItemLoaded() const	{ return m_bItemLoaded; }
		void			SetItemLoaded()	{ m_bItemLoaded = true; }

		void			ClearItem();
		void			SetItem(TItemPos Cell, LPITEM item, bool bWereMine = false);
		LPITEM			GetItem(TItemPos Cell) const;
		LPITEM			GetInventoryItem(WORD wCell) const;
		bool			IsEmptyItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;
		
#ifdef ENABLE_SPECIAL_INVENTORY
		bool			IsEmptySpecialItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;
#endif
		void			SetWear(UINT bCell, LPITEM item);
		LPITEM			GetWear(UINT bCell) const;

		// MYSHOP_PRICE_LIST
		void			UseSilkBotary(void); 		/// 비단 보따리 아이템의 사용

		/// DB 캐시로 부터 받아온 가격정보 리스트를 유저에게 전송하고 보따리 아이템 사용을 처리한다.
		/**
		 * @param [in] p	가격정보 리스트 패킷
		 *
		 * 접속한 후 처음 비단 보따리 아이템 사용 시 UseSilkBotary 에서 DB 캐시로 가격정보 리스트를 요청하고
		 * 응답받은 시점에 이 함수에서 실제 비단보따리 사용을 처리한다.
		 */
		void			UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p);
		// END_OF_MYSHOP_PRICE_LIST

		bool			UseItemEx(LPITEM item, TItemPos DestCell);
		bool			UseItem(TItemPos Cell, TItemPos DestCell = NPOS);

		// ADD_REFINE_BUILDING
		bool			IsRefineThroughGuild() const;
		CGuild *		GetRefineGuild() const;
		int				ComputeRefineFee(int iCost, int iMultiply = 5) const;
		void			PayRefineFee(int iTotalMoney);
		void			SetRefineNPC(LPCHARACTER ch);
		// END_OF_ADD_REFINE_BUILDING

		bool			RefineItem(LPITEM pkItem, LPITEM pkTarget);
		bool			DropItem(TItemPos Cell,  ItemCountType bCount=0);
		bool       DestroyItem(TItemPos Cell);
		bool			GiveRecallItem(LPITEM item);
		void			ProcessRecallItem(LPITEM item);

		//	void			PotionPacket(int iPotionType);
		void			EffectPacket(int enumEffectType);
		void			SpecificEffectPacket(const char filename[128]);

		// ADD_MONSTER_REFINE
		bool			DoRefine(LPITEM item, bool bMoneyOnly = false);
		// END_OF_ADD_MONSTER_REFINE

		bool			DoRefineWithScroll(LPITEM item);
		bool			RefineInformation(UINT wCell, BYTE bType, int iAdditionalCell = -1);

		void			SetRefineMode(int iAdditionalCell = -1);
		void			ClearRefineMode();

		bool			GiveItem(LPCHARACTER victim, TItemPos Cell);
		bool			CanReceiveItem(LPCHARACTER from, LPITEM item) const;
		void			ReceiveItem(LPCHARACTER from, LPITEM item);
		bool			GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector <DWORD> &dwItemVnums, 
							std::vector <DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count);

		bool			MoveItem(TItemPos pos, TItemPos change_pos, ItemCountType num);
		bool			PickupItem(DWORD vid);
		bool			EquipItem(LPITEM item, int iCandidateCell = -1);
		bool			UnequipItem(LPITEM item);

		// 현재 item을 착용할 수 있는 지 확인하고, 불가능 하다면 캐릭터에게 이유를 알려주는 함수
		bool			CanEquipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		// 착용중인 item을 벗을 수 있는 지 확인하고, 불가능 하다면 캐릭터에게 이유를 알려주는 함수
		bool			CanUnequipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		bool			SwapItem(UINT wCell, UINT wDestCell);
		LPITEM			AutoGiveItem(DWORD dwItemVnum, ItemCountType bCount=1, int iRarePct = -1, bool bMsg = true);
		void			AutoGiveItem(LPITEM item, bool longOwnerShip = false);
		
#ifdef ENABLE_SPECIAL_INVENTORY
		int				GetEmptyInventory(LPITEM item) const;
#else
		int				GetEmptyInventory(BYTE size) const;
#endif
		int				GetEmptyDragonSoulInventory(LPITEM pItem) const;
		void			CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const;

		int				CountEmptyInventory() const;

		int				CountSpecifyItem(DWORD vnum) const;
		void			RemoveSpecifyItem(DWORD vnum, DWORD count = 1);
		LPITEM			FindSpecifyItem(DWORD vnum) const;
		LPITEM			FindSpecifyItemAttrInventory(DWORD vnum) const;
		LPITEM			FindItemByID(DWORD id) const;

		int				CountSpecifyTypeItem(BYTE type) const;
		void			RemoveSpecifyTypeItem(BYTE type, DWORD count = 1);

		bool			IsEquipUniqueItem(DWORD dwItemVnum) const;

		// CHECK_UNIQUE_GROUP
		bool			IsEquipUniqueGroup(DWORD dwGroupVnum) const;
		// END_OF_CHECK_UNIQUE_GROUP

		void			SendEquipment(LPCHARACTER ch);
		// End of Item

	protected:

		/// 한 아이템에 대한 가격정보를 전송한다.
		/**
		 * @param [in]	dwItemVnum 아이템 vnum
		 * @param [in]	dwItemPrice 아이템 가격
		 */
		void			SendMyShopPriceListCmd(DWORD dwItemVnum, long long dwItemPrice);

		bool			m_bNoOpenedShop;	///< 이번 접속 후 개인상점을 연 적이 있는지의 여부(열었던 적이 없다면 true)

		bool			m_bItemLoaded;
		int				m_iRefineAdditionalCell;
		bool			m_bUnderRefine;
		DWORD			m_dwRefineNPCVID;

 	public:
		////////////////////////////////////////////////////////////////////////////////////////
		// Money related
		long long		GetGold() const		{ return m_points.gold;	}
		void			SetGold(long long gold)	{ m_points.gold = gold;	}
		bool			DropGold(INT gold);
		long long		GetAllowedGold() const;
#ifdef ENABLE_ASLAN_LOTTERY
		long long		GetLottoMoney() const		{ return m_points.lotto_moneypool;	}
		void			SetLottoMoney(long long lotto_moneypool)	{ m_points.lotto_moneypool = lotto_moneypool;	}

		long long		GetLottoTotalMoney() const		{ return m_points.lotto_totalmoneywin;	}
		void			SetLottoTotalMoney(long long lotto_totalmoneywin)	{ m_points.lotto_totalmoneywin = lotto_totalmoneywin;	}
#endif
#ifdef ENABLE_PVP_ADVANCED
		int				GetDuel(const char* type) const;
		void			SetDuel(const char* type, int value);
#endif

		void			GiveGold(long long iAmount);	// ??? ??? ?? ??, ?? ?? ??
		// End of Money

		////////////////////////////////////////////////////////////////////////////////////////
		// Shop related
	public:
		void			SetShop(LPSHOP pkShop);
		LPSHOP			GetShop() const { return m_pkShop; }
		void			ShopPacket(BYTE bSubHeader);

		void			SetShopOwner(LPCHARACTER ch) { m_pkChrShopOwner = ch; }
		LPCHARACTER		GetShopOwner() const { return m_pkChrShopOwner;}


#ifdef ENABLE_PVP_ADVANCED
		void			DestroyPvP();
#endif

	protected:
		LPSHOP			m_pkShop;
		LPCHARACTER		m_pkChrShopOwner;
		// End of shop

		////////////////////////////////////////////////////////////////////////////////////////
		// Exchange related
		
#ifdef __SKILL_COLOR_SYSTEM__
	public:
		void			SetSkillColor(DWORD* dwSkillColor);
		DWORD* GetSkillColor() { return m_dwSkillColor[0]; }

	protected:
		DWORD			m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif

	public:
		bool			ExchangeStart(LPCHARACTER victim);
		void			SetExchange(CExchange * pkExchange);
		CExchange *		GetExchange() const	{ return m_pkExchange;	}

	protected:
		CExchange *		m_pkExchange;
		// End of Exchange

		////////////////////////////////////////////////////////////////////////////////////////
		// Battle
#ifdef ENABLE_DUNGEON_INFO_SYSTEM
	public:
		void			SetQuestDamage(int iDmg) { iQuestDamage = iDmg; }
		int				GetQuestDamage() { return iQuestDamage; }

	private:
		int				iQuestDamage;
#endif
	public:
		struct TBattleInfo
		{
			int iTotalDamage;
			int iAggro;

			TBattleInfo(int iTot, int iAggr)
				: iTotalDamage(iTot), iAggro(iAggr)
				{}
		};
		typedef std::map<VID, TBattleInfo>	TDamageMap;

		typedef struct SAttackLog
		{
			DWORD	dwVID;
			DWORD	dwTime;
		} AttackLog;

		bool				Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		bool				__Profile__Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		void				DeathPenalty(BYTE bExpLossPercent);
		void				ReviveInvisible(int iDur);

		bool				Attack(LPCHARACTER pkVictim, BYTE bType = 0);
		bool				IsAlive() const		{ return m_pointsInstant.position == POS_DEAD ? false : true; }
		bool				CanFight() const;

		bool				CanBeginFight() const;
		void				BeginFight(LPCHARACTER pkVictim); // pkVictimr과 싸우기 시작한다. (강제적임, 시작할 수 있나 체크하려면 CanBeginFight을 사용)

		bool				CounterAttack(LPCHARACTER pkChr); // 반격하기 (몬스터만 사용)

		bool				IsStun() const;
		void				Stun();
		bool				IsDead() const;
		void				Dead(LPCHARACTER pkKiller = NULL, bool bImmediateDead=false);

#ifdef ENABLE_NEW_PET_SYSTEM
		void				SetImmortal(int st) { m_stImmortalSt = st; };
		bool				IsImmortal() { return 1 == m_stImmortalSt; };
		void				SetNewPetSkillCD(int s, DWORD time) { m_newpetskillcd[s] = time; };
		DWORD				GetNewPetSkillCD(int s) { return m_newpetskillcd[s]; };
#endif

		void				Reward(bool bItemDrop);
		void				RewardGold(LPCHARACTER pkAttacker);

		bool				Shoot(BYTE bType);
		void				FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader);

		void				ForgetMyAttacker();
		void				AggregateMonster();
		void				AttractRanger();
		void				PullMonster();

		int					GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount = 1);
		void				UseArrow(LPITEM pkArrow, DWORD dwArrowCount);

		void				AttackedByPoison(LPCHARACTER pkAttacker);
		void				RemovePoison();

		void				AttackedByFire(LPCHARACTER pkAttacker, int amount, int count);
		void				RemoveFire();

		void				UpdateAlignment(int iAmount);
		int					GetAlignment() const;
#ifdef ENABLE_REBORN_SYSTEM
		void				UpdateReborn(int iAmount);
		int					GetReborn() const;
#endif
		
#ifdef ENABLE_TITLE_SYSTEM
		void				UpdateTitle(int iAmount);
		int					GetTitle() const;
#endif

		//선악치 얻기 
		int					GetRealAlignment() const;
		void				ShowAlignment(bool bShow);
#ifdef ENABLE_REBORN_SYSTEM
		int					GetRealReborn() const;
		void				ShowReborn(bool bShow);
#endif
		
#ifdef ENABLE_TITLE_SYSTEM
		int					GetRealTitle() const;
		void				ShowTitle(bool bShow);
#endif

		void				SetKillerMode(bool bOn);
		bool				IsKillerMode() const;
		void				UpdateKillerMode();

		BYTE				GetPKMode() const;
		void				SetPKMode(BYTE bPKMode);

		void				ItemDropPenalty(LPCHARACTER pkKiller);

		void				UpdateAggrPoint(LPCHARACTER ch, EDamageType type, int dam);

		//
		// HACK
		// 
	public:
		void				CheckPet();
		void SetComboSequence(BYTE seq);
		BYTE GetComboSequence() const;

		void SetLastComboTime(DWORD time);
		DWORD GetLastComboTime() const;

		int GetValidComboInterval() const;
		void SetValidComboInterval(int interval);

		BYTE GetComboIndex() const;

		void IncreaseComboHackCount(int k = 1);
		void ResetComboHackCount();
		void SkipComboAttackByTime(int interval);
		DWORD GetSkipComboAttackByTime() const;

	protected:
		BYTE m_bComboSequence;
		DWORD m_dwLastComboTime;
		int m_iValidComboInterval;
		BYTE m_bComboIndex;
		int m_iComboHackCount;
		DWORD m_dwSkipComboAttackByTime;

	protected:
		void				UpdateAggrPointEx(LPCHARACTER ch, EDamageType type, int dam, TBattleInfo & info);
		void				ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim);

		DWORD				m_dwFlyTargetID;
		std::vector<DWORD>	m_vec_dwFlyTargets;
		TDamageMap			m_map_kDamage;	// 어떤 캐릭터가 나에게 얼마만큼의 데미지를 주었는가?
//		AttackLog			m_kAttackLog;
		DWORD				m_dwKillerPID;

		int					m_iAlignment;		// Lawful/Chaotic value -200000 ~ 200000
		int					m_iRealAlignment;
#ifdef ENABLE_REBORN_SYSTEM
		int					m_iReborn;	
		int					m_iRealReborn;
#endif
#ifdef ENABLE_TITLE_SYSTEM
		int					m_iPrestige;	
		int					m_iRealPrestige;
#endif
		int					m_iKillerModePulse;
		BYTE				m_bPKMode;

		// Aggro
		DWORD				m_dwLastVictimSetTime;
		int					m_iMaxAggro;
		// End of Battle

		// Stone
	public:
		void				SetStone(LPCHARACTER pkChrStone);
		void				ClearStone();
		void				DetermineDropMetinStone();
		DWORD				GetDropMetinStoneVnum() const { return m_dwDropMetinStone; }
		BYTE				GetDropMetinStonePct() const { return m_bDropMetinStonePct; }

	protected:
		LPCHARACTER			m_pkChrStone;		// 나를 스폰한 돌
		CHARACTER_SET		m_set_pkChrSpawnedBy;	// 내가 스폰한 놈들
		DWORD				m_dwDropMetinStone;
		BYTE				m_bDropMetinStonePct;
		// End of Stone

	public:
		enum
		{
			SKILL_UP_BY_POINT,
			SKILL_UP_BY_BOOK,
			SKILL_UP_BY_TRAIN,

			// ADD_GRANDMASTER_SKILL
			SKILL_UP_BY_QUEST,
			// END_OF_ADD_GRANDMASTER_SKILL
		};

		void				SkillLevelPacket();
		void				SkillLevelUp(DWORD dwVnum, BYTE bMethod = SKILL_UP_BY_POINT);
		bool				SkillLevelDown(DWORD dwVnum);
		// ADD_GRANDMASTER_SKILL
		bool				UseSkill(DWORD dwVnum, LPCHARACTER pkVictim, bool bUseGrandMaster = true);
		void				ResetSkill();
		bool                IsSkillCooldown(DWORD dwVnum, float fSkillPower) { return m_SkillUseInfo[dwVnum].IsSkillCooldown(dwVnum, fSkillPower) ? true : false; }
		void				SetSkillLevel(DWORD dwVnum, BYTE bLev);
		int					GetUsedSkillMasterType(DWORD dwVnum);

		bool				IsLearnableSkill(DWORD dwSkillVnum) const;
		// END_OF_ADD_GRANDMASTER_SKILL

		bool				CheckSkillHitCount(const BYTE SkillID, const VID dwTargetVID);
		bool				CanUseSkill(DWORD dwSkillVnum) const;
		bool				IsUsableSkillMotion(DWORD dwMotionIndex) const;
		int					GetSkillLevel(DWORD dwVnum) const;
		int					GetSkillMasterType(DWORD dwVnum) const;
		int					GetSkillPower(DWORD dwVnum, BYTE bLevel = 0) const;

		time_t				GetSkillNextReadTime(DWORD dwVnum) const;
		void				SetSkillNextReadTime(DWORD dwVnum, time_t time);
		void				SkillLearnWaitMoreTimeMessage(DWORD dwVnum);

		void				ComputePassiveSkill(DWORD dwVnum);
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		int					ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0, BOOL isBuffNPC = false);
#else
		int					ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
#ifdef PARTY_BUFF_SYSTEM
		int 				ComputeSkillParty(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
#ifdef __SKILL_COSTUME__
		void 				ChangeBuffParty(DWORD dwVnum, LPCHARACTER pkAttacker);
#endif
		int					ComputeSkillAtPosition(DWORD dwVnum, const PIXEL_POSITION& posTarget, BYTE bSkillLevel = 0);
		void				ComputeSkillPoints();

		void				SetSkillGroup(BYTE bSkillGroup); 
		BYTE				GetSkillGroup() const		{ return m_points.skill_group; }

		int					ComputeCooltime(int time);

		void				GiveRandomSkillBook();

		void				DisableCooltime();
		bool				LearnSkillByBook(DWORD dwSkillVnum, BYTE bProb = 0);
		bool				LearnGrandMasterSkill(DWORD dwSkillVnum);

	private:
		bool				m_bDisableCooltime;
		DWORD				m_dwLastSkillTime;	///< 마지막으로 skill 을 쓴 시간(millisecond).
		// End of Skill

		// MOB_SKILL
	public:
		bool				HasMobSkill() const;
		size_t				CountMobSkill() const;
		const TMobSkillInfo * GetMobSkill(unsigned int idx) const;
		bool				CanUseMobSkill(unsigned int idx) const;
		bool				UseMobSkill(unsigned int idx);
		void				ResetMobSkillCooltime();
	protected:
		DWORD				m_adwMobSkillCooltime[MOB_SKILL_MAX_NUM];
		// END_OF_MOB_SKILL

		// for SKILL_MUYEONG
	public:
		void				StartMuyeongEvent();
		void				StopMuyeongEvent();

	private:
		LPEVENT				m_pkMuyeongEvent;

		// for SKILL_CHAIN lighting
	public:
		int					GetChainLightningIndex() const { return m_iChainLightingIndex; }
		void				IncChainLightningIndex() { ++m_iChainLightingIndex; }
		void				AddChainLightningExcept(LPCHARACTER ch) { m_setExceptChainLighting.insert(ch); }
		void				ResetChainLightningIndex() { m_iChainLightingIndex = 0; m_setExceptChainLighting.clear(); }
		int					GetChainLightningMaxCount() const;
		const CHARACTER_SET& GetChainLightingExcept() const { return m_setExceptChainLighting; }

	private:
		int					m_iChainLightingIndex;
		CHARACTER_SET m_setExceptChainLighting;

		// for SKILL_EUNHYUNG
	public:
		void				SetAffectedEunhyung();
		void				ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
		bool				GetAffectedEunhyung() const { return m_dwAffectedEunhyungLevel; }

	private:
		DWORD				m_dwAffectedEunhyungLevel;

		//
		// Skill levels
		//
	protected:
		TPlayerSkill*					m_pSkillLevels;
		boost::unordered_map<BYTE, int>		m_SkillDamageBonus;
		std::map<int, TSkillUseInfo>	m_SkillUseInfo;

		////////////////////////////////////////////////////////////////////////////////////////
		// AI related
	public:
		void			AssignTriggers(const TMobTable * table);
		LPCHARACTER		GetVictim() const;	// 공격할 대상 리턴
		void			SetVictim(LPCHARACTER pkVictim);
		LPCHARACTER		GetNearestVictim(LPCHARACTER pkChr);
		LPCHARACTER		GetProtege() const;	// 보호해야 할 대상 리턴

		bool			Follow(LPCHARACTER pkChr, float fMinimumDistance = 150.0f);
		bool			Return();
		bool			IsGuardNPC() const;
		bool			IsChangeAttackPosition(LPCHARACTER target) const;
		void			ResetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time() - AI_CHANGE_ATTACK_POISITION_TIME_NEAR;}
		void			SetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time();}

		bool			OnIdle();

		void			OnAttack(LPCHARACTER pkChrAttacker);
		void			OnClick(LPCHARACTER pkChrCauser);

		VID				m_kVIDVictim;

	protected:
		DWORD			m_dwLastChangeAttackPositionTime;
		CTrigger		m_triggerOnClick;
		// End of AI

		////////////////////////////////////////////////////////////////////////////////////////
		// Target
	protected:
		LPCHARACTER				m_pkChrTarget;		// 내 타겟
		CHARACTER_SET	m_set_pkChrTargetedBy;	// 나를 타겟으로 가지고 있는 사람들

	public:
		void				SetTarget(LPCHARACTER pkChrTarget);
		void				BroadcastTargetPacket();
		void				ClearTarget();
		void				CheckTarget();
		LPCHARACTER			GetTarget() const { return m_pkChrTarget; }

		////////////////////////////////////////////////////////////////////////////////////////
		// Safebox
	public:
		int					GetSafeboxSize() const;
		void				QuerySafeboxSize();
		void				SetSafeboxSize(int size);

		CSafebox *			GetSafebox() const;
		void				LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems);
		void				ChangeSafeboxSize(BYTE bSize);
		void				CloseSafebox();

		/// 창고 열기 요청
		/**
		 * @param [in]	pszPassword 1자 이상 6자 이하의 창고 비밀번호
		 *
		 * DB 에 창고열기를 요청한다.
		 * 창고는 중복으로 열지 못하며, 최근 창고를 닫은 시간으로 부터 10초 이내에는 열 지 못한다.
		 */
		void				ReqSafeboxLoad(const char* pszPassword);

		/// 창고 열기 요청의 취소
		/**
		 * ReqSafeboxLoad 를 호출하고 CloseSafebox 하지 않았을 때 이 함수를 호출하면 창고를 열 수 있다.
		 * 창고열기의 요청이 DB 서버에서 실패응답을 받았을 경우 이 함수를 사용해서 요청을 할 수 있게 해준다.
		 */
		void				CancelSafeboxLoad( void ) { m_bOpeningSafebox = false; }

		void				SetMallLoadTime(int t) { m_iMallLoadTime = t; }
		int					GetMallLoadTime() const { return m_iMallLoadTime; }

		CSafebox *			GetMall() const;
		void				LoadMall(int iItemCount, TPlayerItem * pItems);
		void				CloseMall();

		void				SetSafeboxOpenPosition();
		float				GetDistanceFromSafeboxOpen() const;

	protected:
		CSafebox *			m_pkSafebox;
		int					m_iSafeboxSize;
		int					m_iSafeboxLoadTime;
		bool				m_bOpeningSafebox;	///< 창고가 열기 요청 중이거나 열려있는가 여부, true 일 경우 열기요청이거나 열려있음.

		CSafebox *			m_pkMall;
		int					m_iMallLoadTime;

		PIXEL_POSITION		m_posSafeboxOpen;

		////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////
		// Mounting
	public:
		void				MountVnum(DWORD vnum);
		DWORD				GetMountVnum() const { return m_dwMountVnum; }
		DWORD				GetLastMountTime() const { return m_dwMountTime; }

		bool				CanUseHorseSkill();

		// Horse
		virtual	void		SetHorseLevel(int iLevel);

		virtual	bool		StartRiding();
		virtual	bool		StopRiding();

		virtual	DWORD		GetMyHorseVnum() const;

		virtual	void		HorseDie();
		virtual bool		ReviveHorse();

		virtual void		SendHorseInfo();
		virtual	void		ClearHorseInfo();

		void				HorseSummon(bool bSummon, bool bFromFar = false, DWORD dwVnum = 0, const char* name = 0);

		LPCHARACTER			GetHorse() const			{ return m_chHorse; }	 // 현재 소환중인 말
		LPCHARACTER			GetRider() const; // rider on horse
		void				SetRider(LPCHARACTER ch);

		bool				IsRiding() const;

#ifdef __PET_SYSTEM__
	public:
		CPetSystem*			GetPetSystem()				{ return m_petSystem; }

	protected:
		CPetSystem*			m_petSystem;
#endif 
#ifdef ENABLE_NEW_PET_SYSTEM
	public:
		CNewPetSystem*			GetNewPetSystem() { return m_newpetSystem; }

	protected:
		CNewPetSystem*			m_newpetSystem;
#endif
#ifdef LWT_BUFFI_SYSTEM
	public:
		CSupportSystem*			GetSupportSystem()				{ return m_supportSystem; }
	protected:
		CSupportSystem*			m_supportSystem;
#endif

	protected:
		LPCHARACTER			m_chHorse;
		LPCHARACTER			m_chRider;

		DWORD				m_dwMountVnum;
		DWORD				m_dwMountTime;

		BYTE				m_bSendHorseLevel;
		BYTE				m_bSendHorseHealthGrade;
		BYTE				m_bSendHorseStaminaGrade;

		////////////////////////////////////////////////////////////////////////////////////////
		// Detailed Log
	public:
		void				DetailLog() { m_bDetailLog = !m_bDetailLog; }
		void				ToggleMonsterLog();
		void				MonsterLog(const char* format, ...);
	private:
		bool				m_bDetailLog;
		bool				m_bMonsterLog;

		////////////////////////////////////////////////////////////////////////////////////////
		// Empire

	public:
		void 				SetEmpire(BYTE bEmpire);
		BYTE				GetEmpire() const { return m_bEmpire; }

	protected:
		BYTE				m_bEmpire;

		////////////////////////////////////////////////////////////////////////////////////////
		// Regen
	public:
		void				SetRegen(LPREGEN pkRegen);

	protected:
		PIXEL_POSITION			m_posRegen;
		float				m_fRegenAngle;
		LPREGEN				m_pkRegen;
		size_t				regen_id_; // to help dungeon regen identification
		// End of Regen

		////////////////////////////////////////////////////////////////////////////////////////
		// Resists & Proofs
	public:
		bool				CannotMoveByAffect() const;	// 특정 효과에 의해 움직일 수 없는 상태인가?
		bool				IsImmune(DWORD dwImmuneFlag);
		void                UpdateImmuneFlags();
		void				SetImmuneFlag(DWORD dw) { m_pointsInstant.dwImmuneFlag = dw; }

	protected:
		void				ApplyMobAttribute(const TMobTable* table);
		// End of Resists & Proofs

		////////////////////////////////////////////////////////////////////////////////////////
		// QUEST
		// 
	public:
		void				SetQuestNPCID(DWORD vid);
		DWORD				GetQuestNPCID() const { return m_dwQuestNPCVID; }
		LPCHARACTER			GetQuestNPC() const;

		void				SetQuestItemPtr(LPITEM item);
		void				ClearQuestItemPtr();
		LPITEM				GetQuestItemPtr() const;

		void				SetQuestBy(DWORD dwQuestVnum)	{ m_dwQuestByVnum = dwQuestVnum; }
		DWORD				GetQuestBy() const			{ return m_dwQuestByVnum; }

		int					GetQuestFlag(const std::string& flag) const;
		void				SetQuestFlag(const std::string& flag, int value);

		void				ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID);

	private:
		DWORD				m_dwQuestNPCVID;
		DWORD				m_dwQuestByVnum;
		LPITEM				m_pQuestItem;

		// Events
	public:
		bool				StartStateMachine(int iPulse = 1);
		void				StopStateMachine();
		void				UpdateStateMachine(DWORD dwPulse);
		void				SetNextStatePulse(int iPulseNext);

		// 캐릭터 인스턴스 업데이트 함수. 기존엔 이상한 상속구조로 CFSM::Update 함수를 호출하거나 UpdateStateMachine 함수를 사용했는데, 별개의 업데이트 함수 추가함.
		void				UpdateCharacter(DWORD dwPulse);

	protected:
		DWORD				m_dwNextStatePulse;

		// Marriage
	public:
		LPCHARACTER			GetMarryPartner() const;
		void				SetMarryPartner(LPCHARACTER ch);
		int					GetMarriageBonus(DWORD dwItemVnum, bool bSum = true);

		void				SetWeddingMap(marriage::WeddingMap* pMap);
		marriage::WeddingMap* GetWeddingMap() const { return m_pWeddingMap; }

	private:
		marriage::WeddingMap* m_pWeddingMap;
		LPCHARACTER			m_pkChrMarried;

		// Warp Character
	public:
		void				StartWarpNPCEvent();
		void				ChannelSwitch(int new_ch);

	public:
		void				StartSaveEvent();
		void				StartRecoveryEvent();
		void				StartCheckSpeedHackEvent();
		void				StartDestroyWhenIdleEvent();

		LPEVENT				m_pkDeadEvent;
		LPEVENT				m_pkStunEvent;
		LPEVENT				m_pkSaveEvent;
		LPEVENT				m_pkRecoveryEvent;
		LPEVENT				m_pkTimedEvent;
		LPEVENT				m_pkFishingEvent;
		LPEVENT				m_pkAffectEvent;
		LPEVENT				m_pkPoisonEvent;
		LPEVENT				m_pkFireEvent;
		LPEVENT				m_pkWarpNPCEvent;
		//DELAYED_WARP
		//END_DELAYED_WARP

		// MINING
		LPEVENT				m_pkMiningEvent;
		// END_OF_MINING
		LPEVENT				m_pkWarpEvent;
		LPEVENT				m_pkCheckSpeedHackEvent;
		LPEVENT				m_pkDestroyWhenIdleEvent;
		LPEVENT				m_pkPetSystemUpdateEvent;
#ifdef ENABLE_NEW_PET_SYSTEM
		LPEVENT				m_pkNewPetSystemUpdateEvent;
		LPEVENT				m_pkNewPetSystemExpireEvent;
#endif


		bool IsWarping() const { return m_pkWarpEvent ? true : false; }

		bool				m_bHasPoisoned;

		const CMob *		m_pkMobData;
		CMobInstance *		m_pkMobInst;

		std::map<int, LPEVENT> m_mapMobSkillEvent;

		friend struct FuncSplashDamage;
		friend struct FuncSplashAffect;
		friend class CFuncShoot;

	public:
		int				GetPremiumRemainSeconds(BYTE bType) const;

	private:
		int				m_aiPremiumTimes[PREMIUM_MAX_NUM];

		// CHANGE_ITEM_ATTRIBUTES
		static const DWORD		msc_dwDefaultChangeItemAttrCycle;	///< 디폴트 아이템 속성변경 가능 주기
		static const char		msc_szLastChangeItemAttrFlag[];		///< 최근 아이템 속성을 변경한 시간의 Quest Flag 이름
		static const char		msc_szChangeItemAttrCycleFlag[];		///< 아이템 속성병경 가능 주기의 Quest Flag 이름
		// END_OF_CHANGE_ITEM_ATTRIBUTES

		// PC_BANG_ITEM_ADD
	private :
		bool m_isinPCBang;

	public :
		bool SetPCBang(bool flag) { m_isinPCBang = flag; return m_isinPCBang; }
		bool IsPCBang() const { return m_isinPCBang; }
		// END_PC_BANG_ITEM_ADD

		// NEW_HAIR_STYLE_ADD
	public :
		bool ItemProcess_Hair(LPITEM item, int iDestCell);
		// END_NEW_HAIR_STYLE_ADD

	public :
		void ClearSkill();
		void ClearSubSkill();

		// RESET_ONE_SKILL
		bool ResetOneSkill(DWORD dwVnum);
		// END_RESET_ONE_SKILL

	private :
		void SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag);

	// ARENA
	private :
		CArena *m_pArena;
		bool m_ArenaObserver;
		int m_nPotionLimit;

	public :
		void 	SetArena(CArena* pArena) { m_pArena = pArena; }
		void	SetArenaObserverMode(bool flag) { m_ArenaObserver = flag; }

		CArena* GetArena() const { return m_pArena; }
		bool	GetArenaObserverMode() const { return m_ArenaObserver; }

		void	SetPotionLimit(int count) { m_nPotionLimit = count; }
		int		GetPotionLimit() const { return m_nPotionLimit; }
	// END_ARENA

		//PREVENT_TRADE_WINDOW
	public:
		bool	IsOpenSafebox() const { return m_isOpenSafebox ? true : false; }
		void 	SetOpenSafebox(bool b) { m_isOpenSafebox = b; }

		int		GetSafeboxLoadTime() const { return m_iSafeboxLoadTime; }
		void	SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }
		//END_PREVENT_TRADE_WINDOW
	private:
		bool	m_isOpenSafebox;

	public:
		int		GetSkillPowerByLevel(int level, bool bMob = false) const;
		
		//PREVENT_REFINE_HACK
		int		GetRefineTime() const { return m_iRefineTime; }
		void	SetRefineTime() { m_iRefineTime = thecore_pulse(); } 
		int		m_iRefineTime;
		//END_PREVENT_REFINE_HACK

		//RESTRICT_USE_SEED_OR_MOONBOTTLE
		int 	GetUseSeedOrMoonBottleTime() const { return m_iSeedTime; }
		void  	SetUseSeedOrMoonBottleTime() { m_iSeedTime = thecore_pulse(); }
		int 	m_iSeedTime;
		//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
		
		//PREVENT_PORTAL_AFTER_EXCHANGE
		int		GetExchangeTime() const { return m_iExchangeTime; }
		void	SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }
		int		m_iExchangeTime;
		//END_PREVENT_PORTAL_AFTER_EXCHANGE
		
		int 	m_iMyShopTime;
		int		GetMyShopTime() const	{ return m_iMyShopTime; }
		void	SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }

		// Hack 방지를 위한 체크.
		bool	IsHack(bool bSendMsg = true, bool bCheckShopOwner = false, int limittime = g_nPortalLimitTime);

		// MONARCH
		BOOL	IsMonarch() const;
		// END_MONARCH
		void Say(const std::string & s);

		enum MONARCH_COOLTIME
		{
			MC_HEAL = 10,
			MC_WARP	= 60,
			MC_TRANSFER = 60,
			MC_TAX = (60 * 60 * 24 * 7),
			MC_SUMMON = (60 * 60),
		};

		enum MONARCH_INDEX
		{ 
			MI_HEAL = 0,
			MI_WARP,
			MI_TRANSFER,
			MI_TAX,
			MI_SUMMON,
			MI_MAX
		};

		DWORD m_dwMonarchCooltime[MI_MAX];
		DWORD m_dwMonarchCooltimelimit[MI_MAX];

		void  InitMC();
		DWORD GetMC(enum MONARCH_INDEX e) const;
		void SetMC(enum MONARCH_INDEX e);
		bool IsMCOK(enum MONARCH_INDEX e) const;
		DWORD GetMCL(enum MONARCH_INDEX e) const;
		DWORD GetMCLTime(enum MONARCH_INDEX e) const;

	public:
		bool ItemProcess_Polymorph(LPITEM item);

		// by mhh
		LPITEM*	GetCubeItem() { return m_pointsInstant.pCubeItems; }
		bool IsCubeOpen () const	{ return (m_pointsInstant.pCubeNpc?true:false); }
		void SetCubeNpc(LPCHARACTER npc)	{ m_pointsInstant.pCubeNpc = npc; }
		bool CanDoCube() const;

	public:
		bool IsSiegeNPC() const;

	private:
		//중국 전용
		//18세 미만 전용
		//3시간 : 50 % 5 시간 0%
		e_overtime m_eOverTime;

	public:
		bool IsOverTime(e_overtime e) const { return (e == m_eOverTime); }
		void SetOverTime(e_overtime e) { m_eOverTime = e; }

	private:
		int		m_deposit_pulse;

	public:
		void	UpdateDepositPulse();
		bool	CanDeposit() const;

	private:
		void	__OpenPrivateShop();

	public:
		struct AttackedLog
		{
			DWORD 	dwPID;
			DWORD	dwAttackedTime;
			
			AttackedLog() : dwPID(0), dwAttackedTime(0)
			{
			}
		};

		AttackLog	m_kAttackLog;
		AttackedLog m_AttackedLog;
		int			m_speed_hack_count;

	private :
		std::string m_strNewName;

	public :
		const std::string GetNewName() const { return this->m_strNewName; }
		void SetNewName(const std::string name) { this->m_strNewName = name; }

	public :
		void GoHome();

	private :
		std::set<DWORD>	m_known_guild;

	public :
		void SendGuildName(CGuild* pGuild);
		void SendGuildName(DWORD dwGuildID);

	private :
		DWORD m_dwLogOffInterval;

	public :
		DWORD GetLogOffInterval() const { return m_dwLogOffInterval; }

	public:
		bool UnEquipSpecialRideUniqueItem ();

		bool CanWarp () const;

	private:
		DWORD m_dwLastGoldDropTime;

	public:
		void StartHackShieldCheckCycle(int seconds);
		void StopHackShieldCheckCycle();

		bool GetHackShieldCheckMode() const { return m_HackShieldCheckMode; }
		void SetHackShieldCheckMode(bool m) { m_HackShieldCheckMode = m; }

		LPEVENT m_HackShieldCheckEvent;

	private:
		bool	m_HackShieldCheckMode;

	public:
		bool				IsMultiFarmBlocked() const { return m_bFarmState; }
		void				SetMultiFarmBlock(bool bState) { m_bFarmState = bState; }
		void SetLastToggleState(DWORD Toggle_time) { m_toggleTime = Toggle_time; }
		DWORD GetLastToggleState() { return m_toggleTime; }

	private:
		bool				m_bFarmState;
		DWORD m_toggleTime = 0;

		
	public:
		void AutoRecoveryItemProcess (const EAffectTypes);

	public:
		void BuffOnAttr_AddBuffsFromItem(LPITEM pItem);
		void BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem);

	private:
		void BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue);
		void BuffOnAttr_ClearAll();

		typedef std::map <BYTE, CBuffOnAttributes*> TMapBuffOnAttrs;
		TMapBuffOnAttrs m_map_buff_on_attrs;
		// 무적 : 원활한 테스트를 위하여.
	public:
		void SetArmada() { cannot_dead = true; }
		void ResetArmada() { cannot_dead = false; }
	private:
		bool cannot_dead;

#ifdef __PET_SYSTEM__
	private:
		bool m_bIsPet;
	public:
		void SetPet() { m_bIsPet = true; }
		bool IsPet() { return m_bIsPet; }
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	private:
		bool m_bIsNewPet;
		int m_eggvid;
	public:
		void SetNewPet() { m_bIsNewPet = true; }
		bool IsNewPet() const { return m_bIsNewPet ? true : false; }
		void SetEggVid(int vid) { m_eggvid = vid; }
		int GetEggVid() { return m_eggvid; }
#endif

	//최종 데미지 보정.
	private:
		float m_fAttMul;
		float m_fDamMul;
	public:
		float GetAttMul() { return this->m_fAttMul; }
		void SetAttMul(float newAttMul) {this->m_fAttMul = newAttMul; }
		float GetDamMul() { return this->m_fDamMul; }
		void SetDamMul(float newDamMul) {this->m_fDamMul = newDamMul; }

	private:
		bool IsValidItemPosition(TItemPos Pos) const;

		//독일 선물 기능 패킷 임시 저장
	private:
		unsigned int itemAward_vnum;
		char		 itemAward_cmd[20];
		//bool		 itemAward_flag;
	public:
		unsigned int GetItemAward_vnum() { return itemAward_vnum; }
		char*		 GetItemAward_cmd() { return itemAward_cmd;	  }
		//bool		 GetItemAward_flag() { return itemAward_flag; }
		void		 SetItemAward_vnum(unsigned int vnum) { itemAward_vnum = vnum; }
		void		 SetItemAward_cmd(char* cmd) { strcpy(itemAward_cmd,cmd); }
		//void		 SetItemAward_flag(bool flag) { itemAward_flag = flag; }

	public:
		//용혼석
		
		// 캐릭터의 affect, quest가 load 되기 전에 DragonSoul_Initialize를 호출하면 안된다.
		// affect가 가장 마지막에 로드되어 LoadAffect에서 호출함.
		void	DragonSoul_Initialize();

		bool	DragonSoul_IsQualified() const;
		void	DragonSoul_GiveQualification();

		int		DragonSoul_GetActiveDeck() const;
		bool	DragonSoul_IsDeckActivated() const;
		bool	DragonSoul_ActivateDeck(int deck_idx);

		void	DragonSoul_DeactivateAll();
		// 반드시 ClearItem 전에 불러야 한다.
		// 왜냐하면....
		// 용혼석 하나 하나를 deactivate할 때마다 덱에 active인 용혼석이 있는지 확인하고,
		// active인 용혼석이 하나도 없다면, 캐릭터의 용혼석 affect와, 활성 상태를 제거한다.
		// 
		// 하지만 ClearItem 시, 캐릭터가 착용하고 있는 모든 아이템을 unequip하는 바람에,
		// 용혼석 Affect가 제거되고, 결국 로그인 시, 용혼석이 활성화되지 않는다.
		// (Unequip할 때에는 로그아웃 상태인지, 아닌지 알 수 없다.)
		// 용혼석만 deactivate시키고 캐릭터의 용혼석 덱 활성 상태는 건드리지 않는다.
		void	DragonSoul_CleanUp();
		// 용혼석 강화창
	public:
		bool		DragonSoul_RefineWindow_Open(LPENTITY pEntity);
		bool		DragonSoul_RefineWindow_Close();
		LPENTITY	DragonSoul_RefineWindow_GetOpener() { return  m_pointsInstant.m_pDragonSoulRefineWindowOpener; }
		bool		DragonSoul_RefineWindow_CanRefine();
	private:
		// SyncPosition을 악용하여 타유저를 이상한 곳으로 보내는 핵 방어하기 위하여,
		// SyncPosition이 일어날 때를 기록.
		timeval		m_tvLastSyncTime;
		int			m_iSyncHackCount;
	public:
		void			SetLastSyncTime(const timeval &tv) { memcpy(&m_tvLastSyncTime, &tv, sizeof(timeval)); }
		const timeval&	GetLastSyncTime() { return m_tvLastSyncTime; }
		void			SetSyncHackCount(int iCount) { m_iSyncHackCount = iCount;}
		int				GetSyncHackCount() { return m_iSyncHackCount; }
#ifdef __HIDE_COSTUME_SYSTEM__
	public:
		void SetBodyCostumeHidden(bool hidden);
		bool IsBodyCostumeHidden() const { return m_bHideBodyCostume; };

		void SetHairCostumeHidden(bool hidden);
		bool IsHairCostumeHidden() const { return m_bHideHairCostume; };

	#ifdef __SASH_SYSTEM__
		void SetSashCostumeHidden(bool hidden);
		bool IsSashCostumeHidden() const { return m_bHideSashCostume; };
	#endif

	#ifdef __WEAPON_COSTUME_SYSTEM__
		void SetWeaponCostumeHidden(bool hidden);
		bool IsWeaponCostumeHidden() const { return m_bHideWeaponCostume; };
	#endif

	private:
		bool m_bHideBodyCostume;
		bool m_bHideHairCostume;
	#ifdef __SASH_SYSTEM__
		bool m_bHideSashCostume;
	#endif
	#ifdef __WEAPON_COSTUME_SYSTEM__
		bool m_bHideWeaponCostume;
	#endif
#endif
#ifdef ENABLE_ASLAN_LOTTERY
	public:
		void OpenLottoWindow();
		void SendLottoTicketInfo();
		void SendLottoBasicInfo();
		void SendLottoRankingInfo();
	protected:
		DWORD	m_dwLastLottOpenMainTime;
		DWORD	m_dwLastLottoOpenRankingTime;
#endif

#ifdef __SASH_SYSTEM__
	protected:
		bool	m_bSashCombination, m_bSashAbsorption;
	
	public:
		bool	isSashOpened(bool bCombination) {return bCombination ? m_bSashCombination : m_bSashAbsorption;}
		void	OpenSash(bool bCombination);
		void	CloseSash();
		void	ClearSashMaterials();
		bool	CleanSashAttr(LPITEM pkItem, LPITEM pkTarget);
		LPITEM*	GetSashMaterials() {return m_pointsInstant.pSashMaterials;}
		bool	SashIsSameGrade(long lGrade);
		long long	GetSashCombinePrice(long lGrade);
		void	GetSashCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs);
		BYTE	CheckEmptyMaterialSlot();
		void	AddSashMaterial(TItemPos tPos, BYTE bPos);
		void	RemoveSashMaterial(BYTE bPos);
		BYTE	CanRefineSashMaterials();
		void	RefineSashMaterials();
#endif
#ifdef __CHANGELOOK_SYSTEM__
	protected:
		bool	m_bChangeLook;
	
	public:
		bool	isChangeLookOpened() {return m_bChangeLook;}
		void	ChangeLookWindow(bool bOpen = false, bool bRequest = false);
		void	ClearClWindowMaterials();
		LPITEM*	GetClWindowMaterials() {return m_pointsInstant.pClMaterials;}
		BYTE	CheckClEmptyMaterialSlot();
		void	AddClMaterial(TItemPos tPos, BYTE bPos);
		void	RemoveClMaterial(BYTE bPos);
		void	RefineClMaterials();
		bool	CleanTransmutation(LPITEM pkItem, LPITEM pkTarget);
#endif

#ifdef __AURA_SYSTEM__
	private:
		BYTE		m_bAuraRefineWindowType;
		bool		m_bAuraRefineWindowOpen;
		TItemPos	m_pAuraRefineWindowItemSlot[AURA_SLOT_MAX];
		TAuraRefineInfo m_bAuraRefineInfo[AURA_REFINE_INFO_SLOT_MAX];

	protected:
		BYTE		__GetAuraAbsorptionRate(BYTE bLevel, BYTE bBoostIndex) const;
		TAuraRefineInfo __GetAuraRefineInfo(TItemPos Cell);
		TAuraRefineInfo __CalcAuraRefineInfo(TItemPos Cell, TItemPos MaterialCell);
		TAuraRefineInfo __GetAuraEvolvedRefineInfo(TItemPos Cell);

	public:
		void		OpenAuraRefineWindow(LPENTITY pOpener, EAuraWindowType type);
		bool		IsAuraRefineWindowOpen() const { return  m_bAuraRefineWindowOpen; }
		BYTE		GetAuraRefineWindowType() const { return  m_bAuraRefineWindowType; }
		LPENTITY	GetAuraRefineWindowOpener() { return  m_pointsInstant.m_pAuraRefineWindowOpener; }

		bool		IsAuraRefineWindowCanRefine();

		void		AuraRefineWindowCheckIn(BYTE bAuraRefineWindowType, TItemPos AuraCell, TItemPos ItemCell);
		void		AuraRefineWindowCheckOut(BYTE bAuraRefineWindowType, TItemPos AuraCell);
		void		AuraRefineWindowAccept(BYTE bAuraRefineWindowType);
		void		AuraRefineWindowClose();
#endif

public:
	void MountSummon();
	void MountUnsummon();
	void CheckMount();
	void SetHorseAI();
	
#ifdef LWT_BUFFI_SYSTEM
	private:
		bool m_bIsSupport;
		LPCHARACTER m_Owner;
		DWORD m_dwBuffiCostume;
		DWORD m_dwBuffiHair;
		DWORD m_dwBuffiWeapon;
		DWORD m_dwBuffiEffect;
	protected:
		DWORD		m_dwBuffiTime;
	public:
		int		GetBuffiTime() const { return m_dwBuffiTime; }
		void	SetBuffiTime(int time) {m_dwBuffiTime = time; }
		void SetSupport() { m_bIsSupport = true; }
		bool IsSupport() { return m_bIsSupport; }
		void SetBuffiArmor(DWORD vnum) { m_dwBuffiCostume = vnum; }
		void SetBuffiWeapon(DWORD vnum) { m_dwBuffiWeapon = vnum; }
		void SetBuffiHair(DWORD vnum) { m_dwBuffiHair = vnum; }
		void SetBuffiEffect(DWORD vnum) { m_dwBuffiEffect = vnum; }
		DWORD GetBuffiArmor() { return m_dwBuffiCostume; }
		DWORD SetBuffiWeapon() { return m_dwBuffiWeapon; }
		DWORD GetBuffiHair() { return m_dwBuffiHair; }
		DWORD GetBuffiEffect() { return m_dwBuffiEffect; }
		void SendSupportSkillPacket(DWORD skill_vnum, DWORD skillValue);
		void SetOwner(LPCHARACTER owner) { m_Owner = owner; }
		LPCHARACTER GetOwner() { return m_Owner; }
#endif
	public:
		void			SetIndexLotteryReward(BYTE index) { bIndexLotteryReward = index; };
		DWORD			GetIndexLotteryReward()  const { return bIndexLotteryReward; }
	private:
		BYTE			bIndexLotteryReward;
		
	public:
		struct S_CARD
		{
			DWORD	type;
			DWORD	value;
		};

		struct CARDS_INFO
		{
			S_CARD cards_in_hand[MAX_CARDS_IN_HAND];
			S_CARD cards_in_field[MAX_CARDS_IN_FIELD];
			DWORD	cards_left;
			DWORD	field_points;
			DWORD	points;
		};
		
		void			Cards_open(DWORD safemode);
		void			Cards_clean_list();
		DWORD			GetEmptySpaceInHand();
		void			Cards_pullout();
		void			RandomizeCards();
		bool			CardWasRandomized(DWORD type, DWORD value);
		void			SendUpdatedInformations();
		void			SendReward();
		void			CardsDestroy(DWORD reject_index);
		void			CardsAccept(DWORD accept_index);
		void			CardsRestore(DWORD restore_index);
		DWORD			GetEmptySpaceInField();
		DWORD			GetAllCardsCount();
		bool			TypesAreSame();
		bool			ValuesAreSame();
		bool			CardsMatch();
		DWORD			GetLowestCard();
		bool			CheckReward();
		void			CheckCards();
		void			RestoreField();
		void			ResetField();
		void			CardsEnd();
		void			GetGlobalRank(char * buffer, size_t buflen);
		void			GetRundRank(char * buffer, size_t buflen);
	protected:
		CARDS_INFO	character_cards;
		S_CARD	randomized_cards[24];

#ifdef ENABLE_CHECK_PICKUP_HACK
	private:
		DWORD m_dwLastPickupTime;
	public:
		void SetLastPickupTime() { m_dwLastPickupTime = get_dword_time(); }
		DWORD GetLastPickupTime() { return m_dwLastPickupTime; }
#endif
#ifdef ENABLE_CSHIELD
	private:
		LPEVENT m_pkCShieldEvent;
		LPEVENT m_pkCShieldDataEvent;
		spCShield m_cshield;
	public:
		void StartCShieldEvent();
		void StartCShieldDataEvent();
		void SendCShieldPacket(bool start);
		spCShield GetCShield() const { return m_cshield; }
#endif
#ifdef ENABLE_CHECK_GHOSTMODE
	private:
		DWORD m_dwCountGhostmodePoint;
	public:
		DWORD GetGhostmodeCount() { return m_dwCountGhostmodePoint; }
		void AddGhostmodeCount() { m_dwCountGhostmodePoint += 1; }
		void ResetGhostmodeCount() { m_dwCountGhostmodePoint = 0; }
#endif
#ifdef ENABLE_CHECK_WALLHACK
	private:
		DWORD m_dwCountWallhackPoint;
	public:
		DWORD GetWallhackCount() { return m_dwCountWallhackPoint; }
		void AddWallhackCount() { m_dwCountWallhackPoint += 1; }
		void ResetWallhackCount() { m_dwCountWallhackPoint = 0; }
#endif
#ifdef __SKILL_COSTUME__
	public:
		void 			UseSkillCostumeItem(LPITEM item, bool bEquip);
		void 			SetSkillMotion(BYTE * bSkillMotion);
		BYTE* 			GetSkillMotion();
	protected:
		BYTE			m_bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT]; 		
#endif
#if defined(__DAILY_GIFT_SYSTEM__)
	typedef std::vector<TPacketGCDailyGift> TDailyGift;
	public:
		void LoadDailyGiftWeek();
		void CloseDailyGift();

		bool HasDailyGiftMission();
		BYTE GetDailyGiftStatus(BYTE bDay, DWORD dwCollectTime = 0);
		DWORD GetDailyGiftStartTime();
		DWORD GetDailyGiftRenewTime();
		BYTE GetDailyGiftWeek();
		BYTE CheckDailyGiftStatus();
		BYTE RenewDailyGift();
		void CheckDailyGiftRenewTime();
		BYTE DailyGiftOrderWeek();

		void CollectDailyGift(BYTE bSlotIndex, bool bUseTicket);

		bool SetCash(BYTE bType = ERequestCharge_Cash, DWORD dwAmount = 0);
		DWORD GetCash(BYTE bType = ERequestCharge_Cash);

	private:
		TDailyGift m_vecDailyGift;
		DWORD m_dwDailyGiftRenewTime;
		bool m_bDailyGiftLoad;
		bool m_bDailyGiftOpen;
#endif
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	typedef std::list<TPlayerExtBattlePassMission*> ListExtBattlePassMap;
	public:
		void LoadExtBattlePass(DWORD dwCount, TPlayerExtBattlePassMission* data);
		DWORD GetExtBattlePassMissionProgress(DWORD dwBattlePassType, BYTE bMissionIndex, BYTE bMissionType);
		bool IsExtBattlePassCompletedMission(DWORD dwBattlePassType, BYTE bMissionIndex, BYTE bMissionType);
		bool IsExtBattlePassRegistered(BYTE bBattlePassType, DWORD dwBattlePassID);
		void UpdateExtBattlePassMissionProgress(DWORD dwMissionID, DWORD dwUpdateValue, DWORD dwCondition, bool isOverride = false);
		void SetExtBattlePassMissionProgress(BYTE bBattlePassType, DWORD dwMissionIndex, DWORD dwMissionType, DWORD dwUpdateValue);
		
		bool		IsLoadedExtBattlePass()		const	{ return m_bIsLoadedExtBattlePass; }
		int			GetExtBattlePassPremiumID()	const	{ return m_points.battle_pass_premium_id;	}
		void		SetExtBattlePassPremiumID(int battle_pass_premium_id)	{ m_points.battle_pass_premium_id = battle_pass_premium_id;}

		void				SetLastReciveExtBattlePassInfoTime(DWORD time);
		DWORD			GetLastReciveExtBattlePassInfoTime() const	{ return m_dwLastReciveExtBattlePassInfoTime; }
		void				SetLastReciveExtBattlePassOpenRanking(DWORD time);
		DWORD			GetLastReciveExtBattlePassOpenRanking() const	{ return m_dwLastExtBattlePassOpenRankingTime; }
	protected:
		DWORD	m_dwLastReciveExtBattlePassInfoTime;
		DWORD	m_dwLastExtBattlePassOpenRankingTime;
		
	private:
		bool m_bIsLoadedExtBattlePass;
		ListExtBattlePassMap m_listExtBattlePass;
#endif
#ifdef __PICKUP_FILTER__
	public:
		void			SetPickupFilterFlag(uint16_t wPickupFilterFlag) { m_wPickupFilterFlag = wPickupFilterFlag; }
		bool 			IsFilteredPickupItem(uint32_t dwVnum, uint16_t wPickupFilterFlag);
		uint16_t		GetPickupFilterFlag() { return m_wPickupFilterFlag; }
	
	protected:
		uint16_t		m_wPickupFilterFlag;		
#endif 
#ifdef __VIP_SYSTEM__
	public:
		bool 	IsVIP() const;
		bool	ItemProcessVIP(LPITEM item);
		bool 	RemoveVIP();
#endif
#ifdef ENABLE_REWARD_SYSTEM
public:
	void				SetProtectTime(const std::string& flagname, int value);
	int					GetProtectTime(const std::string& flagname) const;
protected:
	std::map<std::string, int>  m_protection_Time;
#endif
#ifdef __PROMO_CODE__
	public:
		void 		SetPromotionCodeRestriction(const char* szPromotionCode);
		void 		RewardPromotionCode(const char* szPromotionCode);
		bool 		CanUsePromotionCode(const char* szPromotionCode);
#endif
#ifdef __HWID_BAN__
	public:
		std::string GetHardwareID();
#endif	
#ifdef __YMIR_AFFECT_FIX__
	public:
		bool CheckTimeUsed(LPITEM item);
#endif
#ifdef __FAKE_PLAYER__
	public:
		bool				IsFakePlayer() const { return m_bIsFakePlayer; }
		void				SetFakePlayer(bool bValue) { m_bIsFakePlayer = bValue; }
		void				SetLanguage(std::string stLanguage) { m_stFakePlayerLanguage = stLanguage; }
		void 				SetFakePlayerGuildID(uint32_t dwID) { m_dwFakePlayerGuildID = dwID; }
		void 				SetFakePlayerBlockEQ(uint8_t bBlock) { m_bFakePlayerBlockEQ = bBlock; }
		uint8_t				GetFakePlayerBlockEQ() { return m_bFakePlayerBlockEQ; }
		void 				FakePlayerCreatePetSystem();
		LPEVENT				m_pFakePlayerDeadEvent;
		
	protected:
		bool				m_bIsFakePlayer;
		std::string			m_stFakePlayerLanguage;
		uint32_t			m_dwFakePlayerGuildID;
		uint8_t				m_bFakePlayerBlockEQ;
#endif
#ifdef ENABLE_SORT_INVEN
	public:
		void			SortInven(BYTE option, uint8_t bInventoryType);
		DWORD			GetLastSortTime() const { return m_dwLastSortTime; }
		void			SetLastSortTime(DWORD time) { m_dwLastSortTime = time; }
	private:
		DWORD				m_dwLastSortTime;
#endif
#ifdef ENABLE_EXPRESSING_EMOTION
	struct EmotionsValues
	{
		int 	id_emotion;
		DWORD 	tiempo_emotion;

	}save_info_emotion,copy_info_emotion;

	public:

		void LoadingInfoEmotions();
		void LoadInfoEmotions();
		void LoadEmotions();
		bool CheckEmotionList(int emotion);
		int EmotionsList();
		void InsertEmotion();
		int CountEmotion();
		void StartCheckTimeEmotion();
		int get_time_emotion(int value);
		int get_id_emotion(int value);


	private:
		LPEVENT	TimeEmotionUpdateTime;
		std::vector<EmotionsValues> load_info_emotion;	
#endif
#ifdef __GEM_SYSTEM__
public:
	int				GetGem() const { return m_points.gem; }
	void			SetGem(int gem) { m_points.gem = gem; }

	void			LoadGemItems();
	void			SaveGemItems();
	void			OpenGemSlot();
	void			BuyGemItem(BYTE slotIndex);
	void			RefreshGemItems(bool withItem = false);
	void			RefreshGemPlayer();
	void			RefreshGemALL(bool withRealTime = false);

protected:
	std::map<BYTE, TGemItem> m_mapGemItems;
#endif
#ifdef __RENEWAL_BRAVE_CAPE__
public:
	void			GetBraveCapeCMDCompare(const char* c_pszArgument);
	LPEVENT			m_pkBraveCapeEvent;
#endif
#ifdef ENABLE_OFFLINESHOP_SYSTEM
	public:
		void 				SetOfflineShop(LPOFFLINESHOP pkOfflineShop){m_pkOfflineShop=pkOfflineShop;}
		LPOFFLINESHOP		GetOfflineShop() { return m_pkOfflineShop; }

		bool				IsOfflineShopNPC() { return (IsNPC() && GetOfflineShop()); }

		bool				GetOfflineShopPanel() {return isOfflineShopPanelOpen;}
		void				SetOfflineShopPanel(bool flag) {isOfflineShopPanelOpen= flag;}

		long long	GetOfflineShopFlag() { return m_pkOfflineShopFlag; }
		void				SetOfflineShopFlag(long long ptr) { m_pkOfflineShopFlag = ptr; }

		bool				CheckWindows(bool bChat);
		bool				CanAddItemShop();
		bool				CanDestroyShop();
		bool				CanOpenOfflineShop();
		bool				CanOpenShopPanel();
		bool				CanRemoveItemShop();
		bool				CanCreateShop();
		bool				CanRemoveLogShop();
		bool				CanWithdrawMoney();
		bool				CanChangeTitle();
		bool				CanChangeDecoration();
		bool				CanBuyItemOfflineShop();
		bool				CanGetBackItems();
		bool				CanAddTimeShop();
#ifdef ENABLE_SHOP_SEARCH_SYSTEM
		bool				CanSearch();
#endif
	protected:
		LPOFFLINESHOP		m_pkOfflineShop;
		bool				isOfflineShopPanelOpen;
		long long			m_pkOfflineShopFlag;
#endif
#ifdef ENABLE_NEW_KICK_HACK_FIX
	private:
			int m_dwAntiFloodPulse;
			DWORD m_dwAntiFloodCount;
			
	public:
			int GetAntiFloodPulse() const { return m_dwAntiFloodPulse; }
			DWORD IncreaseAntiFloodCount() { return ++m_dwAntiFloodCount; }
			void SetAntiFloodPulse(int dwPulse) { m_dwAntiFloodPulse = dwPulse; }
			void SetAntiFloodCount(DWORD dwCount) { m_dwAntiFloodCount = dwCount; }
			bool CheckAntiFlood();
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	protected:
		CBuffNPCActor*	m_buffNPCSystem;
		
	public:
		CBuffNPCActor*	GetBuffNPCSystem() { return m_buffNPCSystem; }

		void			LoadBuffNPC();

		bool			IsBuffNPC() const { return m_bCharType == ECharType::CHAR_TYPE_BUFF_NPC; }

		void			SendBuffLevelUpEffect(int vid, int type, int value, int amount);
		void			SendBuffNPCUseSkillPacket(int skill_vnum, int skill_level);

		DWORD		BuffGetNextExp() const;
		DWORD		BuffGetNextExpByLevel(int level) const;

		LPITEM		GetBuffWear(UINT bCell) const;
		LPITEM		GetBuffEquipmentItem(WORD wCell) const;
		bool			IsBuffEquipUniqueItem(DWORD dwItemVnum) const;
		bool 			CheckBuffEquipmentPositionAvailable(int iWearCell);
		bool 			EquipBuffItem(BYTE cell, LPITEM item);
		bool 			UnequipBuffItem(BYTE cell, LPITEM item);
		bool 			IsBuffEquipEmpty();

		void			SetBuffWearWeapon(DWORD vnum) { m_dwBuffWeapon = vnum; }
		int				GetBuffWearWeapon() { return m_dwBuffWeapon; }
		void			SetBuffWearHead(DWORD value0) { m_dwBuffHead = value0; }
		int				GetBuffWearHead() { return m_dwBuffHead; }
		void			SetBuffWearArmor(DWORD vnum) { m_dwBuffArmor = vnum; }
		int				GetBuffWearArmor() { return m_dwBuffArmor; }

		void			SetBuffNPCInt(int value) { m_dwBuffNPCInt = value; }
		DWORD		GetBuffNPCInt() { return m_dwBuffNPCInt; }

		void			SetBuffHealValue(int value) { m_buffSkillRecoverHPValue = value; }
		DWORD		GetBuffHealValue() { return m_buffSkillRecoverHPValue; }
		
		void			SetBuffSealVID(int vid) { m_sealVID = vid; }
		int				GetBuffSealVID() { return m_sealVID; }
		
		LPEVENT	m_pkBuffNPCSystemUpdateEvent;
		LPEVENT	m_pkBuffNPCSystemExpireEvent;

	private:
		DWORD	m_dwBuffNPCVid;
		DWORD	m_dwBuffNPCInt;
		int			m_sealVID;
		int			m_buffSkillRecoverHPValue;
		
		DWORD	m_dwBuffWeapon;
		DWORD	m_dwBuffHead;
		DWORD	m_dwBuffArmor;
#endif
};

ESex GET_SEX(LPCHARACTER ch);
#endif
