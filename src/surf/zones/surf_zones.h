#pragma once

#include <surf/surf_player.h>
#include <surf/zones/edit.h>

struct ZoneCache_t : ZoneData_t {
	using ZoneData_t::ZoneData_t;

	ZoneCache_t() {
		m_aBeams.fill(CEntityHandle());
	}

	std::array<CHandle<CBeam>, 12> m_aBeams;
};

class CZoneHandle : public CHandle<CBaseTrigger> {
	using CHandle::CHandle;
};

template<>
struct std::hash<CZoneHandle> {
	size_t operator()(const CZoneHandle& handle) const {
		return std::hash<uint32>()(handle.ToInt());
	}
};

class CSurfZoneService : public CSurfBaseService {
private:
	virtual void Reset() override;

public:
	using CSurfBaseService::CSurfBaseService;

public:
	void AddZone(const Vector& vecMin, const Vector& vecMax);
	void EditZone(CCSPlayerPawnBase* pawn, const CPlayerButton* buttons);
	void CreateZone(const Vector& vecMin, const Vector& vecMax, std::array<CHandle<CBeam>, 12>& out);
	CBaseEntity* CreateNormalZone(const Vector& vecMins, const Vector& vecMaxs);

public:
	static void CreatePoints2D(const Vector& vecMin, const Vector& vecMax, Vector out[4]);
	static void CreatePoints3D(const Vector& vecMin, const Vector& vecMax, Vector out[8]);
	static void FillBoxMinMax(Vector& vecMin, Vector& vecMax, bool resize = false);

public:
	CZoneEditProperty m_ZoneEdit;
};

class CSurfZonePlugin : CMovementForward, CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CPlayerButton* buttons, const float (&vec)[3], const QAngle& viewAngles,
									const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;

public:
	std::optional<ZoneCache_t> FindZone(CBaseEntity* pEnt);
	int GetZoneCount(ZoneTrack track, ZoneType type);

public:
	static std::string GetZoneNameByTrack(ZoneTrack track);
	static std::string GetZoneNameByType(ZoneType type);

private:
	void RegisterCommand();

public:
	std::unordered_map<CZoneHandle, ZoneCache_t> m_hZones;
	Vector m_vecTestStartZone = {0.0f, 0.0f, 0.0f};
};

namespace SURF {
	CSurfZonePlugin* ZonePlugin();
} // namespace SURF
