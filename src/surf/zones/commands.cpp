#include "surf_zones.h"
#include <core/concmdmanager.h>
#include <core/menu.h>
#include <utils/utils.h>

static void OpenMenu_SelectZoneType(CBasePlayerController* pController) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto menu = MENU::Create(MENU_CALLBACK_L(player) {
		auto& pZoneService = player->m_pZoneService;
		pZoneService->m_ZoneEdit.m_iType = (ZoneType)iItem;
		pZoneService->m_ZoneEdit.m_iValue = SURF::ZonePlugin()->GetZoneCount(pZoneService->m_ZoneEdit.m_iTrack, (ZoneType)iItem);
		UTIL::PrintChat(pController, "SELECT: %s, value: %d\n", CSurfZonePlugin::GetZoneNameByType((ZoneType)iItem).c_str(),
						pZoneService->m_ZoneEdit.m_iValue);
		pZoneService->m_ZoneEdit.StartEditZone();

		hMenu.Free();
	});

	menu->SetTitle("选择类型");
	for (int i = ZoneType::Zone_Start; i < ZoneType::ZONETYPES_SIZE; i++) {
		menu->AddItem(CSurfZonePlugin::GetZoneNameByType((ZoneType)i));
	}
	menu->Display(player->GetPlayerPawn());
}

static void OpenMenu_SelectZoneTrack(CBasePlayerController* pController) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto menu = MENU::Create(MENU_CALLBACK_L(player) {
		hMenu.Free();

		player->m_pZoneService->m_ZoneEdit.m_iTrack = (ZoneTrack)iItem;
		OpenMenu_SelectZoneType(pController);
	});

	menu->SetTitle("选择赛道");
	for (int i = ZoneTrack::Track_Main; i < ZoneTrack::TRACKS_SIZE; i++) {
		menu->AddItem(CSurfZonePlugin::GetZoneNameByTrack((ZoneTrack)i));
	}
	menu->Display(player->GetPlayerPawn());
}

CCMD_CALLBACK(Command_Zones) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto menu = MENU::Create(MENU_CALLBACK_L() {
		switch (iItem) {
			case 0:
				hMenu.Free();
				OpenMenu_SelectZoneTrack(pController);
				return;
			case 1:
				UTIL::PrintChat(pController, "SELECT 2\n");
				break;
		}

		hMenu.Free();
	});
	menu->SetTitle("区域菜单");
	menu->AddItem("添加");
	menu->AddItem("编辑");
	menu->Display(player->GetPlayerPawn());
}

CCMD_CALLBACK(Command_EditZone) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& pZoneService = player->m_pZoneService;
	pZoneService->m_ZoneEdit.StartEditZone();
}

CCMD_CALLBACK(Command_TPStart) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	player->GetPlayerPawn()->Teleport(&SURF::ZonePlugin()->m_vecTestStartZone, nullptr, nullptr);
}

void CSurfZonePlugin::RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_zones", Command_Zones);
	CONCMD::RegConsoleCmd("sm_editzone", Command_EditZone);
	CONCMD::RegConsoleCmd("sm_r", Command_TPStart);
}
