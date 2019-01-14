#ifndef STORAGE_TESTS_TESTIPTABLE_H
#define STORAGE_TESTS_TESTIPTABLE_H

#include "../IpTable.h"

#include <cstdio>
#include <assert.h>
#include <iostream>
#include <thread>

namespace storage {

class TestIpTable {

	IpTable m_table;
	const size_t m_capacity;

public:

	TestIpTable(unsigned capacity, float load_factor) noexcept
		: m_table(capacity, load_factor, capacity)
		, m_capacity(capacity) {
		assert(m_table.allocate() == 0);
	}

	TestIpTable(const TestIpTable&) = delete;
	TestIpTable(TestIpTable&&) = delete;

	TestIpTable operator=(const TestIpTable&) = delete;
	TestIpTable operator=(TestIpTable&&) = delete;

	~TestIpTable() {}

	void test() noexcept {
		printf("<TestIpTable>...\n");
		printf("capacity_addr=%zu\n", m_capacity);
		printf("storage_bytes=%.2f Kb\n", m_table.storage_bytes() / (float) 1024.0);
		printf("sizeof(IpTable::NodeAddr_t)=%zu\n", sizeof(IpTable::NodeAddr_t));
		printf("sizeof(IpTable::NodeNet_t)=%zu\n", sizeof(IpTable::NodeNet_t));

		unsigned step = 1;
		test_check_addr(step++);
		test_check_addr_odd_even(step++);
		test_check_net(step++);
		test_check_net_special(step++);
		check_empty();
	}

private:

	void test_check_addr(unsigned step) noexcept {
		printf("-> test_check_addr(step=%u)\n", step);
		check_empty();

		IpTable::IPv4Addr_t ip_start = IpTable::as_host_addr(192, 168, 1, 0);
		IpTable::IPv4Addr_t ip_end = ip_start + m_capacity;
		assert((~IpTable::IPv4Addr_t(0)) - m_capacity >= ip_start);

		for(IpTable::IPv4Addr_t ip = ip_start; ip < ip_end; ip++) {
			assert(not m_table.find_in_addrs(ip));
			m_table.append_addr(ip);
		}

		for(IpTable::IPv4Addr_t ip = ip_start; ip < ip_end; ip++) {
			assert(m_table.find_in_addrs(ip));
			m_table.remove_addr(ip);
		}

		for(IpTable::IPv4Addr_t ip = ip_start; ip < ip_end; ip++) {
			assert(not m_table.find_in_addrs(ip));
		}
	}

	void test_check_addr_odd_even(unsigned step) noexcept {
		printf("-> test_check_addr_odd_even(step=%u)\n", step);
		check_empty();

		IpTable::IPv4Addr_t ip_start = IpTable::as_host_addr(192, 168, 1, 0);
		IpTable::IPv4Addr_t ip_end = ip_start + m_capacity;
		assert((~IpTable::IPv4Addr_t(0)) - m_capacity >= ip_start);

		for(IpTable::IPv4Addr_t ip = ip_start; ip < ip_end; ip++) {
			assert(not m_table.find(ip));
			if(ip % 2) {
				m_table.append_addr(ip);
			}
		}

		for(IpTable::IPv4Addr_t ip = ip_start; ip < ip_end; ip++) {
			if(ip % 2) {
				assert(m_table.find(ip));
				m_table.remove_addr(ip);
			}
		}

		for(IpTable::IPv4Addr_t ip = ip_start; ip < ip_end; ip++) {
			assert(not m_table.find(ip));
		}
	}

	void test_check_net(unsigned step) noexcept {
		printf("-> test_check_net(step=%u)\n", step);
		check_empty();

		IpTable::IPv4Addr_t network = IpTable::as_host_addr(192, 168, 1, 0);
		IpTable::IPv4Addr_t mask = IpTable::as_host_addr(255, 255, 255, 0);
		m_table.append_net(network, mask);

		IpTable::IPv4Addr_t ip_start = IpTable::as_host_addr(192, 168, 0, 0);
		IpTable::IPv4Addr_t ip_end = IpTable::as_host_addr(192, 168, 0, 255);
		for(IpTable::IPv4Addr_t ip = ip_start; ip < ip_end; ++ip) {
			assert(not m_table.find(ip));
		}

		ip_start = IpTable::as_host_addr(192, 168, 1, 0);
		ip_end = IpTable::as_host_addr(192, 168, 1, 255);
		for(IpTable::IPv4Addr_t ip = ip_start; ip < ip_end; ++ip) {
			assert(m_table.find(ip));
		}

		ip_start = IpTable::as_host_addr(192, 168, 2, 0);
		ip_end = IpTable::as_host_addr(192, 168, 2, 255);
		for(IpTable::IPv4Addr_t ip = ip_start; ip < ip_end; ++ip) {
			assert(not m_table.find(ip));
		}

		m_table.remove_net(network, mask);
	}

	void test_check_net_special(unsigned step) noexcept {
		printf("-> test_check_net_special(step=%u)\n", step);
		check_empty();

		IpTable::IPv4Addr_t network_empty = IpTable::as_host_addr(0, 0, 0, 0);
		IpTable::IPv4Addr_t network_full = IpTable::as_host_addr(255, 255, 255, 255);
		IpTable::IPv4Addr_t mask = IpTable::as_host_addr(255, 255, 255, 255);
		m_table.append_net(network_empty, mask);
		m_table.append_net(network_full, mask);

		assert(m_table.find(IpTable::as_host_addr(0, 0, 0, 0)));
		assert(m_table.find(IpTable::as_host_addr(255, 255, 255, 255)));

		assert(not m_table.find(IpTable::as_host_addr(1, 0, 0, 0)));
		assert(not m_table.find(IpTable::as_host_addr(0, 1, 0, 0)));
		assert(not m_table.find(IpTable::as_host_addr(0, 0, 1, 0)));
		assert(not m_table.find(IpTable::as_host_addr(0, 0, 0, 1)));

		assert(not m_table.find(IpTable::as_host_addr(255, 0, 0, 0)));
		assert(not m_table.find(IpTable::as_host_addr(0, 255, 0, 0)));
		assert(not m_table.find(IpTable::as_host_addr(0, 0, 255, 0)));
		assert(not m_table.find(IpTable::as_host_addr(0, 0, 0, 255)));

		m_table.remove_net(network_empty, mask);
		m_table.remove_net(network_full, mask);
	}

	void check_empty() const noexcept {
		assert(m_table.size_addr() == 0);
		assert(m_table.size_net() == 0);
		assert(m_table.available_addr() == m_capacity);
		assert(m_table.available_net() == m_capacity);
		assert(m_table.capacity_addr() == m_capacity);
		assert(m_table.capacity_net() == m_capacity);
	}

	static void print_ip_host(const char* name, IpTable::IPv4Addr_t ip) noexcept {
		printf("%s=", name);
		printf("%01u.%01u.%01u.%01u (0x%8x)\n", ((ip >> 24) & 0xFF), ((ip >> 16) & 0xFF), ((ip >> 8) & 0xFF),
		       ((ip) & 0xFF), ip);
	}

};

}; // namespace storage

#endif /* STORAGE_TESTS_TESTIPTABLE_H */

