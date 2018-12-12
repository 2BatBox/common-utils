//#include <thread>
//#include <vector>
//#include <cstring>
//#include <assert.h>
//
//#include <rte_debug.h>
//
//#include "logger/Logger.h"
//#include "dpdk/Utils.h"
//#include "dpdk/Port.h"
//#include "dpdk/Dumper.h"
//#include "dpdk/PortStats.h"
//
//#include "pcap_utils/Reader.h"
//
//#include "CliContext.h"
//#include "dpdk/Dumper.h"
//
//using namespace pcap_utils;
//
//static const char* VERSION = "0.0.1";
//
//void print_stats(uint8_t port) noexcept {
////	dpdk::PortStats::print_default(stdout, port);
//}
//
//int init_port(uint8_t port) noexcept {
//	LOG_RAW("init port %u\n", port);
//
//	static constexpr unsigned TX_QUEUES = 1;
//	static constexpr unsigned TX_QUEUE_LEN = 4 * 1024;
//
//	//	rte_mempool* mp_eth_pool = dpdk::Utils::create_mempool(eth_pool_size, 0, 0, ETH_MBUF_SIZE);
//	//	if (mp_eth_pool == nullptr) {
//	//		LOG_CRITICAL("cannot create ethernet mbuf pool: %s (%d)", rte_strerror(rte_errno), rte_errno);
//	//		return false;
//	//	}
//
//	dpdk::Port dpdk_port(port);
//	dpdk_port.set_tx_queues(TX_QUEUES, TX_QUEUE_LEN);
//	return dpdk_port.init() && dpdk_port.start();
//}
//
//int replay(const char* file_name, rte_mempool* mem_pool, uint8_t port) {
//	Reader pcap_reader = Reader::open(file_name);
//	Frame frame;
//
//	LOG_RAW("start processing '%s'...", file_name);
//
//	rte_mbuf* frame_mbuf;
//	unsigned frames_total = 0;
//
//	while ((pcap_reader.read_next(frame))) {
//		frame_mbuf = dpdk::Utils::mbuf_load(mem_pool, frame.data, frame.hdr.len, frame.hdr.len);
//		if (frame_mbuf == nullptr) {
//			LOG_CRITICAL("cannot load frame %u\n", pcap_reader.next_index());
//			return -1;
//		}
//
//		while (rte_eth_tx_burst(port, 0, &frame_mbuf, 1) == 0);
//
//		rte_pktmbuf_free(frame_mbuf);
//		frames_total++;
//	}
//
//	LOG_RAW("frames = %u", frames_total);
//	return frames_total;
//}
//
//void dump_info(const char* file_name, unsigned& frames, unsigned& max_frame_size) throw (std::logic_error) {
//	Reader pcap_reader = Reader::open(file_name);
//	Frame frame;
//	frames = 0;
//	max_frame_size = 0;
//
//	while ((pcap_reader.read_next(frame))) {
//		frames++;
//		if (frame.hdr.len > max_frame_size) {
//			max_frame_size = frame.hdr.len;
//		}
//	}
//}
//
//void dump_load(const char* file_name, rte_mbuf** frame_mbuf, rte_mempool* mem_pool) throw (std::logic_error) {
//	Reader pcap_reader = Reader::open(file_name);
//	Frame frame;
//	unsigned frames = 0;
//
//	while ((pcap_reader.read_next(frame))) {
//
//		frame_mbuf[frames] = dpdk::Utils::mbuf_load(mem_pool, frame.data, frame.hdr.len, frame.hdr.len);
//		if (frame_mbuf == nullptr) {
//			LOG_CRITICAL("cannot load frame %u\n", pcap_reader.next_index());
//			return;
//		}
////		rte_eth_tx_burst(0, 0, frame_mbuf + frames, 1);
//
//		frames++;
//	}
//}
//
//bool dump_send(uint8_t port, rte_mbuf** frame_mbuf, unsigned frames, rte_mempool* mem_pool_indir) throw (std::logic_error) {
//	rte_mbuf* mbuf = nullptr;
//	for (unsigned i = 0; i < frames; i++) {
//		mbuf = rte_pktmbuf_clone(frame_mbuf[i], mem_pool_indir);
//		if(mbuf == nullptr){
//			LOG_CRITICAL("cannot clone direct buffer\n");
//			return false;
//		}
//		if(rte_eth_tx_burst(port, 0, &mbuf, 1) < 1){
//			rte_pktmbuf_free(mbuf);
//		}
//	}
//	return true;
//}
//
//int main_replay(int argc, char** argv) {
////int main(int argc, char** argv) {
//
////	const char* bin = argv[0];
//
//	try {
//		dpdk::Utils::init(argc, argv);
//		CliContext::parse_args(argc, argv);
//	} catch (const std::logic_error& e) {
//		LOG_CRITICAL("%s", e.what());
//		return 1;
//	}
//
//	if (CliContext::help) {
//		CliContext::print_usage(stdout, VERSION);
//		return 0;
//	} else if (CliContext::version) {
//		CliContext::print_info(stdout, VERSION);
//		return 0;
//	}
//
//	if (CliContext::files.size() == 0) {
//		CliContext::print_usage(stdout, VERSION);
//		return 1;
//	}
//
//	CliContext::dump(stdout);
//
//	dpdk::Dumper::eth_dev_info(stdout, 0);
//	dpdk::Dumper::link_status(stdout, 0);
//
//
//	// Ports
//	uint8_t ports = rte_eth_dev_count();
//	if (not ports) {
//		LOG_CRITICAL("the application has no ports.");
//		return 1;
//	}
//
//	for (unsigned port = 0; port < ports; port++) {
//		if (not init_port(port)) {
//			LOG_CRITICAL("cannot initialize port %u : %s (%d)", port, rte_strerror(rte_errno), rte_errno);
//			return 1;
//		}
//	}
//
//	const char* file_name = CliContext::files.at(0).c_str();
//	unsigned frames;
//	unsigned max_frame_size;
//
//	dump_info(file_name, frames, max_frame_size);
//
//	LOG_RAW("         file : '%s'", file_name);
//	LOG_RAW(" frames total : %u", frames);
//	LOG_RAW("max frame len : %u", max_frame_size);
//
//	rte_mempool* mem_pool_dir = dpdk::Utils::create_mempool(frames, 0, 0, RTE_MBUF_DEFAULT_BUF_SIZE);
//	if (mem_pool_dir == nullptr) {
//		LOG_CRITICAL("cannot allocate direct mempool.");
//		return 1;
//	}
//
//	rte_mempool* mem_pool_indir = dpdk::Utils::create_mempool(frames, 0, 0, 0);
//	if (mem_pool_indir == nullptr) {
//		LOG_CRITICAL("cannot allocate indirect mempool.");
//		return 1;
//	}
//
//
//	rte_mbuf** frame_mbuf = new rte_mbuf*[frames];
//
//	dump_load(file_name, frame_mbuf, mem_pool_dir);
//
////	while () {
////		LOG_RAW("sending '%s'...", file_name);
////	}
//
//	dump_send(0, frame_mbuf, frames, mem_pool_indir);
//
//	return 0;
//}