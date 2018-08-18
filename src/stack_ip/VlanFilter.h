#ifndef STACK_IP_VLAN_FILTER_H
#define STACK_IP_VLAN_FILTER_H

#include <vector>
#include <stdexcept>
#include <cstdint>

namespace stack_ip {

class VlanFilter {
	static constexpr size_t TABLE_SIZE = 1 << 12; // 12 bit of VLAN id
	std::vector<bool> m_table;
	
public:
	VlanFilter(bool allow_by_default) noexcept : m_table(TABLE_SIZE) {
		if(allow_by_default){
			for (auto elem : m_table) {
				elem = true;
            }
		}
	}
	
	inline bool set(uint16_t index, bool state) throw(std::out_of_range) {
		return m_table.at(index) = state;
	}
	
	inline bool check(uint16_t index) throw(std::out_of_range) {
		return m_table.at(index); // using bounds checking on purpose
	}
	
};


}; // namespace stack_ip

#endif /* STACK_IP_VLAN_FILTER_H */

