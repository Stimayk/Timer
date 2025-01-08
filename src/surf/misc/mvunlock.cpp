#include <core/memory.h>

class CMovementUnlocker : CCoreForward {
private:
	virtual void OnPluginStart() override;
};

CMovementUnlocker g_MovementUnlocker;

void CMovementUnlocker::OnPluginStart() {
	static auto fn = GAMEDATA::GetMemSig("ServerMovementUnlock");
	SURF_ASSERT(fn);

	WIN_LINUX(libmem::StoreToAddress(fn, 0xEB), libmem::StoreToAddress(fn, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90));
}
