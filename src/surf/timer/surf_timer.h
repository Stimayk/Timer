#pragma once

#include <surf/api.h>

class CSurfTimerPlugin : CSurfForward, CMovementForward, CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnPhysicsSimulatePost(CCSPlayerController* pController) override;
	virtual bool OnEnterZone(const ZoneCache_t& zone, CSurfPlayer* player) override;
	virtual bool OnStayZone(const ZoneCache_t& zone, CSurfPlayer* player) override;

private:
	void RegisterCommand();
};

class CSurfTimerService : CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;

	void DoTimerStart(bool playSound = true);
	void DoTimerEnd();

public:
	bool m_bTimerRunning {};
	f64 m_fCurrentTime {};
	f64 m_fLastEndTime {};
	f64 m_fLastFalseEndTime {};
	f64 m_fLastStartSoundTime {};
	f64 m_fLastMissedTimeSoundTime {};
	bool m_bValidTime {};

	i32 m_iCurrentStage {};

	bool m_bPaused {};
};

namespace SURF {
	extern CSurfTimerPlugin* TimerPlugin();

	void FormatTime(f64 time, char* output, u32 length, bool precise = true);
	CUtlString FormatTime(f64 time, bool precise = true);
	void FormatDiffTime(f64 time, char* output, u32 length, bool precise = true);
	CUtlString FormatDiffTime(f64 time, bool precise = true);
} // namespace SURF
