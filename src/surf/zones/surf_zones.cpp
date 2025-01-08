#include "surf_zones.h"
#include <utils/utils.h>
#include <surf/api.h>
#include <core/sdkhook.h>

bool Trigger_OnStartTouch(CBaseEntity* pSelf, CBaseEntity* pOther);
void Trigger_OnStartTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther);
void Trigger_OnTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther);
void Trigger_OnEndTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther);

#define _ZONE_DEBUG

CSurfZonePlugin g_SurfZonePlugin;

CSurfZonePlugin* SURF::ZonePlugin() {
	return &g_SurfZonePlugin;
}

void CSurfZonePlugin::OnPluginStart() {
	RegisterCommand();
}

void CSurfZonePlugin::OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CPlayerButton* buttons, const float (&vec)[3], const QAngle& viewAngles,
										 const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pawn);
	if (!player) {
		return;
	}

	player->m_pZoneService->EditZone(pawn, buttons);
}

std::optional<ZoneCache_t> CSurfZonePlugin::FindZone(CBaseEntity* pEnt) {
	auto handle = pEnt->GetRefEHandle();
	if (m_hZones.contains(handle)) {
		return m_hZones.at(handle);
	}

	return std::nullopt;
}

int CSurfZonePlugin::GetZoneCount(ZoneTrack track, ZoneType type) {
	int count = 0;
	for (const auto& pair : m_hZones) {
		const auto& cache = pair.second;
		if (cache.m_iTrack == track && cache.m_iType == type) {
			count++;
		}
	}

	return count;
}

std::string CSurfZonePlugin::GetZoneNameByTrack(ZoneTrack track) {
	switch (track) {
		case ZoneTrack::Track_Main:
			return "主线";
		case ZoneTrack::Track_Bonus:
			return "奖励";
	}

	return "undefined track";
}

std::string CSurfZonePlugin::GetZoneNameByType(ZoneType type) {
	switch (type) {
		case Zone_Start:
			return "起点";
		case Zone_End:
			return "终点";
		case Zone_Stage:
			return "关卡";
		case Zone_Checkpoint:
			return "检查点";
		case Zone_Stop:
			return "停止";
		case Zone_Teleport:
			return "传送区域";
		case Zone_Mark:
			return "标记区域";
		default:
			return "未知类型";
	}
}

void CSurfZoneService::AddZone(const Vector& vecMin, const Vector& vecMax) {
	auto pZone = this->CreateNormalZone(vecMin, vecMax);
	CZoneHandle hRefZone = pZone->GetRefEHandle();

	Vector mins(vecMin), maxs(vecMax);
	FillBoxMinMax(mins, maxs);
	ZoneCache_t cache;
	this->CreateZone(mins, maxs, cache.m_aBeams);
	cache.m_iTrack = m_ZoneEdit.m_iTrack;
	cache.m_iType = m_ZoneEdit.m_iType;
	cache.m_iValue = m_ZoneEdit.m_iValue;

	SURF::ZonePlugin()->m_hZones[hRefZone] = cache;
}

void CSurfZoneService::EditZone(CCSPlayerPawnBase* pawn, const CPlayerButton* buttons) {
	auto& iEditStep = m_ZoneEdit.m_iStep;
	if (m_ZoneEdit.m_bEnabled) {
		trace_t tr;
		UTIL::GetPlayerAiming(pawn, tr);
		Vector& aim = tr.m_vEndPos;

		if (buttons->Pressed(IN_USE)) {
			iEditStep = (ZoneEditStep)(iEditStep + 1);

			m_ZoneEdit.CreateEditZone(aim);
		}

		m_ZoneEdit.UpdateZone(aim);
	}
}

void CSurfZoneService::CreateZone(const Vector& vecMin, const Vector& vecMax, std::array<CHandle<CBeam>, 12>& out) {
	Vector points[8];
	CreatePoints3D(vecMin, vecMax, points);
	for (int i = 0; i < 12; i++) {
		CBeam* beam = (CBeam*)UTIL::CreateBeam(points[CZoneEditProperty::m_iZonePairs3D[i][0]], points[CZoneEditProperty::m_iZonePairs3D[i][1]]);
		out[i] = beam->GetRefEHandle();
	}
}

CBaseEntity* CSurfZoneService::CreateNormalZone(const Vector& vecMins, const Vector& vecMaxs) {
	Vector vecCenter = (vecMins + vecMaxs) / 2.0;
	if (g_SurfZonePlugin.m_vecTestStartZone.Length() == 0) {
		g_SurfZonePlugin.m_vecTestStartZone = vecCenter;
	}
	Vector mins(vecMins), maxs(vecMaxs);
	FillBoxMinMax(mins, maxs, true);
	auto pZone = MEM::CALL::CreateAABBTrigger(vecCenter, mins, maxs);
	if (!pZone) {
		SURF_ASSERT(false);
		return nullptr;
	}

	SDKHOOK::HookEntity<SDKHook_StartTouch>(pZone, Trigger_OnStartTouch);
	SDKHOOK::HookEntity<SDKHook_StartTouchPost>(pZone, Trigger_OnStartTouchPost);
	SDKHOOK::HookEntity<SDKHook_TouchPost>(pZone, Trigger_OnTouchPost);
	SDKHOOK::HookEntity<SDKHook_EndTouchPost>(pZone, Trigger_OnEndTouchPost);

	pZone->m_pEntity->m_name = GameEntitySystem()->AllocPooledString("surf_zone");

	return pZone;
}

void CSurfZoneService::CreatePoints2D(const Vector& vecMin, const Vector& vecMax, Vector out[4]) {
	out[0] = Vector(vecMin.x, vecMin.y, vecMin.z);
	out[1] = Vector(vecMax.x, vecMin.y, vecMin.z);
	out[2] = Vector(vecMax.x, vecMax.y, vecMin.z);
	out[3] = Vector(vecMin.x, vecMax.y, vecMin.z);
}

void CSurfZoneService::CreatePoints3D(const Vector& vecMin, const Vector& vecMax, Vector out[8]) {
	out[0] = vecMin;
	out[7] = vecMax;

	for (int i = 1; i < 7; i++) {
		out[i].x = (i & 4) ? vecMax.x : vecMin.x;
		out[i].y = (i & 2) ? vecMax.y : vecMin.y;
		out[i].z = (i & 1) ? vecMax.z : vecMin.z;
	}
}

void CSurfZoneService::FillBoxMinMax(Vector& vecMin, Vector& vecMax, bool resize) {
	if (vecMin.x > vecMax.x) {
		std::swap((float&)vecMin.x, (float&)vecMax.x);
	}
	if (vecMin.y > vecMax.y) {
		std::swap((float&)vecMin.y, (float&)vecMax.y);
	}
	if (vecMin.z > vecMax.z) {
		std::swap((float&)vecMin.z, (float&)vecMax.z);
	}

	if (!resize) {
		return;
	}

	// Calculate the size of the original bounding box
	Vector size(vecMax.x - vecMin.x, vecMax.y - vecMin.y, vecMax.z - vecMin.z);

	// Generate new mins and maxs centered at (0, 0, 0)
	vecMin.x = -size.x / 2;
	vecMin.y = -size.y / 2;
	vecMin.z = -size.z / 2;

	vecMax.x = size.x / 2;
	vecMax.y = size.y / 2;
	vecMax.z = size.z / 2;
}

void CSurfZoneService::Reset() {
	m_ZoneEdit.Init(this);
}

bool Trigger_OnStartTouch(CBaseEntity* pSelf, CBaseEntity* pOther) {
	if (!pOther->IsPawn()) {
		return true;
	}

	// if not our zone, ignore endtouch fix
	auto res = SURF::ZonePlugin()->FindZone(pSelf);
	if (!res.has_value()) {
		return true;
	}

	auto player = SURF::GetPlayerManager()->ToPlayer((CBasePlayerPawn*)pOther);
	if (!player) {
		return true;
	}

	if (player->m_bJustTeleported) {
		auto hTrigger = pSelf->GetRefEHandle();
		auto hPlayer = pOther->GetRefEHandle();
		UTIL::RequestFrame([hTrigger, hPlayer] {
			auto pTrigger = (CBaseEntity*)hTrigger.Get();
			if (!pTrigger) {
				return;
			}
			auto pPlayer = (CBaseEntity*)hPlayer.Get();
			if (!pPlayer) {
				return;
			}

			Trigger_OnStartTouchPost(pTrigger, pPlayer);
		});
		return false;
	}

	return true;
}

void Trigger_OnStartTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther) {
	if (!pOther->IsPawn()) {
		return;
	}

	auto res = SURF::ZonePlugin()->FindZone(pSelf);
	if (!res.has_value()) {
		return;
	}

	auto player = SURF::GetPlayerManager()->ToPlayer((CBasePlayerPawn*)pOther);
	if (!player) {
		return;
	}

	auto& zone = res.value();
	FORWARD_POST(CSurfForward, OnEnterZone, zone, player);

#ifdef _ZONE_DEBUG
	UTIL::PrintChatAll("start touch! self: %s, other: %s\n", pSelf->m_pEntity->m_name, pOther->GetClassname());
#endif
}

void Trigger_OnTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther) {
	if (!pOther->IsPawn()) {
		return;
	}

	auto res = SURF::ZonePlugin()->FindZone(pSelf);
	if (!res.has_value()) {
		return;
	}

	auto player = SURF::GetPlayerManager()->ToPlayer((CBasePlayerPawn*)pOther);
	if (!player) {
		return;
	}

	auto& zone = res.value();
	FORWARD_POST(CSurfForward, OnStayZone, zone, player);
}

void Trigger_OnEndTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther) {
	if (!pOther->IsPawn()) {
		return;
	}

	auto res = SURF::ZonePlugin()->FindZone(pSelf);
	if (!res.has_value()) {
		return;
	}

	auto player = SURF::GetPlayerManager()->ToPlayer((CBasePlayerPawn*)pOther);
	if (!player) {
		return;
	}

	auto& zone = res.value();
	FORWARD_POST(CSurfForward, OnLeaveZone, zone, player);

#ifdef _ZONE_DEBUG
	UTIL::PrintChatAll("end touch!\n");
#endif
}
