#include <core/eventmanager.h>
#include <core/interfaces.h>
#include <sdk/gamerules.h>
#include <utils/utils.h>
#include <sdk/entity/ccsplayercontroller.h>
#include <surf/misc/surf_misc.h>

static void OnPlayerDeath(IGameEvent* pEvent, const char* szName, bool bDontBroadcast) {
	auto pController = (CCSPlayerController*)pEvent->GetPlayerController("userid");
	if (!pController) {
		return;
	}

	CHandle<CCSPlayerController> hController = pController->GetRefEHandle();

	UTIL::RequestFrame([hController]() {
		CCSPlayerController* pController = hController.Get();
		if (!pController) {
			return;
		}

		CBasePlayerPawn* pPawn = pController->GetCurrentPawn();
		if (!pPawn || pPawn->IsAlive()) {
			return;
		}

		pPawn->m_clrRender(Color(255, 255, 255, 0));
	});
}

static void OnRoundPrestart(IGameEvent* pEvent, const char* szName, bool bDontBroadcast) {}

static void OnRoundStart(IGameEvent* pEvent, const char* szName, bool bDontBroadcast) {
	IFACE::pEngine->ServerCommand("sv_full_alltalk 1");

	CCSGameRules* gameRules = UTIL::GetGameRules();
	if (gameRules) {
		gameRules->m_bGameRestart(true);
		gameRules->m_iRoundWinStatus(1);
		// Make sure that the round time is synchronized with the global time.
		gameRules->m_fRoundStartTime().SetTime(0.0f);
		gameRules->m_flGameStartTime().SetTime(0.0f);
	}
}

static void OnPlayerTeam(IGameEvent* pEvent, const char* szName, bool bDontBroadcast) {
	pEvent->SetBool("silent", true);
}

static void OnPlayerSpawm(IGameEvent* pEvent, const char* szName, bool bDontBroadcast) {
	auto pController = (CCSPlayerController*)pEvent->GetPlayerController("userid");
	if (!pController) {
		return;
	}

	CHandle<CCSPlayerController> hController = pController->GetRefEHandle();

	UTIL::RequestFrame([hController]() {
		CCSPlayerController* pController = hController.Get();
		if (!pController) {
			return;
		}

		if (!pController->m_bPawnIsAlive()) {
			return;
		}

		CBasePlayerPawn* pPawn = pController->GetCurrentPawn();
		if (!pPawn || !pPawn->IsAlive()) {
			return;
		}

		pPawn->SetCollisionGroup(COLLISION_GROUP_DEBRIS);
		pPawn->m_clrRender(Color(255, 255, 255, 255));
	});
}

void CSurfMiscPlugin::HookEvents() {
	EVENT::HookEvent("player_death", ::OnPlayerDeath);
	EVENT::HookEvent("round_prestart", ::OnRoundPrestart);
	EVENT::HookEvent("round_start", ::OnRoundStart);
	EVENT::HookEvent("player_team", ::OnPlayerTeam);
	EVENT::HookEvent("player_spawn", ::OnPlayerSpawm);
}
