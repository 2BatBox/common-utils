#ifndef STACK_IP_VLAN_FILTER_H
#define STACK_IP_VLAN_FILTER_H

#include "procotols/Vlan.h"

#include <arpa/inet.h>
#include <vector>
#include <stdexcept>
#include <cstdint>

namespace stack_ip {

class VlanFilter {
	static constexpr size_t TABLE_SIZE = 1 << 12; // 12 bit wide
	std::vector<bool> m_table;

public:

	enum class Mode {
		ANY, // any VLAN packages are allowed
		INCLUDE_LIST, // none VLAN packages are allowed except the include list ids
		EXCLUDE_LIST, // any VLAN packages are allowed except the exclude list ids
		NONE // none VLAN packages are allowed
	};

	VlanFilter() noexcept : m_table(TABLE_SIZE) {}

	void set_mode(Mode mode, const std::vector<uint16_t>& vlan_list) noexcept {
		switch(mode) {
			case Mode::ANY:
				allow_by_default(true);
				break;

			case Mode::INCLUDE_LIST:
				allow_by_default(false);
				for(auto elem : vlan_list) {
					set(elem, true);
				}
				break;

			case Mode::EXCLUDE_LIST:
				allow_by_default(true);
				for(auto elem : vlan_list) {
					set(elem, false);
				}
				break;

			case Mode::NONE:
				allow_by_default(false);
				break;
		}
	}

	/**
	 * @param index - VLAN id in host byte order
	 * @param state - true if VLAN id @index is allowed
	 */
	inline void set(uint16_t index, bool state) throw(std::out_of_range) {
		m_table.at(index) = state;
	}

	/**
	 * @param index - VLAN id in host byte order
	 * @return - true if VLAN id @index is allowed
	 */
	inline bool check(uint16_t index) throw(std::out_of_range) {
		return m_table.at(index); // using bounds checking on purpose
	}

	/**
	 * @param hdr - VLAN header
	 * @return - true if VLAN id @index is allowed
	 */
	inline bool check(const Vlan::Header* hdr) throw(std::out_of_range) {
		uint16_t vlan_tci;
		vlan_tci = ntohs(hdr->vlan_tci);
		vlan_tci = vlan_tci & 0xfff;
		return m_table.at(vlan_tci); // using bounds checking on purpose
	}

	static const char* mode_name(Mode mode) noexcept {
		switch(mode) {
			case Mode::ANY:
				return "Any";
			case Mode::EXCLUDE_LIST:
				return "exclude list";
			case Mode::INCLUDE_LIST:
				return "include list";
			case Mode::NONE:
				return "none";
			default:
				return "unknown";
		}

	}

private:

	void allow_by_default(bool state) noexcept {
		for(auto elem : m_table) {
			elem = state;
		}
	}

};

}; // namespace stack_ip

#endif /* STACK_IP_VLAN_FILTER_H */

