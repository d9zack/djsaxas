// ------------------ Petsystem edit by Aslan v1.0 --------------------//
//*********************************************************************//
//		socket 0 = IsActive() -- For Active Slot Effect
//		socket 1 = Realtime Socket (UNIX time to Dead)
//		socket 2 = Borntime (UNIX time from Borntime)
//		socket 3 = Bornduration (UNIX Duration Time)
//		socket 4 = Pet Level
//		socket 5 = Evolution
//		
//		attrtype0 	= 
//		attrvalue0 	= HP Value
//		attrtype1 	= 
//		attrvalue1 	= ATK Value
//		attrtype2 	= 
//		attrvalue2 	= DEF Value
//		attrtype3 	= Skill Lv Slot 1
//		attrvalue3 	= Skill ID Slot 1
//		attrtype4 	= Skill Lv Slot 2
//		attrvalue4 	= Skill ID Slot 2
//		attrtype5 	= Skill Lv Slot 3
//		attrvalue5 	= Skill ID Slot 3
//		attrtype6 	= 
//		attrvalue6 	= 
//
//
//************************************************//

#include "stdafx.h"
#include "utils.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "char_petsystem.h"
#include "../../common/VnumHelper.h"
#include "packet.h"
#include "item_manager.h"
#include "item.h"
#include "db.h"


extern int passes_per_sec;
EVENTINFO(newpetsystem_event_info)
{
	CNewPetSystem* pPetSystem;
};

EVENTINFO(newpetsystem_event_infoe)
{
	CNewPetSystem* pPetSystem;
};

EVENTFUNC(newpetsystem_update_event)
{
	newpetsystem_event_info* info = dynamic_cast<newpetsystem_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	CNewPetSystem*	pPetSystem = info->pPetSystem;

	if (NULL == pPetSystem)
		return 0;

	
	pPetSystem->Update(0);
	// 0.25초마다 갱신.
	return PASSES_PER_SEC(1) / 4;
}

EVENTFUNC(newpetsystem_expire_event)
{
	newpetsystem_event_infoe* info = dynamic_cast<newpetsystem_event_infoe*>(event->info);
	if (info == NULL)
	{
		sys_err("check_speedhack_event> <Factor> Null pointer");
		return 0;
	}

	CNewPetSystem*	pPetSystem = info->pPetSystem;

	if (NULL == pPetSystem)
		return 0;


	pPetSystem->UpdateTime();
	// 0.25초마다 갱신.
	return PASSES_PER_SEC(1);
}

///////////////////////////////////////////////////////////////////////////////////////
//  CPetActor
///////////////////////////////////////////////////////////////////////////////////////

CNewPetActor::CNewPetActor(LPCHARACTER owner, DWORD vnum, DWORD options)
{
	m_dwVnum = vnum;
	m_dwVID = 0;
	m_dwlevel = 1;
	m_dwlevelstep = 0;
	m_dwExpFromMob = 0;
	m_dwExpFromItem = 0;
	m_dwexp = 0;
	m_dwexpitem = 0;
	m_dwOptions = options;
	m_dwLastActionTime = 0;

	m_pkChar = 0;
	m_pkOwner = owner;

	m_originalMoveSpeed = 0;
	
	m_dwSummonItemVID = 0;
	m_dwSummonItemID = 0;
	m_dwSummonItemVnum = 0;

	m_dwevolution = 0;
	m_dwage = 0;
	m_dwbirthday = 0;

	m_dwitemduration = 0;
	m_dwbornduration = 0;

	m_dwTimePet = 0;
	m_dwslotimm = 0;
	m_dwImmTime = 0;

	m_dwagebonus = 0,
	m_dwpettype = 0,
	m_dwtypebonus0 = 0,
	m_dwtypebonus1 = 0,
	m_dwtypebonus2 = 0,

	m_dwskill[0] = 0;
	m_dwskill[1] = 0;
	m_dwskill[2] = 0;

	for (int s = 0; s < 9; ++s) {
		m_dwpetslotitem[s] = -1;
		m_dwpetinvtype[s] = -1;
	}

	//Sofern kein Slot frei ist wird der Wert auf -1 gesetzt
	m_dwskillslot[0] = -1;
	m_dwskillslot[1] = -1;
	m_dwskillslot[2] = -1;

	int btype[3] = { 1, 53, 63};
	for (int x = 0; x < 3; ++x)
	{
		m_dwbonuspet[x][0] = btype[x];
		m_dwbonuspet[x][1] = 0;
	}
}

CNewPetActor::~CNewPetActor()
{
	this->Unsummon();

	m_pkOwner = 0;
}

void CNewPetActor::SetSummonItem(LPITEM pItem)
{
	if (NULL == pItem)
	{
		m_dwSummonItemVID = 0;
		m_dwSummonItemID = 0;
		m_dwSummonItemVnum = 0;
		return;
	}

	m_dwSummonItemVID = pItem->GetVID();
	m_dwSummonItemID = pItem->GetID();
	m_dwSummonItemVnum = pItem->GetVnum();
}

DWORD CNewPetActor::Summon(const char* petName, LPITEM pSummonItem, bool bSpawnFar)
{
	long x = m_pkOwner->GetX();
	long y = m_pkOwner->GetY();
	long z = m_pkOwner->GetZ();

	if (true == bSpawnFar)
	{
		x += (number(0, 1) * 2 - 1) * number(2000, 2500);
		y += (number(0, 1) * 2 - 1) * number(2000, 2500);
	}
	else
	{
		x += number(-100, 100);
		y += number(-100, 100);
	}

	if (0 != m_pkChar)
	{
		m_pkChar->Show (m_pkOwner->GetMapIndex(), x, y);
		m_dwVID = m_pkChar->GetVID();

		return m_dwVID;
	}
	
	m_pkChar = CHARACTER_MANAGER::instance().SpawnMob(
				m_dwVnum, 
				m_pkOwner->GetMapIndex(), 
				x, y, z,
				false, (int)(m_pkOwner->GetRotation()+180), false);

	if (0 == m_pkChar)
	{
		sys_err("[CPetSystem::Summon] Failed to summon the pet. (vnum: %d)", m_dwVnum);
		return 0;
	}

	m_pkChar->SetNewPet();
	
	m_pkChar->SetEmpire(m_pkOwner->GetEmpire());

	m_dwVID = m_pkChar->GetVID();

	char szQuery1[1024];
	snprintf(szQuery1, sizeof(szQuery1), "SELECT name,level,evolution,exp,expi,bonus0,bonus1,bonus2,skill0,skill0lv,skill1,skill1lv,skill2,skill2lv,item_duration,born_duration,UNIX_TIMESTAMP(birthday),(UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(birthday))/60,age_bonus,pettype,typebonus0,typebonus1,typebonus2 FROM new_petsystem WHERE id = %lu ", pSummonItem->GetID());
	std::auto_ptr<SQLMsg> pmsg2(DBManager::instance().DirectQuery(szQuery1));
	if (pmsg2->Get()->uiNumRows > 0) {
		MYSQL_ROW row = mysql_fetch_row(pmsg2->Get()->pSQLResult);			
		this->SetName(row[0]);		
		this->SetLevel(atoi(row[1]));
		this->m_dwevolution 			= atoi(row[2]);	
		this->SetExp(atoi(row[3]), 0);		
		this->SetExp(atoi(row[4]), 1);
		this->m_dwbonuspet[0][1] 	= atoi(row[5]);
		this->m_dwbonuspet[1][1] 	= atoi(row[6]);
		this->m_dwbonuspet[2][1] 	= atoi(row[7]);
		this->m_dwskillslot[0] 			= atoi(row[8]);
		this->m_dwskill[0] 				= atoi(row[9]);
		this->m_dwskillslot[1] 			= atoi(row[10]);
		this->m_dwskill[1] 				= atoi(row[11]);
		this->m_dwskillslot[2] 			= atoi(row[12]);
		this->m_dwskill[2] 				= atoi(row[13]);
		this->m_dwitemduration 		= atoi(row[14]);
		this->m_dwbornduration 		= atoi(row[15]);
		this->m_dwbirthday 			= atoi(row[16]);
		this->m_dwage 					= atoi(row[17]);	
		this->m_dwagebonus 			= atoi(row[18]);
		this->m_dwpettype 			= atoi(row[19]);
		this->m_dwtypebonus0 		= atoi(row[20]);
		this->m_dwtypebonus1 		= atoi(row[21]);
		this->m_dwtypebonus2 		= atoi(row[22]);	
	}
	else
		this->SetName(petName);

	this->SetSummonItem(pSummonItem);

	m_dwitemduration = pSummonItem->GetSocket(1);
	
	pSummonItem->SetSocket(0, true);							// Is Pet Active
	pSummonItem->SetSocket(2, m_dwbirthday);			// Born Duration
	pSummonItem->SetSocket(3, m_dwbornduration);				// Pet Birthday (Borntime)
	pSummonItem->SetSocket(4, m_dwlevel);					// Pet Level
	pSummonItem->SetSocket(5, m_dwevolution);				// Pet Evolution
	
	pSummonItem->SetForceAttribute(0, 1, m_dwbonuspet[0][1]);
	pSummonItem->SetForceAttribute(1, 1, m_dwbonuspet[1][1]);
	pSummonItem->SetForceAttribute(2, 1, m_dwbonuspet[2][1]);
	
	pSummonItem->SetForceAttribute(3, m_dwskill[0], m_dwskillslot[0]);
	pSummonItem->SetForceAttribute(4, m_dwskill[1], m_dwskillslot[1]);
	pSummonItem->SetForceAttribute(5, m_dwskill[2], m_dwskillslot[2]);
	pSummonItem->Lock(true);

	m_pkOwner->ComputePoints();
	m_pkChar->Show(m_pkOwner->GetMapIndex(), x, y, z);
	
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetIcon %d", m_dwSummonItemVnum);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetEvolution %d", m_dwevolution);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetName %s", m_name.c_str());
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetLevel %d %d", m_dwlevel, m_dwevolution);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetDuration %d %d", m_dwitemduration, m_dwbornduration);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetBonus %d %d %d", m_dwbonuspet[0][1], m_dwbonuspet[1][1], m_dwbonuspet[2][1]);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetAge %d", m_dwage);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", 0, m_dwskillslot[0], m_dwskill[0]);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", 1, m_dwskillslot[1], m_dwskill[1]);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", 2, m_dwskillslot[2], m_dwskill[2]);
	
	if (m_dwlevel == NEW_PET_EVOLUTION_1_LEVEL && m_dwevolution == 0 or m_dwlevel == NEW_PET_EVOLUTION_2_LEVEL && m_dwevolution == 1 or m_dwlevel == NEW_PET_EVOLUTION_3_LEVEL && m_dwevolution == 2)
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetCanEvolution %d", 1);
	
	if(GetLevel() == NEW_PET_MAX_LEVEL)
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, 0, 0, 0);
	else
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, m_dwexp, m_dwexpitem, m_pkChar->PetGetNextExp());
	
	this->GiveBuff();

	return m_dwVID;
}

void CNewPetActor::Unsummon()
{
	if (true == this->IsSummoned())
	{
		SaveSQL();
		
		this->ClearBuff();

		LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
		if (pSummonItem != NULL){
			pSummonItem->SetSocket(0, false);							// Is Pet Active
			
			pSummonItem->SetForceAttribute(0, 1, m_dwbonuspet[0][1]);
			pSummonItem->SetForceAttribute(1, 1, m_dwbonuspet[1][1]);
			pSummonItem->SetForceAttribute(2, 1, m_dwbonuspet[2][1]);
			
			pSummonItem->SetForceAttribute(3, m_dwskill[0], m_dwskillslot[0]);
			pSummonItem->SetForceAttribute(4, m_dwskill[1], m_dwskillslot[1]);
			pSummonItem->SetForceAttribute(5, m_dwskill[2], m_dwskillslot[2]);
			pSummonItem->Lock(false);
		}
		this->SetSummonItem(NULL);
		if (NULL != m_pkOwner)
			m_pkOwner->ComputePoints();

		if (NULL != m_pkChar)
			M2_DESTROY_CHARACTER(m_pkChar);

		m_pkChar = 0;
		m_dwVID = 0;
		m_dwlevel = 1;
		m_dwlevelstep = 0;
		m_dwExpFromMob = 0;
		m_dwExpFromItem = 0;
		m_dwexp = 0;
		m_dwexpitem = 0;
		m_dwTimePet = 0;
		m_dwImmTime = 0;
		m_dwslotimm = 0;
		m_dwbirthday = 0;

		for (int s = 0; s < 9; ++s) {
			m_dwpetslotitem[s] = -1;
			m_dwpetinvtype[s] = -1;
		}
		ClearBuff();
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetUnsummon");
	}
}

void CNewPetActor:: IncreasePetBonus() {
	
	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_LEVELUP"));

	int age_bonus = m_dwagebonus;
	if (age_bonus > NEW_PET_AGE_MAX_BONUS_TIMES)
		age_bonus = NEW_PET_AGE_MAX_BONUS_TIMES;
	
	if (m_dwpettype == 1)
	{
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_HP == 0) { 
			int bonus = number(Pet_Type_Attr_Table[1][0][0], Pet_Type_Attr_Table[1][0][1]) + age_bonus;
			m_dwbonuspet[0][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_HP"), bonus_info);
		}
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_ATK == 0) { 
			int bonus = number(Pet_Type_Attr_Table[1][1][0], Pet_Type_Attr_Table[1][1][1]) + age_bonus;
			m_dwbonuspet[1][1] += bonus;
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_ATT"), bonus_info);
		}
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_MOB == 0) { 
			int bonus = number(Pet_Type_Attr_Table[1][2][0], Pet_Type_Attr_Table[1][2][1]) + age_bonus;
			m_dwbonuspet[2][1] += bonus;
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_MOB"), bonus_info);
		}
	}
	else if (m_dwpettype == 3)
	{
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_HP == 0) { 
			int bonus = number(Pet_Type_Attr_Table[3][0][0], Pet_Type_Attr_Table[3][0][1]) + age_bonus;
			m_dwbonuspet[0][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_HP"), bonus_info);
		}
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_ATK == 0) { 
			int bonus = number(Pet_Type_Attr_Table[3][1][0], Pet_Type_Attr_Table[3][1][1]) + age_bonus;
			m_dwbonuspet[1][1] += bonus;
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_ATT"), bonus_info);
		}
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_MOB == 0) { 
			int bonus = number(Pet_Type_Attr_Table[3][2][0], Pet_Type_Attr_Table[3][2][1]) + age_bonus;
			m_dwbonuspet[2][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_MOB"), bonus_info);
		}
	}
	else if (m_dwpettype == 5)
	{
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_HP == 0) { 
			int bonus = number(Pet_Type_Attr_Table[5][0][0], Pet_Type_Attr_Table[5][0][1]) + age_bonus;
			m_dwbonuspet[0][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_HP"), bonus_info);
		}
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_ATK == 0) { 
			int bonus = number(Pet_Type_Attr_Table[5][1][0], Pet_Type_Attr_Table[5][1][1]) + age_bonus;
			m_dwbonuspet[1][1] += bonus;
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_ATT"), bonus_info);
		}
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_MOB == 0) { 
			int bonus = number(Pet_Type_Attr_Table[5][2][0], Pet_Type_Attr_Table[5][2][1]) + age_bonus;
			m_dwbonuspet[2][1] += bonus;
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_MOB"), bonus_info);
		}
	}
	else if (m_dwpettype == 7)
	{
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_HP == 0) { 
			int bonus = number(Pet_Type_Attr_Table[7][0][0], Pet_Type_Attr_Table[7][0][1]) + age_bonus;
			m_dwbonuspet[0][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_HP"), bonus_info);
		}
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_ATK == 0) { 
			int bonus = number(Pet_Type_Attr_Table[7][1][0], Pet_Type_Attr_Table[7][1][1]) + age_bonus;
			m_dwbonuspet[1][1] += bonus;
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_ATT"), bonus_info);
		}
		if (m_dwlevel % NEW_PET_GET_BONUS_EVERY_LEVEL_MOB == 0) { 
			int bonus = number(Pet_Type_Attr_Table[7][2][0], Pet_Type_Attr_Table[7][2][1]) + age_bonus;
			m_dwbonuspet[2][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_MOB"), bonus_info);
		}
	}
	else
	{
		if (GetLevel() % NEW_PET_GET_CONSTANT_BONUS_EVERY_LEVEL_HP == 0) {  
			int bonus = m_dwtypebonus0 + age_bonus;
			m_dwbonuspet[0][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_HP"), bonus_info);
		}
		if (GetLevel() % NEW_PET_GET_CONSTANT_BONUS_EVERY_LEVEL_ATK == 0) {  
			int bonus = m_dwtypebonus1 + age_bonus;
			m_dwbonuspet[1][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_ATT"), bonus_info);
		}
		if (GetLevel() % NEW_PET_GET_CONSTANT_BONUS_EVERY_LEVEL_MOB == 0) {  
			int bonus = m_dwtypebonus2 + age_bonus;
			m_dwbonuspet[2][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_GIVE_BONUS_MOB"), bonus_info);
		}
	}
	
	//m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_LVLUP_POINTS"), rand_for_tp, rand_for_def, rand_for_mp );
	
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());

	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetBonus %d %d %d", m_dwbonuspet[0][1], m_dwbonuspet[1][1], m_dwbonuspet[2][1]);
	if (pSummonItem != NULL){
		pSummonItem->SetForceAttribute(0, 1, m_dwbonuspet[0][1]);
		pSummonItem->SetForceAttribute(1, 1, m_dwbonuspet[1][1]);
		pSummonItem->SetForceAttribute(2, 1, m_dwbonuspet[2][1]);
	}

	SaveSQL();
	ClearBuff();
	GiveBuff();
	
	if (NULL != m_pkOwner)
		m_pkOwner->ComputePoints();
}

void CNewPetActor::ChangePetAttr() 
{
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());

	int age_bonus = m_dwagebonus;
	if (age_bonus > NEW_PET_AGE_MAX_BONUS_TIMES)
		age_bonus = NEW_PET_AGE_MAX_BONUS_TIMES;
	
	int pettype = 0;
	int typebonus0 = 0;
	int typebonus1 = 0;
	int typebonus2 = 0;
	
	int rand_typebonus0 = 0;
	int rand_typebonus1 = 0;
	int rand_typebonus2 = 0;
		
	int rand_bonus0 = 0;
	int rand_bonus1 = 0;
	int rand_bonus2 = 0;
	
	//				Type Chance:		1		2		3		4		5		6		7		8
	const int prob_type_table[] = { 	15, 	30, 	45,		58,		70,		80,		88,		95 };
	int rand = number(1, 95);
	
	rand_bonus0 += number(Pet_Type_Attr_Table[0][0][0], Pet_Type_Attr_Table[0][0][1]);
	rand_bonus1 += number(Pet_Type_Attr_Table[0][1][0], Pet_Type_Attr_Table[0][1][1]);
	rand_bonus2 += number(Pet_Type_Attr_Table[0][2][0], Pet_Type_Attr_Table[0][2][1]);
		
	for (int i = 0; i < SPECIAL_BORN_PETS_MAX_NUM; ++i) 
	{
		if (pSummonItem->GetVnum() == Special_Born_Attr_Table[i][0][0])
		{
			rand_bonus0 = 0 + number(Special_Born_Attr_Table[i][1][0], Special_Born_Attr_Table[i][1][1]);
			rand_bonus1 = 0 + number(Special_Born_Attr_Table[i][2][0], Special_Born_Attr_Table[i][2][1]);
			rand_bonus2 = 0 + number(Special_Born_Attr_Table[i][3][0], Special_Born_Attr_Table[i][3][1]);
		}
	}
	
	if (rand <= prob_type_table[0])
	{
		pettype = 1;
	} 
	else if (rand <= prob_type_table[1])
	{
		pettype = 2;
		rand_typebonus0 = number(Pet_Type_Attr_Table[2][0][0], Pet_Type_Attr_Table[2][0][1]);
		rand_typebonus1 = number(Pet_Type_Attr_Table[2][1][0], Pet_Type_Attr_Table[2][1][1]);
		rand_typebonus2 = number(Pet_Type_Attr_Table[2][2][0], Pet_Type_Attr_Table[2][2][1]);
	}
	else if (rand <= prob_type_table[2])
	{
		pettype = 3;
	}
	else if (rand <= prob_type_table[3])
	{
		pettype = 4;
		rand_typebonus0 = number(Pet_Type_Attr_Table[4][0][0], Pet_Type_Attr_Table[4][0][1]);
		rand_typebonus1 = number(Pet_Type_Attr_Table[4][1][0], Pet_Type_Attr_Table[4][1][1]);
		rand_typebonus2 = number(Pet_Type_Attr_Table[4][2][0], Pet_Type_Attr_Table[4][2][1]);
	}
	else if (rand <= prob_type_table[4])
	{
		pettype = 5;
	}
	else if (rand <= prob_type_table[5])
	{
		pettype = 6;
		rand_typebonus0 = number(Pet_Type_Attr_Table[6][0][0], Pet_Type_Attr_Table[6][0][1]);
		rand_typebonus1 = number(Pet_Type_Attr_Table[6][1][0], Pet_Type_Attr_Table[6][1][1]);
		rand_typebonus2 = number(Pet_Type_Attr_Table[6][2][0], Pet_Type_Attr_Table[6][2][1]);
	}
	else if (rand <= prob_type_table[6])
	{
		pettype = 7;
	}
	else if (rand <= prob_type_table[7])
	{
		pettype = 8;
		rand_typebonus0 = number(Pet_Type_Attr_Table[8][0][0], Pet_Type_Attr_Table[8][0][1]);
		rand_typebonus1 = number(Pet_Type_Attr_Table[8][1][0], Pet_Type_Attr_Table[8][1][1]);
		rand_typebonus2 = number(Pet_Type_Attr_Table[8][2][0], Pet_Type_Attr_Table[8][2][1]);
	}
	
	for (int i = 0; i < m_dwlevel; ++i) 
	{
		if (pettype == 1)
		{
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_HP == 0) { rand_bonus0 += number(Pet_Type_Attr_Table[1][0][0], Pet_Type_Attr_Table[1][0][1]) + age_bonus; }
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_ATK == 0) { rand_bonus1 += number(Pet_Type_Attr_Table[1][1][0], Pet_Type_Attr_Table[1][1][1]) + age_bonus; }
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_MOB == 0) { rand_bonus2 += number(Pet_Type_Attr_Table[1][2][0], Pet_Type_Attr_Table[1][2][1]) + age_bonus; }
		}
		else if (pettype == 3)
		{
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_HP == 0) { rand_bonus0 += number(Pet_Type_Attr_Table[3][0][0], Pet_Type_Attr_Table[3][0][1]) + age_bonus; }
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_ATK == 0) { rand_bonus1 += number(Pet_Type_Attr_Table[3][1][0], Pet_Type_Attr_Table[3][1][1]) + age_bonus; }
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_MOB == 0) { rand_bonus2 += number(Pet_Type_Attr_Table[3][2][0], Pet_Type_Attr_Table[3][2][1]) + age_bonus; }
		}
		else if (pettype == 5)
		{
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_HP == 0) { rand_bonus0 += number(Pet_Type_Attr_Table[5][0][0], Pet_Type_Attr_Table[5][0][1]) + age_bonus; }
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_ATK == 0) { rand_bonus1 += number(Pet_Type_Attr_Table[5][1][0], Pet_Type_Attr_Table[5][1][1]) + age_bonus; }
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_MOB == 0) { rand_bonus2 += number(Pet_Type_Attr_Table[5][2][0], Pet_Type_Attr_Table[5][2][1]) + age_bonus; }
		}
		else if (pettype == 7)
		{
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_HP == 0) { rand_bonus0 += number(Pet_Type_Attr_Table[7][0][0], Pet_Type_Attr_Table[7][0][1]) + age_bonus; }
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_ATK == 0) { rand_bonus1 += number(Pet_Type_Attr_Table[7][1][0], Pet_Type_Attr_Table[7][1][1]) + age_bonus; }
			if (i % NEW_PET_GET_BONUS_EVERY_LEVEL_MOB == 0) { rand_bonus2 += number(Pet_Type_Attr_Table[7][2][0], Pet_Type_Attr_Table[7][2][1]) + age_bonus; }
		}
		else
		{
			if (i % NEW_PET_GET_CONSTANT_BONUS_EVERY_LEVEL_HP == 0) { rand_bonus0 += rand_typebonus0 + age_bonus; }
			if (i % NEW_PET_GET_CONSTANT_BONUS_EVERY_LEVEL_ATK == 0) { rand_bonus1 += rand_typebonus1 + age_bonus; }
			if (i % NEW_PET_GET_CONSTANT_BONUS_EVERY_LEVEL_MOB == 0) { rand_bonus2 += rand_typebonus2 + age_bonus; }
		}
	}

	m_dwbonuspet[0][1] = rand_bonus0;
	m_dwbonuspet[1][1] = rand_bonus1;
	m_dwbonuspet[2][1] = rand_bonus2;
		
	m_dwpettype = pettype;
	m_dwtypebonus0 = rand_typebonus0;
	m_dwtypebonus1 = rand_typebonus1;
	m_dwtypebonus2 = rand_typebonus2;

	if (pSummonItem != NULL){
		pSummonItem->SetForceAttribute(0, 1, m_dwbonuspet[0][1]);
		pSummonItem->SetForceAttribute(1, 1, m_dwbonuspet[1][1]);
		pSummonItem->SetForceAttribute(2, 1, m_dwbonuspet[2][1]);
	}
	
#ifdef NEW_PET_SYSTEM_CHANGE_ATTR_EFFECTS
	m_pkChar->EffectPacket(SE_EFFECT_PET_CHANGE_ATTR);
#endif
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetAttrFinish %d", m_dwpettype);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetBonus %d %d %d", m_dwbonuspet[0][1], m_dwbonuspet[1][1], m_dwbonuspet[2][1]);
	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_NEW_ATTR"), m_dwpettype);
	
#ifdef ENABLE_EXTENDED_BATTLE_PASS
	m_pkOwner->UpdateExtBattlePassMissionProgress(PET_ENCHANT, 1, m_dwpettype);
#endif
	SaveSQL();
	ClearBuff();
	GiveBuff();
}

bool CNewPetActor::IncreasePetSkill(int skill) {
	
	int itemvnum = 55009 + skill;
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
	
	if (GetLevel() < 40 && m_dwevolution < 0){
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_CANNOT_LEARN_SKILLS"));
		return false;
	}

	for (int i = 0; i < 3; ++i) {
		if (m_dwskillslot[i] == skill) {
			if (m_dwskill[i] < 20) {
				m_dwskill[i] += 1;
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_LERN_SKILL"), m_dwskill[i]);
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", i, m_dwskillslot[i], m_dwskill[i]);
				
				pSummonItem->SetForceAttribute(i+3, m_dwskill[i], m_dwskillslot[i]);
				ClearBuff();
				GiveBuff();
	
				return true;
			}
			else {
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_SKILL_MAX_POINT"));
				return false;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		if (m_dwskillslot[i] == 0) { 
		//if (m_dwskillslot[i] == 0 || m_dwskillslot[i] == -1) { 
			m_dwskillslot[i] = skill;
			m_dwskill[i] = 1;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_NEW_SKILL"));
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", i, m_dwskillslot[i], m_dwskill[i]);
			
			pSummonItem->SetForceAttribute(i+3, m_dwskill[i], m_dwskillslot[i]);
			ClearBuff();
			GiveBuff();

			return true;
		}
	}

	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_CANNOT_LEARN_NEW_SKILL"));
	return false;
}
 
void CNewPetActor::RemovePetSkill(int skillslot, int itemvnum) {
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
	
	if (itemvnum == 55030) // Pet Revertus
	{
		int skilldesc = skillslot+1;
		
		if (m_dwskillslot[skillslot] > 0)
		{
			m_dwskillslot[skillslot] = 0;
			m_dwskill[skillslot] = 0;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_REMOVE_SKILL"), skilldesc);
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", skillslot, m_dwskillslot[skillslot], m_dwskill[skillslot]);	
			pSummonItem->SetForceAttribute(skillslot+3, m_dwskill[skillslot], m_dwskillslot[skillslot]);
		}
	} 
	else if (itemvnum == 55029) // Pet Reverti
	{

		if (m_dwevolution >= 1)
		{
			m_dwskillslot[0] = 0;
			m_dwskill[0] = 0;
			pSummonItem->SetForceAttribute(3, m_dwskill[0], m_dwskillslot[0]);
		}
		if (m_dwevolution >= 2)
		{
			m_dwskillslot[1] = 0;
			m_dwskill[1] = 0;
			pSummonItem->SetForceAttribute(4, m_dwskill[1], m_dwskillslot[1]);
		}
		if (m_dwevolution >= 3)
		{
			m_dwskillslot[2] = 0;
			m_dwskill[2] = 0;
			pSummonItem->SetForceAttribute(5, m_dwskill[2], m_dwskillslot[2]);
		}

		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", 0, m_dwskillslot[0], m_dwskill[0]);	
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", 1, m_dwskillslot[1], m_dwskill[1]);	
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", 2, m_dwskillslot[2], m_dwskill[2]);

		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_REMOVE_SKILL_ALL"));

	}
	
	SaveSQL();
	ClearBuff();
	GiveBuff();
}

void CNewPetActor::DoPetSkillAlone() 
{
	/* if (GetLevel() < 81 || m_dwevolution < 3)
		return; */
	
	/***************/
	// Restauration
	/***************/
	if (m_dwskillslot[0] == 10 and get_global_time() - m_pkOwner->GetNewPetSkillCD(0) >= (int)Pet_Skill_Table[9][2] or m_dwskillslot[1] == 10 and get_global_time() - m_pkOwner->GetNewPetSkillCD(0) >= (int)Pet_Skill_Table[9][2] or m_dwskillslot[2] == 10 and get_global_time() - m_pkOwner->GetNewPetSkillCD(0) >= (int)Pet_Skill_Table[9][2])
	{
		if (m_pkOwner->GetHPPct() <= 20 and m_pkOwner->GetHPPct() > 0) 
		{
			int SkillslotLevel = 0;
			if (m_dwskillslot[0] == 10)
				SkillslotLevel = 0;
			else if (m_dwskillslot[1] == 10)
				SkillslotLevel = 1;
			else if (m_dwskillslot[2] == 10)
				SkillslotLevel = 2;
			
			m_pkOwner->SetNewPetSkillCD(0, get_global_time());
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkillCooltime %d %d", SkillslotLevel, (int)Pet_Skill_Table[9][2]);
			
			int random = number(1, 100);
			int chance = Pet_SKill_TablePerc[1][m_dwskill[SkillslotLevel]];
			
			if (random <= chance)
			{
				int HealHP = (int)Pet_Skill_Table[9][2 + m_dwskill[SkillslotLevel]];
				m_pkOwner->PointChange(POINT_HP, HealHP);
#ifdef NEW_PET_SYSTEM_SKILL_EFFECTS
				m_pkOwner->EffectPacket(SE_EFFECT_PET_SKILL_RESTAURATION);
				m_pkChar->EffectPacket(SE_EFFECT_PET_SKILL_RESTAURATION);
#endif
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_USE_SKILL_10"), HealHP);
			}
			else
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_USE_SKILL_10_FAIL"));
		}
	}

	/***************/
	// Immortal
	/***************/
	if (m_dwskillslot[0] == 17 and get_global_time() - m_pkOwner->GetNewPetSkillCD(1) >= (int)Pet_Skill_Table[16][2] or m_dwskillslot[1] == 17 and get_global_time() - m_pkOwner->GetNewPetSkillCD(1) >= (int)Pet_Skill_Table[16][2] or m_dwskillslot[2] == 17 and get_global_time() - m_pkOwner->GetNewPetSkillCD(1) >= (int)Pet_Skill_Table[16][2])
	{
		if (m_pkOwner->GetHPPct() < 50 and m_pkOwner->IsPosition(POS_FIGHTING))
		{
			int SkillslotLevel = 0;
			if (m_dwskillslot[0] == 17)
				SkillslotLevel = 0;
			else if (m_dwskillslot[1] == 17)
				SkillslotLevel = 1;
			else if (m_dwskillslot[2] == 17)
				SkillslotLevel = 2;

			m_pkOwner->SetNewPetSkillCD(1, get_global_time());
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkillCooltime %d %d", SkillslotLevel, (int)Pet_Skill_Table[16][2]);

			int random = number(1, 100);
			int chance = Pet_SKill_TablePerc[2][m_dwskill[SkillslotLevel]];
			
			if (random <= chance)
			{
				m_pkOwner->SetImmortal(1);
				m_dwImmTime = get_global_time();
#ifdef NEW_PET_SYSTEM_SKILL_EFFECTS
				m_pkOwner->EffectPacket(SE_EFFECT_PET_SKILL_IMMORTAL);
				m_pkChar->EffectPacket(SE_EFFECT_PET_SKILL_IMMORTAL);
#endif
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_USE_SKILL_17"));
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_SKILL_CANNOT_DEAD"));
			}
			else
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_USE_SKILL_17_FAIL"));
		}
	}

	/***************/
	// Panacea
	/***************/
	if (m_dwskillslot[0] == 18 and get_global_time() - m_pkOwner->GetNewPetSkillCD(2) >= (int)Pet_Skill_Table[17][2] or m_dwskillslot[1] == 18 and get_global_time() - m_pkOwner->GetNewPetSkillCD(2) >= (int)Pet_Skill_Table[17][2] or m_dwskillslot[2] == 18 and get_global_time() - m_pkOwner->GetNewPetSkillCD(2) >= (int)Pet_Skill_Table[17][2])
	{
		if (m_pkOwner->FindAffect(AFFECT_STUN) or m_pkOwner->FindAffect(AFFECT_SLOW) or m_pkOwner->FindAffect(AFFECT_POISON))
		{
			int SkillslotLevel = 0;
			if (m_dwskillslot[0] == 18)
				SkillslotLevel = 0;
			else if (m_dwskillslot[1] == 18)
				SkillslotLevel = 1;
			else if (m_dwskillslot[2] == 18)
				SkillslotLevel = 2;
			
			m_pkOwner->SetNewPetSkillCD(2, get_global_time());
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkillCooltime %d %d", SkillslotLevel, (int)Pet_Skill_Table[17][2]);
			
			int random = number(1, 100);
			int chance = Pet_SKill_TablePerc[3][m_dwskill[SkillslotLevel]];
			
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "Skill 18: Random %d <= %d Chance", random, chance);
			
			if (random <= chance)
			{
				m_pkOwner->RemoveBadAffect();
#ifdef NEW_PET_SYSTEM_SKILL_EFFECTS
				m_pkOwner->EffectPacket(SE_EFFECT_PET_SKILL_PANACEA);
				m_pkChar->EffectPacket(SE_EFFECT_PET_SKILL_PANACEA);
#endif
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_USE_SKILL_18"));
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_SKILL_REMOVE_BAD_AFFECT"));
			}
			else
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_USE_SKILL_18_FAIL"));
		}
	}
	
	/***************/
	// Featherlight
	/***************/
	if (m_dwskillslot[0] == 23 and get_global_time() - m_pkOwner->GetNewPetSkillCD(3) >= (int)Pet_Skill_Table[22][2] or m_dwskillslot[1] == 23 and get_global_time() - m_pkOwner->GetNewPetSkillCD(3) >= (int)Pet_Skill_Table[22][2] or m_dwskillslot[2] == 23 and get_global_time() - m_pkOwner->GetNewPetSkillCD(3) >= (int)Pet_Skill_Table[22][2])
	{
		if (m_pkOwner->FindAffect(AFFECT_STUN) or m_pkOwner->FindAffect(AFFECT_SLOW) or m_pkOwner->FindAffect(AFFECT_POISON))
		{
			int SkillslotLevel = 0;
			if (m_dwskillslot[0] == 23)
				SkillslotLevel = 0;
			else if (m_dwskillslot[1] == 23)
				SkillslotLevel = 1;
			else if (m_dwskillslot[2] == 23)
				SkillslotLevel = 2;
			
			m_pkOwner->SetNewPetSkillCD(3, get_global_time());
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkillCooltime %d %d", SkillslotLevel, (int)Pet_Skill_Table[22][2]);
			
			int random = number(1, 100);
			int chance = Pet_SKill_TablePerc[4][m_dwskill[SkillslotLevel]];
				
			if (random <= chance)
			{
				m_pkOwner->RemoveBadAffect();
#ifdef NEW_PET_SYSTEM_SKILL_EFFECTS
				m_pkOwner->EffectPacket(SE_EFFECT_PET_SKILL_PANACEA);
				m_pkChar->EffectPacket(SE_EFFECT_PET_SKILL_PANACEA);
#endif
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_USE_SKILL_23"));
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_SKILL_REMOVE_BAD_AFFECT"));
			}
			else
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_USE_SKILL_23_FAIL"));
		}
	}
}

void CNewPetActor::GiveBuff()
{
	int cbonus[3] = { m_pkOwner->GetMaxHP(), m_pkOwner->GetPoint(POINT_ATT_GRADE), m_pkOwner->GetPoint(POINT_ATTBONUS_MONSTER) };
	for (int i = 0; i < 3; ++i) {
		if (i == 2){
			//m_pkOwner->AddAffect(AFFECT_NEW_PET, POINT_ATTBONUS_MONSTER, 20, 0,  60 * 60 * 24 * 365, 0, false);
			m_pkOwner->AddAffect(AFFECT_NEW_PET, POINT_ATTBONUS_MONSTER, round(m_dwbonuspet[i][1]/10), 0,  60 * 60 * 24 * 365, 0, false);
			//m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "MonsterBoni: %d", round(m_dwbonuspet[i][1]/10));
		}
		else
			m_pkOwner->AddAffect(AFFECT_NEW_PET, aApplyInfo[m_dwbonuspet[i][0]].bPointType, (int)(cbonus[i]*m_dwbonuspet[i][1]/1000), 0,  60 * 60 * 24 * 365, 0, false);
	}
	
	//if (GetLevel() > 81 && m_dwevolution == 3) {
	for (int s = 0; s < 3; s++) {
		switch (m_dwskillslot[s]) {
			/*
				Pet_Skill_Table[m_dwskillslot[s] - 1][0]; 							// Affect Value
				Pet_Skill_Table[m_dwskillslot[s] - 1][1]; 							// Is Active/Passive Skill
				Pet_Skill_Table[m_dwskillslot[s] - 1][2]; 							// Cooltime
				Pet_Skill_Table[m_dwskillslot[s] - 1][2 + m_dwskill[s]];		// Skill Values per Level
			*/
			case 1: //78 - Resistance: Warrior
			case 2: //80 - Resistance: Sura
			case 3: //79 - Resistance: Assassin
			case 4: //81 - Resistance: Shaman
			case 5: //93 - Resistance: Lycan
			case 6: //53 - Beserker (+ Attack Value)
			case 7: //97 - Anti-Magic (Reduce magic resistance)
			case 8: //XX - Haste (Skill Cooldown reduced)
			case 9: //16 - Drill (Chance of piercing hits)
			case 11: //XX - Vampirism (Leech HP from Enemy)
			case 12: //XX - Spiritualism (Leech SP from Enemy)
			case 13: // 27 - Bulwark (Physical damage block)
			case 14: // 28 - Reflection (Damage taken back to enemy)
			case 15: // 44 - Yang Drop
			case 16: // 172 - Range (Range of your bow is incrased)
			case 19: // XX - Eagle Eyes (EXP Bonus)
			case 20: // XX - Master Brewer (Effectiveness of Regeneration Potions)
			case 21: // XX - Monster Hunter (Str vs Monsters)
			case 22: // XX - Life Drain (Gives HP Absorb from killed monsters)
				m_pkOwner->AddAffect(AFFECT_NEW_PET, aApplyInfo[Pet_Skill_Table[m_dwskillslot[s] - 1][0]].bPointType, Pet_Skill_Table[m_dwskillslot[s] - 1][2 + m_dwskill[s]], 0, 60 * 60 * 24 * 365, 0, false);
				break;
			default:
				return;
		}
	}
	m_pkOwner->UpdatePacket();
}

void CNewPetActor::ClearBuff()
{
	m_pkOwner->RemoveAffect(AFFECT_NEW_PET);
	return ;
}

void CNewPetActor::SetItemCube(int pos, int invpos, int invtype) {
	if (pos > 180 || pos < 0)
		return;

	m_dwpetslotitem[pos] = invpos;
	m_dwpetinvtype[pos] = -invtype;
}

void CNewPetActor::ItemCubeFeed(int type)
{						
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());

	for (int i = 0; i < 9; ++i) 
	{
		if (m_dwpetslotitem[i] != -1) {
			LPITEM itemxp = m_pkOwner->GetInventoryItem(m_dwpetslotitem[i]);
				
			if (!itemxp)
				return;
			if (itemxp->GetID() == ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetID() || m_pkOwner->GetExchange() || m_pkOwner->GetShopOwner() || m_pkOwner->IsOpenSafebox() || m_pkOwner->IsCubeOpen())
				return;
			
			// Lifetime
			if(type == 1)
			{
				if (itemxp->GetVnum() >= 55401 && itemxp->GetVnum() <= 55499 || itemxp->GetVnum() >= 55701 && itemxp->GetVnum() <= 55799 || itemxp->GetVnum() == 55001) 
				{
					int give_lifetime = 0;
					
					if(itemxp->GetVnum() == 55001)
						give_lifetime = m_dwbornduration/2;
					
					else if (itemxp->GetVnum() >= 55401 && itemxp->GetVnum() <= 55499)
						give_lifetime = m_dwbornduration * 3 / 100;
					
					else if (itemxp->GetVnum() >= 55701 && itemxp->GetVnum() <= 55799)
						give_lifetime = m_dwbornduration * 3 / 100;
					
					if ((m_dwitemduration -  time(0)) + give_lifetime > m_dwbornduration)
						m_dwitemduration = time(0) + m_dwbornduration;
					else
						m_dwitemduration += give_lifetime;
					
					pSummonItem->SetSocket(1, m_dwitemduration);

					m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetDuration %d %d", m_dwitemduration, m_dwbornduration);
					
					m_dwpetslotitem[i] = -1;
					m_dwpetinvtype[i] = -1;
						
					if (itemxp->GetVnum() >= 55701 && itemxp->GetVnum() <= 55799)
						DBManager::instance().DirectQuery("DELETE from new_petsystem WHERE id='%d'", itemxp->GetID());

					ITEM_MANAGER::instance().RemoveItem(itemxp);
					
					SaveSQL();
				}
			}
			
			// Item-EXP
			else if(type == 3)
			{
				if(itemxp->GetVnum() >= 77760 && itemxp->GetVnum() <= 77763)
				{
					if(GetLevel() == NEW_PET_MAX_LEVEL){
						m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your pet has already reached the maximum level, you cannot give it an item EXP."));
						return;
					}
					bool can_feed = false;
					if (m_dwlevel >= NEW_PET_MAX_LEVEL) 
						m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_FEED_IEXP_MAX_LEVEL"));
						
					if(GetExpI() >= GetNextExpFromItem())
						m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_FEED_IEXP_MAX_IEXP"));
					
					int EXP = 0;
					if (itemxp->GetVnum() == 77760) {
						if (m_dwevolution != 0) 
							m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The food is only suitable for young pets."));
						else {
							EXP = 20000;
							can_feed = true;
						}
					}
					else if (itemxp->GetVnum() == 77761) {
						if (m_dwevolution != 1) 
							m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The food is only suitable for Wild Pets."));
						else {
							EXP = 250000;
							can_feed = true;
						}
					}
					else if (itemxp->GetVnum() == 77762) {
						if (m_dwevolution != 2) 
							m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The food is only suitable for Brave Pets."));
						else {
							EXP = 550000;
							can_feed = true;
						}
					}
					else if (itemxp->GetVnum() == 77763) {
						if (m_dwevolution != 3) 
							m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The food is only suitable for Heroic Pets."));
						else {
							EXP = 950000;
							can_feed = true;
						}
					}
					
					if (can_feed == true)
					{
						SetExp(EXP, 1);
						m_pkOwner->CreateFly(FLY_SP_BIG, GetCharacter());
						itemxp->SetCount(itemxp->GetCount() -1);
						
					}
					m_dwpetslotitem[i] = -1;
					m_dwpetinvtype[i] = -1;
				}
				else
					m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only Pet-Food feed."));
			}
		}
	}
}


void CNewPetActor::SetName(const char* name)
{
	std::string petName = "";

	if (0 != m_pkOwner && 
		0 == name && 
		0 != m_pkOwner->GetName())
	{
		petName += "'s Pet";
	}
	else
		petName += name;

	if (true == IsSummoned())
		m_pkChar->SetName(petName);

	m_name = petName;
}

void CNewPetActor::SetLevel(DWORD level)
{
	m_pkChar->SetLevel(static_cast<char>(level));
	m_dwlevel = level;
	m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 1, GetLevel(), 1);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetLevel %d %d %d %d", m_dwlevel, m_dwevolution);
	if(GetLevel() == NEW_PET_MAX_LEVEL)
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, 0, 0, 0);
	SetNextExp(m_pkChar->PetGetNextExp());
	m_pkChar->UpdatePacket();
	m_pkOwner->UpdatePacket();
}

void CNewPetActor::SetNextLevel()
{
	if(GetLevel() == NEW_PET_MAX_LEVEL){
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "Maximum level reached!");
		return;
	}

	SetLevel(GetLevel() + 1);
	m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 1, GetLevel(), 1);
	IncreasePetBonus();
	m_dwlevelstep = 0;
	m_dwexp = 0;
	m_dwexpitem = 0;
	m_pkChar->SetExp(0);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, m_dwexp, m_dwexpitem, m_pkChar->PetGetNextExp());
		
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
	pSummonItem->SetSocket(4, m_dwlevel);
	
	if(GetEvolution() == 0 && GetLevel() == 40){
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "Evolution 1");
		IncreasePetEvolution();
	}
	else if(GetEvolution() == 1 && GetLevel() == 80){
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "Evolution 2");
		IncreasePetEvolution();
	}
	else if(GetEvolution() == 2 && GetLevel() == 100){
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "Evolution 3");
		IncreasePetEvolution();
	}
	return;
}

void CNewPetActor::SetEvolution(int lv) {
	if (lv == 40){
		m_dwevolution = 1;
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_EVOLUTION_1"));
	}
	else if (lv == 80 && GetEvolution() == 1){
		m_dwevolution = 2;
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_EVOLUTION_2"));
	}
	else if (lv >= 80 && GetEvolution() == 2){
		m_dwevolution = 3;
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_EVOLUTION_3"));
	}
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetEvolution %d", m_dwevolution);
	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetLevel %d %d", lv, m_dwevolution);
}

bool CNewPetActor::IncreasePetEvolution() {
	if (m_dwevolution < 3) {
		if (GetLevel() == 40 && m_dwevolution < 1 || GetLevel() == 80 && m_dwevolution < 2 || GetLevel() == 100 && m_dwevolution < 3) {
			m_dwevolution += 1;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_EVOLUTION"), m_dwevolution);
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetEvolution %d", m_dwevolution);
			
			if (m_dwlevel >= NEW_PET_EVOLUTION_3_LEVEL && m_dwevolution == 2)
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetCanEvolution %d", 1);
			else
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetCanEvolution %d", 0);

			LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
			pSummonItem->SetSocket(5, m_dwevolution);				// Pet Evolution
			
			if (m_dwevolution == 1) {
				m_dwskillslot[0] = 0;
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_EVOLUTION_CAN_LEARN_NEW_SKILL"));
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", 0, m_dwskillslot[0], m_dwskill[0]);
				pSummonItem->SetForceAttribute(3, m_dwskill[0], m_dwskillslot[0]);
			}
			if (m_dwevolution == 2) {
				m_dwskillslot[1] = 0;
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_EVOLUTION_CAN_LEARN_NEW_SKILL"));
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", 1, m_dwskillslot[1], m_dwskill[1]);
				pSummonItem->SetForceAttribute(4, m_dwskill[1], m_dwskillslot[1]);
			}
			if (m_dwevolution == 3) {
				m_dwskillslot[2] = 0;
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_EVOLUTION_CAN_LEARN_NEW_SKILL"));
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetSkill %d %d %d", 2, m_dwskillslot[2], m_dwskill[2]);
				pSummonItem->SetForceAttribute(5, m_dwskill[2], m_dwskillslot[2]);
			}
		}
		else
			return false;
	}
	else {
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_MAX_EVOLUTION"));
		return false;
	}
	return true;
}

void CNewPetActor::SetExp(DWORD exp, int mode)
{
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());

	if(GetLevel() == NEW_PET_MAX_LEVEL) {
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, 0, 0, 0);
		return;
	}

	if (exp < 0)
		exp = MAX(m_dwexp - exp, 0);
	
	if(mode == 0)
	{
		if(GetExp() + exp >= GetNextExpFromMob() && GetExpI() >= GetNextExpFromItem())
		{
			if(GetEvolution() == 0 && GetLevel() == 40)
				return;
			else if(GetEvolution() == 1 && GetLevel() == 80)
				return;
			else if(GetEvolution() == 2 && GetLevel() == 100)
				return;
			else if(GetLevel() == NEW_PET_MAX_LEVEL)
				return;
		}
	}
	else if(mode == 1)
	{
		if(GetExpI() + exp >= GetNextExpFromItem() && GetExp() >= GetNextExpFromMob())
		{
			if(GetEvolution() == 0 && GetLevel() == 40)
				return;
			else if(GetEvolution() == 1 && GetLevel() == 80)
				return;
			else if(GetEvolution() == 2 && GetLevel() == 100)
				return;
			else if(GetLevel() == NEW_PET_MAX_LEVEL)
				return;
		}
	}
		
	if (mode == 0) {
		if (GetExp() + exp >= GetNextExpFromMob()) {
			if (GetExpI() >= GetNextExpFromItem()){
				SetLevel(GetLevel() + 1);
				m_dwexp = 0;
				m_dwexpitem = 0;
				m_pkChar->SetExp(0);
				m_dwlevelstep = 0;
				IncreasePetBonus();
				
				m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 1, GetLevel(), 1);
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, m_dwexp, m_dwexpitem, m_pkChar->PetGetNextExp());
				pSummonItem->SetSocket(4, m_dwlevel);
				
				if (m_dwlevel == NEW_PET_EVOLUTION_1_LEVEL && m_dwevolution == 0 or m_dwlevel == NEW_PET_EVOLUTION_2_LEVEL && m_dwevolution == 1 or m_dwlevel >= NEW_PET_EVOLUTION_3_LEVEL && m_dwevolution == 2)
					m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetCanEvolution %d", 1);
	
				return;
			}else {
				m_dwlevelstep = 4;
				exp = GetNextExpFromMob() - GetExp();
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, m_dwexp, m_dwexpitem, m_pkChar->PetGetNextExp());
			}
		}
		m_dwexp += exp;
		m_pkChar->SetExp(m_dwexp);
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, m_dwexp, m_dwexpitem, m_pkChar->PetGetNextExp());
		if (GetLevelStep() < 4) {
			if (GetExp() >= GetNextExpFromMob() / 4 * 3 && m_dwlevelstep != 3) {
				m_dwlevelstep = 3;
				//m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 25, GetLevel(), 1);
			}else if (GetExp() >= GetNextExpFromMob() / 4 * 2 && m_dwlevelstep != 2) {
				m_dwlevelstep = 2;
				//m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 25, GetLevel(), 1);
			}else if (GetExp() >= GetNextExpFromMob() / 4 && m_dwlevelstep != 1) {
				m_dwlevelstep = 1;
				//m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 25, GetLevel(), 1);
			}
		}
	}
	else if(mode == 1) 
	{
		if (GetExpI() + exp >= GetNextExpFromItem()) {
			if (GetExp() >= GetNextExpFromMob()) {
				SetLevel(GetLevel() + 1);
				m_dwexp = 0;
				m_dwexpitem = 0;
				m_pkChar->SetExp(0);
				m_dwlevelstep = 0;
				IncreasePetBonus();
				
				m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 1, GetLevel(), 1);
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, m_dwexp, m_dwexpitem, m_pkChar->PetGetNextExp());
				pSummonItem->SetSocket(4, m_dwlevel);
				
				if (m_dwlevel == NEW_PET_EVOLUTION_1_LEVEL && m_dwevolution == 0 || m_dwlevel == NEW_PET_EVOLUTION_2_LEVEL && m_dwevolution == 1 || m_dwlevel >= NEW_PET_EVOLUTION_3_LEVEL && m_dwevolution == 2)
					m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetCanEvolution %d", 1);

				return;
				
				if (GetExpI() > GetNextExpFromItem()) {
					m_dwexpitem = GetNextExpFromItem();
					m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, m_dwexp, m_dwexpitem, m_pkChar->PetGetNextExp());
				}
			}else {
				exp = GetNextExpFromItem() - GetExpI();
				m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, m_dwexp, m_dwexpitem, m_pkChar->PetGetNextExp());
			}
		}
		m_dwexpitem += exp;
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetExp %d %d %d %d", m_dwlevel, m_dwexp, m_dwexpitem, m_pkChar->PetGetNextExp());
	}
}

void CNewPetActor::SetNextExp(int nextExp)
{	
	m_dwExpFromMob = (nextExp/10)* 9;
	m_dwExpFromItem = nextExp - m_dwExpFromMob;

}

void CNewPetActor::UpdateTime() 
{

	DoPetSkillAlone();
	m_dwTimePet += 1;

	if (m_dwTimePet >= 60) {
		m_dwTimePet = 0;
		
		m_dwage += 1;
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetAge %d", m_dwage);
		m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "PetDuration %d %d", m_dwitemduration, m_dwbornduration);
	}
}

bool CNewPetActor::Update(DWORD deltaTime)
{
	bool bResult = true;

	if (IsSummoned()) {
		if (m_pkOwner->IsImmortal() && Pet_Skill_Table[16][2 + m_dwskill[m_dwslotimm]] <= (get_global_time() - m_dwImmTime)*10) {
			//m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "%d - %d  diff %d  Skilltable %d", get_global_time(), m_dwImmTime, (get_global_time() - m_dwImmTime) * 10, Pet_Skill_Table[16][2 + m_dwskill[m_dwslotimm]]);
			m_dwImmTime = 0;
			m_pkOwner->SetImmortal(0);
		}
	}
	if ((IsSummoned() && (ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetSocket(1) - time(0) <= 0)) || NULL == ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID()) || ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetOwner() != this->GetOwner())
	{
		this->Unsummon();
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_IS_DEAD"));
		return true;
	}

	if (this->IsSummoned() && HasOption(EPetOption_Followable))
		bResult = bResult && this->_UpdateFollowAI();

	return bResult;
}

void CNewPetActor::SaveSQL() {
	DBManager::instance().DirectQuery
	(
		"UPDATE new_petsystem SET level = %d, evolution=%d, exp=%d, expi=%d, bonus0=%d, bonus1=%d, bonus2=%d, skill0=%d, skill0lv= %d, skill1=%d, skill1lv= %d, skill2=%d, skill2lv= %d, item_duration=%d, born_duration=%d, age = (UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(birthday))/60, age_bonus = FLOOR(age / 60 / 24 / %d), pettype=%d, typebonus0=%d, typebonus1=%d, typebonus2=%d WHERE id = %lu ", 
		this->GetLevel(), 
		this->m_dwevolution, 
		this->GetExp(), 
		this->GetExpI(), 
		this->m_dwbonuspet[0][1], 
		this->m_dwbonuspet[1][1], 
		this->m_dwbonuspet[2][1], 
		this->m_dwskillslot[0], 
		this->m_dwskill[0], 
		this->m_dwskillslot[1], 
		this->m_dwskill[1], 
		this->m_dwskillslot[2], 
		this->m_dwskill[2], 
		this->m_dwitemduration, 
		this->m_dwbornduration,
		NEW_PET_AGE_GIVE_BONUS_EVERY_DAYS,
		this->m_dwpettype,
		this->m_dwtypebonus0,
		this->m_dwtypebonus1,
		this->m_dwtypebonus2,
		ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetID()
	);
}

bool CNewPetActor::Follow(float fMinDistance)
{
	if( !m_pkOwner || !m_pkChar) 
		return false;

	float fOwnerX = m_pkOwner->GetX();
	float fOwnerY = m_pkOwner->GetY();

	float fPetX = m_pkChar->GetX();
	float fPetY = m_pkChar->GetY();

	float fDist = DISTANCE_SQRT(fOwnerX - fPetX, fOwnerY - fPetY);
	if (fDist <= fMinDistance)
		return false;

	m_pkChar->SetRotationToXY(fOwnerX, fOwnerY);

	float fx, fy;

	float fDistToGo = fDist - fMinDistance;
	GetDeltaByDegree(m_pkChar->GetRotation(), fDistToGo, &fx, &fy);
	
	if (!m_pkChar->Goto((int)(fPetX+fx+0.5f), (int)(fPetY+fy+0.5f)) )
		return false;

	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);

	return true;
}

// char_state.cpp StateHorse
bool CNewPetActor::_UpdateFollowAI()
{
	if (0 == m_pkChar->m_pkMobData)
	{
		return false;
	}

	if (0 == m_originalMoveSpeed)
	{
		const CMob* mobData = CMobManager::Instance().Get(m_dwVnum);

		if (0 != mobData)
			m_originalMoveSpeed = mobData->m_table.sMovingSpeed;
	}
	float	START_FOLLOW_DISTANCE = 500.0f;
	float	START_RUN_DISTANCE = 1500.0f;	

	float	RESPAWN_DISTANCE = 4500.f;		
	int	APPROACH = 300;

	bool bDoMoveAlone = true;
	bool bRun = false;	

	DWORD currentTime = get_dword_time();

	long ownerX = m_pkOwner->GetX();		long ownerY = m_pkOwner->GetY();
	long charX = m_pkChar->GetX();			long charY = m_pkChar->GetY();

	float fDist = DISTANCE_APPROX(charX - ownerX, charY - ownerY);

	if (fDist >= RESPAWN_DISTANCE)
	{
		float fOwnerRot = m_pkOwner->GetRotation() * 3.141592f / 180.f;
		float fx = -APPROACH * cos(fOwnerRot);
		float fy = -APPROACH * sin(fOwnerRot);
		if (m_pkChar->Show(m_pkOwner->GetMapIndex(), ownerX + fx, ownerY + fy))
		{
			return true;
		}
	}
	
	
	if (fDist >= START_FOLLOW_DISTANCE)
	{
		if( fDist >= START_RUN_DISTANCE)
		{
			bRun = true;
		}

		m_pkChar->SetNowWalking(!bRun);
		
		Follow(APPROACH);

		m_pkChar->SetLastAttacked(currentTime);
		m_dwLastActionTime = currentTime;
	}
	//else
	//{
	//	if (fabs(m_pkChar->GetRotation() - GetDegreeFromPositionXY(charX, charY, ownerX, ownerX)) > 10.f || fabs(m_pkChar->GetRotation() - GetDegreeFromPositionXY(charX, charY, ownerX, ownerX)) < 350.f)
	//	{
	//		m_pkChar->Follow(m_pkOwner, APPROACH);
	//		m_pkChar->SetLastAttacked(currentTime);
	//		m_dwLastActionTime = currentTime;
	//	}
	//}
	
	else if (currentTime - m_dwLastActionTime > number(5000, 12000))
	{
		this->_UpdatAloneActionAI(START_FOLLOW_DISTANCE, START_FOLLOW_DISTANCE);
	}
	else
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	return true;
}

bool CNewPetActor::_UpdatAloneActionAI(float fMinDist, float fMaxDist)
{
	float fDist = number(fMinDist, fMaxDist);
	float r = (float)number (0, 359);
	float dest_x = GetOwner()->GetX() + fDist * cos(r);
	float dest_y = GetOwner()->GetY() + fDist * sin(r);

	//m_pkChar->SetRotation(number(0, 359));        // 방향은 랜덤으로 설정

	//GetDeltaByDegree(m_pkChar->GetRotation(), fDist, &fx, &fy);

	// 느슨한 못감 속성 체크; 최종 위치와 중간 위치가 갈수없다면 가지 않는다.
	//if (!(SECTREE_MANAGER::instance().IsMovablePosition(m_pkChar->GetMapIndex(), m_pkChar->GetX() + (int) fx, m_pkChar->GetY() + (int) fy) 
	//			&& SECTREE_MANAGER::instance().IsMovablePosition(m_pkChar->GetMapIndex(), m_pkChar->GetX() + (int) fx/2, m_pkChar->GetY() + (int) fy/2)))
	//	return true;

	m_pkChar->SetNowWalking(true);

	//if (m_pkChar->Goto(m_pkChar->GetX() + (int) fx, m_pkChar->GetY() + (int) fy))
	//	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	if (!m_pkChar->IsStateMove() && m_pkChar->Goto(dest_x, dest_y))
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	m_dwLastActionTime = get_dword_time();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
//  CPetSystem
///////////////////////////////////////////////////////////////////////////////////////

CNewPetSystem::CNewPetSystem(LPCHARACTER owner)
{
//	assert(0 != owner && "[CPetSystem::CPetSystem] Invalid owner");

	m_pkOwner = owner;
	m_dwUpdatePeriod = 400;

	m_dwLastUpdateTime = 0;
}

CNewPetSystem::~CNewPetSystem()
{
	Destroy();
}

void CNewPetSystem::ChangePetAttr() 
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->ChangePetAttr();
			}
		}
	}

}
void  CNewPetSystem::RemovePetSkill(int skillslot, int itemvnum) {
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->RemovePetSkill(skillslot, itemvnum);
			}
		}
	}
}

bool  CNewPetSystem::IncreasePetSkill(int skill) {
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->IncreasePetSkill(skill);
			}
		}
	}
	return false;
}

bool  CNewPetSystem::IncreasePetEvolution() {
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->IncreasePetEvolution();
			}
		}
	}
	return false;
}


void CNewPetSystem::Destroy()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;

		if (0 != petActor)
		{
			delete petActor;
		}
	}
	event_cancel(&m_pkNewPetSystemUpdateEvent);
	event_cancel(&m_pkNewPetSystemExpireEvent);
	m_petActorMap.clear();
}


void CNewPetSystem::UpdateTime()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;

		if (0 != petActor && petActor->IsSummoned())
		{
			petActor->UpdateTime();
		}
	}
}

bool CNewPetSystem::Update(DWORD deltaTime)
{
	bool bResult = true;

	DWORD currentTime = get_dword_time();

	if (m_dwUpdatePeriod > currentTime - m_dwLastUpdateTime)
		return true;
	
	std::vector <CNewPetActor*> v_garbageActor;

	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;

		if (0 != petActor && petActor->IsSummoned())
		{
			LPCHARACTER pPet = petActor->GetCharacter();
			
			if (NULL == CHARACTER_MANAGER::instance().Find(pPet->GetVID()))
			{
				v_garbageActor.push_back(petActor);
			}
			else
			{
				bResult = bResult && petActor->Update(deltaTime);
			}
		}
	}
	for (std::vector<CNewPetActor*>::iterator it = v_garbageActor.begin(); it != v_garbageActor.end(); it++)
		DeletePet(*it);

	m_dwLastUpdateTime = currentTime;

	return bResult;
}

void CNewPetSystem::DeletePet(DWORD mobVnum)
{
	TNewPetActorMap::iterator iter = m_petActorMap.find(mobVnum);

	if (m_petActorMap.end() == iter)
	{
		sys_err("[CPetSystem::DeletePet] Can't find pet on my list (VNUM: %d)", mobVnum);
		return;
	}

	CNewPetActor* petActor = iter->second;

	if (0 == petActor)
		sys_err("[CPetSystem::DeletePet] Null Pointer (petActor)");
	else
		delete petActor;

	m_petActorMap.erase(iter);	
}

void CNewPetSystem::DeletePet(CNewPetActor* petActor)
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		if (iter->second == petActor)
		{
			delete petActor;
			m_petActorMap.erase(iter);

			return;
		}
	}

	sys_err("[CPetSystem::DeletePet] Can't find petActor(0x%x) on my list(size: %d) ", petActor, m_petActorMap.size());
}

void CNewPetSystem::Unsummon(DWORD vnum, bool bDeleteFromList)
{
	CNewPetActor* actor = this->GetByVnum(vnum);

	if (0 == actor)
	{
		sys_err("[CPetSystem::GetByVnum(%d)] Null Pointer (petActor)", vnum);
		return;
	}
	actor->Unsummon();

	if (true == bDeleteFromList)
		this->DeletePet(actor);

	bool bActive = false;
	for (TNewPetActorMap::iterator it = m_petActorMap.begin(); it != m_petActorMap.end(); it++)
	{
		bActive |= it->second->IsSummoned();
	}
	if (false == bActive)
	{
		event_cancel(&m_pkNewPetSystemUpdateEvent);
		event_cancel(&m_pkNewPetSystemExpireEvent);
		m_pkNewPetSystemUpdateEvent = NULL;
		m_pkNewPetSystemExpireEvent = NULL;
	}
}

DWORD CNewPetSystem::GetSummonedPetItemID()
{
	DWORD itemid = 0;
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				itemid = petActor->GetSummonItemID();
				break;
			}			
		}
	}
	return itemid;

}

bool CNewPetSystem::IsActivePet()
{
	bool state = false;
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				state = true;
				break;
			}			
		}
	}
	return state;

}

int CNewPetSystem::GetLevelStep()
{
	int step = 4;
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				step = petActor->GetLevelStep();
				break;
			}
		}
	}
	return step;
}

void CNewPetSystem::SetExp(int iExp, int mode)
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				petActor->SetExp(iExp, mode);
				break;
			}
		}
	}
}

void CNewPetSystem::SetNextLevel()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				petActor->SetNextLevel();
				break;
			}
		}
	}
}

int CNewPetSystem::GetEvolution()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetEvolution();
			}
		}
	}
	return -1;
}

int CNewPetSystem::GetLevel()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetLevel();
			}
		}
	}
	return -1;
}

int CNewPetSystem::GetPetType()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetPetType();
			}
		}
	}
	return -1;
}

int CNewPetSystem::GetSkillSlot0()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetSkillSlot0();
			}
		}
	}
	return -1;
}

int CNewPetSystem::GetSkillSlot1()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetSkillSlot1();
			}
		}
	}
	return -1;
}

int CNewPetSystem::GetSkillSlot2()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetSkillSlot2();
			}
		}
	}
	return -1;
}

int CNewPetSystem::GetSkillLevel0()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetSkillLevel0();
			}
		}
	}
	return -1;
}

int CNewPetSystem::GetSkillLevel1()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetSkillLevel1();
			}
		}
	}
	return -1;
}

int CNewPetSystem::GetSkillLevel2()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetSkillLevel2();
			}
		}
	}
	return -1;
}

int CNewPetSystem::GetExp()
{
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->GetExp();
			}
		}
	}
	return 0;
}

void CNewPetSystem::SetItemCube(int pos, int invpos, int invtype) {
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->SetItemCube(pos, invpos, invtype);
			}
		}
	}
}

void CNewPetSystem::ItemCubeFeed(int type) {
	for (TNewPetActorMap::iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;
		if (petActor != 0)
		{
			if (petActor->IsSummoned()) {
				return petActor->ItemCubeFeed(type);
			}
		}
	}
}

CNewPetActor* CNewPetSystem::Summon(DWORD mobVnum, LPITEM pSummonItem, const char* petName, bool bSpawnFar, DWORD options)
{
	CNewPetActor* petActor = this->GetByVnum(mobVnum);

	// 등록된 펫이 아니라면 새로 생성 후 관리 목록에 등록함.
	if (0 == petActor)
	{
		petActor = M2_NEW CNewPetActor(m_pkOwner, mobVnum, options);
		m_petActorMap.insert(std::make_pair(mobVnum, petActor));
	}

	DWORD petVID = petActor->Summon(petName, pSummonItem, bSpawnFar);

	if (NULL == m_pkNewPetSystemUpdateEvent)
	{
		newpetsystem_event_info* info = AllocEventInfo<newpetsystem_event_info>();

		info->pPetSystem = this;

		m_pkNewPetSystemUpdateEvent = event_create(newpetsystem_update_event, info, PASSES_PER_SEC(1) / 4);	// 0.25초	
	}

	if (NULL == m_pkNewPetSystemExpireEvent)
	{
		newpetsystem_event_infoe* infoe = AllocEventInfo<newpetsystem_event_infoe>();

		infoe->pPetSystem = this;

		m_pkNewPetSystemExpireEvent = event_create(newpetsystem_expire_event, infoe, PASSES_PER_SEC(1) );	// 1 volata per sec
	}

	return petActor;
}


CNewPetActor* CNewPetSystem::GetByVID(DWORD vid) const
{
	CNewPetActor* petActor = 0;

	bool bFound = false;

	for (TNewPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		petActor = iter->second;

		if (0 == petActor)
		{
			sys_err("[CPetSystem::GetByVID(%d)] Null Pointer (petActor)", vid);
			continue;
		}

		bFound = petActor->GetVID() == vid;

		if (true == bFound)
			break;
	}

	return bFound ? petActor : 0;
}

CNewPetActor* CNewPetSystem::GetByVnum(DWORD vnum) const
{
	CNewPetActor* petActor = 0;

	TNewPetActorMap::const_iterator iter = m_petActorMap.find(vnum);

	if (m_petActorMap.end() != iter)
		petActor = iter->second;

	return petActor;
}

size_t CNewPetSystem::CountSummoned() const
{
	size_t count = 0;

	for (TNewPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;

		if (0 != petActor)
		{
			if (petActor->IsSummoned())
				++count;
		}
	}

	return count;
}

void CNewPetSystem::RefreshBuff()
{
	for (TNewPetActorMap::const_iterator iter = m_petActorMap.begin(); iter != m_petActorMap.end(); ++iter)
	{
		CNewPetActor* petActor = iter->second;

		if (0 != petActor)
		{
			if (petActor->IsSummoned())
			{
				petActor->ClearBuff();
				petActor->GiveBuff();
			}
		}
	}
}
