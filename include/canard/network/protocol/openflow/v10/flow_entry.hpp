#ifndef CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_HPP
#define CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_HPP

#include <cstdint>
#include <utility>
#include <canard/network/protocol/openflow/v10/action_list.hpp>
#include <canard/network/protocol/openflow/v10/match_set.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v10 {

    class flow_entry_id
    {
    public:
        flow_entry_id(match_set const& match
                    , std::uint16_t const priority) noexcept
            : match_(match)
            , priority_(priority)
        {
        }

        auto match() const noexcept
            -> match_set const&
        {
            return match_;
        }

        auto priority() const noexcept
            -> std::uint16_t
        {
            return priority_;
        }

        static auto table_miss() noexcept
            -> flow_entry_id
        {
            return flow_entry_id{match_set{}, 0};
        }

    private:
        match_set match_;
        std::uint16_t priority_;
    };


    class flow_entry
    {
    public:
        flow_entry(flow_entry_id const& identifer
                 , std::uint64_t const cookie
                 , action_list actions)
            : identifier_(identifer)
            , cookie_(cookie)
            , actions_(std::move(actions))
        {
        }

        flow_entry(match_set const& match
                 , std::uint16_t const priority
                 , std::uint64_t const cookie
                 , action_list actions)
            : identifier_{match, priority}
            , cookie_(cookie)
            , actions_(std::move(actions))
        {
        }

        auto id() const noexcept
            -> flow_entry_id const&
        {
            return identifier_;
        }

        auto match() const noexcept
            -> match_set const&
        {
            return id().match();
        }

        auto priority() const noexcept
            -> std::uint16_t
        {
            return id().priority();
        }

        auto cookie() const noexcept
            -> std::uint64_t
        {
            return cookie_;
        }

        auto actions() const& noexcept
            -> action_list const&
        {
            return actions_;
        }

        auto actions() && noexcept
            -> action_list&&
        {
            return std::move(actions_);
        }

        void actions(action_list actions)
        {
            actions_ = std::move(actions);
        }

    private:
        flow_entry_id identifier_;
        std::uint64_t cookie_;
        action_list actions_;
    };


    class timeouts
    {
    public:
        constexpr timeouts(
                  std::uint16_t const idle_timeout
                , std::uint16_t const hard_timeout) noexcept
            : idle_timeout_(idle_timeout)
            , hard_timeout_(hard_timeout)
        {
        }

        constexpr auto idle_timeout() const noexcept
            -> std::uint16_t
        {
            return idle_timeout_;
        }

        constexpr auto hard_timeout() const noexcept
            -> std::uint16_t
        {
            return hard_timeout_;
        }

    private:
        std::uint16_t idle_timeout_;
        std::uint16_t hard_timeout_;
    };


    class counters
    {
    public:
        constexpr counters(
                  std::uint64_t const packet_count
                , std::uint64_t const byte_count) noexcept
            : packet_count_(packet_count)
            , byte_count_(byte_count)
        {
        }

        constexpr auto packet_count() const noexcept
            -> std::uint64_t
        {
            return packet_count_;
        }

        constexpr auto byte_count() const noexcept
            -> std::uint64_t
        {
            return byte_count_;
        }

    private:
        std::uint64_t packet_count_;
        std::uint64_t byte_count_;
    };


    class elapsed_time
    {
    public:
        constexpr elapsed_time(
                std::uint32_t const duration_sec
                , std::uint32_t const duration_nsec) noexcept
            : duration_sec_(duration_sec)
            , duration_nsec_(duration_nsec)
        {
        }

        constexpr auto duration_sec() const noexcept
            -> std::uint32_t
        {
            return duration_sec_;
        }

        constexpr auto duration_nsec() const noexcept
            -> std::uint32_t
        {
            return duration_nsec_;
        }

    private:
        std::uint32_t duration_sec_;
        std::uint32_t duration_nsec_;
    };

} // namespace v10
} // namespace openflow
} // namespace network
} // namespace canard

#endif // CANARD_NETWORK_OPENFLOW_V10_FLOW_ENTRY_HPP
