#include "utils.h"
#include <fstream>
#include <steam/isteamgameserver.h>
#include <core/interfaces.h>
#include <public/iserver.h>
#include <sdk/entity/ccsplayercontroller.h>
#include <sdk/entity/ccsplayerpawn.h>
#include <cs2surf.h>
#include <worldsize.h>

std::string UTIL::GetWorkingDirectory() {
	return PATH::Join(std::filesystem::current_path().string(), "..", "..", GAME_NAME, "addons", "cs2surf");
}

std::string UTIL::GetPublicIP() {
	static std::string ipv4;
	if (!ipv4.empty()) {
		return ipv4;
	}

	auto sAddr = SteamGameServer()->GetPublicIP();
	if (!sAddr.IsSet()) {
		return std::string();
	}

	uint32_t ip = sAddr.m_unIPv4;
	std::ostringstream oss;

	// clang-format off
	oss << ((ip >> 24) & 0xFF) << "." 
		<< ((ip >> 16) & 0xFF) << "." 
		<< ((ip >>  8) & 0xFF) << "." 
		<< ((ip >>  0) & 0xFF);
	// clang-format on

	ipv4 = oss.str();
	return ipv4;
}

json UTIL::LoadJsonc(std::string sFilePath) {
	std::ifstream file(sFilePath);
	if (!file.good()) {
		return json();
	}

	return json::parse(file, nullptr, true, true);
}

std::wstring UTIL::ToWideString(const char* pszCharStr) {
	if (!pszCharStr || !pszCharStr[0]) {
		return std::wstring();
	}

	auto size_needed = WIN_LINUX(MultiByteToWideChar(CP_UTF8, 0, pszCharStr, -1, NULL, 0), mbstowcs(NULL, pszCharStr, 0));
	if (size_needed > 0) {
		std::wstring wSayContent;
		wSayContent.resize(size_needed, L'\0');
#ifdef _WIN32
		MultiByteToWideChar(CP_UTF8, 0, pszCharStr, -1, &wSayContent[0], size_needed);
#else
		mbstowcs(&wSayContent[0], pszCharStr, size_needed);
#endif
		return wSayContent;
	}

	return std::wstring();
}

CGlobalVars* UTIL::GetGlobals() {
	INetworkGameServer* server = g_pNetworkServerService->GetIGameServer();

	if (!server) {
		return nullptr;
	}

	return server->GetGlobals();
}

CGlobalVars* UTIL::GetServerGlobals() {
	return SurfPlugin()->simulatingPhysics ? &(SurfPlugin()->serverGlobals) : GetGlobals();
};

CBasePlayerController* UTIL::GetController(CBaseEntity* entity) {
	CCSPlayerController* controller = nullptr;
	if (!V_stricmp(entity->GetClassname(), "observer")) {
		CBasePlayerPawn* pawn = static_cast<CBasePlayerPawn*>(entity);
		if (!pawn->m_hController()->IsValid() || pawn->m_hController()->Get() == 0) {
			for (i32 i = 0; i <= GetServerGlobals()->maxClients; i++) {
				controller = (CCSPlayerController*)UTIL::GetController(CPlayerSlot(i));
				if (controller && controller->m_hObserverPawn() && controller->m_hObserverPawn().Get() == entity) {
					return controller;
				}
			}
			return nullptr;
		}
		return pawn->m_hController()->Get();
	}
	if (entity->IsPawn()) {
		CBasePlayerPawn* pawn = static_cast<CBasePlayerPawn*>(entity);
		if (!pawn->m_hController()->IsValid() || pawn->m_hController()->Get() == 0) {
			// Seems like the pawn lost its controller, we can try looping through the controllers to find this pawn instead.
			for (i32 i = 0; i <= GetServerGlobals()->maxClients; i++) {
				controller = (CCSPlayerController*)UTIL::GetController(CPlayerSlot(i));
				if (controller && controller->m_hPlayerPawn() && controller->m_hPlayerPawn().Get() == entity) {
					return controller;
				}
			}
			return nullptr;
		}
		return pawn->m_hController()->Get();
	} else if (entity->IsController()) {
		return static_cast<CBasePlayerController*>(entity);
	} else {
		return nullptr;
	}
}

CBasePlayerController* UTIL::GetController(CPlayerSlot slot) {
	if (!GameEntitySystem() || !IsPlayerSlot(slot)) {
		return nullptr;
	}
	CBaseEntity* ent = static_cast<CBaseEntity*>(GameEntitySystem()->GetEntityInstance(CEntityIndex(slot.Get() + 1)));
	if (!ent) {
		return nullptr;
	}
	return ent->IsController() ? static_cast<CBasePlayerController*>(ent) : nullptr;
}

bool UTIL::IsPlayerSlot(CPlayerSlot slot) {
	int iSlot = slot.Get();
	return iSlot >= 0 && iSlot < GetGlobals()->maxClients;
}

CUtlVector<CServerSideClient*>* UTIL::GetClientList() {
	if (!g_pNetworkServerService) {
		return nullptr;
	}

	static int offset = GAMEDATA::GetOffset("ClientOffset");
	return reinterpret_cast<CUtlVector<CServerSideClient*>*>(((uint8_t*)g_pNetworkServerService->GetIGameServer() + offset));
}

CServerSideClient* UTIL::GetClientBySlot(CPlayerSlot slot) {
	return (GetClientList() && GetController(slot)) ? GetClientList()->Element(slot.Get()) : nullptr;
}

CCSGameRules* UTIL::GetGameRules() {
	CCSGameRulesProxy* proxy = nullptr;
	proxy = (CCSGameRulesProxy*)FindEntityByClassname(proxy, "cs_gamerules");
	if (proxy) {
		return proxy->m_pGameRules();
	}
	return nullptr;
}

bool UTIL::TraceLine(const Vector& vecStart, const Vector& vecEnd, CEntityInstance* ignore1, CGameTrace* tr, uint64 traceLayer, uint64 excludeLayer) {
	Ray_t ray;
	CTraceFilter filter;
	filter.SetPassEntity1(ignore1);
	filter.m_nInteractsWith = traceLayer;
	filter.m_nInteractsExclude = excludeLayer;

	return MEM::CALL::TraceShape(ray, vecStart, vecEnd, filter, tr);
}

void UTIL::GetPlayerAiming(CCSPlayerPawnBase* pPlayer, CGameTrace& ret) {
	Vector from = pPlayer->GetEyePosition();

	Vector forward;
	AngleVectors(pPlayer->m_angEyeAngles(), &forward);
	Vector to = from + forward * MAX_COORD_FLOAT;

	TraceLine(from, to, pPlayer, &ret, MASK_SOLID, CONTENTS_TRIGGER | CONTENTS_PLAYER | CONTENTS_PLAYER_CLIP);
}

CBaseEntity* UTIL::FindEntityByClassname(CEntityInstance* start, const char* name) {
	if (!GameEntitySystem()) {
		return nullptr;
	}
	EntityInstanceByClassIter_t iter(start, name);

	return static_cast<CBaseEntity*>(iter.Next());
}

CBaseEntity* UTIL::CreateBeam(const Vector& from, const Vector& to, Color color, float width, CBaseEntity* owner) {
	CBeam* beam = (CBeam*)MEM::CALL::CreateEntityByName("beam");
	if (!beam) {
		return nullptr;
	}

	beam->Teleport(&from, nullptr, nullptr);

	beam->m_clrRender(color);
	beam->m_fWidth(width);
	beam->m_vecEndPos(to);
	beam->m_fadeMinDist(-1.0f);

	if (owner != nullptr) {
		beam->m_hOwnerEntity(owner->GetRefEHandle());
	}

	beam->DispatchSpawn();

	return beam;
}
