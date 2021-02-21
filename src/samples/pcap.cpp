#include <cstdio>
#include <cstdlib>

#include <pcapwrap/Reader.h>
#include <pcapwrap/Writer.h>
#include <pcapwrap/Dumper.h>

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("qlibs::pcap sample application\n");
		printf("usage: %s <pcap-file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	std::string fin = argv[1];
	std::string fout(fin);
	fout += ".out.pcap";
	auto reader = pcapwrap::Reader::open(fin);
	auto writer = pcapwrap::Writer::open(fout);
	pcapwrap::Frame frame;
	while(reader.next(frame)) {
		pcapwrap::Dumper::frame(stdout, frame);
		// dummy modification
		frame.nanosec(frame.nanosec() + 1);
		writer.write(frame);
	}

	printf("<---- the end of main() ---->\n");
	return EXIT_SUCCESS;
}
