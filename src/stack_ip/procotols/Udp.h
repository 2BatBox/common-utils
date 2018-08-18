#ifndef STACK_IP_PROTOCOLS_UDP_H
#define STACK_IP_PROTOCOLS_UDP_H

#include <arpa/inet.h>
#include <netinet/udp.h>

#include "../stack_ip.h"

namespace stack_ip {

class Udp {
public:

    using Header = udphdr;

    static inline bool validate_packet(DefaultPacketReader& pkt) noexcept
    {
        const Header* hdr;
        pkt.assign_stay(hdr);
        uint16_t packet_nb = ntohs(hdr->len);
        unsigned available = pkt.available();

        if (
            size_t(packet_nb) < sizeof(Header)
            || available < packet_nb
            ) {
            return false;
        }

        pkt.tail_move_back(available - packet_nb);
        return true;
    }

    static inline bool validate_header(const DefaultPacketReader& pkt) noexcept
    {
        return pkt.available(sizeof(Header));
    }

    static Protocol next(DefaultPacketReader& pkt) noexcept
    {
        pkt.head_move(sizeof(Header));
        return Protocol::END;
    }

    static inline constexpr unsigned length_header(const DefaultPacketReader&) noexcept
    {
        return sizeof(Header);
    }

    static inline unsigned length_payload(const DefaultPacketReader& pkt) noexcept
    {
        const Header* hdr;
        pkt.assign_stay(hdr);
        return ntohs(hdr->len) - sizeof(Header);
    }

};

}; // namespace stack_ip

#endif /* STACK_IP_PROTOCOLS_UDP_H */

