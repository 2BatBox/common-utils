#ifndef IPFILTER_H
#define IPFILTER_H

#include "../cache/LinkedHashPool.h"
#include "../dpdk/Allocator.h"
#include "procotols/IPv4.h"
#include "parsers/StackHdrParser.h"

#include <cstdint>
#include <rte_mbuf.h>

namespace stack_ip {

class IpFilter {
public:

	enum class Mode {
		SRC, // filtering by source address
		DST, // filtering by destination address
		SRC_DST, // filtering by source and destination address
		NONE, // no IPv4 packets are allowed except src=0.0.0.0 and\or dst=0.0.0.0
		ANY // offload all the traffic
	};

private:
	using IPv4Addr_t = stack_ip::IPv4::Addr;
	using Node_t = cache::LinkedHashPoolEmptyNode<IPv4Addr_t>;
	using Pool_t = cache::LinkedHashPool<
		Node_t,
		std::hash<IPv4Addr_t>,
		dpdk::Allocator<Node_t>,
		dpdk::Allocator<intrusive::MapBucket<Node_t> >
		>;

	using Iterator_t = typename Pool_t::Iterator_t;

	Pool_t m_pool;
	Mode m_mode;

public:

	IpFilter(unsigned capacity, float load_factor) noexcept :
	m_pool(capacity, load_factor) { };

	int allocate(Mode mode) noexcept {
		m_mode = mode;
		return m_pool.allocate();
	}

	uint16_t process(rte_mbuf** input, const uint16_t len) noexcept {
		static unsigned counter = 0;
		uint16_t result = 0;
		for (uint16_t i = 0; i < len; i++) {
			if (check_pkt(input[i])) {
				input[result++] = input[i];
			} else {
				rte_pktmbuf_free(input[i]);
			}

			counter++;
		}
		return result;
	}

	/**
	 * @param ip_addr - IP address in host byte order
	 */
	void lock(IPv4Addr_t ip_addr) noexcept {
		bool key_collision;
		m_pool.push_back(ip_addr, key_collision);
		assert(not key_collision); //TODO:
	}

	/**
	 * @param ip_addr - IP address in host byte order
	 */
	void unlock(IPv4Addr_t ip_addr) noexcept {
		auto it = m_pool.find(ip_addr);
		assert(it); //TODO:
		m_pool.remove(it);
	}

	size_t size() const noexcept {
		return m_pool.size();
	}

private:

	bool check_pkt(rte_mbuf* input) const noexcept {
		stack_ip::StackHdrParser pkt(binio::as_const_area(rte_ctrlmbuf_data(input), input->data_len));
		const stack_ip::IPv4::Header* ip_hdr;

		if (pkt.parse()) {
			stack_ip::Protocol proto = pkt.protocol();

			unsigned spin_guard = 0;
			while (proto != stack_ip::Protocol::END) {
				assert(spin_guard++ < 1000); //TODO: 

				switch (proto) {
				case stack_ip::Protocol::L3_IPv4:
					pkt.assign_stay(ip_hdr);
					return check_ip(
						ntohl(ip_hdr->saddr),
						ntohl(ip_hdr->daddr)
						);

				default:
					break;
				}

				proto = pkt.next();
			}
		}
		return true;
	}

	inline bool check_ip(uint32_t src, uint32_t dst) const noexcept {
		if (src == 0 || dst == 0) {
			return true;
		}

		switch (m_mode) {
		case Mode::SRC:
			return (m_pool.find(src) == m_pool.end());

		case Mode::DST:
			return (m_pool.find(dst) == m_pool.end());

		case Mode::SRC_DST:
			return (
				m_pool.find(src) == m_pool.end()
				&& m_pool.find(dst) == m_pool.end()
				);

		case Mode::NONE:
			return false;

		case Mode::ANY:
			return true;
		}

		return true;
	}

};

}; // namespace stack_ip

#endif /* IPFILTER_H */

