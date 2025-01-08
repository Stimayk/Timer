#include <pch.h>
#include <core/concmdmanager.h>

class CBlockRadio : CCoreForward {
private:
	virtual void OnPluginStart() override;
};

CBlockRadio g_BlockRadio;

void CBlockRadio::OnPluginStart() {
	constexpr static const char* blockedCommands[] = {"playerchatwheel", "player_ping", "roger",     "negative",    "cheer",     "holdpos",
													  "thanks",          "go",          "fallback",  "sticktog",    "followme",  "compliment",
													  "enemyspot",       "needbackup",  "takepoint", "sectorclear", "inposition"};

	for (size_t i = 0; i < sizeof(blockedCommands) / sizeof(blockedCommands[0]); i++) {
		// clang-format off
		CONCMD::AddCommandListener(blockedCommands[i], CCMDLISTENER_CALLBACK_L() { return false; });
		// clang-format on
	}
}
