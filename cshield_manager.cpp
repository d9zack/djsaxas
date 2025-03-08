#include "stdafx.h"
#include "config.h"

#ifdef ENABLE_CSHIELD
#include "cshield_manager.h"

static LPEVENT running_event = NULL;

EVENTINFO(EventsManagerInfoData)
{
	CShieldManager* pEvents;

	EventsManagerInfoData()
		: pEvents(0)
	{
	}
};

EVENTFUNC(event_timer)
{
	if (event == NULL)
		return 0;

	if (event->info == NULL)
		return 0;

	EventsManagerInfoData* info = dynamic_cast<EventsManagerInfoData*>(event->info);

	if (info == NULL)
		return 0;

	CShieldManager* pInstance = info->pEvents;

	if (pInstance == NULL)
		return 0;

	CShieldManager::instance().CShieldUpdate();
	return PASSES_PER_SEC(AUTOUPDATE_INTERVAL);
}

void CShieldManager::CShieldUpdate()
{
	UpdateCShield(VERIFICATION_KEY);
}

bool CShieldManager::Initialize()
{
	if (running_event != NULL)
	{
		event_cancel(&running_event);
		running_event = NULL;
	}

	EventsManagerInfoData* info = AllocEventInfo<EventsManagerInfoData>();
	info->pEvents = this;

	InitCShield(VERIFICATION_KEY);

#ifdef AUTOUPDATE_DATA
	running_event = event_create(event_timer, info, PASSES_PER_SEC(AUTOUPDATE_INTERVAL));
#endif
	return true;
}

void CShieldManager::Destroy()
{
	if (running_event != NULL)
	{
		event_cancel(&running_event);
		running_event = NULL;
	}
}
#endif
