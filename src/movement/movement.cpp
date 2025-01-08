#include "movement.h"

FORWARD_INIT(CMovementForward);

CMovementPlayer* CMovementPlayerManager::ToPlayer(CServerSideClientBase* pClient) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(pClient));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CPlayerPawnComponent* component) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(component));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CBasePlayerController* controller) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(controller));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CBasePlayerPawn* pawn) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(pawn));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CPlayerSlot slot) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(slot));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CEntityIndex entIndex) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(entIndex));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CPlayerUserId userID) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(userID));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CSteamID steamid, bool validate) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(steamid, validate));
}

void CMovementPlayer::Reset() {
	CPlayer::Reset();
	this->landingVelocity.Init();
	this->jumped = false;
}

CCSPlayer_MovementServices* CMovementPlayer::GetMoveServices() {
	if (!this->GetPlayerPawn()) {
		return nullptr;
	}
	return static_cast<CCSPlayer_MovementServices*>(this->GetPlayerPawn()->m_pMovementServices());
}

void CMovementPlayer::GetBBoxBounds(bbox_t& bounds) {
	bounds.mins = {-16.0f, -16.0f, 0.0f};
	bounds.maxs = {16.0f, 16.0f, 72.0f};
	if (this->GetMoveServices() && this->GetMoveServices()->m_bDucked()) {
		bounds.maxs.z = 54.0f;
	}
}

void CMovementPlayer::RegisterTakeoff(bool jumped) {
	this->jumped = jumped;
}

void CMovementPlayer::RegisterLanding(const Vector& landingVelocity, bool distbugFix) {
	this->landingVelocity = landingVelocity;
}

void CMovementPlayer::GetOrigin(Vector& origin) {
	if (this->processingMovement && this->currentMoveData) {
		origin = this->currentMoveData->m_vecAbsOrigin;
	} else {
		CBasePlayerPawn* pawn = this->GetPlayerPawn();
		if (!pawn) {
			return;
		}

		origin = pawn->m_CBodyComponent()->m_pSceneNode()->m_vecAbsOrigin();
	}
}

void CMovementPlayer::SetOrigin(const Vector& origin) {
	if (this->processingMovement && this->currentMoveData) {
		this->currentMoveData->m_vecAbsOrigin = origin;
	} else {
		CBasePlayerPawn* pawn = this->GetPlayerPawn();
		if (!pawn) {
			return;
		}
		pawn->Teleport(&origin, NULL, NULL);
	}

	// Origin teleported, remove all errors.
	this->GetMoveServices()->m_flAccumulatedJumpError(0);
}

void CMovementPlayer::GetVelocity(Vector& velocity) {
	if (this->processingMovement && this->currentMoveData) {
		velocity = this->currentMoveData->m_vecVelocity;
	} else {
		CBasePlayerPawn* pawn = this->GetPlayerPawn();
		if (!pawn) {
			return;
		}
		velocity = pawn->m_vecAbsVelocity();
	}
}

void CMovementPlayer::SetVelocity(const Vector& velocity) {
	if (this->processingMovement && this->currentMoveData) {
		this->currentMoveData->m_vecVelocity = velocity;
	} else {
		CBasePlayerPawn* pawn = this->GetPlayerPawn();
		if (!pawn) {
			return;
		}
		pawn->Teleport(NULL, NULL, &velocity);
	}
}

void CMovementPlayer::GetAngles(QAngle& angles) {
	if (this->processingMovement && this->currentMoveData) {
		angles = this->currentMoveData->m_vecViewAngles;
	} else {
		angles = this->moveDataPost.m_vecViewAngles;
	}
}

void CMovementPlayer::SetAngles(const QAngle& angles) {
	CBasePlayerPawn* pawn = this->GetPlayerPawn();
	if (!pawn) {
		return;
	}

	// Don't change the pitch of the absolute angles because it messes with the player model.
	QAngle absAngles = angles;
	absAngles.x = 0;

	pawn->Teleport(NULL, &absAngles, NULL);
	MEM::CALL::SnapViewAngles(pawn, angles);
}
