#ifndef DPDK_PORTSTATS_H
#define DPDK_PORTSTATS_H

#include "../utils/Colors.h"

#include <cstdint>
#include <vector>
#include <rte_ethdev.h>

namespace dpdk {

class PortStats {
#ifndef PRINT_FLAG
#define PRINT_FLAG(message, value, flag) { \
    if((flag & value) == flag) { \
        fprintf(file, message #flag); \
        fprintf(file, "\n"); \
    }};
#endif

public:

	static constexpr uint64_t FACTOR_KILO = 1024;
	static constexpr uint64_t FACTOR_MEGA = FACTOR_KILO * FACTOR_KILO;
	static constexpr uint64_t FACTOR_GIGA = FACTOR_MEGA * FACTOR_KILO;

	static void print_port_stat(FILE* out, uint8_t port, bool rx = true, bool tx = true) {
		rte_eth_stats stat;
		rte_eth_stats_get(port, &stat);
		if(rx) {
			fprintf(out, "%sipackets    : %lu\n", utils::Colors::GREEN, stat.ipackets);
			fprintf(out, "%sibytes      : %lu\n", utils::Colors::GREEN, stat.ibytes);
		}
		if(tx) {
			fprintf(out, "%sopackets    : %lu\n", utils::Colors::GREEN, stat.opackets);
			fprintf(out, "%sobytes      : %lu\n", utils::Colors::GREEN, stat.obytes);
		}

		if(rx) {
			fprintf(out, "%simissed     : %lu\n", utils::Colors::RED, stat.imissed);
			fprintf(out, "%sierrors     : %lu\n", utils::Colors::RED, stat.ierrors);
			fprintf(out, "%srx_nombuf   : %lu%s\n", utils::Colors::RED, stat.rx_nombuf, utils::Colors::NORMAL);
		}
		if(tx) {
			fprintf(out, "%soerrors     : %lu%s\n", utils::Colors::RED, stat.oerrors, utils::Colors::NORMAL);
		}
	}

	static void print_port_stat_extended(FILE* out, uint8_t port) {
		std::vector<rte_eth_xstat_name> names;
		std::vector<rte_eth_xstat> values;

		int size = rte_eth_xstats_get_names(port, nullptr, 0);
		if(size > 0) {

			names.resize(size);
			values.resize(size);

			int ret = rte_eth_xstats_get_names(port, &names.front(), size);
			if(ret > 0) {

				int ret = rte_eth_xstats_get(port, &values.front(), names.size());
				if(ret > 0 && (unsigned) ret == names.size()) {
					for(unsigned i = 0; i < names.size(); i++) {
						const char* mode = get_mode(names[i].name);
						fprintf(out, "%s%30s:\t%lu\n", mode, names[i].name, values[i].value);
					}
					fprintf(out, "%s\n", utils::Colors::NORMAL);
				}
			}
		}
	}

	static void get_io_rate(uint8_t port, rte_eth_stats& delta) noexcept {
		static rte_eth_stats previous[RTE_MAX_ETHPORTS];
		rte_eth_stats current;
		rte_eth_stats_get(port, &current);

		delta.ipackets = current.ipackets - previous[port].ipackets;
		delta.opackets = current.opackets - previous[port].opackets;
		delta.ibytes = current.ibytes - previous[port].ibytes;
		delta.obytes = current.obytes - previous[port].obytes;
		delta.imissed = current.imissed - previous[port].imissed;
		delta.ierrors = current.ierrors - previous[port].ierrors;
		delta.oerrors = current.oerrors - previous[port].oerrors;
		delta.rx_nombuf = current.rx_nombuf - previous[port].rx_nombuf;
		previous[port] = current;
	}

	static void print_irate(FILE* out, rte_eth_stats& rate) {
		fprintf(out, "%s", utils::Colors::GREEN);
		dpdk::PortStats::print_factorize(out, rate.ibytes * 8, "bit ");
		fprintf(out, " | ");
		dpdk::PortStats::print_factorize(out, rate.ipackets, "pkt ");
		fprintf(out, "%s", utils::Colors::NORMAL);
	}

	static void print_orate(FILE* out, rte_eth_stats& rate) {
		fprintf(out, "%s", utils::Colors::GREEN);
		dpdk::PortStats::print_factorize(out, rate.obytes * 8, "bit ");
		fprintf(out, " | ");
		dpdk::PortStats::print_factorize(out, rate.opackets, "pkt ");
		fprintf(out, "%s", utils::Colors::NORMAL);
	}

	static void print_err(FILE* out, rte_eth_stats& rate) {
		fprintf(out, "%s", utils::Colors::RED);
		fprintf(out, "imissed=%lu pkt | ", rate.imissed);
		fprintf(out, "rx_nombuf=%lu pkt", rate.rx_nombuf);
		fprintf(out, "%s", utils::Colors::NORMAL);
	}

	static void link_status(FILE* file, uint8_t port) {
		rte_eth_link link;
		memset(&link, 0, sizeof(link));
		rte_eth_link_get(port, &link);

		fprintf(file, "link=%s ", (link.link_status ? "UP" : "DOWN"));
		fprintf(file, "speed=%u ", link.link_speed);
		fprintf(file, "duplex=%s ", (link.link_duplex ? "FULL" : "HALF"));
		fprintf(file, "autoneg=%s \n", (link.link_autoneg ? "AUTONEG" : "FIXED"));
	}

	static void eth_dev_info(FILE* file, uint8_t port) {
		rte_eth_dev_info info;
		rte_eth_dev_info_get(port, &info);

		fprintf(file, "==== rte_eth_dev_info (port=%u) \"rte_ethdev.h\" ====\n", port);
		fprintf(file, "\t driver_name: \"%s\"\n", info.driver_name);
		fprintf(file, "\t if_index: %d\n", info.if_index);
		fprintf(file, "\t min_rx_bufsize: %u\n", info.min_rx_bufsize);
		fprintf(file, "\t max_rx_pktlen: %u\n", info.max_rx_pktlen);
		fprintf(file, "\t max_rx_queues: %u\n", info.max_rx_queues);
		fprintf(file, "\t max_tx_queues: %u\n", info.max_tx_queues);
		fprintf(file, "\t max_mac_addrs: %u\n", info.max_mac_addrs);
		fprintf(file, "\t max_hash_mac_addrs: %u\n", info.max_hash_mac_addrs);
		fprintf(file, "\t max_vfs: %u\n", info.max_vfs);
		fprintf(file, "\t max_vmdq_pools: %u\n", info.max_vmdq_pools);
		fprintf(file, "\t reta_size: %u\n", info.reta_size);
		fprintf(file, "\t rx_offload_capa: 0x%x\n", info.rx_offload_capa);
		PRINT_FLAG("\t\t", info.rx_offload_capa, DEV_RX_OFFLOAD_VLAN_STRIP);
		PRINT_FLAG("\t\t", info.rx_offload_capa, DEV_RX_OFFLOAD_IPV4_CKSUM);
		PRINT_FLAG("\t\t", info.rx_offload_capa, DEV_RX_OFFLOAD_UDP_CKSUM);
		PRINT_FLAG("\t\t", info.rx_offload_capa, DEV_RX_OFFLOAD_TCP_CKSUM);
		PRINT_FLAG("\t\t", info.rx_offload_capa, DEV_RX_OFFLOAD_TCP_LRO);
		PRINT_FLAG("\t\t", info.rx_offload_capa, DEV_RX_OFFLOAD_QINQ_STRIP);
		PRINT_FLAG("\t\t", info.rx_offload_capa, DEV_RX_OFFLOAD_OUTER_IPV4_CKSUM);
		//        PRINT_FLAG("\t\t", info.rx_offload_capa, DEV_RX_OFFLOAD_MACSEC_STRIP);

		fprintf(file, "\t tx_offload_capa: 0x%x\n", info.tx_offload_capa);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_VLAN_INSERT);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_IPV4_CKSUM);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_UDP_CKSUM);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_TCP_CKSUM);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_SCTP_CKSUM);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_TCP_TSO);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_UDP_TSO);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_OUTER_IPV4_CKSUM);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_QINQ_INSERT);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_VXLAN_TNL_TSO);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_GRE_TNL_TSO);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_IPIP_TNL_TSO);
		PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_GENEVE_TNL_TSO);
		//        PRINT_FLAG("\t\t", info.tx_offload_capa, DEV_TX_OFFLOAD_MACSEC_INSERT);

		fprintf(file, "\t speed_capa: 0x%x\n", info.speed_capa);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_AUTONEG);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_FIXED);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_10M_HD);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_10M);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_100M_HD);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_100M);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_1G);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_2_5G);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_5G);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_10G);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_20G);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_25G);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_40G);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_50G);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_56G);
		PRINT_FLAG("\t\t", info.speed_capa, ETH_LINK_SPEED_100G);
	}

	static void print_port_mac(FILE* file, unsigned port) {
		ether_addr addr;
		rte_eth_macaddr_get(port, &addr);
		for(int i = 0; i < 6; i++) {
			fprintf(file, "%02x", addr.addr_bytes[i]);
			if(i < 5)
				fprintf(file, ".");
		}
	}

	template<typename T>
	static char factorize_value(T& value) noexcept {
		if(value > FACTOR_GIGA) {
			value /= FACTOR_GIGA;
			return 'G';
		} else if(value > FACTOR_MEGA) {
			value /= FACTOR_MEGA;
			return 'M';
		} else if(value > FACTOR_KILO) {
			value /= FACTOR_KILO;
			return 'K';
		} else {
			return 0;
		}
	}

	template<typename T>
	static void print_factorize(FILE* out, T input, const char* postfix) noexcept {
		float value = input;
		char factor = factorize_value(value);
		if(factor) {
			fprintf(out, "%.2f %c%s", value, factor, postfix);
		} else {
			fprintf(out, "%zu %s", uint64_t(value), postfix);
		}
	}

private:

	static const char* get_mode(const char* statName) {
		const char* mode = utils::Colors::GREEN;
		if(statName == nullptr || strlen(statName) == 0)
			return mode;
		if(strstr(statName, "error") || strstr(statName, "missed"))
			mode = utils::Colors::RED;
		return mode;
	}

};

}; // namespace dpdk

#endif /* DPDK_PORTSTATS_H */

