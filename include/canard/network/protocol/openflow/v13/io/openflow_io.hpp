#ifndef CANARD_NETWORK_OPENFLOW_V13_IO_HPP
#define CANARD_NETWORK_OPENFLOW_V13_IO_HPP

#include <cstdint>
#include <array>
#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/v13/messages.hpp>
#include <canard/network/protocol/openflow/v13/io/enum_to_string.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

    auto port_state_string(std::uint32_t const port_state)
        -> std::string
    {
        return boost::algorithm::join(std::array<std::string, 3>{{
                  (protocol::OFPPS_LINK_DOWN & port_state ? "LINK_DOWN" : "")
                , (protocol::OFPPS_BLOCKED & port_state ? "BLOCKED" : "")
                , (protocol::OFPPS_LIVE & port_state ? "LIVE" : "")
        }}, " ");
    }

    template <class OStream>
    auto operator<<(OStream& os, v13::port const& port)
        -> OStream&
    {
        return os << boost::format{"port[port_no=%u, name=%s, config=%s, state=(%s), curr_speed=%u, max_speed=%u]"}
            % port.port_no()
            % port.name()
            % port.config()
            % port_state_string(port.state())
            % port.current_speed()
            % port.max_speed()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, messages::multipart::flow_stats const& stats)
        -> OStream&
    {
        return os << boost::format{"flow_stats[entry=##, table_id=%u, flags=%#x, duration_sec=%u, duration_nsec=%u]"}
            % std::uint16_t{stats.table_id()}
            % stats.flags()
            % stats.duration_sec()
            % stats.duration_nsec()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, table_stats const& stats)
        -> OStream&
    {
        return os << boost::format{"table_stats[table_id=%u, active_count=%u, lookup_count=%u, matched_count=%u]"}
            % std::uint16_t{stats.table_id()}
            % stats.active_count()
            % stats.lookup_count()
            % stats.matched_count()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, port_stats const& stats)
        -> OStream&
    {
        return os << boost::format{
            "port_stats[port_no=%u, "
            "rx_packets=%u, tx_packets=%u, rx_bytes=%#x, tx_bytes=%#x, rx_dropped=%u, tx_dropped=%u, rx_errors=%u, tx_errors=%u, "
            "rx_frame_err=%u, rx_over_err=%u, rx_crc_err=%u, collisions=%u, duration_sec=%u, duration_nsec=%u]"
        }
        % stats.port_no()
        % stats.rx_packets()
        % stats.tx_packets()
        % stats.rx_bytes()
        % stats.tx_bytes()
        % stats.rx_dropped()
        % stats.tx_dropped()
        % stats.rx_errors()
        % stats.tx_errors()
        % stats.rx_frame_err()
        % stats.rx_over_err()
        % stats.rx_crc_err()
        % stats.collisions()
        % stats.duration_sec()
        % stats.duration_nsec()
        ;
    }

    template <class OStream>
    class ostream_visitor
    {
    public:
        using result_type = OStream&;

        explicit ostream_visitor(OStream& os)
            : os{os}
        {
        }

        template <class T>
        auto operator()(T const& t) const
            -> OStream&
        {
            return os << "\n\t\t" << t;
        }

    private:
        OStream& os;
    };

    template <class OStream>
    auto operator<<(OStream& os, table_features const& features)
        -> OStream&
    {
        os << boost::format{"table_features[table_id=%u, name=%s, metadata_match=%#x, metadata_write=%#x, config=%#x, max_entries=%u]"}
            % std::uint16_t{features.table_id()}
            % features.name()
            % features.metadata_match()
            % features.metadata_write()
            % features.config()
            % features.max_entries()
            ;
        boost::for_each(features, [&](table_feature_properties::variant const& prop) {
            // auto const visitor = ostream_visitor<OStream>{os};
            // boost::apply_visitor(visitor, prop);
        });
        return os;
    }

namespace messages {

    template <class OStream>
    auto operator<<(OStream& os, hello const& hello)
        -> OStream&
    {
        return os << boost::format("%s: version=%#x, xid=%#x")
            % v13::to_string(hello.type())
            % std::uint32_t{hello.version()}
            % hello.xid()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, error const& error)
        -> OStream&
    {
        return os << boost::format("%s: xid=%#x type=%u, failed_request.type=%u")
            % v13::to_string(error.type())
            % error.xid()
            % v13::to_string(error.error_type())
            % std::uint32_t{error.failed_request_header().type}
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, echo_request const& echo_req)
        -> OStream&
    {
        return os << boost::format("%s: xid=%#x") % v13::to_string(echo_req.type()) % echo_req.xid();
    }

    template <class OStream>
    auto operator<<(OStream& os, echo_reply const& echo_rep)
        -> OStream&
    {
        return os << boost::format("%s: xid=%#x") % v13::to_string(echo_rep.type()) % echo_rep.xid();
    }

    template <class OStream>
    auto operator<<(OStream& os, features_reply const& features_rep)
        -> OStream&
    {
        return os << boost::format("%s: xid=%#x, n_buffers=%u, n_tables=%u, auxiliary_id=%u")
            % v13::to_string(features_rep.type())
            % features_rep.xid()
            % features_rep.num_buffers()
            % std::uint32_t{features_rep.num_tables()}
            % std::uint32_t{features_rep.auxiliary_id()}
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, get_config_reply const& reply)
        -> OStream&
    {
        return os << boost::format{"%s: xid=%#x, flags=%#x, miss_send_length=%u"}
            % v13::to_string(reply.type())
            % reply.xid()
            % reply.flags()
            % reply.miss_send_length()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, messages::multipart::description_reply const& reply)
        -> OStream&
    {
        return os << boost::format{"%s: xid=%#x, flags=%#x, mfr_desc=%s, hw_desc=%s, sw_desc=%s, serial_num=%s, dp_desc=%s"}
            % v13::to_string(reply.multipart_type())
            % reply.xid()
            % reply.flags()
            % reply.manufacture_desc()
            % reply.hardware_desc()
            % reply.software_desc()
            % reply.serial_number()
            % reply.datapath_desc()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, messages::multipart::flow_stats_reply const& reply)
        -> OStream&
    {
        os << boost::format{"%s: xid=%#x, flags=%#x, "}
            % v13::to_string(reply.multipart_type())
            % reply.xid()
            % reply.flags();
            ;
        boost::for_each(reply, [&](messages::multipart::flow_stats const& stats) {
            os << "\n\t" << stats;
        });
        return os;
    }

    template <class OStream>
    auto operator<<(OStream& os, messages::multipart::aggregate_stats_reply const& reply)
        -> OStream&
    {
        return os << boost::format{"%s: xid=%#x, flags=%#x, packet_count=%llu, byte_count=%#x, flow_count=%lu"}
            % v13::to_string(reply.multipart_type())
            % reply.xid()
            % reply.flags()
            % reply.packet_count()
            % reply.byte_count()
            % reply.flow_count()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, table_stats_reply const& reply)
        -> OStream&
    {
        os << boost::format{"%s: xid=%#x, flags=%#x, "}
            % v13::to_string(reply.multipart_type())
            % reply.xid()
            % reply.flags();
            ;
        boost::for_each(reply, [&](table_stats const& stats) {
            os << "\n\t" << stats;
        });
        return os;
    }

    template <class OStream>
    auto operator<<(OStream& os, port_stats_reply const& reply)
        -> OStream&
    {
        os << boost::format{"%s: xid=%#x, flags=%#x, "}
            % v13::to_string(reply.multipart_type())
            % reply.xid()
            % reply.flags();
            ;
        boost::for_each(reply, [&](port_stats const& stats) {
            os << "\n\t" << stats;
        });
        return os;
    }

    template <class OStream>
    auto operator<<(OStream& os, table_features_reply const& reply)
        -> OStream&
    {
        os << boost::format("%s: xid=%#x, flags=%#x, ")
            % v13::to_string(reply.multipart_type())
            % reply.xid()
            % reply.flags()
            ;
        boost::for_each(reply, [&](table_features const& features) {
            os << "\n\t" << features;
        });
        return os;
    }

    template <class OStream>
    auto operator<<(OStream& os, port_description_reply const& reply)
        -> OStream&
    {
        os << boost::format("%s: xid=%#x, flags=%#x, ")
            % v13::to_string(reply.multipart_type())
            % reply.xid()
            % reply.flags()
            ;
        boost::for_each(reply, [&](port const& port) {
            os << "\n\t" << port;
        });
        return os;
    }

    template <class OStream>
    auto operator<<(OStream& os, barrier_reply const& rep)
        -> OStream&
    {
        return os << boost::format("%s: xid=%#x")
            % v13::to_string(rep.type())
            % rep.xid()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, packet_in const& pkt_in)
        -> OStream&
    {
        return os << boost::format("%s: xid=%#x, reason=%#x, table_id=%u, buffer_id=%#x, total_len=%u, cookie=%#x")
            % v13::to_string(pkt_in.type())
            % pkt_in.xid()
            % v13::to_string(pkt_in.reason())
            % std::uint32_t{pkt_in.table_id()}
            % pkt_in.buffer_id()
            % pkt_in.total_length()
            % pkt_in.cookie()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, flow_removed const& removed)
        -> OStream&
    {
        return os << boost::format{"%s: xid=%#x, reason=%s, match=##, priority=%u, table_id=%u, duration_sec=%u, duration_nsec=%u"}
            % v13::to_string(removed.type())
            % removed.xid()
            % v13::to_string(removed.reason())
            % removed.priority()
            % std::uint16_t{removed.table_id()}
            % removed.duration_sec()
            % removed.duration_nsec()
            ;
    }

    template <class OStream>
    auto operator<<(OStream& os, port_status const& port_status)
        -> OStream&
    {
        return os << boost::format("%s: xid=%#x, reason=%#x, %s")
            % v13::to_string(port_status.type())
            % port_status.xid()
            % v13::to_string(port_status.reason())
            % port_status.port()
            ;
    }

} // namespace messages
} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V13_IO_HPP
