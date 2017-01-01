#include <iostream>
#include <sstream>
#include <type_traits>
#include <boost/format.hpp>
#include <canard/network/protocol/openflow/controller.hpp>
#include <canard/network/protocol/openflow/v10/io/enum_to_string.hpp>
#include <canard/network/protocol/openflow/v10/secure_channel.hpp>

namespace allium = canard::network::openflow;
namespace ofp = canard::net::ofp;
namespace v10 = ofp::v10;

struct switch_description;
using controller = allium::controller<switch_description>;

namespace detail {

  template <class Port>
  auto port_data(Port const& port)
    -> std::string
  {
    std::ostringstream os{};
    os << boost::format{
      "Port no: %|1$|\n"
      "  Hardware Address: %|2$|\n"
      "  Name: %|3$|\n"
      "  Config: 0x%|4$08X|\n"
      "  State: 0x%|5$08X|\n"
    } % port.port_no()
      % port.hardware_address()
      % port.name()
      % port.config()
      % port.state()
    ;
    return os.str();
  }

  template <class PortList>
  auto port_list_data(PortList const& port_list)
    -> std::string
  {
    std::ostringstream os{};
    for (auto const& port : port_list) {
      os << detail::port_data(port);
    }
    return os.str();
  }

  template <class Table>
  auto table_data(Table const& table)
    -> std::string
  {
    std::ostringstream os{};
    os << boost::format{
      "Table id: %|1$|\n"
      "  Name: %|2$|\n"
      "  Wildcards: 0x%|3$08X|\n"
      "  Max entries: %|4$|\n"
      "  Active count: %|5$|\n"
      "  Lookup count: %|6$|\n"
      "  Matched count: %|7$|\n"
    } % std::uint32_t{table.table_id()}
      % table.name()
      % table.wildcards()
      % table.max_entries()
      % table.active_count()
      % table.lookup_count()
      % table.matched_count()
    ;
    return os.str();
  }

  template <class TableList>
  auto table_list_data(TableList const& table_list)
    -> std::string
  {
    std::ostringstream os{};
    for (auto const& table : table_list) {
      os << detail::table_data(table);
    }
    return os.str();
  }

  template <class PortStats>
  auto port_stats_data(PortStats const& port_stats)
    -> std::string
  {
    std::ostringstream os{};
    os << boost::format{
      "Port no: %|1$|\n"
      "  RX packets: %|2$|\n"
      "  TX packets: %|3$|\n"
      "  RX bytes: %|4$|\n"
      "  TX bytes: %|5$|\n"
      "  RX dropped packets: %|6$|\n"
      "  TX dropped packets: %|7$|\n"
      "  RX errors: %|8$|\n"
      "  TX errors: %|9$|\n"
      "  RX frame alignment errors: %|10$|\n"
      "  RX overrun errors: %|11$|\n"
      "  RX CRC errors: %|12$|\n"
      "  Collisions: %|13$|\n"
    } % port_stats.port_no()
      % port_stats.rx_packets()
      % port_stats.tx_packets()
      % port_stats.rx_bytes()
      % port_stats.tx_bytes()
      % port_stats.rx_dropped()
      % port_stats.tx_dropped()
      % port_stats.rx_errors()
      % port_stats.tx_errors()
      % port_stats.rx_frame_errors()
      % port_stats.rx_over_errors()
      % port_stats.rx_crc_errors()
      % port_stats.collisions()
    ;
    return os.str();
  }

  template <class PortStatsList>
  auto port_stats_list_data(PortStatsList const& port_stats_list)
    -> std::string
  {
    std::ostringstream os{};
    for (auto const& port_stats : port_stats_list) {
      os << detail::port_stats_data(port_stats);
    }
    return os.str();
  }

  template <class QueueStats>
  auto queue_stats_data(QueueStats const& queue_stats)
    -> std::string
  {
    std::ostringstream os{};
    os << boost::format{
      "Port no: %|1$|\n"
      "Queue id: %|2$|\n"
      "  TX bytes: %|3$|\n"
      "  TX packets: %|4$|\n"
      "  TX errors: %|5$|\n"
    } % queue_stats.port_no()
      % queue_stats.queue_id()
      % queue_stats.tx_bytes()
      % queue_stats.tx_packets()
      % queue_stats.tx_errors()
    ;
    return os.str();
  }

  template <class QueueStatsList>
  auto queue_stats_list_data(QueueStatsList const& queue_stats_list)
    -> std::string
  {
    std::ostringstream os{};
    for (auto const& queue_stats : queue_stats_list) {
      os << detail::queue_stats_data(queue_stats);
    }
    return os.str();
  }

} // namespace detail

struct switch_description
{
  using versions = std::tuple<allium::v10::version>;

  template <class Channel>
  void handle(Channel&& channel, ofp::hello const& hello)
  {
    std::cout
      << boost::format{
        "==== Switch Connected ====\n"
        "Verson: 0x%|1$02X|\n"
        "=========================="
      } % hello.version()
      << std::endl;
    channel->async_send(v10::messages::features_request{});
    channel->async_send(v10::messages::get_config_request{});
    channel->async_send(v10::messages::statistics::description_request{});
    channel->async_send(v10::messages::statistics::aggregate_stats_request{
        ofp::v10::match{}, 0xff
    });
    channel->async_send(v10::messages::statistics::table_stats_request{});
    channel->async_send(v10::messages::statistics::port_stats_request{
        v10::protocol::port_no::any
    });
    channel->async_send(v10::messages::statistics::queue_stats_request{
        v10::protocol::OFPQ_ALL, v10::protocol::port_no::all
    });
  }

  template <class Channel>
  void handle(Channel&& channel, allium::goodbye const& goodbye)
  {
    std::cout
      << boost::format{
        "== Switch disconnected ===\n"
        "Reason: %1%\n"
        "=========================="
      } % goodbye.reason().message()
      << std::endl;
  }

  template <class Channel>
  void handle(Channel&& channel, v10::messages::features_reply&& features)
  {
    std::cout
      << boost::format{
        "===== Switch Features ====\n"
        "Datapath id: 0x%|1$016X|\n"
        "Max buffered packets: %|2$|\n"
        "Table numbers: %|3$|\n"
        "Capabilities: 0x%|4$08X|\n"
        "Actions: 0x%|5$08X|\n"
      } % features.datapath_id()
        % features.num_buffers()
        % std::uint16_t{features.num_tables()}
        % features.capabilities()
        % features.actions()
      << detail::port_list_data(features.ports())
      << "=========================="
      << std::endl;
  }

  template <class Channel>
  void handle(Channel&& channel, v10::messages::get_config_reply&& switch_config)
  {
    std::cout
      << boost::format{
        "====== Switch Config =====\n"
        "Flags: 0x%|1$04X|\n"
        "Max packet bytes sent to controller: %|2$|\n"
        "=========================="
      } % switch_config.flags()
        % switch_config.miss_send_length()
      << std::endl;
  }

  template <class Channel>
  void handle(Channel&& channel, v10::messages::port_status&& port_status)
  {
    std::cout
      << boost::format{
        "======= Port Status ======\n"
        "Reason: %|1$|\n"
      } % allium::v10::to_string(port_status.reason())
      << detail::port_data(port_status)
      << "=========================="
      << std::endl;
  }

  template <class Channel>
  void handle(Channel&& channel, v10::messages::statistics::description_reply&& description)
  {
    std::cout
      << boost::format{
        "=== Switch Description ===\n"
        "Manufacture description:\n"
        "  %1%\n"
        "Hardware description:\n"
        "  %2%\n"
        "Software description:\n"
        "  %3%\n"
        "Serial number:\n"
        "  %4%\n"
        "Datapath description:\n"
        "  %5%\n"
        "=========================="
      } % description.manufacture_desc()
        % description.hardware_desc()
        % description.software_desc()
        % description.serial_number()
        % description.datapath_desc()
      << std::endl;
  }

  template <class Channel>
  void handle(Channel&& channel, v10::messages::statistics::aggregate_stats_reply&& aggregate_stats)
  {
    std::cout
      << boost::format{
        "== Flow Aggregate Stats ==\n"
        "Packet count: %1%\n"
        "Byte count: %2%\n"
        "Flow count: %3%\n"
        "=========================="
      } % aggregate_stats.packet_count()
        % aggregate_stats.byte_count()
        % aggregate_stats.flow_count()
      << std::endl;
  }

  template <class Channel>
  void handle(Channel&& channel, v10::messages::statistics::table_stats_reply&& table_stats)
  {
    std::cout
      << boost::format{
        "====== Table Stats =======\n"
      }
      << detail::table_list_data(table_stats.body())
      << "=========================="
      << std::endl;
  }

  template <class Channel>
  void handle(Channel&& channel, v10::messages::statistics::port_stats_reply&& port_stats)
  {
    std::cout
      << boost::format{
        "======= Port Stats =======\n"
      }
      << detail::port_stats_list_data(port_stats.body())
      << "=========================="
      << std::endl;
  }

  template <class Channel>
  void handle(Channel&& channel, v10::messages::statistics::queue_stats_reply&& queue_stats)
  {
    std::cout
      << boost::format{
        "====== Queue Stats =======\n"
      }
      << detail::queue_stats_list_data(queue_stats.body())
      << "=========================="
      << std::endl;
  }

  template <class Channel, class Message>
  void handle(Channel&&, Message const&) {}
};

int main()
{
  auto handler = switch_description{};
  controller cont{controller::options{handler}.address("0.0.0.0").port("6633")};
  try {
    cont.run();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

