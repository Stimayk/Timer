#pragma once

#include <pch.h>
#include <core/adminmanager.h>

#include <sdk/entity/ccsplayercontroller.h>

#define SCMD_CALLBACK(fn)            static void fn(const std::vector<std::string>& vArgs)
#define SCMD_CALLBACK_L(...)         [__VA_ARGS__](const std::vector<std::string>& vArgs) -> void
#define CCMD_CALLBACK(fn)            static void fn(CCSPlayerController* pController, const std::vector<std::string>& vArgs)
#define CCMD_CALLBACK_L(...)         [__VA_ARGS__](CCSPlayerController * pController, const std::vector<std::string>& vArgs) -> void
#define CCMDLISTENER_CALLBACK(fn)    static bool fn(CCSPlayerController* pController, const std::vector<std::string>& vArgs)
#define CCMDLISTENER_CALLBACK_L(...) [__VA_ARGS__](CCSPlayerController * pController, const std::vector<std::string>& vArgs) -> bool

using SrvCmd_Callback = std::function<void(const std::vector<std::string>& vArgs)>;
using ConCmd_Callback = std::function<void(CCSPlayerController* pController, const std::vector<std::string>& vArgs)>;
using ConCmdListener_Callback = std::function<bool(CCSPlayerController* pController, const std::vector<std::string>& vArgs)>;

namespace CONCMD {
	class CConCmdManager : CCoreForward {
		virtual bool OnClientCommand(ISource2GameClients* pClient, CPlayerSlot slot, const CCommand& args) override;
		virtual bool OnDispatchConCommand(ICvar* pCvar, ConCommandHandle cmd, const CCommandContext& ctx, const CCommand& args) override;

	public:
		struct CConCmdInfo {
			ConCmd_Callback callback;
			AdminFlag adminFlags = AdminFlag::None;
			std::string description;
			int64_t cmdFlags = FCVAR_NONE;
		};

		struct CSrvCmdInfo {
			SrvCmd_Callback callback;
			std::string description;
			int64_t cmdFlags = FCVAR_NONE;
		};

		std::unordered_map<std::wstring, std::vector<CConCmdInfo>> m_umConCmds;
		std::unordered_map<std::wstring, std::vector<CSrvCmdInfo>> m_umSrvCmds;
		std::unordered_map<std::wstring, std::vector<ConCmdListener_Callback>> m_umConCmdListeners;
	};

	void RegServerCmd(std::string cmd, SrvCmd_Callback cb, std::string description = std::string(), int64_t cmdFlag = FCVAR_NONE);
	void RegConsoleCmd(std::string cmd, ConCmd_Callback cb, std::string description = std::string(), int64_t cmdFlag = FCVAR_NONE);
	void RegAdminCmd(std::string cmd, ConCmd_Callback cb, AdminFlag adminFlags, std::string description = std::string(),
					 int64_t cmdFlag = FCVAR_NONE);
	void AddCommandListener(std::string cmd, ConCmdListener_Callback cb);

	extern CConCmdManager* GetManager();
} // namespace CONCMD
