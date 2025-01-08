#pragma once

#include <core/forwards.h>

using CEdictBitVec = CBitVec<MAX_EDICTS>;

class CHidePlugin : CCoreForward {
private:
	virtual void OnClientSendSnapshotBefore(CServerSideClient* pClient) override;

public:
	void Set(CBasePlayerController* pOwner, CBaseEntity* pTarget, bool bShouldHide);
	void SetExclude(CBasePlayerController* pExcludedOwner, CBaseEntity* pTarget, bool bShouldHide);
	void Remove(CBasePlayerController* pOwner, CBaseEntity* pTarget);
	void Reset(CBasePlayerController* pOwner);

private:
	void ClearEntityTransmitInfo(CEdictBitVec* pTransmitEntity, CEdictBitVec* pTransmitAlways, CBaseEntity* pEntity);
	void ClearEntityChildEntities(CEdictBitVec* pTransmitEntity, CEdictBitVec* pTransmitAlways, CBaseEntity* pEntity);

private:
	std::array<CEdictBitVec, MAXPLAYERS> m_vBlockTransmit;
};

namespace SURF {
	namespace MISC {
		extern CHidePlugin* HidePlugin();
	} // namespace MISC
} // namespace SURF
