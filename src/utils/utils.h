#pragma once

#include <pch.h>

#include <string>
#include <filesystem>

#include <sdk/serversideclient.h>
#include <sdk/gamerules.h>

#include <utils/ctimer.h>
#include <utils/print.h>

#define JSON_GETTER(_json, key, ret) \
	do { \
		if (_json.contains(#key)) \
			ret = _json[#key].get<decltype(ret)>(); \
	} while (0)

namespace UTIL {
	namespace PATH {
		template<typename... Args>
		std::string Join(const Args&... args) {
			std::filesystem::path result;
			((result /= std::filesystem::path(args)), ...);
			return result.string();
		}
	} // namespace PATH

	std::string GetWorkingDirectory();
	std::string GetPublicIP();
	json LoadJsonc(std::string sFilePath);
	std::wstring ToWideString(const char* pszCharStr);

	CGlobalVars* GetGlobals();
	CGlobalVars* GetServerGlobals();
	CBasePlayerController* GetController(CBaseEntity* entity);
	CBasePlayerController* GetController(CPlayerSlot slot);
	bool IsPlayerSlot(CPlayerSlot slot);
	CUtlVector<CServerSideClient*>* GetClientList();
	CServerSideClient* GetClientBySlot(CPlayerSlot slot);
	CCSGameRules* GetGameRules();

	bool TraceLine(const Vector& vecStart, const Vector& vecEnd, CEntityInstance* ignore1, CGameTrace* tr, uint64 traceLayer,
				   uint64 excludeLayer = 0);
	void GetPlayerAiming(CCSPlayerPawnBase* pPlayer, CGameTrace& ret);

	CBaseEntity* FindEntityByClassname(CEntityInstance* start, const char* name);
	CBaseEntity* CreateBeam(const Vector& from, const Vector& to, Color color = Color(0, 255, 0, 255), float width = 1.5f,
							CBaseEntity* owner = nullptr);
} // namespace UTIL
