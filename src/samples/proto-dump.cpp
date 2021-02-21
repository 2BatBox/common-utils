#include <cstdio>
#include <cstdlib>
#include <pcapwrap/Reader.h>
#include <pcapwrap/Writer.h>
#include <pcapwrap/Dumper.h>
#include <proto/parsers/HeaderParser.h>
#include <proto/Dumper.h>


bool process(pcapwrap::Frame& frame) noexcept {
	proto::HeaderParser hp(frame.m_data, frame.m_hdr.caplen);

	const proto::Ethernet::Header* hdr_eth = nullptr;
	const proto::Vlan::Header* hdr_vlan = nullptr;
	const proto::IPv4::Header* hdr_ip_v4 = nullptr;
	const proto::IPv6::Header* hdr_ip_v6 = nullptr;
	const proto::Tcp::Header* hdr_tcp = nullptr;
	const proto::Udp::Header* hdr_udp = nullptr;

	while(hp.protocol() != proto::END) {

		switch(hp.protocol()) {
			case proto::L2_ETHERNET:
				hp.assign(hdr_eth);
				proto::Dumper::header(stdout, hdr_eth);
				break;
			case proto::L2_VLAN:
				hp.assign(hdr_vlan);
				proto::Dumper::header(stdout, hdr_vlan);
				break;
			case proto::L3_IPv4:
				hp.assign(hdr_ip_v4);
				proto::Dumper::header(stdout, hdr_ip_v4);
				break;
			case proto::L3_IPv6:
				hp.assign(hdr_ip_v6);
				proto::Dumper::header(stdout, hdr_ip_v6);
				break;
			case proto::L4_TCP:
				hp.assign(hdr_tcp);
				proto::Dumper::header(stdout, hdr_tcp);
				break;
			case proto::L4_UDP:
				hp.assign(hdr_udp);
				proto::Dumper::header(stdout, hdr_udp);
				break;

			default:
				break;
		}
		hp.next();
	}

	printf("\n");
	return true;
}

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("qlibs::proto sample application\n");
		printf("usage: %s <pcap-file(s)>\n", argv[0]);
		return EXIT_FAILURE;
	}

	std::string fin = argv[1];
	auto reader = pcapwrap::Reader::open(fin);
	pcapwrap::Frame frame;
	while(reader.next(frame)) {
		pcapwrap::Dumper::frame(stdout, frame);
		process(frame);
	}

	printf("<---- the end of main() ---->\n");
	return EXIT_SUCCESS;
}
