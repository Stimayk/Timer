#pragma once

#include <core/forwards.h>

class CShowTrigger : CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnActivateServer(CNetworkGameServerBase* pGameServer) override;

public:
	void TransmitTriggers(const std::vector<CHandle<CBaseTrigger>>& vTriggers, bool bTransmit);
};

namespace SURF {
	namespace MISC {
		extern CShowTrigger* ShowTriggerPlugin();
	} // namespace MISC
} // namespace SURF
