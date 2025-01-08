#include "surf_timer.h"
#include <core/concmdmanager.h>

CCMD_CALLBACK(Command_StartTimer) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& pTimerService = player->m_pTimerService;
	pTimerService->DoTimerStart();
}

CCMD_CALLBACK(Command_EndTimer) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& pTimerService = player->m_pTimerService;
	pTimerService->DoTimerEnd();
}

void CSurfTimerPlugin::RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_r", Command_StartTimer);
	CONCMD::RegConsoleCmd("sm_end", Command_EndTimer);
}
