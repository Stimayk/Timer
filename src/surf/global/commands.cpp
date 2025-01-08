#include "surf_global.h"
#include <core/concmdmanager.h>

CCMD_CALLBACK(Command_GlobalCheck) {
	SURF::GlobalPlugin()->GlobalCheck(pController);
}

SCMD_CALLBACK(Command_GlobalCheck_Srv) {
	SURF::GlobalPlugin()->GlobalCheck(nullptr);
}

CCMD_CALLBACK(Command_GlobalAPIInfo) {}

CCMD_CALLBACK(Command_GlobalAPIReload) {
	SURF::GlobalPlugin()->ReadAPIKey();
}

void CSurfGlobalAPIPlugin::RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_gc", Command_GlobalCheck);
	CONCMD::RegConsoleCmd("sm_globalcheck", Command_GlobalCheck);
	CONCMD::RegServerCmd("sm_gc", Command_GlobalCheck_Srv);
	CONCMD::RegServerCmd("sm_globalcheck", Command_GlobalCheck_Srv);
	CONCMD::RegAdminCmd("sm_globalapi_info", Command_GlobalAPIInfo, AdminFlag::Root);
	CONCMD::RegAdminCmd("sm_globalapi_reload_apikey", Command_GlobalAPIReload, AdminFlag::Root);
}
