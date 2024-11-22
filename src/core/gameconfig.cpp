#include "gameconfig.h"
#include <fstream>
#include <libmem/libmem_helper.h>
#include <memaddr.h>
#include <utils/utils.h>

using namespace libmodule;

CGameConfig::CGameConfig(std::string sFilePath) : m_Json(nullptr), m_sFilePath(sFilePath) {
	std::string sPath = UTIL::PATH::Join(UTIL::GetWorkingDirectory(), "gamedata", sFilePath);
	m_Json = UTIL::LoadJsonc(sPath);
	SURF_ASSERT(!m_Json.empty());
}

int CGameConfig::GetOffset(std::string name) {
	if (m_Json.is_null()) {
		SURF_ASSERT(false);
		return -1;
	}

	if (m_Json.find("Offset") == m_Json.end()) {
		SURF_ASSERT(false);
		return -1;
	}

	auto& offset = m_Json["Offset"];
	if (offset.is_null() || offset.empty()) {
		SURF_ASSERT(false);
		return -1;
	}

	auto& element = offset[name];
	if (element.is_null() || element.empty()) {
		SURF_ASSERT(false);
		return -1;
	}

	return element[WIN_LINUX("windows", "linux")].get<int>();
}

void* CGameConfig::GetMemSig(std::string name) {
	if (m_pMemSig.count(name)) {
		return m_pMemSig[name];
	}

	if (m_Json.is_null() || m_Json.empty()) {
		SURF_ASSERT(false);
		return nullptr;
	}

	if (m_Json.find("Signature") == m_Json.end()) {
		SURF_ASSERT(false);
		return nullptr;
	}

	auto& signature = m_Json["Signature"];
	if (signature.is_null() || signature.empty()) {
		SURF_ASSERT(false);
		return nullptr;
	}

	auto& element = signature[name];
	if (element.is_null() || element.empty()) {
		SURF_ASSERT(false);
		return nullptr;
	}

	auto lib = MODULE_PREFIX + element["library"].get<std::string>() + MODULE_EXT;
	auto sig = element[WIN_LINUX("windows", "linux")].get<std::string>();
	auto addr = libmem::SignScan(sig.c_str(), lib.c_str());
	SURF_ASSERT(addr);
	m_pMemSig[name] = addr;
	return addr;
}

void* CGameConfig::GetAddress(std::string name) {
	if (m_Json.is_null() || m_Json.empty()) {
		SURF_ASSERT(false);
		return nullptr;
	}

	if (m_Json.find("Addresses") == m_Json.end()) {
		SURF_ASSERT(false);
		return nullptr;
	}

	auto& address = m_Json["Addresses"];
	if (address.is_null() || address.empty()) {
		SURF_ASSERT(false);
		return nullptr;
	}

	auto& element = address[name];
	if (element.is_null() || element.empty()) {
		SURF_ASSERT(false);
		return nullptr;
	}

	auto signature = element["signature"].get<std::string>();
	auto base = this->GetMemSig(signature.c_str());
	if (!base) {
		SURF_ASSERT(false);
		return nullptr;
	}

	auto& offset = element[WIN_LINUX("windows", "linux")];
	if (offset.is_null() || offset.empty()) {
		SURF_ASSERT(false);
		return nullptr;
	}

	auto dereference = offset["dereference"].get<bool>();
	auto offset_func = offset["offset_func"].get<int>();
	auto offset_opcode = offset["offset_opcode"].get<int>();
	auto opcode_length = offset["opcode_length"].get<int>();

	CMemory addr = CMemory(base);
	addr.OffsetSelf(offset_func);
	addr.ResolveRelativeAddressSelf(offset_opcode, opcode_length);
	if (dereference) {
		addr.DerefSelf();
	}

	SURF_ASSERT(addr.GetPtr());

	return addr.RCast<void*>();
}
