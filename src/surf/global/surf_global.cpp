#include "surf_global.h"
#include <surf/api.h>
#include <utils/utils.h>
#include <fmt/format.h>
#include <regex>

CSurfGlobalAPIPlugin g_GlobalApi;

CSurfGlobalAPIPlugin* SURF::GlobalPlugin() {
	return &g_GlobalApi;
}

void CSurfGlobalAPIPlugin::OnPluginStart() {
	RegisterCommand();
	ReadAPIKey();
}

void CSurfGlobalAPIPlugin::OnActivateServer(CNetworkGameServerBase* pGameServer) {
	Reset();

	GetAuthStatus(HTTPRES_CALLBACK_L() {
		SURF::GlobalPlugin()->m_sBearerToken = "";
		FORWARD_POST(CSurfGlobalForward, OnInit);
	});
}

void CSurfGlobalAPIPlugin::OnClientActive(ISource2GameClients* pClient, CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(slot);
	if (!pPlayer) {
		return;
	}

	pPlayer->m_pGlobalAPIService->CheckGlobalBan();
}

void CSurfGlobalAPIPlugin::Reset() {
	m_bAPIKeyCheck = false;
	m_bBannedCommandsCheck = false;
	m_bEnforcerOnFreshMap = false;
	m_bMapValidated = false;
	m_iMapWorkshopID = -1;
	m_iCurrentMapWorkshopID = -1;
}

void CSurfGlobalAPIPlugin::ReadAPIKey() {
	static auto sPath = UTIL::PATH::Join(UTIL::GetWorkingDirectory(), "configs", "api.jsonc");
	json j = UTIL::LoadJsonc(sPath);
	if (j.is_null() || j.empty()) {
		return;
	}

	JSON_GETTER(j, global_key, m_sAPIKey);
	JSON_GETTER(j, zone_helper_key, m_sZoneHelperKey);

	if (j.contains("endpoints")) {
		auto& endpoints = j["endpoints"];
		for (const auto& [name, endpoint] : endpoints.items()) {
			m_umEndpoint[name] = SURF::GLOBAL::BaseUrl + endpoint.get<std::string>();
		}
	}
}

bool CSurfGlobalAPIPlugin::MapCheck() const {
	return m_bMapValidated;
}

void CSurfGlobalAPIPlugin::GlobalCheck(CBasePlayerController* pController) const {
	bool bClientGloballyVerified = true;
	if (pController) {
		auto pSurfPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
		if (pSurfPlayer) {
			bClientGloballyVerified = pSurfPlayer->m_pGlobalAPIService->m_bGloballyVerified;
		}
	}

	auto GCArg = [](bool bStatus) -> const char* { return bStatus ? "[green]✓" : "[darkred]X"; };

	// clang-format off
	std::string sGC = fmt::format("[grey]API接入检测:\n[grey]API密钥 {}[grey] | 插件 {}[grey] | 参数设置 {}[grey] | 地图 {}[grey] | 你 {}",
					GCArg(m_bAPIKeyCheck),
					GCArg(m_bBannedCommandsCheck),
					GCArg(m_bEnforcerOnFreshMap),
					GCArg(MapCheck()),
					GCArg(bClientGloballyVerified));
	// clang-format on

	if (!pController) {
		std::regex pattern(R"(\[[^\]]*\])");
		sGC = std::regex_replace(sGC, pattern, "");
	}

	UTIL::Print(pController, sGC.c_str());
}

void CSurfGlobalAPIService::Reset() {
	m_bGloballyVerified = false;
}

void CSurfGlobalAPIService::CheckGlobalBan() {
	auto steam64 = this->GetPlayer()->GetSteamId64();
	// globalapi http stuff
}
