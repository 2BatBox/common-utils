#ifndef IPTABLE_H
#define IPTABLE_H

#include "../stack_ip/procotols/IPv4.h"
#include "../intrusive_pool/HashQueuePool.h"
#include "../intrusive_pool/DequePool.h"
#include "../dpdk/Allocator.h"

#include <arpa/inet.h>

#include <cstdint>

namespace storage {

class IpTable {
	friend class TestIpTable;

public:
	using IPv4Addr_t = stack_ip::IPv4::Addr;
	struct IPv4Network_t {
		IPv4Addr_t network;
		IPv4Addr_t mask;
	};

private:
	using NodeAddr_t = intrusive::HashQueuePoolEmptyNode<IPv4Addr_t>;
	using PoolAddr_t = intrusive::HashQueuePool<
		NodeAddr_t,
		std::hash<IPv4Addr_t>,
		dpdk::Allocator<NodeAddr_t>,
		dpdk::Allocator<intrusive::HashMapBucket<NodeAddr_t> >
	>;

	using NodeNet_t = intrusive::DequePoolNode<IPv4Network_t>;
	using PoolNet_t = intrusive::DequePool<
		NodeNet_t,
		dpdk::Allocator<NodeNet_t>
	>;

	using Iterator_t = typename PoolNet_t::Iterator_t;

	PoolAddr_t m_pool_addr;
	PoolNet_t m_pool_net;

public:

	IpTable(unsigned capacity_addr, float load_factor, unsigned capacity_net) noexcept
		: m_pool_addr(capacity_addr, load_factor)
		, m_pool_net(capacity_net) {};

	int allocate() noexcept {
		return m_pool_addr.allocate() || m_pool_net.allocate();
	}

	/**
	* @param addr - an IP address.
	* @return true if the table contains addr.
	*/
	inline bool find(IPv4Addr_t addr) const noexcept {
		return find_in_addrs(addr) || find_in_nets(addr);
	}

	/**
	 * @param addr - an IP address.
	 * @return true if the table contains addr as an individual IP address.
	 */
	inline bool find_in_addrs(IPv4Addr_t addr) const noexcept {
		return m_pool_addr.find(addr) != m_pool_addr.cend();
	}

	/**
	* @param addr - an IP address.
	* @return true if the table contains addr as a network address range.
	*/
	inline bool find_in_nets(IPv4Addr_t addr) const noexcept {
		for(auto it = m_pool_net.cbegin(); it != m_pool_net.cend(); ++it){
			IPv4Addr_t network = addr & it->value.mask;
			if(network == it->value.network){
				return true;
			}
		}
		return false;
	}

	/**
	 * @param addr - an IP address.
	 */
	inline void append_addr(IPv4Addr_t addr) noexcept {
		if(not m_pool_addr.available()) {
			m_pool_addr.pop_front();
		}
		m_pool_addr.push_back(addr);
	}

	/**
	* @param addr - an IP address of the network.
	* @param addr - a mask of the network.
	*/
	inline void append_net(IPv4Addr_t net, IPv4Addr_t mask) noexcept {
		if(not m_pool_net.available()) {
			m_pool_net.pop_front();
		}
		auto it = m_pool_net.push_back();
		if(it){
			it->value.network = net;
			it->value.mask = mask;
		}
	}

	/**
	 * @param addr - an IP address.
	 */
	inline void remove_addr(IPv4Addr_t addr) noexcept {
		auto it = m_pool_addr.find(addr);
		if(it) {
			m_pool_addr.remove(it);
		}
	}

	/**
	* @param addr - an IP address of the network.
	* @param addr - a mask of the network.
	*/
	inline void remove_net(IPv4Addr_t net, IPv4Addr_t mask) noexcept {
		for(auto it = m_pool_net.begin(); it != m_pool_net.end(); ++it){
			if(net == it->value.network && mask == it->value.mask){
				m_pool_net.remove(it);
				return;
			}
		}
	}

	size_t size_addr() const noexcept {
		return m_pool_addr.size();
	}

	size_t size_net() const noexcept {
		return m_pool_net.size();
	}

	size_t available_addr() const noexcept {
		return m_pool_addr.available();
	}

	size_t available_net() const noexcept {
		return m_pool_net.available();
	}

	size_t capacity_addr() const noexcept {
		return m_pool_addr.capacity();
	}

	size_t capacity_net() const noexcept {
		return m_pool_net.capacity();
	}

	inline size_t storage_bytes() noexcept {
		return m_pool_addr.storage_bytes() + m_pool_net.storage_bytes();
	}

	static IPv4Addr_t as_host_addr(unsigned b0, unsigned b1, unsigned b2, unsigned b3) noexcept {
		IPv4Addr_t addr = 0;
		addr |= b0 & 0xFF;
		addr <<= 8;
		addr |= b1 & 0xFF;
		addr <<= 8;
		addr |= b2 & 0xFF;
		addr <<= 8;
		addr |= b3 & 0xFF;
		return addr;
	}

	inline static IPv4Addr_t as_net_addr(unsigned b0, unsigned b1, unsigned b2, unsigned b3) noexcept {
		return htonl(as_host_addr(b0, b1, b2, b3));
	}

	inline static IPv4Addr_t as_net_addr(IPv4Addr_t addr) noexcept {
		return htonl(addr);
	}

};

}; // namespace storage

#endif /* IPTABLE_H */

