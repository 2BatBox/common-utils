#ifndef DPDK_DUMPER_H
#define DPDK_DUMPER_H

#include <rte_mbuf.h>
#include <rte_ethdev.h>
#include <rte_net.h>
#include <rte_eal_memconfig.h>
#include <rte_malloc.h>

namespace dpdk {

class Dumper {
#ifndef PRINT_FLAG
#define PRINT_FLAG(message, value, flag) { \
    if((flag & value) == flag) { \
        fprintf(file, message #flag); \
        fprintf(file, "\n"); \
    }};
#endif


public:

	static void m_buf(FILE* file, const rte_mbuf* buf, int seg_num = 0) {
		fprintf(file, "==== rte_mbuf (%p) seg=%u", buf, seg_num);
		fprintf(file, "%s", buf->ol_flags & IND_ATTACHED_MBUF ? "IND_ATTACHED_MBUF " : "");
		fprintf(file, "%s", buf->ol_flags & CTRL_MBUF_FLAG ? "CTRL_MBUF_FLAG " : "");
		fprintf(file, "\n");

		fprintf(file, "struct mbuf=%zu | ", sizeof (rte_mbuf));
		fprintf(file, "private=%u | ", buf->priv_size);
		fprintf(file, "head_room=%u | ", buf->data_off);
		fprintf(file, "data=%u | ", buf->data_len);
		fprintf(file, "tail_room=%u\n", buf->buf_len - buf->data_off - buf->data_len);

		fprintf(file, "buf_addr=%p ", buf->buf_addr);
		fprintf(file, "buf_len=%u ", buf->buf_len);
		fprintf(file, "pkt_len=%u ", buf->pkt_len);
		fprintf(file, "nb_segs=%u ", buf->nb_segs);
		fprintf(file, "next=%p\n", buf->next);

		if (buf->next && seg_num > -1)
			m_buf(file, buf->next, ++seg_num);

	}

	static void m_buf_segment(FILE* file, const rte_mbuf* buf) {
		m_buf(file, buf, -1);
	}

	static void config_info(FILE* file) {
		rte_config* config = rte_eal_get_configuration();
		fprintf(file, "==== rte_config (%p) \"rte_eal.h\" ====\n", config);
		fprintf(file, "\tuint32_t master_lcore: %u\n", config->master_lcore);
		fprintf(file, "\tuint32_t lcore_count: %u\n", config->lcore_count);
		fprintf(file, "\tprocess type: %s\n", proc_type_as_string(config->process_type));
		fprintf(file, "\tRTE enabled cores:\n");
		for (unsigned i = 0; i < RTE_MAX_LCORE; i++) {
			if (config->lcore_role[i] == ROLE_RTE)
				fprintf(file, "\t\tlcore %u\n", i);
		}
		mem_config(file, config->mem_config);
	}

	static void mem_config(FILE* file, const rte_mem_config* config) {
		fprintf(file, "==== rte_mem_config (%p) \"rte_eal_memconfig.h\" ====\n", config);
		fprintf(file, "\tuint32_t nchannel: %u\n", config->nchannel);
		fprintf(file, "\tuint32_t nrank: %u\n", config->nrank);
		fprintf(file, "\tuint32_t memzone_cnt: %u\n", config->memzone_cnt);
		for (unsigned i = 0; i < config->memzone_cnt; ++i)
			memzone(file, config->memzone + i);
	}

	static void memzone(FILE* file, const rte_memzone* zone) {
		fprintf(file, "==== rte_memzone (%p) \"rte_memzone.h\" ====\n", zone);
		fprintf(file, "\tname: %s\n", zone->name);
		fprintf(file, "\tphys_addr: 0x%lx (%lu)\n", zone->phys_addr, zone->phys_addr);
		fprintf(file, "\tvirtual addr: %p (%lu)\n", zone->addr, (uint64_t)zone->addr);
		fprintf(file, "\tlen: 0x%zx (%zu)\n", zone->len, zone->len);
		fprintf(file, "\thugepage_sz: 0x%lx (%lu)\n", zone->hugepage_sz, zone->hugepage_sz);
		fprintf(file, "\tsocket_id: %u\n", zone->socket_id);
		fprintf(file, "\tflags: %u\n", zone->flags);
		fprintf(file, "\tmemseg_id: %u\n", zone->memseg_id);
	}

	static const char* proc_type_as_string(enum rte_proc_type_t type) {
		switch (type) {
		case RTE_PROC_AUTO:
			return "RTE_PROC_AUTO";
		case RTE_PROC_PRIMARY:
			return "RTE_PROC_PRIMARY";
		case RTE_PROC_SECONDARY:
			return "RTE_PROC_SECONDARY";
		case RTE_PROC_INVALID:
			return "RTE_PROC_INVALID";
		default:
			return "UNKNOWN";
		}
	}

	static void malloc_stat(FILE* out) noexcept {
		rte_malloc_socket_stats stat;
		fprintf(out, "---- malloc statistics ----\n");
		for (unsigned i = 0; i < RTE_MAX_NUMA_NODES; i++) {
			rte_malloc_get_socket_stats(i, &stat);
			if (stat.heap_totalsz_bytes) {
				fprintf(out, "socket[%u] : ", i);
				fprintf(out, "heap_total=%zuMb ", (stat.heap_totalsz_bytes) / 1024 / 1024);
				fprintf(out, "heap_allocated=%zuMb ", (stat.heap_allocsz_bytes) / 1024 / 1024);
				fprintf(out, "\n");
			}
		}
	}
};

}; // namespace dpdk

#endif /* DPDK_DUMPER_H */

