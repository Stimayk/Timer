#pragma once

#include <tier0/logging.h>
#include <cstdint>
#include <type_traits>

namespace LOG {
	inline Color g_iLogColor;

	void Setup(Color color);
	void Setup(uint32_t color32);

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) == 0, void> Print(const char* fmt) {
		::ConColorMsg(g_iLogColor, "%s\n", fmt);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) != 0, void> Print(const char* fmt, Args... args) {
		::ConColorMsg(g_iLogColor, fmt, args...);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) == 0, void> Warning(const char* fmt) {
		::Warning("%s\n", fmt);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) != 0, void> Warning(const char* fmt, Args... args) {
		::Warning(fmt, args...);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) == 0, void> Error(const char* fmt) {
		::Error("%s\n", fmt);
	}

	template<typename... Args>
	std::enable_if_t<sizeof...(Args) != 0, void> Error(const char* fmt, Args... args) {
		::Error(fmt, args...);
	}
} // namespace LOG
