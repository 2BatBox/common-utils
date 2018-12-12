#ifndef DPDK_PORT_H
#define DPDK_PORT_H

#include <rte_eal.h>
#include <rte_ethdev.h>

namespace dpdk {

class Port {
	// default configuration
	static constexpr uint8_t RX_PTHRESH = 8; /**< Default values of RX prefetch threshold reg. */
	static constexpr uint8_t RX_HTHRESH = 8; /**< Default values of RX host threshold reg. */
	static constexpr uint8_t RX_WTHRESH = 4; /**< Default values of RX write-back threshold reg. */

	static constexpr uint8_t TX_PTHRESH = 36; /**< Default values of TX prefetch threshold reg. */
	static constexpr uint8_t TX_HTHRESH = 4; /**< Default values of TX host threshold reg. */
	static constexpr uint8_t TX_WTHRESH = 4; /**< Default values of TX write-back threshold reg. */

	const uint8_t m_port_id;
	unsigned m_nb_rx_queues;
	unsigned m_nb_rx_descriptors;
	unsigned m_nb_tx_queues;
	unsigned m_nb_tx_descriptors;
	rte_mempool* m_rx_mem_pool;
	rte_eth_conf m_config;
	rte_eth_rxconf m_rx_config;
	rte_eth_txconf m_tx_config;

public:

	Port(uint8_t port) noexcept :
	m_port_id(port),
	m_nb_rx_queues(0),
	m_nb_rx_descriptors(0),
	m_nb_tx_queues(0),
	m_nb_tx_descriptors(0),
	m_rx_mem_pool(nullptr),
	m_config(default_eth_config()),
	m_rx_config(default_rx_conf()),
	m_tx_config(default_tx_conf()) { }

	uint8_t id() const noexcept {
		return m_port_id;
	}

	void set_rx_queues(unsigned nb_queues, unsigned nb_descriptors, rte_mempool* mem_pool) noexcept {
		m_nb_rx_queues = nb_queues;
		m_nb_rx_descriptors = nb_descriptors;
		m_rx_mem_pool = mem_pool;
	}

	void set_tx_queues(unsigned queues, unsigned queue_length) noexcept {
		m_nb_tx_queues = queues;
		m_nb_tx_descriptors = queue_length;
	}

	rte_eth_conf& config() noexcept {
		return m_config;
	}

	rte_eth_rxconf& rx_config() noexcept {
		return m_rx_config;
	}

	rte_eth_txconf& tx_config() noexcept {
		return m_tx_config;
	}

	int init() noexcept {
		int socket_id = rte_eth_dev_socket_id(m_port_id);

		if (m_port_id >= rte_eth_dev_count())
			return -1;

		int result = rte_eth_dev_configure(m_port_id, m_nb_rx_queues, m_nb_tx_queues, &m_config);
		if (result)
			return result;

		for (uint16_t queue = 0; queue < m_nb_rx_queues; queue++) {
			result = rte_eth_rx_queue_setup(m_port_id, queue, m_nb_rx_descriptors, socket_id, nullptr, m_rx_mem_pool);
			if (result < 0)
				return result;
		}

		for (uint16_t queue = 0; queue < m_nb_tx_queues; queue++) {
			result = rte_eth_tx_queue_setup(m_port_id, queue, m_nb_tx_descriptors, socket_id, nullptr);
			if (result < 0)
				return result;
		}

		return 0;
	}

	int start(bool promiscuous = true) noexcept {
		int result = rte_eth_dev_start(m_port_id);
		if (result)
			return result;

		if (promiscuous) {
			rte_eth_promiscuous_enable(m_port_id);
		}
		return result;
	}

private:

	static rte_eth_conf default_eth_config() noexcept {
		rte_eth_conf conf;
		memset(&conf, 0, sizeof (conf));
		rte_eth_rxmode* rxmode = &conf.rxmode;
		rxmode->max_rx_pkt_len = ETHER_MAX_LEN;
		return conf;
	}

	const struct rte_eth_conf* get_port_conf() noexcept {
		static rte_eth_conf result;
		memset(&result, 0, sizeof (result));
		result.rxmode.split_hdr_size = 0;
		result.rxmode.header_split = 0; /**< Header Split disabled */
		result.rxmode.hw_ip_checksum = 0; /**< IP checksum offload disable */
		result.rxmode.hw_vlan_filter = 0; /**< VLAN filtering disabled */
		result.rxmode.jumbo_frame = 0; /**< Jumbo Frame Support disabled */
		result.rxmode.hw_strip_crc = 0; /**< CRC stripped by hardware */
		result.txmode.mq_mode = ETH_MQ_TX_NONE;
		return &result;
	}

	const rte_eth_rxconf default_rx_conf() noexcept {
		static rte_eth_rxconf result;
		memset(&result, 0, sizeof (result));
		result.rx_thresh.pthresh = RX_PTHRESH;
		result.rx_thresh.hthresh = RX_HTHRESH;
		result.rx_thresh.wthresh = RX_WTHRESH;
		return result;
	}

	const rte_eth_txconf default_tx_conf() noexcept {
		static rte_eth_txconf result;
		memset(&result, 0, sizeof (result));
		result.tx_thresh.pthresh = TX_PTHRESH;
		result.tx_thresh.hthresh = TX_HTHRESH;
		result.tx_thresh.wthresh = TX_WTHRESH;
		result.tx_free_thresh = 0; /* Use PMD default values */
		result.tx_rs_thresh = 0; /* Use PMD default values */
		return result;
	}

};

}; // namespace dpdk

#endif /* DPDK_PORT_H */

