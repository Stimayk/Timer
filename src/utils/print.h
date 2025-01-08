#pragma once

#include <pch.h>
#include <core/logger.h>

enum MsgDest : int32_t {
	HUD_PRINTNOTIFY = 1,
	HUD_PRINTCONSOLE = 2,
	HUD_PRINTTALK = 3,
	HUD_PRINTCENTER = 4,
	HUD_PRINTTALK2 = 5, // Not sure what the difference between this and HUD_PRINTTALK is...
	HUD_PRINTALERT = 6
};

namespace UTIL {
	void ClientPrintFilter(IRecipientFilter* filter, int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3,
						   const char* param4);
	void SayChat(CBasePlayerController* pControler, const char* format, ...);
	void PrintConsole(CBasePlayerController* pControler, const char* format, ...);
	void PrintChat(CBasePlayerController* pControler, const char* format, ...);
	void PrintCentre(CBasePlayerController* pControler, const char* format, ...);
	void PrintAlert(CBasePlayerController* pControler, const char* format, ...);
	void PrintHTMLCentre(CBasePlayerController* pControler, const char* format, ...); // This one uses HTML formatting.

	void PrintConsoleAll(const char* format, ...);
	void PrintChatAll(const char* format, ...);
	void PrintCentreAll(const char* format, ...);
	void PrintAlertAll(const char* format, ...);
	void PrintHTMLCentreAll(const char* format, ...); // This one uses HTML formatting.

	// Color print
	void CPrintChat(CBasePlayerController* pControler, const char* format, ...);
	void CPrintChatAll(const char* format, ...);

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) == 0, void> Print(CBasePlayerController* pControler, const char* fmt) {
		if (!pControler) {
			LOG::Print(fmt);
		} else {
			CPrintChat(pControler, "%s\n", fmt);
		}
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) != 0, void> Print(CBasePlayerController* pControler, const char* fmt, Args... args) {
		if (!pControler) {
			LOG::Print(fmt, args...);
		} else {
			CPrintChat(pControler, fmt, args...);
		}
	}
} // namespace UTIL
