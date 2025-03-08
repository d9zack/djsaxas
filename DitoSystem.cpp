#include "stdafx.h"
#include "DitoSystem.h"
#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "mob_manager.h"
#include "affect.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "skill.h"
#include "utils.h"
#include "../../common/VnumHelper.h"
#include "packet.h"
#include "constants.h"
#include "db.h"
#include "item.h"
#include "item_manager.h"

EVENTINFO(ditosystem_event_info)
{
	CDitoActor* pDitoActor;
};

EVENTFUNC(ditosystem_update_event)
{
	ditosystem_event_info* info = dynamic_cast<ditosystem_event_info*>( event->info );
	if ( info == NULL )
		return 0;

	CDitoActor*	pDitoActor = info->pDitoActor;

	if (NULL == pDitoActor)
		return 0;

	pDitoActor->Update();

	return PASSES_PER_SEC(1) / 4;
}

CDitoActor::CDitoActor()
{
	/* Actor&&owner */
	m_pkActor = NULL;
	m_pkOwner = NULL;
	/* Events */
	m_pkDitoSystemUpdateEvent = NULL;
	m_dwLevel=0;
}

CDitoActor::~CDitoActor()
{
	Unsummon();
}


bool CDitoActor::Update()
{
	bool bResult = true;
	bool bUpdatePacket = false;

	LPCHARACTER m_pkChar = m_pkActor;

	if (!m_pkChar || !m_pkOwner)
		return false;


	if (m_pkOwner->IsDead() || (IsSummoned() && m_pkChar->IsDead()))
	{		
		this->Unsummon();
		return true;
	}

	if (this->IsSummoned())
	{
		for (int i = 0; i<POINT_MAX_NUM; i++)
		{
			int value = m_pkOwner->GetPoint(i);
			if (value <= 0)
			{
				continue;
			}

			int value2 = m_pkOwner->GetRealPoint(i);
			int oringial_prob2=value2;

			int oringial_prob = value;
			
			if (oringial_prob==0)
			{
				if (i == POINT_MOV_SPEED || i == POINT_ATT_SPEED)
					m_pkChar->SetPoint(i, m_pkOwner->GetPoint(i));
				else
					m_pkActor->SetPoint(i, 0);
				continue;
			}
		
			if (i == POINT_MOV_SPEED || i == POINT_ATT_SPEED)
			{
				m_pkChar->SetPoint(i, m_pkOwner->GetPoint(i));
				if (oringial_prob2 != 0)
					m_pkChar->SetRealPoint(i, m_pkOwner->GetRealPoint(i));
			}
			else
			{
				m_pkChar->SetPoint(i, oringial_prob);

				if (oringial_prob2 != 0)
					m_pkChar->SetRealPoint(i, oringial_prob2);
			}
		}
		for (int x = 0; x<GUILD_SKILL_END; x++)
		{
			m_pkChar->SetSkillLevel(x, m_pkOwner->GetSkillLevel(x));
		}

		for (int j = 0; j<PART_MAX_NUM; j++)
		{
			if (m_pkChar->GetPart(j) != m_pkOwner->GetPart(j))
			{
				m_pkChar->SetPart(j, m_pkOwner->GetPart(j));
				bUpdatePacket=true;
			}
		}

		if (m_pkOwner->GetMountVnum())
		{
			m_pkChar->MountVnum(m_pkOwner->GetMountVnum());
			bUpdatePacket=true;
		}
		else
		{
			m_pkChar->MountVnum(0);
			bUpdatePacket=true;
		}
		if (m_pkActor->GetLevel() != m_pkOwner->GetLevel())	
		{
			m_pkActor->SetLevel(m_pkOwner->GetLevel());
			m_pkActor->SpecificEffectPacket("d:/ymir work/effect/etc/levelup_1/level_up.mse");
			bUpdatePacket=true;
		}
		if (m_pkActor->GetRealAlignment() != m_pkOwner->GetRealAlignment())
		{
			int Align = m_pkActor->GetRealAlignment();
			m_pkActor->UpdateAlignment(-Align);
			m_pkActor->UpdateAlignment(m_pkOwner->GetRealAlignment());
			bUpdatePacket=true;
		}
		for (int k=0; k<AFF_BITS_MAX;k++)
		{
			if (m_pkActor->IsAffectFlag(k) && !m_pkOwner->IsAffectFlag(k))
			{
				m_pkActor->RemoveAffectFlag(k);
				bUpdatePacket=true;
			}
			if (m_pkOwner->IsAffectFlag(k))
			{
				m_pkActor->SetAffectFlag(k);
				bUpdatePacket=true;
			}
		}
		
	}

	if (bUpdatePacket)
	{
		bResult=true;
		m_pkChar->UpdatePacket();
	}

	return true;
}
void CDitoActor::Unsummon()
{
	if (!GetOwner() || !GetActor())
		return;

	if (NULL != m_pkActor)
	{
		m_pkActor->m_pkDito=NULL;

		if (NULL != m_pkActor)
			M2_DESTROY_CHARACTER(m_pkActor);

		m_pkActor = NULL;

	}

	if (m_pkDitoSystemUpdateEvent)
	{
		event_cancel(&m_pkDitoSystemUpdateEvent);
		m_pkDitoSystemUpdateEvent=NULL;
	}


}


bool CDitoActor::Summon()
{	

	m_pkActor = CHARACTER_MANAGER::Instance().CreateCharacter("");
	m_pkActor->m_pkDito=this;
	CHARACTER_MANAGER::Instance().CopyPC(m_pkOwner, m_pkActor);
	long x = m_pkOwner->GetX();
	long y = m_pkOwner->GetY();
	long z = m_pkOwner->GetZ();

	x += number(-100, 100);
	y += number(-100, 100);
	m_pkActor->Show(m_pkOwner->GetMapIndex(), x, y, z);

	LPCHARACTER src = m_pkOwner;
	LPCHARACTER ch = m_pkActor;

	m_pkActor->SetSkillGroup(src->GetSkillGroup());
	CHARACTER_MANAGER::Instance().CopyPC(m_pkOwner, m_pkActor);
	m_pkActor->SetSkills(src->GetSkills());
	m_pkActor->UpdatePacket();

	
	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "This dito gives you %d percent.", (m_dwLevel));


	ditosystem_event_info* info = AllocEventInfo<ditosystem_event_info>();

	info->pDitoActor= this;


	m_pkDitoSystemUpdateEvent = event_create(ditosystem_update_event, info, PASSES_PER_SEC(1) / 4);
	
	return this->IsSummoned();
	
}

bool CDitoActor::SetOwner(LPCHARACTER pkOwner)
{
	if (!pkOwner)
		return false;

	if (!m_pkOwner)
	{
		m_pkOwner = pkOwner;
		return true;
	}
	if (m_pkOwner == pkOwner)
		return false;

	m_pkOwner = pkOwner;
		
	return true;
	
}


