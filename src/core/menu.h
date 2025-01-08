#pragma once

#include <pch.h>
#include <core/playermanager.h>

enum class EMenuType {
	POINT_WORLDTEXT = 0
};

class CBaseMenu;

class CMenuHandle {
public:
	bool IsValid() const {
		return (m_pMenu != nullptr);
	}

	void Free();

public:
	CBaseMenu* m_pMenu = nullptr;
};

using MenuHandler = std::function<void(CMenuHandle& hMenu, CBasePlayerController* pController, int iItem)>;

#define MENU_CALLBACK(cb)    void cb(CMenuHandle& hMenu, CBasePlayerController* pController, int iItem)
#define MENU_CALLBACK_L(...) [__VA_ARGS__](CMenuHandle & hMenu, CBasePlayerController * pController, int iItem)

class CBaseMenu {
public:
	CBaseMenu(MenuHandler pFnHandler, std::string sTitle = "") : m_pFnMenuHandler(pFnHandler), m_sTitle(sTitle) {
		AllocatePage();
	}

	virtual ~CBaseMenu() {
		m_pFnMenuHandler = nullptr;
	}

	virtual void SetTitle(const std::string_view& sTitle) {
		m_sTitle = sTitle;
	}

	virtual void SetPrevious(CBaseMenu* pPrevious) {
		m_pLast = pPrevious;
	}

	virtual void SetNext(CBaseMenu* pNext) {
		m_pNext = pNext;
	}

	virtual void AddItem(std::string sItem) {
		if (m_vItems.empty() || m_vItems.back().size() >= PAGE_SIZE) {
			AllocatePage();
		}

		m_vItems.back().emplace_back(sItem);
	}

	virtual std::string GetItem(int iPageIndex, int iItemIndex);

public:
	virtual void Display(CCSPlayerPawnBase* pPawn, int iPageIndex = 0) = 0;

public:
	size_t GetPageLength() const {
		return m_vItems.size();
	}

private:
	void AllocatePage() {
		std::vector<std::string> nullItems;
		nullItems.reserve(PAGE_SIZE);
		m_vItems.emplace_back(nullItems);
	}

public:
	static const size_t PAGE_SIZE = 6;
	std::string m_sTitle;
	std::vector<std::vector<std::string>> m_vItems;
	MenuHandler m_pFnMenuHandler;

protected:
	CBaseMenu* m_pLast;
	CBaseMenu* m_pNext;
};

class CWorldTextMenu : public CBaseMenu {
public:
	CWorldTextMenu(MenuHandler pFnHandler, std::string sTitle = "");

	virtual ~CWorldTextMenu() override;
	virtual void Display(CCSPlayerPawnBase* pPawn, int iPageIndex = 0) override;

public:
	static Vector GetAimPoint(const Vector& eyePosition, const QAngle& eyeAngles, float distanceToTarget = 100.0);

private:
	CHandle<CPointWorldText> m_hWorldText;
	CHandle<CPointWorldText> m_hBackground;
};

class CMenuPlayer : public CPlayer {
public:
	using CPlayer::CPlayer;

	virtual void Init(int iSlot) override {
		CPlayer::Init(iSlot);

		ResetMenu();
	}

	virtual void Reset() override {
		CPlayer::Reset();

		ResetMenu();
	}

private:
	void ResetMenu() {
		if (m_pCurrentMenu.IsValid()) {
			m_pCurrentMenu.Free();
		}

		m_iCurrentPage = 0;
	}

public:
	CMenuHandle m_pCurrentMenu;
	int m_iCurrentPage;
};

class CMenuManager : CPlayerManager {
public:
	CMenuManager() {
		for (int i = 0; i < MAXPLAYERS; i++) {
			m_pPlayers[i] = std::make_unique<CMenuPlayer>(i);
		}
	}

	virtual CMenuPlayer* ToPlayer(CBasePlayerController* controller) const override {
		return static_cast<CMenuPlayer*>(CPlayerManager::ToPlayer(controller));
	}

	virtual CMenuPlayer* ToPlayer(CBasePlayerPawn* pawn) const override {
		return static_cast<CMenuPlayer*>(CPlayerManager::ToPlayer(pawn));
	}

private:
	static void OnMenuItemSelect(CCSPlayerController* pController, const std::vector<std::string>& vArgs);

private:
	virtual void OnPluginStart() override;
};

inline void CMenuHandle::Free() {
	if (m_pMenu) {
		delete m_pMenu;
		m_pMenu = nullptr;
	}
}

namespace MENU {
	extern CMenuManager* GetManager();

	[[nodiscard]] CBaseMenu* Create(MenuHandler pMenuHandler, EMenuType eMenuType = EMenuType::POINT_WORLDTEXT);
} // namespace MENU
