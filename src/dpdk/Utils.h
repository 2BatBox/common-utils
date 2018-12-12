#ifndef DPDK_UTILS_H
#define DPDK_UTILS_H

#include <vector>

#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_debug.h>

namespace dpdk {

class Utils {
public:

	static int init(int& argc, char**& argv) noexcept {
		int ret = rte_eal_init(argc, argv);
		if (ret < 0)
			rte_panic("Cannot init EAL\n");
		argc -= ret;
		argv += ret;

		return ret;
	}

	static int create_rings(
		rte_ring** buffer,
		unsigned buffer_len,
		unsigned ring_size,
		int socket_id,
		unsigned flags
		) noexcept {
		static unsigned global_index = 0;
		char ring_name[32];
		for (unsigned i = 0; i < buffer_len; ++i) {
			snprintf(ring_name, sizeof (ring_name), "ring_%u", global_index++);
			buffer[i] = rte_ring_create(ring_name, ring_size, socket_id, flags);
			if (!buffer[i]) {
				// Free all allocated rings
				for (unsigned j = 0; j < i; j++) {
					rte_ring_free(buffer[j]);
				}
				return -1;
			}
		}

		return 0;
	}

	static rte_mbuf* mbuf_load(rte_mempool* mem_pool, const void* vdata, unsigned data_len, uint16_t pkt_len) noexcept {
		unsigned loaded = 0;
		unsigned capacity = 0;
		rte_mbuf* root = nullptr;
		rte_mbuf* current = nullptr;
		rte_mbuf* next = nullptr;
		const uint8_t* data = static_cast<const uint8_t*>(vdata);

		do {
			next = rte_pktmbuf_alloc(mem_pool);
			if (next) {

				next->data_off = 0;
				capacity = RTE_MIN(next->buf_len, data_len - loaded);
				rte_memcpy(rte_ctrlmbuf_data(next), data + loaded, capacity);

				loaded += capacity;
				next->data_len = capacity;
				next->pkt_len = pkt_len;

				if (current)
					current->next = next;

				current = next;

				if (root == nullptr)
					root = current;
			} else {
				if (root)
					rte_pktmbuf_free(root);
				return nullptr;
			}
		} while (loaded < data_len);

		return root;
	}

	static unsigned mbuf_copy(rte_mbuf* dst_solid, const rte_mbuf* src) noexcept {
		dst_solid->data_off = 0;
		dst_solid->data_len = 0;

		while (src && (src->data_len + dst_solid->data_len) <= dst_solid->buf_len) {
			rte_memcpy(rte_ctrlmbuf_data(dst_solid) + dst_solid->data_len, rte_ctrlmbuf_data(src), src->data_len);
			dst_solid->data_len += src->data_len;
			src = src->next;
		}
		return dst_solid->data_len;
	}

};

}; // namespace dpdk

#endif /* DPDK_UTILS_H */

