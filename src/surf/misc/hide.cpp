#include <pch.h>
#include "hide.h"
#include <sdk/serversideclient.h>
#include <sdk/entity/ccsplayercontroller.h>
#include <utils/utils.h>
#include <core/playermanager.h>

CHidePlugin g_HidePlugin;

CHidePlugin* SURF::MISC::HidePlugin() {
	return &g_HidePlugin;
}

void SetMenuEntityTransmiter(CBaseEntity* pMenu, CBasePlayerController* pOwner) {
	g_HidePlugin.SetExclude(pOwner, pMenu, true);
}

void CHidePlugin::ClearEntityTransmitInfo(CEdictBitVec* pTransmitEntity, CEdictBitVec* pTransmitAlways, CBaseEntity* pEntity) {
	if (!pEntity) {
		return;
	}

	int iEntityIndex = pEntity->GetEntityIndex().Get();
	if (iEntityIndex < 0) {
		return;
	}

	pTransmitEntity->Clear(iEntityIndex);
	if (pTransmitAlways->Get(iEntityIndex)) {
		pTransmitAlways->Clear(iEntityIndex);
	}
};

void CHidePlugin::ClearEntityChildEntities(CEdictBitVec* pTransmitEntity, CEdictBitVec* pTransmitAlways, CBaseEntity* pEntity) {
	CBodyComponent* pBodyComponent = pEntity->m_CBodyComponent();
	if (!pBodyComponent) {
		return;
	}

	CGameSceneNode* pSceneNode = pBodyComponent->m_pSceneNode();
	if (!pSceneNode) {
		return;
	}

	for (CGameSceneNode* pChild = pSceneNode->m_pChild(); pChild; pChild = pChild->m_pNextSibling()) {
		CBaseEntity* pChildOwnerEntity = (CBaseEntity*)pChild->m_pOwner();
		if (!pChildOwnerEntity) {
			continue;
		}

		ClearEntityTransmitInfo(pTransmitEntity, pTransmitAlways, pChildOwnerEntity);
	}
}

void CHidePlugin::Set(CBasePlayerController* pOwner, CBaseEntity* pTarget, bool bShouldHide) {
	if (!pOwner || !pTarget) {
		return;
	}

	int iOwnerSlot = pOwner->GetPlayerSlot();
	if (!UTIL::IsPlayerSlot(iOwnerSlot)) {
		return;
	}

	int iTargetIdx = pTarget->GetEntityIndex().Get();

	if (bShouldHide) {
		m_vBlockTransmit.at(iOwnerSlot).Set(iTargetIdx);
	} else {
		m_vBlockTransmit.at(iOwnerSlot).Clear(iTargetIdx);
	}
}

void CHidePlugin::SetExclude(CBasePlayerController* pExcludedOwner, CBaseEntity* pTarget, bool bShouldHide) {
	if (!pExcludedOwner || !pTarget) {
		return;
	}

	auto vOnlinePlayers = GetPlayerManager()->GetOnlinePlayers();
	for (const auto& pPlayer : vOnlinePlayers) {
		CCSPlayerController* pTargetController = pPlayer->GetController();
		if (pTargetController && pTargetController->GetPlayerSlot() != pExcludedOwner->GetPlayerSlot()) {
			this->Set(pTargetController, pTarget, bShouldHide);
		}
	}
}

void CHidePlugin::Remove(CBasePlayerController* pOwner, CBaseEntity* pTarget) {
	this->Set(pOwner, pTarget, false);
}

void CHidePlugin::Reset(CBasePlayerController* pOwner) {
	if (!pOwner) {
		return;
	}

	int iOwnerSlot = pOwner->GetPlayerSlot();
	if (!UTIL::IsPlayerSlot(iOwnerSlot)) {
		return;
	}

	m_vBlockTransmit.at(iOwnerSlot).ClearAll();
}

void CHidePlugin::OnClientSendSnapshotBefore(CServerSideClient* pClient) {
	if (pClient->IsFakeClient() || pClient->m_bIsHLTV) {
		return;
	}

	auto pTransmitEntity = pClient->m_PackInfo.m_pTransmitEntity;
	auto pTransmitAlways = pClient->m_PackInfo.m_pTransmitAlways;
	if (!pTransmitEntity || !pTransmitAlways) {
		return;
	}

	int iClientSlot = pClient->GetPlayerSlot().Get();
	int iClientEntIndex = pClient->GetEntityIndex().Get();
	int iEntityIndex = -1;
	while ((iEntityIndex = pTransmitEntity->FindNextSetBit(iEntityIndex + 1)) >= 0) {
		if (!pTransmitEntity->Get(iEntityIndex)) {
			continue;
		}

		if (!m_vBlockTransmit.at(iClientSlot).Get(iEntityIndex)) {
			continue;
		}

		CBaseEntity* pEnt = (CBaseEntity*)GameEntitySystem()->GetEntityInstance(CEntityIndex(iEntityIndex));
		if (!pEnt) {
			continue;
		}

		if (pEnt->IsPawn()) {
			CCSPlayerController* pController = static_cast<CCSPlayerPawn*>(pEnt)->m_hOriginalController().Get();
			if (pController && pController->GetEntityIndex().Get() == iClientEntIndex) {
				Plat_FatalErrorFunc("CHidePlugin::OnClientSendSnapshotBefore: hide self Pawn is not allowed. (Client slot: %d, Entity idx: %d)\n",
									iClientSlot, iClientEntIndex);
				DebuggerBreak();
			}
		} else if (pEnt->IsController()) {
			if (static_cast<CCSPlayerController*>(pEnt)->GetEntityIndex().Get() == iClientEntIndex) {
				Plat_FatalErrorFunc(
					"CHidePlugin::OnClientSendSnapshotBefore: hide self Controller is not allowed. (Client slot: %d, Entity idx: %d)\n", iClientSlot,
					iClientEntIndex);
				DebuggerBreak();
			}
		}

		ClearEntityChildEntities(pTransmitEntity, pTransmitAlways, pEnt);
		ClearEntityTransmitInfo(pTransmitEntity, pTransmitAlways, pEnt);
	}
}
