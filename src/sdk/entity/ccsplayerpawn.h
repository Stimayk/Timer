#pragma once

#include "cbaseplayerpawn.h"

class CCSPlayer_ViewModelServices;
class CCSBot;

class CCSPlayerPawnBase : public CBasePlayerPawn {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayerPawnBase);

	SCHEMA_FIELD(CCSPlayer_ViewModelServices*, m_pViewModelServices);
	SCHEMA_FIELD(QAngle, m_angEyeAngles);

	Vector GetEyePosition();
};

class CCSPlayerPawn : public CCSPlayerPawnBase {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayerPawn);
	SCHEMA_FIELD(float, m_ignoreLadderJumpTime);
	SCHEMA_FIELD(float, m_flSlopeDropOffset);
	SCHEMA_FIELD(float, m_flSlopeDropHeight);
	SCHEMA_FIELD(float, m_flVelocityModifier);
	SCHEMA_FIELD(CCSBot*, m_pBot);

public:
	template<typename T = CBasePlayerController>
	T* GetController() {
		return (T*)(m_hController()->Get());
	}

	void Respawn() {
		CALL_VIRTUAL(void, GAMEDATA::GetOffset("Respawn"), this);
	}
};

class CBot {
public:
	DECLARE_SCHEMA_CLASS(CBot);

	SCHEMA_FIELD(uint64_t, m_buttonFlags);
};

class CCSBot : public CBot {
public:
	DECLARE_SCHEMA_CLASS(CCSBot);
};
