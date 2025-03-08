#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../../libthecore/include/xmd5.h"
#endif

#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "dev_log.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "threeway_war.h"
#include "log.h"
#ifdef ENABLE_REBORN_SYSTEM	
	#include "reborn.h"
#endif
#ifdef ENABLE_TITLE_SYSTEM	
	#include "title.h"
#endif
#ifdef ENABLE_MAINTENANCE_SYSTEM	
	#include "maintenance.h"
#endif
#ifdef ENABLE_PREMIUM_SYSTEM
	#include "premium_system.h"
#endif
#ifdef LWT_BUFFI_SYSTEM
#include "lwt_buffi.h"
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
#include "char_petsystem.h"
#endif
#ifdef __WORLD_BOSS_YUMA__
#include "worldboss.h"
#endif

#include "../../common/VnumHelper.h"
#ifdef __AUCTION__
#include "auction_manager.h"
#endif
#ifdef __SYSTEM_SEARCH_ITEM_MOB__
	#include <string>
	#include <boost/algorithm/string.hpp>
#endif

#include "shop.h"

#ifdef __FAKE_PLAYER__
	#include "FakePlayerManager.h"
#endif


extern int g_server_id;

extern int g_nPortalLimitTime;

ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		// ���� �ƴ� �ٸ�Ż���� Ÿ���ִ�.
		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�̹� Ż���� �̿����Դϴ�."));
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ���� ��ȯ���ּ���."));
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

ACMD(do_user_horse_back)
{
	if (ch->GetHorse() != NULL)
	{
		ch->MountUnsummon();
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ ���� ������ �մϴ�."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ���� ��ȯ���ּ���."));
	}
}

ACMD(do_user_horse_feed)
{
	if (ch->GetHorse() == NULL)
	{
		if (ch->IsHorseRiding() == false)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ���� ��ȯ���ּ���."));
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ź ���¿����� ���̸� �� �� �����ϴ�."));
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ %s%s �־����ϴ�."), 
				ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName,
				g_iUseLocale ? "" : under_han(ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName) ? LC_TEXT("��") : LC_TEXT("��"));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s �������� �ʿ��մϴ�"), ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName);
	}
}

ACMD(do_bioport)
{
	// ���λ����� �� ���¿����� �� ���̸� �� �� ����.
	if (!ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cant warp atm");
		return;
	}
	
	// KEVIN ANPASSEN
	if (ch->GetEmpire() == 1)
	{
		ch->WarpSet(498500, 957000);
	}
	
	if (ch->GetEmpire() == 2)
	{
		ch->WarpSet(89200, 182300);
	}
	
	if (ch->GetEmpire() == 3)
	{
		ch->WarpSet(950700, 233400);
	}
}

#ifdef __BACK_DUNGEON__
ACMD(do_back_dungeon)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 3) { return; }
	DWORD mapIdx;
	if (!str_to_number(mapIdx, vecArgs[2].c_str()))
		return;
	if(vecArgs[1] == "ok")
		CHARACTER_MANAGER::Instance().CheckBackDungeon(ch, BACK_DUNGEON_WARP, mapIdx);
	else if(vecArgs[1] == "no")
		CHARACTER_MANAGER::Instance().CheckBackDungeon(NULL, BACK_DUNGEON_REMOVE, mapIdx);
}
#endif

#ifdef ENABLE_EVENT_MANAGER
ACMD(do_event_manager)
{
	CHARACTER_MANAGER::Instance().SendDataPlayer(ch);
}
#endif

#ifdef ENABLE_PREMIUM_SYSTEM
ACMD(do_premium)
{
	//BASIC
	char arg1[256], arg2[256], arg3[256];
	int arg3_int = 0;
	argument = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	const char* arg3_2 = one_argument(argument, arg3, sizeof(arg3));
	strcat(strcat(arg3, " "),arg3_2);
	
	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("premium_command_syntax"));
		return;
	}
	
	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);
	
	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s_premium_no_target"), arg2);
		return;
	}
	
	if(!strcmp(arg1, "set")) {
		str_to_number(arg3_int, arg3);
		if(isdigit(*arg3) || arg3_int >= 0)
		{
			if(CPremiumSystem::instance().IsPremium(tch))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s_premium_is_already_active"), tch->GetName());
				return;
			}
			
			CPremiumSystem::instance().SetPremium(tch, arg3_int);
			CPremiumSystem::instance().RefreshPremium(tch);
#ifdef ENABLE_UPDATE_PACKET_IN_PREMIUM_SYSTEM
			tch->UpdatePacket();
#endif

			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%i_premium_set_admin_message"), tch->GetName(), arg3_int);
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%i_premium_set_target_message"), ch->GetName(), arg3_int);
			return;
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("premium_set_command_syntax"));
			return;
		}
	}
	else if(!strcmp(arg1, "remove")) 
	{
		if(!CPremiumSystem::instance().IsPremium(tch))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s_premium_is_not_active"), tch->GetName());
			return;			
		}
		
		CPremiumSystem::instance().RemovePremium(tch);
		CPremiumSystem::instance().RefreshPremium(tch);
#ifdef ENABLE_UPDATE_PACKET_IN_PREMIUM_SYSTEM
		tch->UpdatePacket();
#endif
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s_premium_remove_admin_message"), tch->GetName());
		tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s_premium_remove_target_message"), ch->GetName());		
		return;
	}
	else if(!strcmp(arg1, "refresh"))
	{
		CPremiumSystem::instance().RefreshPremium(tch);
#ifdef ENABLE_UPDATE_PACKET_IN_PREMIUM_SYSTEM
		tch->UpdatePacket();
#endif
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s_premium_refresh_admin_message"), tch->GetName());
		tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s_premium_refresh_target_message"), ch->GetName());		
		return;
	}
	else if(!strcmp(arg1, "check"))
	{
		const char* status = "0";
		if(CPremiumSystem::instance().IsPremium(tch))
			status = "active";
		else
			status = "noactive";
		
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%s_premium_status_message"), tch->GetName(), status);
		return;
	}
	else if(!strcmp(arg1, "check2"))
	{
		const char* status = "0";
		if(CPremiumSystem::instance().IsPremium2(tch))
			status = "active";
		else
			status = "noactive";
		
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%s_premium_status_message"), tch->GetName(), status);
		return;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("premium_command_syntax"));
		return;
	}
}

ACMD(do_refresh_premium)
{
	CPremiumSystem::instance().RefreshPremium(ch);
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("premium_refreshed"));
}
#endif

#define MAX_REASON_LEN		128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo() 
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator()(LPDESC d)
	{
		if(d->GetCharacter())
		{
#ifdef FLUSH_AT_SHUTDOWN
			d->GetCharacter()->SaveReal();
			DWORD pid = d->GetCharacter()->GetPlayerID();
			db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
			db_clientdesc->Packet(&pid, sizeof(DWORD));
#endif
			if(d->GetCharacter()->GetGMLevel() == GM_PLAYER)
			{
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
			}
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
	: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		char buf[64];
		snprintf(buf, sizeof(buf), LC_TEXT("�˴ٿ��� %d�� ���ҽ��ϴ�."), *pSec);
		SendNotice(buf);

		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	char buf[64];
	snprintf(buf, sizeof(buf), LC_TEXT("%d�� �� ������ �˴ٿ� �˴ϴ�."), iSec);

	SendNotice(buf);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	if (NULL == ch)
	{
		sys_err("Accept shutdown command from %s.", ch->GetName());
	}
	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}

#ifdef ENABLE_REBORN_SYSTEM
ACMD(do_set_reborn)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "your argument is invalid");
		return;
	}
	
	if (!strcmp(arg1, "accept"))
	{			
		RebornManager::instance().SetLevel(ch);
	}	
	
	if (!strcmp(arg1, "anonymous"))
	{	
		int isAnonymous = ch->GetQuestFlag("reborn.valueAnonymous");
		
		if (isAnonymous != 2 && isAnonymous != 1) 
		{	
			ch->SetQuestFlag("reborn.valueAnonymous", 1);
			RebornManager::instance().SetAnonymous(ch, "anonymous");
		}	
		else
			RebornManager::instance().SetAnonymous(ch, "anonymous");	
	}
	
	if (!strcmp(arg1, "remove_affect_administrator"))
	{				
		RebornManager::instance().SetAffect(ch, "remove_affect_administrator");
	}	
}
#endif

#ifdef ENABLE_TITLE_SYSTEM	
ACMD(do_prestige_title)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "your argument is invalid");
		return;
	}
	
	if (!strcmp(arg1, "prestige_0")){			
		TitleManager::instance().SetTitle(ch, "disable");}	
	
	if (!strcmp(arg1, "prestige_1")){		
		TitleManager::instance().SetTitle(ch, "title1");}
	
	if (!strcmp(arg1, "prestige_2")){		
		TitleManager::instance().SetTitle(ch, "title2");}	
	
	if (!strcmp(arg1, "prestige_3")){		
		TitleManager::instance().SetTitle(ch, "title3");}
	
	if (!strcmp(arg1, "prestige_4")){		
		TitleManager::instance().SetTitle(ch, "title4");}	
	
	if (!strcmp(arg1, "prestige_5")){		
		TitleManager::instance().SetTitle(ch, "title5");}	
	
	if (!strcmp(arg1, "prestige_6")){		
		TitleManager::instance().SetTitle(ch, "title6");}		
	
	if (!strcmp(arg1, "prestige_7")){		
		TitleManager::instance().SetTitle(ch, "title7");}	
	
	if (!strcmp(arg1, "prestige_8")){		
		TitleManager::instance().SetTitle(ch, "title8");}		
	
	if (!strcmp(arg1, "prestige_9")){		
		TitleManager::instance().SetTitle(ch, "title9");}	
	
	if (!strcmp(arg1, "prestige_10")){		
		TitleManager::instance().SetTitle(ch, "title10");}	
	
	if (!strcmp(arg1, "prestige_11")){		
		TitleManager::instance().SetTitle(ch, "title11");}	
	
	if (!strcmp(arg1, "prestige_12")){		
		TitleManager::instance().SetTitle(ch, "title12");}	
	
	if (!strcmp(arg1, "prestige_13")){		
		TitleManager::instance().SetTitle(ch, "title13");}	
	
	if (!strcmp(arg1, "prestige_14")){		
		TitleManager::instance().SetTitle(ch, "title14");}	
	
	if (!strcmp(arg1, "prestige_15")){		
		TitleManager::instance().SetTitle(ch, "title15");}
	
	if (!strcmp(arg1, "prestige_16")){		
		TitleManager::instance().SetTitle(ch, "title16");}	
	
	if (!strcmp(arg1, "prestige_17")){		
		TitleManager::instance().SetTitle(ch, "title17");}	
	
	if (!strcmp(arg1, "prestige_18")){		
		TitleManager::instance().SetTitle(ch, "title18");}	
	
	if (!strcmp(arg1, "prestige_19")){		
		TitleManager::instance().SetTitle(ch, "title19");}	
	
	if (!strcmp(arg1, "buy_prestige_1")){		
		TitleManager::instance().BuyPotion(ch, "buy_potion_1");}	

	if (!strcmp(arg1, "buy_prestige_2")){		
		TitleManager::instance().BuyPotion(ch, "buy_potion_2");}		

	if (!strcmp(arg1, "buy_prestige_3")){		
		TitleManager::instance().BuyPotion(ch, "buy_potion_3");}	

	if (!strcmp(arg1, "vegas_transform_title")){		
		TitleManager::instance().TransformTitle(ch);}			
}
#endif

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );
	
	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		if (true == LC_IsEurope() || true == LC_IsYMIR() || true == LC_IsKorea())
		{
			switch (info->subcmd)
			{
				case SCMD_LOGOUT:
				case SCMD_QUIT:
				case SCMD_PHASE_SELECT:
					{
						TPacketNeedLoginLogInfo acc_info;
						acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;

						db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

						LogManager::instance().DetailLoginLog( false, ch );
					}
					break;
			}
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				break;

			case SCMD_PHASE_SELECT:
				{
					ch->Disconnect("timed_event - SCMD_PHASE_SELECT");

					if (d)
					{
						d->SetPhase(PHASE_SELECT);
					}
				}
				break;
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d�� ���ҽ��ϴ�."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

#ifdef ENABLE_MAINTENANCE_SYSTEM
ACMD(do_maintenance_text)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));	
	
	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE, "<Syntax> The arguments available for this command are:");
		ch->ChatPacket(CHAT_TYPE_NOTICE, "<Syntax> /m_text disable");
		ch->ChatPacket(CHAT_TYPE_NOTICE, "<Syntax> /m_text enable <text>");
		return;
	}

	if (*arg1 && !strcmp(arg1, "disable"))
	{			
		MaintenanceManager::instance().Send_Text(ch, "rmf");
	}

	else if (*arg1 && !strcmp(arg1, "enable"))		
	{			
		const char* sReason = one_argument(argument, arg2, sizeof(arg2));
		MaintenanceManager::instance().Send_Text(ch, sReason);
	}	
}


ACMD(do_maintenance)
{
	char arg1[256];
	char arg2[256];
	
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	
	if (*arg1 && !strcmp(arg1, "force_stop"))
	{			
		MaintenanceManager::instance().Send_DisableSecurity(ch);
	}
	else
	{			
		long time_maintenance = parse_time_str(arg1);
		long duration_maintenance = parse_time_str(arg2);

		MaintenanceManager::instance().Send_ActiveMaintenance(ch, time_maintenance, duration_maintenance);
	}
}	
#endif

ACMD(do_cmd)
{
	/* RECALL_DELAY
	   if (ch->m_pkRecallEvent != NULL)
	   {
	   ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��� �Ǿ����ϴ�."));
	   event_cancel(&ch->m_pkRecallEvent);
	   return;
	   }
	// END_OF_RECALL_DELAY */

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��� �Ǿ����ϴ�."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�α��� ȭ������ ���� ���ϴ�. ��ø� ��ٸ�����."));
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ ���� �մϴ�. ��ø� ��ٸ�����."));
			break;

		case SCMD_PHASE_SELECT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ĳ���͸� ��ȯ �մϴ�. ��ø� ��ٸ�����."));
			break;
	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) &&
		false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()) &&
	   	(!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}
	
	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				{
					if (ch->IsPosition(POS_FIGHTING))
						info->left_second = 10;
					else
						info->left_second = 3;
				}

				info->ch		= ch;
				info->subcmd		= subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
	}
}

ACMD(do_mount)
{
	/*
	   char			arg1[256];
	   struct action_mount_param	param;

	// �̹� Ÿ�� ������
	if (ch->GetMountingChr())
	{
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	return;

	param.x		= atoi(arg1);
	param.y		= atoi(arg2);
	param.vid	= ch->GetMountingChr()->GetVID();
	param.is_unmount = true;

	float distance = DISTANCE_SQRT(param.x - (DWORD) ch->GetX(), param.y - (DWORD) ch->GetY());

	if (distance > 600.0f)
	{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�� �� ������ ���� ��������."));
	return;
	}

	action_enqueue(ch, ACTION_TYPE_MOUNT, &param, 0.0f, true);
	return;
	}

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	return;

	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(atoi(arg1));

	if (!tch->IsNPC() || !tch->IsMountable())
	{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ű⿡�� Ż �� �����."));
	return;
	}

	float distance = DISTANCE_SQRT(tch->GetX() - ch->GetX(), tch->GetY() - ch->GetY());

	if (distance > 600.0f)
	{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�� �� ������ ���� Ÿ����."));
	return;
	}

	param.vid		= tch->GetVID();
	param.is_unmount	= false;

	action_enqueue(ch, ACTION_TYPE_MOUNT, &param, 0.0f, true);
	 */
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

#ifdef __WORLD_BOSS_YUMA__
ACMD(do_boss_debug)
{
	CWorldBossManager::instance().GetWorldbossInfo(ch);
}
#endif

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

	if (NULL == ch->m_pkDeadEvent)
		return;

	int iTimeToDead = (event_time(ch->m_pkDeadEvent) / passes_per_sec);

	if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{
				//���� ���ϰ�쿡�� üũ ���� �ʴ´�.
				if (false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ����� �� �� �����ϴ�. (%d�� ����)"), iTimeToDead - (180 - g_nPortalLimitTime));
					return;
				}
			}

			if (iTimeToDead > 180)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ����� �� �� �����ϴ�. (%d�� ����)"), iTimeToDead - 180);
				return;
			}
		}
	}

	//PREVENT_HACK
	//DESC : â��, ��ȯ â �� ��Ż�� ����ϴ� ���׿� �̿�ɼ� �־
	//		��Ÿ���� �߰� 
	if (subcmd == SCMD_RESTART_TOWN)
	{
		if (ch->IsHack())
		{
			//����, �����ʿ����� üũ ���� �ʴ´�.
			if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) ||
			   	false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ����� �� �� �����ϴ�. (%d�� ����)"), iTimeToDead - (180 - g_nPortalLimitTime));
				return;
			}
		}

		if (iTimeToDead > 180)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� �������� ����� �� �� �����ϴ�. (%d �� ����)"), iTimeToDead - 180);
			return;
		}
	}
	//END_PREVENT_HACK

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();

	//FORKED_LOAD
	//DESC: ��Ÿ� ������ ��Ȱ�� �Ұ�� ���� �Ա��� �ƴ� ��Ÿ� ������ ������������ �̵��ϰ� �ȴ�.
	if (1 == quest::CQuestManager::instance().GetEventFlag("threeway_war"))
	{
		if (subcmd == SCMD_RESTART_TOWN || subcmd == SCMD_RESTART_HERE)
		{
			if (true == CThreeWayWar::instance().IsThreeWayWarMapIndex(ch->GetMapIndex()) &&
					false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

				ch->ReviveInvisible(5);
#ifdef WJ_EXTENDED_PET_SYSTEM
				ch->CheckPet();
#endif
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

				return;
			}

			//���� 
			if (true == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				if (CThreeWayWar::instance().GetReviveTokenForPlayer(ch->GetPlayerID()) <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�������� ��Ȱ ��ȸ�� ��� �Ҿ����ϴ�! ������ �̵��մϴ�!"));
					ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
				}
				else
				{
					ch->Show(ch->GetMapIndex(), GetSungziStartX(ch->GetEmpire()), GetSungziStartY(ch->GetEmpire()));
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);
#ifdef WJ_EXTENDED_PET_SYSTEM
				ch->CheckPet();
#endif

				return;
			}
		}
	}
	//END_FORKED_LOAD

	if (ch->GetDungeon())
		ch->GetDungeon()->UseRevive(ch);

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;

		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					sys_log(0, "do_restart: restart town");
					PIXEL_POSITION pos;

					if (CWarMapManager::instance().GetStartPosition(ch->GetMapIndex(), ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						ch->Show(ch->GetMapIndex(), pos.x, pos.y);
					else
						ch->ExitToSavedLocation();

					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef WJ_EXTENDED_PET_SYSTEM
					ch->CheckPet();
#endif
					break;

				case SCMD_RESTART_HERE:
					sys_log(0, "do_restart: restart here");
					ch->RestartAtSamePos();
					//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef WJ_EXTENDED_PET_SYSTEM
					ch->CheckPet();
#endif
					break;
			}

			return;
		}
	}

	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			sys_log(0, "do_restart: restart town");
			PIXEL_POSITION pos;

			if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
				ch->WarpSet(pos.x, pos.y);
			else
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

			ch->PointChange(POINT_HP, 50 - ch->GetHP());
			ch->DeathPenalty(1);
			break;

		case SCMD_RESTART_HERE:
			sys_log(0, "do_restart: restart here");
			ch->RestartAtSamePos();
			//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
			//ch->PointChange(POINT_HP, 50 - ch->GetHP());  // Nicht Full TP nach tod
			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->DeathPenalty(0);
			ch->ReviveInvisible(5);
#ifdef WJ_EXTENDED_PET_SYSTEM
			ch->CheckPet();
#endif
			break;
	}
}

#define MAX_STAT 90

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�а� �߿��� �ɷ��� �ø� �� �����ϴ�."));
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	int iStatUp = 1;
	if (*arg2)
		iStatUp = atoi(arg2);

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�а� �߿��� �ɷ��� �ø� �� �����ϴ�."));
		return;
	}

	if (ch->GetPoint(POINT_STAT) < iStatUp)
		iStatUp = ch->GetPoint(POINT_STAT);

	BYTE idx = 0;
	
	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	if ((ch->GetRealPoint(idx) + iStatUp) > MAX_STAT)
		iStatUp = MAX_STAT - ch->GetRealPoint(idx);

	if (iStatUp < 1)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + iStatUp);
	ch->SetPoint(idx, ch->GetPoint(idx) + iStatUp);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
		ch->PointChange(POINT_MAX_HP, 0);
	else if (idx == POINT_HT)
		ch->PointChange(POINT_MAX_SP, 0);

	ch->PointChange(POINT_STAT, -iStatUp);
	ch->ComputePoints();
}

#ifdef ENABLE_PVP_ADVANCED
#include <string>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
const char* m_nDuelTranslate[] = /* You can translate here */
{
	"[You] The money must be in numbers.",
	"[You] The amount introduced is less than 0.", 
	"[You] Entered amount is as high as the maximum possible value in inventory.",
	"[You] The amount introduced is less than that amount that you own in inventory.",
	"[You] The amount introduced + current money is greater than the maximum amount possible in inventory.",
	"[Victim] The amount introduced + current money is greater than the maximum amount possible in inventory.",
	"[Victim] The amount introduced is less than that amount that victim own in inventory.",
	"Now you're in a duel.",
	"The victim was already in a duel, you cannot send him invitation.",
	"Viewing the equipment already is BLOCKED.",
	"Viewing the equipment was BLOCKED, now no one will be able to see when you send your request for a duel.",
	"Viewing the equipment was UNBLOCKED, now all people will be able to see your equipment in duel.",
	"Viewing the equipment already is UNBLOCK."	
}; 

const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};

void Duel_SendMessage(LPCHARACTER ch, const char* msg)
{
	if (!ch)
		return;

	std::string textLine;
	
	textLine = msg;
	boost::algorithm::replace_all(textLine, " ", "$");
				 
	char buf[512+1];
	snprintf(buf, sizeof(buf), "BINARY_Duel_SendMessage %s", textLine.c_str());
	ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
}	

ACMD(do_pvp)
{
	if (!ch)
		return;
	
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����忡�� ����Ͻ� �� �����ϴ�."));
		return;
	}

	char arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256], arg9[256], arg10[256];
	
	pvp_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3), arg4, sizeof(arg4), arg5, sizeof(arg5), arg6, sizeof(arg6), arg7, sizeof(arg7), arg8, sizeof(arg8), arg9, sizeof(arg9), arg10, sizeof(arg10));	
	
	DWORD vid = 0;
	str_to_number(vid, arg1);	
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

	if (pkVictim->GetArena() != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ ������Դϴ�."));
		return;
	}
	
	if (ch->GetExchange() || pkVictim->GetExchange())
	{
		CPVPManager::instance().Decline(ch, pkVictim);
		CPVPManager::instance().Decline(pkVictim, ch);
		return;
	}
#ifdef ENABLE_NEW_KICK_HACK_FIX
	if (ch->CheckAntiFlood())
	{
		if (ch->GetDesc())
			ch->GetDesc()->DelayedDisconnect(1);
	
		return;
	}
#endif
	
	if (*arg2 && !strcmp(arg2, "accept"))
	{	
		int chA_nBetMoney = ch->GetQuestFlag(szTableStaticPvP[8]);
		int chB_nBetMoney = pkVictim->GetQuestFlag(szTableStaticPvP[8]);

		if ((ch->GetGold() < chA_nBetMoney) || (pkVictim->GetGold() < chB_nBetMoney))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "<PvP> Can't start duel because something is wrong with bet money.");
			pkVictim->ChatPacket(CHAT_TYPE_INFO, "<PvP> Can't start duel because something is wrong with bet money.");
			CPVPManager::instance().Decline(ch, pkVictim);
			CPVPManager::instance().Decline(pkVictim, ch);
			return;
		}

		ch->SetDuel("IsFight", 1);
		pkVictim->SetDuel("IsFight", 1);
		
		if (chA_nBetMoney > 0 && chA_nBetMoney > 0)
		{
			ch->PointChange(POINT_GOLD, - chA_nBetMoney, true);	
			pkVictim->PointChange(POINT_GOLD, - chB_nBetMoney, true);	
		}
		
		CPVPManager::instance().Insert(ch, pkVictim);
		return;
	}
	
	int m_BlockChangeItem = 0, m_BlockBuff = 0, m_BlockPotion = 0, m_BlockRide = 0, m_BlockPet = 0, m_BlockPoly = 0, m_BlockParty = 0, m_BlockExchange = 0, m_BetMoney = 0;
	
	str_to_number(m_BlockChangeItem, arg2);
	str_to_number(m_BlockBuff, arg3);
	str_to_number(m_BlockPotion, arg4);
	str_to_number(m_BlockRide, arg5);
	str_to_number(m_BlockPet, arg6);
	str_to_number(m_BlockPoly, arg7);
	str_to_number(m_BlockParty, arg8);
	str_to_number(m_BlockExchange, arg9);

	
	if (!isdigit(*arg2) && !isdigit(*arg3) && !isdigit(*arg4) && !isdigit(*arg5) && !isdigit(*arg6) && !isdigit(*arg7) && !isdigit(*arg8) && !isdigit(*arg9) && !isdigit(*arg10))
	{
		Duel_SendMessage(ch, m_nDuelTranslate[0]);
		return;
	}	
	
	if (m_BetMoney < 0)
	{
		Duel_SendMessage(ch, m_nDuelTranslate[1]);
		return;
	}	
	
	if (m_BetMoney >= GOLD_MAX)
	{
		Duel_SendMessage(ch, m_nDuelTranslate[2]);
		return;
	}
	
	if (ch->GetGold() < m_BetMoney)
	{
		Duel_SendMessage(ch, m_nDuelTranslate[3]);
		return;
	}
	
	if ((ch->GetGold() + m_BetMoney) > GOLD_MAX)
	{
		Duel_SendMessage(ch, m_nDuelTranslate[4]);
		return;
	}
	
	if ((pkVictim->GetGold() + m_BetMoney) > GOLD_MAX)
	{
		Duel_SendMessage(ch, m_nDuelTranslate[5]);		
		return;
	}
	
	if (pkVictim->GetGold() < m_BetMoney)
	{
		Duel_SendMessage(ch, m_nDuelTranslate[6]);
		return;
	}
	
	if (*arg1 && *arg2 && *arg3 && *arg4 && *arg5 && *arg6 && *arg7 && *arg8 && *arg9 && *arg10)
	{
		ch->SetDuel("BlockChangeItem", m_BlockChangeItem);			ch->SetDuel("BlockBuff", m_BlockBuff);
		ch->SetDuel("BlockPotion", m_BlockPotion);					ch->SetDuel("BlockRide", m_BlockRide);
		ch->SetDuel("BlockPet", m_BlockPet);						ch->SetDuel("BlockPoly", m_BlockPoly);	
		ch->SetDuel("BlockParty", m_BlockParty);					ch->SetDuel("BlockExchange", m_BlockExchange);
		ch->SetDuel("BetMoney", 0);

		pkVictim->SetDuel("BlockChangeItem", m_BlockChangeItem);	pkVictim->SetDuel("BlockBuff", m_BlockBuff);
		pkVictim->SetDuel("BlockPotion", m_BlockPotion);			pkVictim->SetDuel("BlockRide", m_BlockRide);
		pkVictim->SetDuel("BlockPet", m_BlockPet);					pkVictim->SetDuel("BlockPoly", m_BlockPoly);	
		pkVictim->SetDuel("BlockParty", m_BlockParty);				pkVictim->SetDuel("BlockExchange", m_BlockExchange);
		pkVictim->SetDuel("BetMoney", 0);
			
		CPVPManager::instance().Insert(ch, pkVictim); 
	}	
}

ACMD(do_pvp_advanced)
{   
	if (!ch)
		return;

	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����忡�� ����Ͻ� �� �����ϴ�."));
		return;
	}
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

    if (!pkVictim)
        return;

    if (pkVictim->IsNPC())
        return;

	if (pkVictim->GetArena() != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ ������Դϴ�."));
		return;
	}
	
	if (ch->GetQuestFlag(szTableStaticPvP[9]) > 0)
	{
		Duel_SendMessage(ch, m_nDuelTranslate[7]);
		return;
	}
	
	if (pkVictim->GetQuestFlag(szTableStaticPvP[9]) > 0)
	{
		Duel_SendMessage(ch, m_nDuelTranslate[8]);
		return;
	}
	
	int statusEq = pkVictim->GetQuestFlag(BLOCK_EQUIPMENT_);
#ifdef __FAKE_PLAYER__
	if (pkVictim->IsFakePlayer())
		statusEq = pkVictim->GetFakePlayerBlockEQ();
#endif	
	CGuild * g = pkVictim->GetGuild();

	const char* m_Name = pkVictim->GetName();	
	const char* m_GuildName = "-";
		
	int m_Vid = pkVictim->GetVID();	
	int m_Level = pkVictim->GetLevel();
	int m_PlayTime = pkVictim->GetRealPoint(POINT_PLAYTIME);
	int m_MaxHP = pkVictim->GetMaxHP();
	int m_MaxSP = pkVictim->GetMaxSP();
	
	DWORD m_Race = pkVictim->GetRaceNum();
	
	if (g)
	{ 
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Duel_GetInfo %d %s %s %d %d %d %d %d", m_Vid, m_Name, g->GetName(), m_Level, m_Race, m_PlayTime, m_MaxHP, m_MaxSP);
		
		if (statusEq < 1)
			pkVictim->SendEquipment(ch);
	}
	else
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Duel_GetInfo %d %s %s %d %d %d %d %d", m_Vid, m_Name, m_GuildName, m_Level, m_Race, m_PlayTime, m_MaxHP, m_MaxSP);
		
		if (statusEq < 1)
			pkVictim->SendEquipment(ch);
}

ACMD(do_decline_pvp)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;
	
	DWORD vid = 0;
	str_to_number(vid, arg1);
	
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);
	
	if (!pkVictim)
		return;
	
	if (pkVictim->IsNPC())
		return;
	
	CPVPManager::instance().Decline(ch, pkVictim);
}

ACMD(do_block_equipment)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument (argument, arg1, sizeof(arg1));
	
	if (!ch->IsPC() || NULL == ch)
		return;
	
	int statusEq = ch->GetQuestFlag(BLOCK_EQUIPMENT_);
	
	if (!strcmp(arg1, "BLOCK"))
	{	
		if (statusEq > 0)
		{	
			Duel_SendMessage(ch, m_nDuelTranslate[9]);
			return;
		}	
		else
			ch->SetQuestFlag(BLOCK_EQUIPMENT_, 1);
			Duel_SendMessage(ch, m_nDuelTranslate[10]);
	}
	
	if (!strcmp(arg1, "UNBLOCK"))
	{
		if (statusEq == 0)
		{	
			Duel_SendMessage(ch, m_nDuelTranslate[12]);
			return;
		}	
		else	
			ch->SetQuestFlag(BLOCK_EQUIPMENT_, 0);
			Duel_SendMessage(ch, m_nDuelTranslate[11]);
	}
}
#endif

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ��忡 �������� �ʽ��ϴ�."));
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ��� ��ų ������ ������ ������ �����ϴ�."));
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:
				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

//
// @version	05/06/20 Bang2ni - Ŀ�ǵ� ó�� Delegate to CHARACTER class
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
// @version	05/06/20 Bang2ni - Ŀ�ǵ� ó�� Delegate to CHARACTER class
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �߸��� ��ȣ�� �Է��ϼ̽��ϴ�."));
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �߸��� ��ȣ�� �Է��ϼ̽��ϴ�."));
		return;
	}

	if (LC_IsBrazil() == true)
	{
		for (int i = 0; i < 6; ++i)
		{
			if (arg2[i] == '\0')
				break;

			if (isalpha(arg2[i]) == false)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> ��й�ȣ�� �����ڸ� �����մϴ�."));
				return;
			}
		}
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �߸��� ��ȣ�� �Է��ϼ̽��ϴ�."));
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> â�� �̹� �����ֽ��ϴ�."));
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10) // 10�ʿ� �ѹ��� ��û ����
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> â�� ������ 10�� �ȿ��� �� �� �����ϴ�."));
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ������ ��Ƽ ���� ó���� �� �� �����ϴ�."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� �ȿ����� ��Ƽ���� ���� �� �����ϴ�."));
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ���� �����̽��ϴ�."));
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_channel_switch)
		{

			char arg1[256];
			one_argument(argument, arg1, sizeof(arg1));

			if (!*arg1)
					return;
			
			int new_ch;
			str_to_number(new_ch, arg1);
			if( new_ch <1 || new_ch >5)   // REPLACE 2 WITH YOUR MAX_CHANNEL 
				return;
			if (!ch->IsPC())
				return;
			ch->ChannelSwitch(new_ch);
		    
		}

ACMD(do_war)
{
	//�� ��� ������ ������
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	//���������� üũ�ѹ�!
	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �̹� �ٸ� ���￡ ���� �� �Դϴ�."));
		return;
	}

	//�Ķ���͸� �ι�� ������
	char arg1[256], arg2[256];
	int type = GUILD_WAR_TYPE_FIELD;
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	if (*arg2)
	{
	str_to_number(type, arg2);
 
	if (type >= GUILD_WAR_TYPE_MAX_NUM)
		type = GUILD_WAR_TYPE_FIELD;
		 
	if(type < 0)
		return;
	}

	//����� ������ ���̵� ���µ�
	DWORD gm_pid = g->GetMasterPID();

	//���������� üũ(������ ����常�� ����)
	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������� ���� ������ �����ϴ�."));
		return;
	}

	//��� ��带 ������
	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �׷� ��尡 �����ϴ�."));
		return;
	}

	//�������� ���� üũ
	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ��尡 �̹� ���� �� �Դϴ�."));
					return;
				}

				int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

				if (g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� �����Ͽ� ������� �� �� �����ϴ�."));
					return;
				}

				if (opp_g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ����� ���� �����Ͽ� ������� �� �� �����ϴ�."));
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�̹� �������� ���� ����Դϴ�."));
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ��尡 �̹� ���� �� �Դϴ�."));
					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �̹� ������ ����� ��� �Դϴ�."));
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �̹� ���� ���� ����Դϴ�."));
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		// ������� �� �� �ִ� ������ ���������ʴ´�.
		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ������ ���ڶ� ������� �� �� �����ϴ�."));
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������� �ϱ� ���ؼ� �ּ��� %d���� �־�� �մϴ�."), GUILD_WAR_MIN_MEMBER_COUNT);
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	// �ʵ��� üũ�� �ϰ� ������ üũ�� ������ �³��Ҷ� �Ѵ�.
	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ����� ���� ������ ���ڶ� ������� �� �� �����ϴ�."));
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ����� ���� ���� �����Ͽ� ������� �� �� �����ϴ�."));
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ����� ������� �������� �ƴմϴ�."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;
		
		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());
		
		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ����� ������� �������� �ƴմϴ�."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������� ���� ������ �����ϴ�."));
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �׷� ��尡 �����ϴ�."));
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

	ch->SetPKMode(mode);
}


// Vor neuen Kickhack fix

//ACMD(do_messenger_auth)
//{
//	if (ch->GetArena())
//	{
//		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����忡�� ����Ͻ� �� �����ϴ�."));
//		return;
//	}
//
//	char arg1[256], arg2[256];
//	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
//
//	if (!*arg1 || !*arg2)
//		return;
//
//	char answer = LOWER(*arg1);
//
//	if (answer != 'y')
//	{
//		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);
//
//		if (tch)
//			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ������ ���� ģ�� ����� �ź� ���߽��ϴ�."), ch->GetName());
//	}
//
//	MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, answer == 'y' ? false : true); // DENY
//}

#ifdef ENABLE_NEW_FLOOD_KICK_HACK_FIX_NR2
ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��e��AAa������ ��c��eCI��C ��o ������A��I��U."));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);
	bool bIsDenied = answer != 'y';
	bool bIsAdded = MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, bIsDenied); // DENY
	if (bIsAdded && bIsDenied)
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ��OA����I ��IAI A�̡��� ��i��IA�� ��A��I ��cC����A��I��U."), ch->GetName());
	}

}
#endif


ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
	if (true == ch->UnEquipSpecialRideUniqueItem())
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsHorseRiding())
		{
			ch->StopRiding(); 
		}
	}
	else
	{
		ch->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("�κ��丮�� �� ���� ���� �� �����ϴ�."));
	}

}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{
	if (ch->GetGMLevel() <= GM_PLAYER)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
			return;

		if (!tch->IsPC()
#ifdef __FAKE_PLAYER__
			&& !tch->IsFakePlayer()
#endif				
		)
			return;
		/*
		   int iSPCost = ch->GetMaxSP() / 3;

		   if (ch->GetSP() < iSPCost)
		   {
		   ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���ŷ��� �����Ͽ� �ٸ� ����� ��� �� �� �����ϴ�."));
		   return;
		   }
		   ch->PointChange(POINT_SP, -iSPCost);
		 */
		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����忡�� ����Ͻ� �� �����ϴ�."));
		return;
	}

	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�̹� ��Ƽ�� ���� �����Ƿ� ���Խ�û�� �� �� �����ϴ�."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

ACMD(do_monarch_warpto)
{
	if (true == LC_IsYMIR() || true == LC_IsKorea())
		return;

	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���ָ��� ��� ������ ����Դϴ�"));
		return;
	}
	
	//���� ��Ÿ�� �˻�
	if (!ch->IsMCOK(CHARACTER::MI_WARP))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d �ʰ� ��Ÿ���� �������Դϴ�."), ch->GetMCLTime(CHARACTER::MI_WARP));
		return;
	}

	//���� �� ��ȯ ��� 
	const int WarpPrice = 10000;
	
	//���� ���� �˻� 
	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ���� �����մϴ�. ���� : %u �ʿ�ݾ� : %u"), NationMoney, WarpPrice);
		return;	
	}

	int x = 0, y = 0;
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����: warpto <character name>"));
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("Ÿ���� �������Դ� �̵��Ҽ� �����ϴ�"));
				return;
			}

			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ش� ������ %d ä�ο� �ֽ��ϴ�. (���� ä�� %d)"), pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("�ش� �������� �̵��� �� �����ϴ�."));
				return;
			}

			PIXEL_POSITION pos;
	
			if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map (index %d)", pkCCI->lMapIndex);
			else
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, "You warp to (%d, %d)", pos.x, pos.y);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ���Է� �̵��մϴ�"), arg1);
				ch->WarpSet(pos.x, pos.y);
				
				//���� �� �谨	
				CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

				//��Ÿ�� �ʱ�ȭ 
				ch->SetMC(CHARACTER::MI_WARP);
			}
		}
		else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no one by that name");
		}

		return;
	}
	else
	{
		if (tch->GetEmpire() != ch->GetEmpire())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ÿ���� �������Դ� �̵��Ҽ� �����ϴ�"));
			return;
		}
		if (!IsMonarchWarpZone(tch->GetMapIndex()))
		{
			ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("�ش� �������� �̵��� �� �����ϴ�."));
			return;
		}
		x = tch->GetX();
		y = tch->GetY();
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ���Է� �̵��մϴ�"), arg1);
	ch->WarpSet(x, y);
	ch->Stop();

	//���� �� �谨	
	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	//��Ÿ�� �ʱ�ȭ 
	ch->SetMC(CHARACTER::MI_WARP);
}

ACMD(do_monarch_transfer)
{
	if (true == LC_IsYMIR() || true == LC_IsKorea())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����: transfer <name>"));
		return;
	}
	
	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���ָ��� ��� ������ ����Դϴ�"));
		return;
	}
	
	//���� ��Ÿ�� �˻�
	if (!ch->IsMCOK(CHARACTER::MI_TRANSFER))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d �ʰ� ��Ÿ���� �������Դϴ�."), ch->GetMCLTime(CHARACTER::MI_TRANSFER));	
		return;
	}

	//���� ���� ��� 
	const int WarpPrice = 10000;

	//���� ���� �˻� 
	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ���� �����մϴ�. ���� : %u �ʿ�ݾ� : %u"), NationMoney, WarpPrice);
		return;	
	}


	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ٸ� ���� ������ ��ȯ�� �� �����ϴ�."));
				return;
			}
			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ���� %d ä�ο� ���� �� �Դϴ�. (���� ä��: %d)"), arg1, pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("�ش� �������� �̵��� �� �����ϴ�."));
				return;
			}
			if (!IsMonarchWarpZone(ch->GetMapIndex()))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("�ش� �������� ��ȯ�� �� �����ϴ�."));
				return;
			}

			TPacketGGTransfer pgg;

			pgg.bHeader = HEADER_GG_TRANSFER;
			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			pgg.lX = ch->GetX();
			pgg.lY = ch->GetY();

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGTransfer));
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ���� ��ȯ�Ͽ����ϴ�."), arg1);
			
			//���� �� �谨	
			CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);
			//��Ÿ�� �ʱ�ȭ 
			ch->SetMC(CHARACTER::MI_TRANSFER);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�Է��Ͻ� �̸��� ���� ����ڰ� �����ϴ�."));
		}

		return;
	}


	if (ch == tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ڽ��� ��ȯ�� �� �����ϴ�."));
		return;
	}

	if (tch->GetEmpire() != ch->GetEmpire())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ٸ� ���� ������ ��ȯ�� �� �����ϴ�."));
		return;
	}
	if (!IsMonarchWarpZone(tch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("�ش� �������� �̵��� �� �����ϴ�."));
		return;
	}
	if (!IsMonarchWarpZone(ch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("�ش� �������� ��ȯ�� �� �����ϴ�."));
		return;
	}

	//tch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
	tch->WarpSet(ch->GetX(), ch->GetY(), ch->GetMapIndex());
	
	//���� �� �谨	
	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);
	//��Ÿ�� �ʱ�ȭ 
	ch->SetMC(CHARACTER::MI_TRANSFER);
}

ACMD(do_monarch_info)
{
	if (CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))	
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ���� ����"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			if (n == ch->GetEmpire())
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s����] : %s  �����ݾ� %lld "), EMPIRE_NAME(n), p->name[n], p->money[n]);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s����] : %s  "), EMPIRE_NAME(n), p->name[n]);
				
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ����"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s����] : %s  "), EMPIRE_NAME(n), p->name[n]);
				
		}
	}
	
}	

ACMD(do_elect)
{
	db_clientdesc->DBPacketHeader(HEADER_GD_COME_TO_VOTE, ch->GetDesc()->GetHandle(), 0);
}

// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

extern void BroadcastNotice(const char * c_pszBuf);

ACMD(do_monarch_tax)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: monarch_tax <1-50>");
		return;
	}

	// ���� �˻�	
	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���ָ��� ����Ҽ� �ִ� ����Դϴ�"));
		return;
	}

	// ���ݼ��� 
	int tax = 0;
	str_to_number(tax,  arg1);

	if (tax < 1 || tax > 50)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1-50 ������ ��ġ�� �������ּ���"));

	quest::CQuestManager::instance().SetEventFlag("trade_tax", tax); 

	// ���ֿ��� �޼��� �ϳ�
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ %d %�� �����Ǿ����ϴ�"));

	// ���� 
	char szMsg[1024];	

	snprintf(szMsg, sizeof(szMsg), "������ ������ ������ %d %% �� ����Ǿ����ϴ�", tax);
	BroadcastNotice(szMsg);

	snprintf(szMsg, sizeof(szMsg), "�����δ� �ŷ� �ݾ��� %d %% �� ����� ���Ե˴ϴ�.", tax);
	BroadcastNotice(szMsg);

	// ��Ÿ�� �ʱ�ȭ 
	ch->SetMC(CHARACTER::MI_TAX); 
}

static const DWORD cs_dwMonarchMobVnums[] =
{
	191, //	��߽�
	192, //	����
	193, //	����
	194, //	ȣ��
	391, //	����
	392, //	����
	393, //	����
	394, //	����
	491, //	��ȯ
	492, //	����
	493, //	����
	494, //	����
	591, //	����ܴ���
	691, //	���� ����
	791, //	�б�����
	1304, // ��������
	1901, // ����ȣ
	2091, // ���հŹ�
	2191, // �Ŵ�縷�ź�
	2206, // ȭ����i
	0,
};

ACMD(do_monarch_mob)
{
	char arg1[256];
	LPCHARACTER	tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���ָ��� ����Ҽ� �ִ� ����Դϴ�"));
		return;
	}
	
	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mmob <mob name>");
		return;
	}

	BYTE pcEmpire = ch->GetEmpire();
	BYTE mapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(ch->GetMapIndex());

	if (LC_IsYMIR() == true || LC_IsKorea() == true)
	{
		if (mapEmpire != pcEmpire && mapEmpire != 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ڱ� ���信���� ����� �� �ִ� ����Դϴ�"));
			return;
		}
	}

	// ���� �� ��ȯ ��� 
	const int SummonPrice = 5000000;

	// ���� ��Ÿ�� �˻�
	if (!ch->IsMCOK(CHARACTER::MI_SUMMON))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d �ʰ� ��Ÿ���� �������Դϴ�."), ch->GetMCLTime(CHARACTER::MI_SUMMON));	
		return;
	}
	
	// ���� ���� �˻� 
	if (!CMonarch::instance().IsMoneyOk(SummonPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ���� �����մϴ�. ���� : %u �ʿ�ݾ� : %u"), NationMoney, SummonPrice);
		return;	
	}

	const CMob * pkMob;
	DWORD vnum = 0;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	DWORD count;

	// ��ȯ ���� �� �˻�
	for (count = 0; cs_dwMonarchMobVnums[count] != 0; ++count)
		if (cs_dwMonarchMobVnums[count] == vnum)
			break;

	if (0 == cs_dwMonarchMobVnums[count])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��ȯ�Ҽ� ���� ���� �Դϴ�. ��ȯ������ ���ʹ� Ȩ�������� �����ϼ���"));
		return;
	}

	tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum, 
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750), 
			ch->GetY() - number(200, 750), 
			ch->GetX() + number(200, 750), 
			ch->GetY() + number(200, 750), 
			true,
			pkMob->m_table.bType == CHAR_TYPE_STONE,
			true);

	if (tch)
	{
		// ���� �� �谨	
		CMonarch::instance().SendtoDBDecMoney(SummonPrice, ch->GetEmpire(), ch);

		// ��Ÿ�� �ʱ�ȭ 
		ch->SetMC(CHARACTER::MI_SUMMON); 
	}
}

static const char* FN_point_string(int apply_number)
{
	switch (apply_number)
	{
		case POINT_MAX_HP:	return LC_TEXT("�ִ� ����� +%d");
		case POINT_MAX_SP:	return LC_TEXT("�ִ� ���ŷ� +%d");
		case POINT_HT:		return LC_TEXT("ü�� +%d");
		case POINT_IQ:		return LC_TEXT("���� +%d");
		case POINT_ST:		return LC_TEXT("�ٷ� +%d");
		case POINT_DX:		return LC_TEXT("��ø +%d");
		case POINT_ATT_SPEED:	return LC_TEXT("���ݼӵ� +%d");
		case POINT_MOV_SPEED:	return LC_TEXT("�̵��ӵ� %d");
		case POINT_CASTING_SPEED:	return LC_TEXT("��Ÿ�� -%d");
		case POINT_HP_REGEN:	return LC_TEXT("����� ȸ�� +%d");
		case POINT_SP_REGEN:	return LC_TEXT("���ŷ� ȸ�� +%d");
		case POINT_POISON_PCT:	return LC_TEXT("������ %d");
		case POINT_STUN_PCT:	return LC_TEXT("���� +%d");
		case POINT_SLOW_PCT:	return LC_TEXT("���ο� +%d");
		case POINT_CRITICAL_PCT:	return LC_TEXT("%d%% Ȯ���� ġ��Ÿ ����");
		case POINT_RESIST_CRITICAL:	return LC_TEXT("����� ġ��Ÿ Ȯ�� %d%% ����");
		case POINT_PENETRATE_PCT:	return LC_TEXT("%d%% Ȯ���� ���� ����");
		case POINT_RESIST_PENETRATE: return LC_TEXT("����� ���� ���� Ȯ�� %d%% ����");
		case POINT_ATTBONUS_HUMAN:	return LC_TEXT("�ΰ��� ���� Ÿ��ġ +%d%%");
		case POINT_ATTBONUS_ANIMAL:	return LC_TEXT("������ ���� Ÿ��ġ +%d%%");
		case POINT_ATTBONUS_ORC:	return LC_TEXT("������ Ÿ��ġ +%d%%");
		case POINT_ATTBONUS_MILGYO:	return LC_TEXT("�б��� Ÿ��ġ +%d%%");
		case POINT_ATTBONUS_UNDEAD:	return LC_TEXT("��ü�� Ÿ��ġ +%d%%");
		case POINT_ATTBONUS_DEVIL:	return LC_TEXT("�Ǹ��� Ÿ��ġ +%d%%");
		case POINT_STEAL_HP:		return LC_TEXT("Ÿ��ġ %d%% �� ��������� ���");
		case POINT_STEAL_SP:		return LC_TEXT("Ÿ��ġ %d%% �� ���ŷ����� ���");
		case POINT_MANA_BURN_PCT:	return LC_TEXT("%d%% Ȯ���� Ÿ�ݽ� ��� ���ŷ� �Ҹ�");
		case POINT_DAMAGE_SP_RECOVER:	return LC_TEXT("%d%% Ȯ���� ���ؽ� ���ŷ� ȸ��");
		case POINT_BLOCK:			return LC_TEXT("����Ÿ�ݽ� �� Ȯ�� %d%%");
		case POINT_DODGE:			return LC_TEXT("Ȱ ���� ȸ�� Ȯ�� %d%%");
		case POINT_RESIST_SWORD:	return LC_TEXT("�Ѽհ� ��� %d%%");
		case POINT_RESIST_TWOHAND:	return LC_TEXT("��հ� ��� %d%%");
		case POINT_RESIST_DAGGER:	return LC_TEXT("�μհ� ��� %d%%");
		case POINT_RESIST_BELL:		return LC_TEXT("��� ��� %d%%");
		case POINT_RESIST_FAN:		return LC_TEXT("��ä ��� %d%%");
		case POINT_RESIST_BOW:		return LC_TEXT("Ȱ���� ���� %d%%");
		case POINT_RESIST_FIRE:		return LC_TEXT("ȭ�� ���� %d%%");
		case POINT_RESIST_ELEC:		return LC_TEXT("���� ���� %d%%");
		case POINT_RESIST_MAGIC:	return LC_TEXT("���� ���� %d%%");
		case POINT_RESIST_WIND:		return LC_TEXT("�ٶ� ���� %d%%");
		case POINT_RESIST_ICE:		return LC_TEXT("�ñ� ���� %d%%");
		case POINT_RESIST_EARTH:	return LC_TEXT("���� ���� %d%%");
		case POINT_RESIST_DARK:		return LC_TEXT("��� ���� %d%%");
		case POINT_REFLECT_MELEE:	return LC_TEXT("���� Ÿ��ġ �ݻ� Ȯ�� : %d%%");
		case POINT_REFLECT_CURSE:	return LC_TEXT("���� �ǵ����� Ȯ�� %d%%");
		case POINT_POISON_REDUCE:	return LC_TEXT("�� ���� %d%%");
		case POINT_KILL_SP_RECOVER:	return LC_TEXT("%d%% Ȯ���� ����ġ�� ���ŷ� ȸ��");
		case POINT_EXP_DOUBLE_BONUS:	return LC_TEXT("%d%% Ȯ���� ����ġ�� ����ġ �߰� ���");
		case POINT_GOLD_DOUBLE_BONUS:	return LC_TEXT("%d%% Ȯ���� ����ġ�� �� 2�� ���");
		case POINT_ITEM_DROP_BONUS:	return LC_TEXT("%d%% Ȯ���� ����ġ�� ������ 2�� ���");
		case POINT_POTION_BONUS:	return LC_TEXT("���� ���� %d%% ���� ����");
		case POINT_KILL_HP_RECOVERY:	return LC_TEXT("%d%% Ȯ���� ����ġ�� ����� ȸ��");
//		case POINT_IMMUNE_STUN:	return LC_TEXT("�������� ���� %d%%");
//		case POINT_IMMUNE_SLOW:	return LC_TEXT("�������� ���� %d%%");
//		case POINT_IMMUNE_FALL:	return LC_TEXT("�Ѿ����� ���� %d%%");
//		case POINT_SKILL:	return LC_TEXT("");
//		case POINT_BOW_DISTANCE:	return LC_TEXT("");
		case POINT_ATT_GRADE_BONUS:	return LC_TEXT("���ݷ� +%d");
		case POINT_DEF_GRADE_BONUS:	return LC_TEXT("���� +%d");
		case POINT_MAGIC_ATT_GRADE:	return LC_TEXT("���� ���ݷ� +%d");
		case POINT_MAGIC_DEF_GRADE:	return LC_TEXT("���� ���� +%d");
//		case POINT_CURSE_PCT:	return LC_TEXT("");
		case POINT_MAX_STAMINA:	return LC_TEXT("�ִ� ������ +%d");
		case POINT_ATTBONUS_WARRIOR:	return LC_TEXT("���翡�� ���� +%d%%");
		case POINT_ATTBONUS_ASSASSIN:	return LC_TEXT("�ڰ����� ���� +%d%%");
		case POINT_ATTBONUS_SURA:		return LC_TEXT("���󿡰� ���� +%d%%");
		case POINT_ATTBONUS_SHAMAN:		return LC_TEXT("���翡�� ���� +%d%%");
		case POINT_ATTBONUS_MONSTER:	return LC_TEXT("���Ϳ��� ���� +%d%%");
		case POINT_MALL_ATTBONUS:		return LC_TEXT("���ݷ� +%d%%");
		case POINT_MALL_DEFBONUS:		return LC_TEXT("���� +%d%%");
		case POINT_MALL_EXPBONUS:		return LC_TEXT("����ġ %d%%");
		case POINT_MALL_ITEMBONUS:		return LC_TEXT("������ ����� %.1f��");
		case POINT_MALL_GOLDBONUS:		return LC_TEXT("�� ����� %.1f��");
		case POINT_MAX_HP_PCT:			return LC_TEXT("�ִ� ����� +%d%%");
		case POINT_MAX_SP_PCT:			return LC_TEXT("�ִ� ���ŷ� +%d%%");
		case POINT_SKILL_DAMAGE_BONUS:	return LC_TEXT("��ų ������ %d%%");
		case POINT_NORMAL_HIT_DAMAGE_BONUS:	return LC_TEXT("��Ÿ ������ %d%%");
		case POINT_SKILL_DEFEND_BONUS:		return LC_TEXT("��ų ������ ���� %d%%");
		case POINT_NORMAL_HIT_DEFEND_BONUS:	return LC_TEXT("��Ÿ ������ ���� %d%%");
//		case POINT_PC_BANG_EXP_BONUS:	return LC_TEXT("");
//		case POINT_PC_BANG_DROP_BONUS:	return LC_TEXT("");
//		case POINT_EXTRACT_HP_PCT:	return LC_TEXT("");
		case POINT_RESIST_WARRIOR:	return LC_TEXT("������ݿ� %d%% ����");
		case POINT_RESIST_ASSASSIN:	return LC_TEXT("�ڰ����ݿ� %d%% ����");
		case POINT_RESIST_SURA:		return LC_TEXT("������ݿ� %d%% ����");
		case POINT_RESIST_SHAMAN:	return LC_TEXT("������ݿ� %d%% ����");
		default:					return NULL;
	}
}

static bool FN_hair_affect_string(LPCHARACTER ch, char *buf, size_t bufsiz)
{
	if (NULL == ch || NULL == buf)
		return false;

	CAffect* aff = NULL;
	time_t expire = 0;
	struct tm ltm;
	int	year, mon, day;
	int	offset = 0;

	aff = ch->FindAffect(AFFECT_HAIR);

	if (NULL == aff)
		return false;

	expire = ch->GetQuestFlag("hair.limit_time");

	if (expire < get_global_time())
		return false;

	// set apply string
	offset = snprintf(buf, bufsiz, FN_point_string(aff->bApplyOn), aff->lApplyValue);

	if (offset < 0 || offset >= (int) bufsiz)
		offset = bufsiz - 1;

	localtime_r(&expire, &ltm);

	year	= ltm.tm_year + 1900;
	mon		= ltm.tm_mon + 1;
	day		= ltm.tm_mday;

	snprintf(buf + offset, bufsiz - offset, LC_TEXT(" (������ : %d�� %d�� %d��)"), year, mon, day);

	return true;
}

ACMD(do_costume)
{
	#ifdef __SASH_SYSTEM__
	char buf[768];
	#else
	char buf[512];
	#endif
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	CItem * pBody = ch->GetWear(WEAR_COSTUME_BODY);
	CItem * pHair = ch->GetWear(WEAR_COSTUME_HAIR);
	#ifdef __SASH_SYSTEM__
	CItem * pSash = ch->GetWear(WEAR_COSTUME_SASH);
	#endif
	ch->ChatPacket(CHAT_TYPE_INFO, "COSTUME status:");
	if (pBody)
	{
		const char* itemName = pBody->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  BODY: %s", itemName);
		
		if (pBody->IsEquipped() && arg1[0] == 'b')
			ch->UnequipItem(pBody);
	}
	
	if (pHair)
	{
		const char* itemName = pHair->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  HAIR: %s", itemName);

		for (int i = 0; i < pHair->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pHair->GetAttribute(i);
			if (attr.bType > 0)
			{
				const char * pAttrName = FN_point_string(attr.bType);
				if (pAttrName == NULL)
					continue;
				
				snprintf(buf, sizeof(buf), FN_point_string(attr.bType), attr.sValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pHair->IsEquipped() && arg1[0] == 'h')
			ch->UnequipItem(pHair);
	}
	
#ifdef __SASH_SYSTEM__
	if (pSash)
	{
		const char * itemName = pSash->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  SASH: %s", itemName);
		for (int i = 0; i < pSash->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pSash->GetAttribute(i);
			if (attr.bType > 0)
			{
				const char * pAttrName = FN_point_string(attr.bType);
				if (pAttrName == NULL)
					continue;
				
				snprintf(buf, sizeof(buf), FN_point_string(attr.bType), attr.sValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pSash->IsEquipped() && arg1[0] == 's')
			ch->UnequipItem(pSash);
	}
#endif
#ifdef __AURA_SYSTEM__
	CItem* pAura = ch->GetWear(WEAR_COSTUME_AURA);
	if (pAura)
	{
		const char* itemName = pAura->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  AURA : %s", itemName);
		if (pAura->IsEquipped() && arg1[0] == 'a')
			ch->UnequipItem(pAura);
	}
#endif

}

ACMD(do_hair)
{
	char buf[256];

	if (false == FN_hair_affect_string(ch, buf, sizeof(buf)))
		return;

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s",
						index, item ? item->GetName() : "<NONE>");
		++index;
	}
}

//gift notify quest command
ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

#ifdef ENABLE_CUBE_RENEWAL
ACMD(do_cube)
{

	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		default:
			return;
	}
}
#else
ACMD(do_cube)
{
	if (!ch->CanDoCube())
		return;

	dev_log(LOG_DEB0, "CUBE COMMAND <%s>: %s", ch->GetName(), argument);
	int cube_index = 0, inven_index = 0;
	const char *line;

	int inven_type = 0;
	
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		// print usage
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: cube open");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube close");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube add <inveltory_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube delete <cube_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube list");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube cancel");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube make [all]");
		return;
	}

	const std::string& strArg1 = std::string(arg1);

	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		case 'c':	// close
			Cube_close(ch);
			break;

		case 'l':	// list
			Cube_show_list(ch);
			break;

		case 'a':	// add cue_index inven_index
			{
				if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
					return;

				str_to_number(cube_index, arg2);
				str_to_number(inven_index, arg3);
				Cube_add_item (ch, cube_index, inven_index);
			}
			break;

		case 'd':	// delete
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;

				str_to_number(cube_index, arg2);
				Cube_delete_item (ch, cube_index);
			}
			break;

		case 'm':	// make
			if (0 != arg2[0])
			{
				while (true == Cube_make(ch))
					dev_log (LOG_DEB0, "cube make success");
			}
			else
				Cube_make(ch);
			break;

		default:
			return;
	}
}
#endif

ACMD(do_in_game_mall)
{
	ch->ChatPacket(CHAT_TYPE_INFO, "Visit our homepage: https://elendos.shop/ to use the itemshop.");
	return;
		
	if (LC_IsYMIR() == true || LC_IsKorea() == true)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://metin2.co.kr/04_mall/mall/login.htm");
		return;
	}

	if (true == LC_IsTaiwan())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://203.69.141.203/mall/mall/item_main.htm");
		return;
	}

	// ��_�� �赵���� �����۸� URL �ϵ��ڵ� �߰�
	if (true == LC_IsWE_Korea())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://metin2.co.kr/50_we_mall/mall/login.htm");
		return;
	}

	if (LC_IsJapan() == true)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://mt2.oge.jp/itemmall/itemList.php");
		return;
	}
	
	if (LC_IsNewCIBN() == true && test_server)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://218.99.6.51/04_mall/mall/login.htm");
		return;
	}

	if (LC_IsSingapore() == true)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://www.metin2.sg/ishop.php");
		return;
	}	
	
	/*
	if (LC_IsCanada() == true)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "mall http://mall.z8games.com/mall_entry.aspx?tb=m2");
		return;
	}*/

	if (LC_IsEurope() == true)
	{
		char country_code[3];

		switch (LC_GetLocalType())
		{
			case LC_GERMANY:	country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_FRANCE:		country_code[0] = 'f'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_ITALY:		country_code[0] = 'i'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_SPAIN:		country_code[0] = 'e'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_UK:			country_code[0] = 'e'; country_code[1] = 'n'; country_code[2] = '\0'; break;
			case LC_TURKEY:		country_code[0] = 't'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_POLAND:		country_code[0] = 'p'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_PORTUGAL:	country_code[0] = 'p'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_GREEK:		country_code[0] = 'g'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_RUSSIA:		country_code[0] = 'r'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_DENMARK:	country_code[0] = 'd'; country_code[1] = 'k'; country_code[2] = '\0'; break;
			case LC_BULGARIA:	country_code[0] = 'b'; country_code[1] = 'g'; country_code[2] = '\0'; break;
			case LC_CROATIA:	country_code[0] = 'h'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_MEXICO:		country_code[0] = 'm'; country_code[1] = 'x'; country_code[2] = '\0'; break;
			case LC_ARABIA:		country_code[0] = 'a'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_CZECH:		country_code[0] = 'c'; country_code[1] = 'z'; country_code[2] = '\0'; break;
			case LC_ROMANIA:	country_code[0] = 'r'; country_code[1] = 'o'; country_code[2] = '\0'; break;
			case LC_HUNGARY:	country_code[0] = 'h'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_NETHERLANDS: country_code[0] = 'n'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_USA:		country_code[0] = 'u'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_CANADA:	country_code[0] = 'c'; country_code[1] = 'a'; country_code[2] = '\0'; break;
			default:
				if (test_server == true)
				{
					country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0';
				}
				break;
		}

		char buf[512 + 1];
		char sas[33];
		MD5_CTX ctx;
		const char sas_key[] = "GF9001";

		snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

		MD5Init(&ctx);
		MD5Update(&ctx, (const unsigned char *)buf, strlen(buf));
#ifdef __FreeBSD__
		MD5End(&ctx, sas);
#else
		static const char hex[] = "0123456789abcdef";
		unsigned char digest[16];
		MD5Final(digest, &ctx);
		int i;
		for (i = 0; i < 16; ++i) {
			sas[i + i] = hex[digest[i] >> 4];
			sas[i + i + 1] = hex[digest[i] & 0x0f];
		}
		sas[i + i] = '\0';
#endif

		snprintf(buf, sizeof(buf), "mall https://%s/ishop?pid=%u&c=%s&sid=%d&sas=%s&type=game",
			g_strWebMallURL.c_str(), ch->GetPlayerID(), country_code, g_server_id, sas);

		ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
	}
}

// �ֻ���
ACMD(do_dice) 
{
	ch->ChatPacket(CHAT_TYPE_INFO, "Captain Fliege m?hte das nicht!");
}

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

ACMD(do_ride)
{
    dev_log(LOG_DEB0, "[DO_RIDE] start");
    if (ch->IsDead() || ch->IsStun())
	return;

    // ������
    {
	if (ch->IsHorseRiding())
	{
	    dev_log(LOG_DEB0, "[DO_RIDE] stop riding");
	    ch->StopRiding(); 
	    return;
	}

	if (ch->GetMountVnum())
	{
	    dev_log(LOG_DEB0, "[DO_RIDE] unmount");
	    do_unmount(ch, NULL, 0, 0);
	    return;
	}
    }

    // Ÿ��
    {
	if (ch->GetHorse() != NULL)
	{
	    dev_log(LOG_DEB0, "[DO_RIDE] start riding");
	    ch->StartRiding();
	    return;
	}

	for (UINT i=0; i<INVENTORY_MAX_NUM; ++i)
	{
	    LPITEM item = ch->GetInventoryItem(i);
	    if (NULL == item)
		continue;

	    // ����ũ Ż�� ������
		if (item->IsRideItem())
		{
			if (NULL==ch->GetWear(WEAR_UNIQUE1) || NULL==ch->GetWear(WEAR_UNIQUE2))
			{
				dev_log(LOG_DEB0, "[DO_RIDE] USE UNIQUE ITEM");
				//ch->EquipItem(item);
				ch->UseItem(TItemPos (INVENTORY, i));
				return;
			}
		}

	    // �Ϲ� Ż�� ������
	    // TODO : Ż�Ϳ� SubType �߰�
	    switch (item->GetVnum())
	    {
		case 71114:	// �����̿��
		case 71116:	// ��߽��̿��
		case 71118:	// �������̿��
		case 71120:	// ���ڿ��̿��
		    dev_log(LOG_DEB0, "[DO_RIDE] USE QUEST ITEM");
		    ch->UseItem(TItemPos (INVENTORY, i));
		    return;
	    }

		// GF mantis #113524, 52001~52090 �� Ż��
		if( (item->GetVnum() > 52000) && (item->GetVnum() < 52091) )	{
			dev_log(LOG_DEB0, "[DO_RIDE] USE QUEST ITEM");
			ch->UseItem(TItemPos (INVENTORY, i));
		    return;
		}
	}
    }


    // Ÿ�ų� ���� �� ������
    ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ���� ��ȯ���ּ���."));
}

#ifdef __AUCTION__
// temp_auction
ACMD(do_get_item_id_list)
{
	for (int i = 0; i < INVENTORY_MAX_NUM; i++)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (item != NULL)
			ch->ChatPacket(CHAT_TYPE_INFO, "name : %s id : %d", item->GetProto()->szName, item->GetID());
	}
}

// temp_auction

ACMD(do_enroll_auction)
{
	char arg1[256];
	char arg2[256];
	char arg3[256];
	char arg4[256];
	two_arguments (two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3), arg4, sizeof(arg4));
	
	DWORD item_id = strtoul(arg1, NULL, 10);
	BYTE empire = strtoul(arg2, NULL, 10);
	int bidPrice = strtol(arg3, NULL, 10);
	int immidiatePurchasePrice = strtol(arg4, NULL, 10);

	LPITEM item = ITEM_MANAGER::instance().Find(item_id);
	if (item == NULL)
		return;

	AuctionManager::instance().enroll_auction(ch, item, empire, bidPrice, immidiatePurchasePrice);
}

ACMD(do_enroll_wish)
{
	char arg1[256];
	char arg2[256];
	char arg3[256];
	one_argument (two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3));
	
	DWORD item_num = strtoul(arg1, NULL, 10);
	BYTE empire = strtoul(arg2, NULL, 10);
	int wishPrice = strtol(arg3, NULL, 10);

	AuctionManager::instance().enroll_wish(ch, item_num, empire, wishPrice);
}

ACMD(do_enroll_sale)
{
	char arg1[256];
	char arg2[256];
	char arg3[256];
	one_argument (two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3));
	
	DWORD item_id = strtoul(arg1, NULL, 10);
	DWORD wisher_id = strtoul(arg2, NULL, 10);
	int salePrice = strtol(arg3, NULL, 10);

	LPITEM item = ITEM_MANAGER::instance().Find(item_id);
	if (item == NULL)
		return;

	AuctionManager::instance().enroll_sale(ch, item, wisher_id, salePrice);
}

// temp_auction
// packet���� ����ϰ� �ϰ�, �̰� �����ؾ��Ѵ�.
ACMD(do_get_auction_list)
{
	char arg1[256];
	char arg2[256];
	char arg3[256];
	two_arguments (one_argument (argument, arg1, sizeof(arg1)), arg2, sizeof(arg2), arg3, sizeof(arg3));

	AuctionManager::instance().get_auction_list (ch, strtoul(arg1, NULL, 10), strtoul(arg2, NULL, 10), strtoul(arg3, NULL, 10));
}
//
//ACMD(do_get_wish_list)
//{
//	char arg1[256];
//	char arg2[256];
//	char arg3[256];
//	two_arguments (one_argument (argument, arg1, sizeof(arg1)), arg2, sizeof(arg2), arg3, sizeof(arg3));
//
//	AuctionManager::instance().get_wish_list (ch, strtoul(arg1, NULL, 10), strtoul(arg2, NULL, 10), strtoul(arg3, NULL, 10));
//}
ACMD (do_get_my_auction_list)
{
	char arg1[256];
	char arg2[256];
	two_arguments (argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	AuctionManager::instance().get_my_auction_list (ch, strtoul(arg1, NULL, 10), strtoul(arg2, NULL, 10));
}

ACMD (do_get_my_purchase_list)
{
	char arg1[256];
	char arg2[256];
	two_arguments (argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	AuctionManager::instance().get_my_purchase_list (ch, strtoul(arg1, NULL, 10), strtoul(arg2, NULL, 10));
}

ACMD (do_auction_bid)
{
	char arg1[256];
	char arg2[256];
	two_arguments (argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	AuctionManager::instance().bid (ch, strtoul(arg1, NULL, 10), strtoul(arg2, NULL, 10));
}

ACMD (do_auction_impur)
{
	char arg1[256];
	one_argument (argument, arg1, sizeof(arg1));

	AuctionManager::instance().immediate_purchase (ch, strtoul(arg1, NULL, 10));
}

ACMD (do_get_auctioned_item)
{
	char arg1[256];
	char arg2[256];
	two_arguments (argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	AuctionManager::instance().get_auctioned_item (ch, strtoul(arg1, NULL, 10), strtoul(arg2, NULL, 10));
}

ACMD (do_buy_sold_item)
{
	char arg1[256];
	char arg2[256];
	one_argument (argument, arg1, sizeof(arg1));

	AuctionManager::instance().get_auctioned_item (ch, strtoul(arg1, NULL, 10), strtoul(arg2, NULL, 10));
}

ACMD (do_cancel_auction)
{
	char arg1[256];
	one_argument (argument, arg1, sizeof(arg1));

	AuctionManager::instance().cancel_auction (ch, strtoul(arg1, NULL, 10));
}

ACMD (do_cancel_wish)
{
	char arg1[256];
	one_argument (argument, arg1, sizeof(arg1));

	AuctionManager::instance().cancel_wish (ch, strtoul(arg1, NULL, 10));
}

ACMD (do_cancel_sale)
{
	char arg1[256];
	one_argument (argument, arg1, sizeof(arg1));

	AuctionManager::instance().cancel_sale (ch, strtoul(arg1, NULL, 10));
}

ACMD (do_rebid)
{
	char arg1[256];
	char arg2[256];
	two_arguments (argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	AuctionManager::instance().rebid (ch, strtoul(arg1, NULL, 10), strtoul(arg2, NULL, 10));
}

ACMD (do_bid_cancel)
{
	char arg1[256];
	char arg2[256];
	two_arguments (argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	AuctionManager::instance().bid_cancel (ch, strtoul(arg1, NULL, 10));
}
#endif

#ifdef ENABLE_INVENTORY_SORT
bool sortByType(CItem* a, CItem* b)
{
	return (a->GetType() < b->GetType());
}

bool sortBySubType(CItem* a, CItem* b)
{
	return (a->GetSubType() < b->GetSubType());
}

bool sortByVnum(CItem* a, CItem* b)
{
	return (a->GetVnum() < b->GetVnum());
}

bool sortBySocket(CItem* a, CItem* b)
{
	return (a->GetSocket(0) < b->GetSocket(0));
}

ACMD(do_sort_inventory)
{
	if (!ch->CanHandleItem())
	{
		return;
	}

	std::vector<CItem*> collectItems;
	int totalSize = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);

		if (item)
		{
			totalSize += item->GetSize();
			collectItems.push_back(item);
		}
	}

	if (totalSize - 3 >= INVENTORY_MAX_NUM)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("INVENTORY_FULL_CANNOT_SORT"));
		return;
	}

	for (std::vector<CItem*>::iterator it = collectItems.begin() ; it != collectItems.end(); ++it)
	{
		LPITEM item = *it;
		item->RemoveFromCharacter();
	}

	std::sort(collectItems.begin(), collectItems.end(), sortByType);
	std::sort(collectItems.begin(), collectItems.end(), sortBySubType);
	std::sort(collectItems.begin(), collectItems.end(), sortByVnum);
	std::sort(collectItems.begin(), collectItems.end(), sortBySocket);

	for (std::vector<CItem*>::iterator iit = collectItems.begin(); iit < collectItems.end(); ++iit)
	{
		LPITEM sortedItem = *iit;
		if (sortedItem)
		{
			DWORD dwCount = sortedItem->GetCount();

			if (sortedItem->IsStackable() && !IS_SET(sortedItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
			{
				for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
				{
					LPITEM item2 = ch->GetInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

						ItemCountType dwCount2 = MIN(g_bItemCountLimit - item2->GetCount(), dwCount);
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}
			}

			if (dwCount > 0)
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				int cell = ch->GetEmptyInventory(sortedItem);
#else
				int cell = ch->GetEmptyInventory(sortedItem->GetSize());
#endif
				sortedItem->AddToCharacter(ch, TItemPos(INVENTORY, cell));
			}
		}
	}
}

#ifdef ENABLE_SPECIAL_INVENTORY

ACMD(do_sort_special_inventory)
{
	if (!ch->CanHandleItem())
	{
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	BYTE specialInventoryType = atoi(arg1);

	std::vector<CItem*> collectItems;

	int startIndex = SPECIAL_INVENTORY_SLOT_START + (SPECIAL_INVENTORY_PAGE_SIZE * INVENTORY_PAGE_COUNT) * specialInventoryType;
	int endIndex = SPECIAL_INVENTORY_SLOT_START + (SPECIAL_INVENTORY_PAGE_SIZE * INVENTORY_PAGE_COUNT) * (specialInventoryType + 1);

	for (int i = startIndex; i < endIndex; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);

		if (item)
		{
			collectItems.push_back(item);
		}
	}

	for (std::vector<CItem*>::iterator it = collectItems.begin() ; it != collectItems.end(); ++it)
	{
		LPITEM item = *it;
		item->RemoveFromCharacter();
	}

	std::sort(collectItems.begin(), collectItems.end(), sortByType);
	std::sort(collectItems.begin(), collectItems.end(), sortBySubType);
	std::sort(collectItems.begin(), collectItems.end(), sortByVnum);
	std::sort(collectItems.begin(), collectItems.end(), sortBySocket);

	for (std::vector<CItem*>::iterator iit = collectItems.begin(); iit < collectItems.end(); ++iit)
	{
		LPITEM sortedItem = *iit;
		if (sortedItem)
		{
			DWORD dwCount = sortedItem->GetCount();

			if (sortedItem->IsStackable() && !IS_SET(sortedItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
			{
				for (int i = startIndex; i < endIndex; ++i)
				{
					LPITEM item2 = ch->GetInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

						ItemCountType dwCount2 = MIN(g_bItemCountLimit - item2->GetCount(), dwCount);
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}
			}

			if (dwCount > 0)
			{
				int cell = ch->GetEmptyInventory(sortedItem);
				sortedItem->AddToCharacter(ch, TItemPos(INVENTORY, cell));
			}
		}
	}
}

#endif
#endif


#ifdef __SYSTEM_SEARCH_ITEM_MOB__
ACMD(search_drop)
{
	int iWaitMadafaka = ch->GetQuestFlag("search.sijaja");
	if (iWaitMadafaka)
	{
		if (get_global_time() < iWaitMadafaka + 5) 
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You have to wait 5 seconds before making a new search request.");
			return;
		}
	}
	char arg1[4096];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;

	std::string nume_item(arg1);
	boost::algorithm::replace_all(nume_item, "_", " ");
	
	ITEM_MANAGER::instance().FindItemMonster(ch, nume_item);
	ch->SetQuestFlag("search.sijaja", get_global_time());
}
#endif

ACMD(do_cards)
{
	const char *line;

	char arg1[256], arg2[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			if (isdigit(*arg2))
			{
				DWORD safemode;
				str_to_number(safemode, arg2);
				ch->Cards_open(safemode);
			}
			break;
		case 'p':	// open
			ch->Cards_pullout();
			break;
		case 'e':	// open
			ch->CardsEnd();
			break;
		case 'd':	// open
			if (isdigit(*arg2))
			{
				DWORD destroy_index;
				str_to_number(destroy_index, arg2);
				ch->CardsDestroy(destroy_index);
			}
			break;
		case 'a':	// open
			if (isdigit(*arg2))
			{
				DWORD accpet_index;
				str_to_number(accpet_index, arg2);
				ch->CardsAccept(accpet_index);
			}
			break;
		case 'r':	// open
			if (isdigit(*arg2))
			{
				DWORD restore_index;
				str_to_number(restore_index, arg2);
				ch->CardsRestore(restore_index);
			}
			break;
		default:
			return;
	}
}

#ifdef LWT_BUFFI_SYSTEM
ACMD(do_buffi_cikar)
{
	char arg1[10];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;
	BYTE gelen = 0;
	str_to_number(gelen, arg1);
	
	int iWaitMadafaka = ch->GetQuestFlag("search.hur123123ensohn");
	if (iWaitMadafaka)
	{
		if (get_global_time() < iWaitMadafaka + 2) 
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You have to wait 2 seconds before making a new request.");
			return;
		}
	}
	
	
	ch->SetQuestFlag("search.hur123123ensohn", get_global_time());

	if (ch->IsHack(true))
		return;
		
	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return;
	
	if (ch->GetExchange() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->IsOpenSafebox() || ch->IsWarping())
	{
		ch->ChatPacket(1, "ned handeln mensch");
		return;
	}
		
	if (!ch->CanWarp())
	{
		ch->ChatPacket(1, "!CanWarp (10 sec warten lol)");
		return;
	}

	
	BYTE o = 0;

	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(ch->GetSupportSystem()->GetActiveSupport()->GetSummonItemVID());
	
	if (!pSummonItem)
		return;

	switch(gelen)
	{
		case 1:
		{
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM player.buff_items WHERE itemid='%u' AND slot='%d'", pSummonItem->GetID(), 1));

			MYSQL_RES *res = msg->Get()->pSQLResult;
			if (msg->uiSQLErrno != 0 || !res || !msg->Get() || msg->Get()->uiNumRows < 1)
			{
				return;
			}
			MYSQL_ROW row = mysql_fetch_row(res);
			LPITEM item = ch->AutoGiveItem(pSummonItem->GetSocket(1), 1);
			if (!item)
				return;
			
			int i = 3;
			int y = 0;
			while (y < 7)
			{
				BYTE attrtype;
				short attrvalue;
				
				str_to_number(attrtype, row[i++]);
				str_to_number(attrvalue, row[i++]);
				
				item->SetForceAttribute(y, attrtype, attrvalue);
				y++;
			}
			
			y = 0;
			while (y < 3)
			{
				long socket;
				str_to_number(socket, row[i++]);
				
				item->SetSocket(y, socket, false);
				y++;
			}
			
			std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery("DELETE FROM player.buff_items WHERE itemid='%u' AND slot='%d'", pSummonItem->GetID(), 1));
			if (msg2->uiSQLErrno != 0)
			{
				sys_log(0, "<Buffi> Error: #0003 %s", ch->GetName());
			}
			
			pSummonItem->SetSocket(1,o);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "bufficostume %u", o);
		}
		break;
		case 2:
		{
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM player.buff_items WHERE itemid='%u' AND slot='%d'", pSummonItem->GetID(), 3));

			MYSQL_RES *res = msg->Get()->pSQLResult;
			if (msg->uiSQLErrno != 0 || !res || !msg->Get() || msg->Get()->uiNumRows < 1)
			{
				return;
			}
			MYSQL_ROW row = mysql_fetch_row(res);
			LPITEM item = ch->AutoGiveItem(pSummonItem->GetSocket(3), 1);
			if (!item)
				return;
			
			int i = 3;
			int y = 0;
			while (y < 7)
			{
				BYTE attrtype;
				short attrvalue;
				
				str_to_number(attrtype, row[i++]);
				str_to_number(attrvalue, row[i++]);
				
				item->SetForceAttribute(y, attrtype, attrvalue);
				y++;
			}
			
			y = 0;
			while (y < 3)
			{
				long socket;
				str_to_number(socket, row[i++]);
				
				item->SetSocket(y, socket, false);
				y++;
			}
			
			std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery("DELETE FROM player.buff_items WHERE itemid='%u' AND slot='%d'", pSummonItem->GetID(), 3));
			if (msg2->uiSQLErrno != 0)
			{
				sys_log(0, "<Buffi> Error: #0003 %s", ch->GetName());
			}
		
			
			pSummonItem->SetSocket(3,o);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "buffihair %u", o);
		}
		break;
		case 3:
		{
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM player.buff_items WHERE itemid='%u' AND slot='%d'", pSummonItem->GetID(), 2));

			MYSQL_RES *res = msg->Get()->pSQLResult;
			if (msg->uiSQLErrno != 0 || !res || !msg->Get() || msg->Get()->uiNumRows < 1)
			{
				return;
			}
			MYSQL_ROW row = mysql_fetch_row(res);
			LPITEM item = ch->AutoGiveItem(pSummonItem->GetSocket(2), 1);
			if (!item)
				return;
			
			int i = 3;
			int y = 0;
			while (y < 7)
			{
				BYTE attrtype;
				short attrvalue;
				
				str_to_number(attrtype, row[i++]);
				str_to_number(attrvalue, row[i++]);
				
				item->SetForceAttribute(y, attrtype, attrvalue);
				y++;
			}
			
			y = 0;
			while (y < 3)
			{
				long socket;
				str_to_number(socket, row[i++]);
				
				item->SetSocket(y, socket, false);
				y++;
			}
			
			std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery("DELETE FROM player.buff_items WHERE itemid='%u' AND slot='%d'", pSummonItem->GetID(), 2));
			if (msg2->uiSQLErrno != 0)
			{
				sys_log(0, "<Buffi> Error: #0003 %s", ch->GetName());
			}
			pSummonItem->SetSocket(2,o);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "buffiweapon %u", o);
		}
		break;
		case 4:
		{
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM player.buff_items WHERE itemid='%u' AND slot='%d'", pSummonItem->GetID(), 4));

			MYSQL_RES *res = msg->Get()->pSQLResult;
			if (msg->uiSQLErrno != 0 || !res || !msg->Get() || msg->Get()->uiNumRows < 1)
			{
				return;
			}
			MYSQL_ROW row = mysql_fetch_row(res);
			LPITEM item = ch->AutoGiveItem(pSummonItem->GetSocket(4), 1);
			if (!item)
				return;
			
			int i = 3;
			int y = 0;
			while (y < 7)
			{
				BYTE attrtype;
				short attrvalue;
				
				str_to_number(attrtype, row[i++]);
				str_to_number(attrvalue, row[i++]);
				
				item->SetForceAttribute(y, attrtype, attrvalue);
				y++;
			}
			
			y = 0;
			while (y < 3)
			{
				long socket;
				str_to_number(socket, row[i++]);
				
				item->SetSocket(y, socket, false);
				y++;
			}
			
			std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery("DELETE FROM player.buff_items WHERE itemid='%u' AND slot='%d'", pSummonItem->GetID(), 4));
			if (msg2->uiSQLErrno != 0)
			{
				sys_log(0, "<Buffi> Error: #0003 %s", ch->GetName());
			}
			pSummonItem->SetSocket(4,o); // compile
			ch->ChatPacket(CHAT_TYPE_COMMAND, "buffieffect %u", o);
		}
		break;
		default:
			break;
	}
	ch->GetSupportSystem()->GetActiveSupport()->RefreshCostume();
}

ACMD(do_buffi_giydir)
{
	char arg1[256], arg2[256], arg3[256];
	three_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
		return;
		
	int iWaitMadafaka = ch->GetQuestFlag("search.hur12523ensohn");
	if (iWaitMadafaka)
	{
		if (get_global_time() < iWaitMadafaka + 2) 
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You have to wait 2 seconds before making a new request.");
			return;
		}
	}
	
	
	ch->SetQuestFlag("search.hur12523ensohn", get_global_time());
		
	DWORD items = 0; BYTE gelen = 0; DWORD itemPos = 0;
	str_to_number(items, arg1);  str_to_number(gelen, arg2); str_to_number(itemPos, arg3);
	if (ch->GetSupportSystem()->IsActiveSupport())
	{
		LPITEM item = ch->GetInventoryItem(itemPos);
		if (!item->CheckBuffiItem())
		{
			ch->ChatPacket(1, "this is not buffi item");
			return;
		}
		
		LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(ch->GetSupportSystem()->GetActiveSupport()->GetSummonItemVID());
		if (item != NULL)
		{
			if (item->GetVnum() == items)
			{ // 3 = silah - 1 = kostum 2 = sac 
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				{
					ch->ChatPacket(1, LC_TEXT("JUST_SHAMAN_ITEMS"));
					return;
				}
				
				if (item->IsExchanging())
				{
					ch->ChatPacket(1, LC_TEXT("CLOSE_EXCHANGE_WINDOW"));
					return;
				}
				
				if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
					return;
				
				if (ch->GetExchange() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->IsOpenSafebox() || ch->IsWarping())
				{
					ch->ChatPacket(1, "ned handeln mensch");
					return;
				}
					
				if (!ch->CanWarp())
				{
					ch->ChatPacket(1, "!CanWarp (10 sec warten lol)");
					return;
				}
					
				if (!item)
					return;
			
				if (item->IsEquipped())
				{
					ch->ChatPacket(1, "Dat item is ausger?tet du idiot");
					return;
				}
			
				if (item->IsDragonSoul()) // maybe change later
					return;
			
				if (item->GetCell() >= INVENTORY_MAX_NUM ||  IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> â��� �ű� �� ���� ������ �Դϴ�."));
					return;
				}
			
				if (item->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �� �������� ���� �� �����ϴ�."));
					return;
				}
			
				if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �� �������� ���� �� �����ϴ�."));
					return;
				}
			
				if (true == item->isLocked())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �� �������� ���� �� �����ϴ�."));
					return;
				}
				
				switch(gelen)
				{
					case 1:
					{
						char szQuery[1024];
						
						snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.buff_items VALUES (%u, %d, %d, %d, %d,%d, %d,%d, %d, %d, %d,%d, %d,%d, %d,%d, %d, %u, %u, %u)",
								pSummonItem->GetID(), 1, item->GetVnum(),
								item->GetAttributeType(0), item->GetAttributeValue(0),
								item->GetAttributeType(1), item->GetAttributeValue(1),
								item->GetAttributeType(2), item->GetAttributeValue(2),
								item->GetAttributeType(3), item->GetAttributeValue(3),
								item->GetAttributeType(4), item->GetAttributeValue(4),
								item->GetAttributeType(5), item->GetAttributeValue(5),
								item->GetAttributeType(6), item->GetAttributeValue(6), item->GetSocket(0), item->GetSocket(1), item->GetSocket(2));
						
						std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));
						if (msg->uiSQLErrno != 0)
						{
							sys_log(0, "<Buffi> Error: #0002 %s", ch->GetName());
							ch->ChatPacket(CHAT_TYPE_INFO, "<Buffi> Error: #0002");
							return;
						}
						pSummonItem->SetSocket(1,item->GetVnum());
						ch->ChatPacket(CHAT_TYPE_COMMAND, "bufficostume %u", item->GetVnum());
						item->RemoveFromCharacter();
					}
					break;
					case 2:
					{
						char szQuery[1024];
						
						snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.buff_items VALUES (%u, %d, %d, %d, %d,%d, %d,%d, %d, %d, %d,%d, %d,%d, %d,%d, %d, %u, %u, %u)",
								pSummonItem->GetID(), 3, item->GetVnum(),
								item->GetAttributeType(0), item->GetAttributeValue(0),
								item->GetAttributeType(1), item->GetAttributeValue(1),
								item->GetAttributeType(2), item->GetAttributeValue(2),
								item->GetAttributeType(3), item->GetAttributeValue(3),
								item->GetAttributeType(4), item->GetAttributeValue(4),
								item->GetAttributeType(5), item->GetAttributeValue(5),
								item->GetAttributeType(6), item->GetAttributeValue(6), item->GetSocket(0), item->GetSocket(1), item->GetSocket(2));
						
						std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));
						if (msg->uiSQLErrno != 0)
						{
							sys_log(0, "<Buffi> Error: #0002 %s", ch->GetName());
							ch->ChatPacket(CHAT_TYPE_INFO, "<Buffi> Error: #0002");
							return;
						}
						pSummonItem->SetSocket(3,item->GetVnum());
						ch->ChatPacket(CHAT_TYPE_COMMAND, "buffihair %u", item->GetVnum());
						item->RemoveFromCharacter();
					}
					break;
					case 3:
					{
						char szQuery[1024];
						
						snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.buff_items VALUES (%u, %d, %d, %d, %d,%d, %d,%d, %d, %d, %d,%d, %d,%d, %d,%d, %d, %u, %u, %u)",
								pSummonItem->GetID(), 2, item->GetVnum(),
								item->GetAttributeType(0), item->GetAttributeValue(0),
								item->GetAttributeType(1), item->GetAttributeValue(1),
								item->GetAttributeType(2), item->GetAttributeValue(2),
								item->GetAttributeType(3), item->GetAttributeValue(3),
								item->GetAttributeType(4), item->GetAttributeValue(4),
								item->GetAttributeType(5), item->GetAttributeValue(5),
								item->GetAttributeType(6), item->GetAttributeValue(6), item->GetSocket(0), item->GetSocket(1), item->GetSocket(2));
						
						std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));
						if (msg->uiSQLErrno != 0)
						{
							sys_log(0, "<Buffi> Error: #0002 %s", ch->GetName());
							ch->ChatPacket(CHAT_TYPE_INFO, "<Buffi> Error: #0002");
							return;
						}
						pSummonItem->SetSocket(2,item->GetVnum());
						ch->ChatPacket(CHAT_TYPE_COMMAND, "buffiweapon %u", item->GetVnum());
						item->RemoveFromCharacter();
					}
					break;
					case 4:
					{
						char szQuery[1024];
						
						snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.buff_items VALUES (%u, %d, %d, %d, %d,%d, %d,%d, %d, %d, %d,%d, %d,%d, %d,%d, %d, %u, %u, %u)",
								pSummonItem->GetID(), 4, item->GetVnum(),
								item->GetAttributeType(0), item->GetAttributeValue(0),
								item->GetAttributeType(1), item->GetAttributeValue(1),
								item->GetAttributeType(2), item->GetAttributeValue(2),
								item->GetAttributeType(3), item->GetAttributeValue(3),
								item->GetAttributeType(4), item->GetAttributeValue(4),
								item->GetAttributeType(5), item->GetAttributeValue(5),
								item->GetAttributeType(6), item->GetAttributeValue(6), item->GetSocket(0), item->GetSocket(1), item->GetSocket(2));
						
						std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));
						if (msg->uiSQLErrno != 0)
						{
							sys_log(0, "<Buffi> Error: #0002 %s", ch->GetName());
							ch->ChatPacket(CHAT_TYPE_INFO, "<Buffi> Error: #0002");
							return;
						}
						pSummonItem->SetSocket(4,item->GetVnum()); // compile
						ch->ChatPacket(CHAT_TYPE_COMMAND, "buffieffect %u", item->GetVnum());
						item->RemoveFromCharacter();
					}
					break;
					default:
					break;
				}
				
			}
		}
	ch->GetSupportSystem()->GetActiveSupport()->RefreshCostume();
	}
}
#endif

#ifdef ENABLE_AFFECT_BUFF_REMOVE
ACMD(do_remove_buff)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch)
		return;

	int affect = 0;
	str_to_number(affect, arg1);
	CAffect* pAffect = ch->FindAffect(affect);

	if (pAffect)
		ch->RemoveAffect(affect);
}
#endif

#ifdef ENABLE_BIYOLOG
#include "shop_manager.h"
ACMD(do_open_shop)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }

	DWORD shopVnum = 0;
	str_to_number(shopVnum, vecArgs[1].c_str());
	CShopManager::Instance().StartShopping(ch, NULL, shopVnum);
}
ACMD(do_bio)
{
	if (ch->GetLevel() < 30)
		return;

	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }

	int level = ch->GetQuestFlag("bio.level");

	if (level >= bio_max)
		return;
	else if (level < 1)
		return;
	else if (ch->GetLevel() < bio_data[level][0])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("710"), bio_data[level][0]);
		return;
	}

	int count = ch->GetQuestFlag("bio.count");
	int time = ch->GetQuestFlag("bio.time");

	if (vecArgs[1] == "mission")
	{
		if (vecArgs.size() < 4) { return; }
		BYTE isOzut = 0;
		BYTE isUnutkanlik = 0;

		str_to_number(isOzut, vecArgs[2].c_str());
		str_to_number(isUnutkanlik, vecArgs[3].c_str());

		if (count < bio_data[level][2])
		{
			if (ch->CountSpecifyItem(bio_data[level][1]) < 1)
			{
				TItemTable* p = ITEM_MANAGER::instance().GetTable(bio_data[level][1]);
				if(p)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("711"), p->szLocaleName);
				return;
			}

			//bool isUnutkanlikDrink = false;

			if (isUnutkanlik)
			{
				if (ch->CountSpecifyItem(bio_data[level][16]) > 0)
				{
					ch->RemoveSpecifyItem(bio_data[level][16], 1);
					//isUnutkanlikDrink = true;
				}
				else
				{
					if (time > get_global_time())
					{
						TItemTable* p = ITEM_MANAGER::instance().GetTable(bio_data[level][16]);
						if(p)
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("712"), p->szLocaleName);
						return;
					}
				}
			}
			else
			{
				if (time > get_global_time())
				{
					TItemTable* p = ITEM_MANAGER::instance().GetTable(bio_data[level][16]);
					if(p)
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("712"), p->szLocaleName);
					return;
				}
			}


			bool isOzutDrink = false;
			if (isOzut)
			{
				if (ch->CountSpecifyItem(bio_data[level][15]) > 0)
				{
					ch->RemoveSpecifyItem(bio_data[level][15], 1);
					isOzutDrink = true;
				}
			}
			
			int prob = isOzutDrink ? bio_data[level][4] + 50 : bio_data[level][4];
			ch->RemoveSpecifyItem(bio_data[level][1], 1);
			if (prob >= number(1, 100))
			{
				count += 1;
				time = get_global_time() + bio_data[level][3];
				ch->SetQuestFlag("bio.count", count);
				ch->SetQuestFlag("bio.time", time);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", level, count, time);

				TItemTable* p = ITEM_MANAGER::instance().GetTable(bio_data[level][1]);
				if(p)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("713"), p->szLocaleName);
			}
			else
			{
				time = get_global_time() + bio_data[level][3];
				ch->SetQuestFlag("bio.time", time);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", level, count, time);
				
				TItemTable* p = ITEM_MANAGER::instance().GetTable(bio_data[level][1]);
				if(p)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("714"), p->szLocaleName);
			}

			if (bio_data[level][5] != 0)
			{
				if (count == bio_data[level][2])
					ch->ChatPacket(CHAT_TYPE_COMMAND, "biostone %d", level);
				return;
			}
			
		}

		if (bio_data[level][5] != 0)
		{
			if (count == bio_data[level][2])
			{
				if (ch->CountSpecifyItem(bio_data[level][5]) < 1)
				{
					TItemTable* p = ITEM_MANAGER::instance().GetTable(bio_data[level][5]);
					if(p)
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("715"), p->szLocaleName);
					return;
				}
				else
				{
					ch->RemoveSpecifyItem(bio_data[level][5], 1);
					ch->SetQuestFlag("bio.count", count+1);

					if (bio_data[level][14] == 0)
					{
						ch->SetQuestFlag("bio.count", 0);
						ch->SetQuestFlag("bio.level", level + 1);
						ch->SetQuestFlag("bio.time", 0);

						if (bio_data[level][6] != 0)
						{
							long value = bio_data[level][7];
							CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][6]);
							if (affect) {
								value += affect->lApplyValue;
								ch->RemoveAffect(affect);
							}
							ch->AddAffect(AFFECT_COLLECT, bio_data[level][6], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

							//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][6]), bio_data[level][7]);
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
						}

						if (bio_data[level][8] != 0)
						{
							long value = bio_data[level][9];
							CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][8]);
							if (affect) {
								value += affect->lApplyValue;
								ch->RemoveAffect(affect);
							}
							ch->AddAffect(AFFECT_COLLECT, bio_data[level][8], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

							//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][8]), bio_data[level][9]);
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
						}

						if (bio_data[level][10] != 0)
						{
							long value = bio_data[level][11];
							CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][10]);
							if (affect) {
								value += affect->lApplyValue;
								ch->RemoveAffect(affect);
							}
							ch->AddAffect(AFFECT_COLLECT, bio_data[level][10], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

							//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][10]), bio_data[level][11]);
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
						}

						if (bio_data[level][12] != 0)
						{
							long value = bio_data[level][13];
							CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][12]);
							if (affect) {
								value += affect->lApplyValue;
								ch->RemoveAffect(affect);
							}
								
							ch->AddAffect(AFFECT_COLLECT, bio_data[level][12], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

							//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][12]), bio_data[level][13]);
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
						}

						int newLevel = level + 1;
						if (newLevel >= 11)
						{
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("716"));
							ch->ChatPacket(CHAT_TYPE_COMMAND, "bioempty");
							return;
						}
						ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", newLevel,0,0);
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_COMMAND, "bioodul %d", level);
						return;
					}
				}
			}
			else if (count == bio_data[level][2]+1)
				return;
		}
		else
		{
			if (count == bio_data[level][2])
			{
				if (bio_data[level][14] == 0)
				{
					ch->SetQuestFlag("bio.count", 0);
					ch->SetQuestFlag("bio.level", level + 1);
					ch->SetQuestFlag("bio.time", 0);

					if (bio_data[level][6] != 0)
					{
						long value = bio_data[level][7];
						CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][6]);
						if (affect) {
							value += affect->lApplyValue;
							ch->RemoveAffect(affect);
						}
						ch->AddAffect(AFFECT_COLLECT, bio_data[level][6], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

						//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][6]), bio_data[level][7]);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
					}

					if (bio_data[level][8] != 0)
					{
						long value = bio_data[level][9];
						CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][8]);
						if (affect) {
							value += affect->lApplyValue;
							ch->RemoveAffect(affect);
						}
						ch->AddAffect(AFFECT_COLLECT, bio_data[level][8], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

						//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][8]), bio_data[level][9]);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
					}

					if (bio_data[level][10] != 0)
					{
						long value = bio_data[level][11];
						CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][10]);
						if (affect) {
							value += affect->lApplyValue;
							ch->RemoveAffect(affect);
						}
						ch->AddAffect(AFFECT_COLLECT, bio_data[level][10], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

						//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][10]), bio_data[level][11]);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
					}

					if (bio_data[level][12] != 0)
					{
						long value = bio_data[level][13];
						CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][12]);
						if (affect) {
							value += affect->lApplyValue;
							ch->RemoveAffect(affect);
						}
						ch->AddAffect(AFFECT_COLLECT, bio_data[level][12], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

						//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][12]), bio_data[level][13]);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
					}

					int newLevel = level + 1;
					if (newLevel >= bio_max)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("716"));
						ch->ChatPacket(CHAT_TYPE_COMMAND, "bioempty");
						return;
					}
					ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", newLevel, 0, 0);
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_COMMAND, "bioodul %d", level);
					return;
				}
			}
		}
	}
	else if (vecArgs[1] == "gift")
	{
		if (vecArgs.size() < 3) { return; }
		BYTE index = 0;
		str_to_number(index, vecArgs[2].c_str());

		if (index > 2)
			return;

		if (bio_data[level][5] != 0)
		{
			if (count != bio_data[level][2] + 1)
				return;
		}
		else
		{
			if (count != bio_data[level][2])
				return;
		}

		ch->SetQuestFlag("bio.count", 0);
		ch->SetQuestFlag("bio.level", level + 1);
		ch->SetQuestFlag("bio.time", 0);

		if (bio_data[level][6] != 0 && index == 0)
		{
			long value = bio_data[level][7];
			CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][6]);
			if (affect) {
				value += affect->lApplyValue;
				ch->RemoveAffect(affect);
			}
			ch->AddAffect(AFFECT_COLLECT, bio_data[level][6], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][6]), bio_data[level][7]);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
		}

		if (bio_data[level][8] != 0 && index == 1)
		{
			long value = bio_data[level][9];
			CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][8]);
			if (affect) {
				value += affect->lApplyValue;
				ch->RemoveAffect(affect);
			}
			ch->AddAffect(AFFECT_COLLECT, bio_data[level][8], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][8]), bio_data[level][9]);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
		}

		if (bio_data[level][10] != 0 && index == 2)
		{
			long value = bio_data[level][11];
			CAffect* affect = ch->FindAffect(AFFECT_COLLECT, bio_data[level][10]);
			if (affect) {
				value += affect->lApplyValue;
				ch->RemoveAffect(affect);
			}
			ch->AddAffect(AFFECT_COLLECT, bio_data[level][10], value, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);

			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717 E%d:%d"), pointToApply(bio_data[level][10]), bio_data[level][11]);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("717"));
		}

		char flag[100];
		sprintf(flag, "bio.bonus%d", level);
		ch->SetQuestFlag(flag, index + 1);

		int newLevel = level + 1;
		if (newLevel >= bio_max)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("716"));
			ch->ChatPacket(CHAT_TYPE_COMMAND, "bioempty");
			return;
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, "biodata %d %d %d", newLevel, 0, 0);
	}
}
#endif

ACMD(do_lottery_draw)
{
	if (!ch)
		return;
	
    if (ch->IsDead() || ch->GetExchange() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't do that with a window already open!"));
        return;
	}
	
	if (ch->CountSpecifyItem(50992) < 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have this Lotto Ticket for this action!"));
		return;
	}

    BYTE randNumber = number(0, 9);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "LotteryRegisterReward %d", randNumber);
	ch->SetIndexLotteryReward(randNumber);
	ch->RemoveSpecifyItem(50992, 1);
}

ACMD(do_lottery_withdraw_reward)
{
	if (!ch)
		return;
	
    if (ch->IsDead() || ch->GetExchange() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't do that with a window already open!"));
        return;
	}

	if (ch->GetIndexLotteryReward() < 0 || ch->GetIndexLotteryReward() > 9)
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Wrong index request"));
        return;
	}
	
	// Position 4 from ITEMS = { (uiLottert) = 50513
	if (ch->GetIndexLotteryReward() == 0)
		ch->AutoGiveItem(41001, 1);
	
	else if(ch->GetIndexLotteryReward() == 1)
		ch->AutoGiveItem(41002, 1);
		
	else if(ch->GetIndexLotteryReward() == 2)
		ch->AutoGiveItem(41003, 1);
		
	else if(ch->GetIndexLotteryReward() == 3)
		ch->AutoGiveItem(41004, 1);

	else if(ch->GetIndexLotteryReward() == 4)
		ch->AutoGiveItem(41005, 1);
		
	else if(ch->GetIndexLotteryReward() == 5)
		ch->AutoGiveItem(41006, 1);
		
	else if(ch->GetIndexLotteryReward() == 6)
		ch->AutoGiveItem(41007, 1);
		
	else if(ch->GetIndexLotteryReward() == 7)
		ch->AutoGiveItem(41008, 1);
		
	else if(ch->GetIndexLotteryReward() == 8)
		ch->AutoGiveItem(41009, 1);
		
	else if(ch->GetIndexLotteryReward() == 9)
		ch->AutoGiveItem(41010, 1);
}

#ifdef ENABLE_NEW_PET_SYSTEM
ACMD(do_CubePetAdd) {

	int pos = 0;
	int invpos = 0;
	int invtype = 0;

	const char *line;
	char arg1[256], arg2[256], arg3[256], arg4[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
	one_argument(line, arg4, sizeof(arg4));

	if (0 == arg1[0])
		return;
	const std::string& strArg1 = std::string(arg1);
	switch (LOWER(arg1[0]))
	{
	case 'a':	// add cue_index inven_index
	{
		if (0 == arg2[0] || !isdigit(*arg2) ||
			0 == arg3[0] || !isdigit(*arg3) ||
			0 == arg4[0] || !isdigit(*arg4))
			return;

		str_to_number(pos, arg2);
		str_to_number(invpos, arg3);
		str_to_number(invtype, arg4);

	}
	break;

	default:
		return;
	}

	if (ch->GetNewPetSystem()->IsActivePet())
		ch->GetNewPetSystem()->SetItemCube(pos, invpos, invtype);
	else
		return;

}

ACMD(do_PetGetType)
{
	LPITEM item = ch->FindSpecifyItem(55032);
	if (ch->GetNewPetSystem()->IsActivePet()) 
	{
		if (item != NULL and item->GetCount() > 0)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "SayPetType %d", ch->GetNewPetSystem()->GetPetType());
			item->SetCount(item->GetCount() - 1);
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_ORB_COUNT_ZERO"));
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_SCHOULD_SUMMONED"));
}

ACMD(do_PetChangeAttr) {
	
	char arg1[256], arg2[256];
	
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));	
	
	if (!*arg1 || !*arg2)
		return;

	int itempos1 = 0;
	int itempos2 = 0;
	
	str_to_number(itempos1, arg1);
	str_to_number(itempos2, arg2);
	
	LPITEM item = ch->GetInventoryItem(itempos1);
	LPITEM item2 = ch->GetInventoryItem(itempos2);
	
	if (item != NULL and item2 != NULL)
	{
		if (item->GetVnum() >= 55701 and item->GetVnum() <= 55799 and item2->GetVnum() == 55033)
		{
			if (item2->GetCount() > 0)
			{
				if (ch->GetNewPetSystem()->IsActivePet() and item->GetID() == ch->GetNewPetSystem()->GetSummonedPetItemID()) 
				{
					ch->GetNewPetSystem()->ChangePetAttr();
					item2->SetCount(item2->GetCount() - 1);			
				}
				else if (item->GetSocket(1) - time(0) > 0)
				{
					int level = 0;
					int age_bonus = 0;
					
					char szQuery1[1024];
					snprintf(szQuery1, sizeof(szQuery1), "SELECT level,age_bonus FROM new_petsystem WHERE id = %lu ", item->GetID());
					std::auto_ptr<SQLMsg> pmsg2(DBManager::instance().DirectQuery(szQuery1));
					if (pmsg2->Get()->uiNumRows > 0) {
						MYSQL_ROW row = mysql_fetch_row(pmsg2->Get()->pSQLResult);	
						level = atoi(row[0]);
						age_bonus = atoi(row[1]);						
					}
					
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
					
					//					Type Chance:		1		2		3		4		5		6		7		8
					const int prob_type_table[] = { 	15, 	30, 	45,	58,	70,	80,	88,	95 };
					int rand = number(1, 95);
					
					rand_bonus0 += number(Pet_Type_Attr_Table[0][0][0], Pet_Type_Attr_Table[0][0][1]);
					rand_bonus1 += number(Pet_Type_Attr_Table[0][1][0], Pet_Type_Attr_Table[0][1][1]);
					rand_bonus2 += number(Pet_Type_Attr_Table[0][2][0], Pet_Type_Attr_Table[0][2][1]);
						
					for (int i = 0; i < SPECIAL_BORN_PETS_MAX_NUM; ++i) 
					{
						if (item->GetVnum() == Special_Born_Attr_Table[i][0][0])
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
					
					for (int i = 0; i < level; ++i) 
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

					item->SetForceAttribute(0, 1, rand_bonus0);
					item->SetForceAttribute(1, 1, rand_bonus1);
					item->SetForceAttribute(2, 1, rand_bonus2);

					DBManager::instance().DirectQuery
					(
						"UPDATE new_petsystem SET bonus0=%d, bonus1=%d, bonus2=%d, pettype=%d, typebonus0=%d, typebonus1=%d, typebonus2=%d WHERE id = %lu ", 
						rand_bonus0,
						rand_bonus1,
						rand_bonus2,
						pettype,
						rand_typebonus0,
						rand_typebonus1,
						rand_typebonus2,
						item->GetID()
					);
					ch->ChatPacket(CHAT_TYPE_COMMAND, "PetAttrFinish %d", pettype);
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_NEW_ATTR"), pettype);
					item2->SetCount(item2->GetCount() - 1);
#ifdef ENABLE_EXTENDED_BATTLE_PASS
					ch->UpdateExtBattlePassMissionProgress(PET_ENCHANT, 1, pettype);
#endif
				}
				else
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_REFILL_LIFETIME"));
			}
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Fehler"));
		}
	}
}

ACMD(do_PetRemoveSkill) {
	
	char arg1[256], arg2[256], arg3[256];
	
	// arg1 = Skill Slot --- arg2 = itemslot --- arg3 = itemposition
	three_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));	
	
	if (!*arg1)
		return;

	int skillslot = 0;
	int itemvnum = 0;
	int itempos = 0;
	str_to_number(skillslot, arg1);
	str_to_number(itemvnum, arg2);
	str_to_number(itempos, arg3);

	if (skillslot > 2 || skillslot < 0)
		return;

	if (ch->GetNewPetSystem()->IsActivePet())
	{
		if (itemvnum == 55029)
		{
			if (ch->GetNewPetSystem()->GetSkillSlot0() >= 1 or ch->GetNewPetSystem()->GetSkillSlot1() >= 1 or ch->GetNewPetSystem()->GetSkillSlot2() >= 1)
			{
				if (ch->CountSpecifyItem(itemvnum) > 0)
				{
					ch->GetNewPetSystem()->RemovePetSkill(skillslot, itemvnum);
					ch->RemoveSpecifyItem(itemvnum);
				}
			}
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_NOT_ALLOWED_RESET_SKILLS"));
		}
		else if (itemvnum == 55030)
		{
			if (skillslot == 0 and ch->GetNewPetSystem()->GetSkillSlot0() >= 1 or skillslot == 1 and ch->GetNewPetSystem()->GetSkillSlot1() >= 1 or skillslot == 2 and ch->GetNewPetSystem()->GetSkillSlot2() >= 1)
			{
				if (ch->CountSpecifyItem(itemvnum) > 0)
				{
					ch->GetNewPetSystem()->RemovePetSkill(skillslot, itemvnum);
					ch->RemoveSpecifyItem(itemvnum);
				}
			}
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_NOT_ALLOWED_RESET_SKILL"));
		}
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_SCHOULD_SUMMONED"));
}

ACMD(do_FeedCubePet) {
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	DWORD feedtype = 0;
	str_to_number(feedtype, arg1);
	if (ch->GetNewPetSystem()->IsActivePet())
		ch->GetNewPetSystem()->ItemCubeFeed(feedtype);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_SCHOULD_SUMMONED"));
}

ACMD(do_PetEvo) {

	if (ch->GetExchange() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_CANNOT_EVOLVE_NOW"));
		return;
	}
	if (ch->GetNewPetSystem()->IsActivePet()) {

		DWORD items = 3;
		
		int it[3][items] = { 
						{ 27992, 27993, 27994 }, //Here Modify Items to request for 1 evo
						{ 27992, 27993, 27994 }, //Here Modify Items to request for 2 evo
						{ 27992, 27993, 27994 }  //Here Modify Items to request for 3 evo
		};
		int ic[3][items] = {
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 }
		};
		int tmpevo = ch->GetNewPetSystem()->GetEvolution();
		
		
		if (ch->GetNewPetSystem()->GetLevel() == 40 && tmpevo == 0 ||
			ch->GetNewPetSystem()->GetLevel() == 80 && tmpevo == 1 ||
			ch->GetNewPetSystem()->GetLevel() >= 80 && tmpevo == 2) {
			for (int b = 0; b < items; b++) {
				if (ch->CountSpecifyItem(it[tmpevo][b]) < ic[tmpevo][b]) {
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_EVO_ITEM_REQUIRED"));
					for (int c = 0; c < items; c++) {
						DWORD vnum = it[tmpevo][c];
						ch->ChatPacket(CHAT_TYPE_INFO, "%dx - %s ( %d )", ic[tmpevo][c] , ITEM_MANAGER::instance().GetTable(vnum)->szLocaleName, ch->CountSpecifyItem(it[tmpevo][c]));
					}
					return;
				}
			}
			for (int c = 0; c < items; c++) {
				ch->RemoveSpecifyItem(it[tmpevo][c], ic[tmpevo][c]);
			}
			ch->GetNewPetSystem()->IncreasePetEvolution();

		}
		else {
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_CANNOT_EVOLVE"));
			return;
		}

	}else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_SCHOULD_SUMMONED"));
}

ACMD(do_teleportpanel_open) {
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenTeleportPanel");
	return;
}
#endif

#ifdef __HIDE_COSTUME_SYSTEM__
ACMD (do_hide_costume)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	bool hidden = true;
	BYTE bPartPos = 0;
	BYTE bHidden = 0;

	str_to_number(bPartPos, arg1);

	if (*arg2)
	{
		str_to_number(bHidden, arg2);

		if (bHidden == 0)
			hidden = false;
	}

	if (bPartPos == 1)
		ch->SetBodyCostumeHidden(hidden);
	else if (bPartPos == 2)
		ch->SetHairCostumeHidden(hidden);
	#ifdef __SASH_SYSTEM__
	else if (bPartPos == 3)
		ch->SetSashCostumeHidden(hidden);
	#endif
	#ifdef __WEAPON_COSTUME_SYSTEM__
	else if (bPartPos == 4)
		ch->SetWeaponCostumeHidden(hidden);
	#endif
	else
		return;

	ch->UpdatePacket();
}
#endif

#if defined(__PICKUP_FILTER__) && defined(__AUTO_PICKUP__)
ACMD(do_change_item_pickup_filter)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	if (!*arg1 || !*arg2)
		return;

	uint16_t wCell = 0;
	str_to_number(wCell, arg1);
	
	LPITEM item = ch->GetInventoryItem(wCell);	
	if (!item)
		return;
	
	if (EItemTypes::ITEM_USE != item->GetType() && EUseSubTypes::USE_AUTO_PICKUP != item->GetSubType())
		return;
	
	uint16_t wPickupFilterFlag = 0;
	str_to_number(wPickupFilterFlag, arg2);
	
	item->SetSocket(1, wPickupFilterFlag); 								
	item->UpdatePacket(); 
}	
#endif

#ifdef __VIP_SYSTEM__
ACMD(do_convert_vip_affect)
{
	auto pAffect = ch->FindAffect(EAffectTypes::AFFECT_VIP); 
	if (nullptr == pAffect)
		return;
	
	const auto leftDuration = pAffect->lDuration;
	if (leftDuration <= 0)
		return;
		
	if (auto item = ch->AutoGiveItem(ITEM_VIP_UNIQUE); item)
	{
		if (ch->RemoveVIP())
			item->SetSocket(0, leftDuration);
		else
			M2_DESTROY_ITEM(item);
	}
}	
#endif

#ifdef ENABLE_WHEEL_OF_FORTUNE
ACMD(do_wheel_of_fortune)
{
	if (!quest::CQuestManager::instance().GetEventFlag("wheel_of_fortune"))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("When the Wheel of Fortune is open, you will see it InGame."));
		return;
	}
	
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) 
	{ 
		return; 
	}
	else if (vecArgs[1] == "open")
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenWheelofFortune");
		return;
	}
	else if (vecArgs[1] == "start")
	{
		if (ch->CountSpecifyItem(77380) <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("WHEEL_NOT_TICKETS"));
			return;
		}
		else if (ch->GetProtectTime("WheelWorking") != 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("WHEEL_ALREDY_WORKS"));
			return;
		}

		ch->RemoveSpecifyItem(77380, 1);

		std::vector<std::pair<long, long>> m_important_item = {
			{22420,1}, // Hauptgewinn : Neris Kostume (M)
			{22421,1}, // Hauptgewinn : Neris Kostume (W)
			{95002,1}, // Hauptgewinn : Dokument der Dauer
			{77289,1}, // Hauptgewinn : Dokument der Klingenstarke
			{77290,1} // Hauptgewinn : Dokument der Klingenfertigkeiten
			
		};

		std::map<std::pair<long, long>, int> m_normal_item = {
			 {{70022,1},25}, // Nebengewinn: Akzelerator
			 {{53586,1},20}, // Nebengewinn: Hohes Crafting Stueck
			 {{52630,1},25}, // Nebengewinn: Niedriges Crafting Stueck
			 {{37292,1},25}, // Nebengewinn: Buch der Verdammnis
			 {{50516,1},25}, // Nebengewinn: Medaillion der Altesten
			 {{25041,1},25}, // Nebengewinn: Magischer Stein
			 {{50517,5},25}, // Nebengewinn: 5x Stein der Fahigkeiten
			 {{27992,3},20}, // Nebengewinn: 3x Wei��e Perle
			 {{27993,3},20}, // Nebengewinn: 3x Blaue Perle
			 {{27994,3},20}, // Nebengewinn: 3x Rote Perle
			 {{27987,20},25}, // Nebengewinn: 20x Muschel
			 {{71051,6},25}, // Nebengewinn: 6x 6/7 Bonus Einfuegen.
			 {{71052,6},25}, // Nebengewinn: 6x 6/7 Bonus Switchen.
			 {{85003,1},20}, // Nebengewinn: Herrscherband (Edel)
			 {{85013,1},20}, // Nebengewinn: Furstenband (Edel)
			 {{73278,1},20}, // Nebengewinn: Hexer Ring (Ingame 7 Tage)
			 {{73279,1},20}, // Nebengewinn: Goettlicher Ring (Ingame 7 Tage)
			 {{48940,40},25}, // Nebengewinn: 40x Halbmenschen Hammer
			 {{48941,40},25}, // Nebengewinn: 40x Boss Hammer
			 {{48942,40},25}, // Nebengewinn: 40x Metin Hammer
			 {{48943,40},25}, // Nebengewinn: 40x Dunkler Hammer
			 {{48944,40},25}, // Nebengewinn: 40x Natur Hammer
			 {{48945,40},25} // Nebengewinn: 40x Monster Hammer
		};

		std::vector<std::pair<long, long>> m_send_items;
		if (m_important_item.size())
		{
			int random = number(0,m_important_item.size()-1);
			m_send_items.emplace_back(m_important_item[random].first, m_important_item[random].second);
		}

		while (true)
		{
			for (auto it = m_normal_item.begin(); it != m_normal_item.end(); ++it)
			{
				int randomEx = number(0,4);
				if (randomEx == 4)
				{
					int random = number(0,100);
					if (it->second >= random)
					{
						auto itFind = std::find(m_send_items.begin(), m_send_items.end(), it->first);
						if (itFind == m_send_items.end())
						{
							m_send_items.emplace_back(it->first.first, it->first.second);
							if (m_send_items.size() >= 10)
								break;
						}
					}
				}
			}
			if (m_send_items.size() >= 10)
				break;
		}

		std::string cmd_wheel = "";
		if (m_send_items.size())
		{
			for (auto it = m_send_items.begin(); it != m_send_items.end(); ++it)
			{
				cmd_wheel += std::to_string(it->first);
				cmd_wheel += "|";
				cmd_wheel += std::to_string(it->second);
				cmd_wheel += "#";
			}
		}

		int luckyWheel = number(0, 9);
		if(luckyWheel == 0)
			if(number(0,1) == 0)
				luckyWheel = number(0, 9);

		ch->SetProtectTime("WheelLuckyIndex", luckyWheel);
		ch->SetProtectTime("WheelLuckyItemVnum", m_send_items[luckyWheel].first);
		ch->SetProtectTime("WheelLuckyItemCount", m_send_items[luckyWheel].second);

		ch->SetProtectTime("WheelWorking", 1);

		ch->ChatPacket(CHAT_TYPE_COMMAND, "SetItemData %s", cmd_wheel.c_str());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OnSetWhell %d", luckyWheel);
	}
	else if (vecArgs[1] == "done")
	{
		if (ch->GetProtectTime("WheelWorking") == 0)
			return;
		ch->AutoGiveItem(ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GetGiftData %d %d", ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));
		ch->SetProtectTime("WheelLuckyIndex", 0);
		ch->SetProtectTime("WheelLuckyItemVnum", 0);
		ch->SetProtectTime("WheelLuckyItemCount", 0);
		ch->SetProtectTime("WheelWorking", 0);
	}
}
#endif

#ifdef __PROMO_CODE__
ACMD(do_use_promotion_code)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || !ch)
		return;
		
	if (!ch->CanUsePromotionCode(arg1))	
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already used that promotion code."));
		return;
	}
		
	ch->RewardPromotionCode(arg1);
}
#endif


#ifdef ENABLE_REWARD_SYSTEM
ACMD(do_update_reward_data)
{
	CHARACTER_MANAGER::Instance().SendRewardInfo(ch);
}
#endif

#ifdef __RENEWAL_BRAVE_CAPE__
ACMD(do_brave_cape)
{
	ch->GetBraveCapeCMDCompare(argument);
}
#endif

#ifdef __GEM_SYSTEM__
ACMD(do_gem)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "load")
	{
		if (time(0) >= ch->GetQuestFlag("gem.left_time"))
		{
			ch->RefreshGemALL(true);
			ch->ChatPacket(CHAT_TYPE_INFO, "v Shop Updated.");
		}
		else
			ch->RefreshGemPlayer();
	}
	else if (vecArgs[1] == "time")
	{
		if (time(0) >= ch->GetQuestFlag("gem.left_time"))
		{
			ch->RefreshGemALL(true);
			ch->ChatPacket(CHAT_TYPE_INFO, "Gaya Shop Updated.");
		}
	}
	else if (vecArgs[1] == "slot")
	{
		ch->OpenGemSlot();
	}
	else if (vecArgs[1] == "refresh")
	{
		if (ch->CountSpecifyItem(OPEN_GEM_REFRESH_ITEM) < 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enought refresh gaya shop item.");
			return;
		}
		ch->RemoveSpecifyItem(OPEN_GEM_REFRESH_ITEM, 1);
		ch->RefreshGemALL();
	}
	else if (vecArgs[1] == "buy")
	{
		if (vecArgs.size() < 3) { return; }
		BYTE slotIndex;
		if (!str_to_number(slotIndex, vecArgs[2].c_str()))
			return;
		ch->BuyGemItem(slotIndex);
	}
	else if (vecArgs[1] == "gm")
	{
		if (!ch->IsGM())
			return;
		if (vecArgs.size() < 3) { return; }
		if (vecArgs[2] == "slotcount")
		{
			if (vecArgs.size() < 4) { return; }
			BYTE slotCount;
			if (!str_to_number(slotCount, vecArgs[3].c_str()))
				return;
			if (slotCount > 27 - 9)
				return;
			ch->SetQuestFlag("gem.open_slot", slotCount);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GemUpdateSlotCount %d", slotCount);
		}
		else if (vecArgs[2] == "time")
		{
			if (vecArgs.size() < 4) { return; }
			int newTime;
			if (!str_to_number(newTime, vecArgs[3].c_str()))
				return;
			ch->SetQuestFlag("gem.left_time", time(0)+newTime);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GemSetRefreshLeftTime %d", newTime);
		}
	}
}
#endif
