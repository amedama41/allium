#define BOOST_TEST_DYN_LINK
#include <canard/network/protocol/openflow/vector_buffer.hpp>
#include <canard/network/protocol/openflow/v13/message/multipart_message/table_features.hpp>
#include <boost/test/unit_test.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(table_features_test)

struct table_features_fixture {
    static auto create_features(std::uint8_t const table_id)
        -> table_features
    {
        auto const table_name = "table" + std::to_string(table_id);
        return table_features{table_id, table_name.c_str(), 0xffffffffffffffff, 0xffffffffffffffff, 0, 0xffffffff
            , {
                  table_feature_properties::prop_instructions{
                      instruction_id{protocol::OFPIT_GOTO_TABLE}, instruction_id{protocol::OFPIT_WRITE_METADATA}
                    , instruction_id{protocol::OFPIT_WRITE_ACTIONS}, instruction_id{protocol::OFPIT_APPLY_ACTIONS}
                    , instruction_id{protocol::OFPIT_CLEAR_ACTIONS}, instruction_id{protocol::OFPIT_METER}
                    , instruction_experimenter_id{32, {'A', 'B'}}
                  } // 4 + 4 * 6 + (8 + 2) = 38 => 40
                , table_feature_properties::prop_next_tables{
                    1, 2, 3, 4, 5, 6, 7
                  } // 4 + 7 = 11 => 16
                , table_feature_properties::prop_write_actions{
                      action_id{protocol::OFPAT_OUTPUT}, action_id{protocol::OFPAT_COPY_TTL_OUT}, action_id{protocol::OFPAT_COPY_TTL_IN}
                    , action_id{protocol::OFPAT_SET_MPLS_TTL}, action_id{protocol::OFPAT_DEC_MPLS_TTL}
                    , action_id{protocol::OFPAT_PUSH_VLAN}, action_id{protocol::OFPAT_POP_VLAN}
                    , action_id{protocol::OFPAT_PUSH_MPLS}, action_id{protocol::OFPAT_POP_MPLS}
                    , action_id{protocol::OFPAT_SET_QUEUE}, action_id{protocol::OFPAT_GROUP}
                    , action_id{protocol::OFPAT_SET_NW_TTL}, action_id{protocol::OFPAT_DEC_NW_TTL}
                    , action_id{protocol::OFPAT_SET_FIELD}
                    , action_id{protocol::OFPAT_PUSH_PBB}, action_id{protocol::OFPAT_POP_PBB}
                  } // 4 + 4 * 16 = 68 => 72
                , table_feature_properties::prop_apply_actions{
                      action_id{protocol::OFPAT_OUTPUT}, action_id{protocol::OFPAT_COPY_TTL_OUT}, action_id{protocol::OFPAT_COPY_TTL_IN}
                    , action_id{protocol::OFPAT_SET_MPLS_TTL}, action_id{protocol::OFPAT_DEC_MPLS_TTL}
                    , action_id{protocol::OFPAT_PUSH_VLAN}, action_id{protocol::OFPAT_POP_VLAN}
                    , action_id{protocol::OFPAT_PUSH_MPLS}, action_id{protocol::OFPAT_POP_MPLS}
                    , action_id{protocol::OFPAT_SET_QUEUE}, action_id{protocol::OFPAT_GROUP}
                    , action_id{protocol::OFPAT_SET_NW_TTL}, action_id{protocol::OFPAT_DEC_NW_TTL}
                    , action_id{protocol::OFPAT_SET_FIELD}
                    , action_id{protocol::OFPAT_PUSH_PBB}, action_id{protocol::OFPAT_POP_PBB}
                  } // 4 + 4 * 16 = 68 => 72
                , table_feature_properties::prop_match{
                      oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IN_PORT, false, 4}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_METADATA, false, 8}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_DST, true, 12}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_SRC, true, 12}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_TYPE, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID, true, 4}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_PCP, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_DSCP, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_ECN, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_PROTO, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_SRC, true, 8}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_DST, true, 8}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_SRC, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_DST, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_SRC, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_DST, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_SRC, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_DST, false, 2}
                    , oxm_experimenter_id{(std::uint32_t{protocol::OFPXMC_EXPERIMENTER} << 16) | 16, 32}
                  } // 4 + 4 * 18 + 8 = 84 => 88
                , table_feature_properties::prop_write_setfield{
                      oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_DST, true, 12}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_SRC, true, 12}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_TYPE, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID, true, 4}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_PCP, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_DSCP, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_ECN, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_PROTO, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_SRC, true, 8}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_DST, true, 8}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_SRC, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_DST, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_SRC, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_DST, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_SRC, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_DST, false, 2}
                  } // 4 + 4 * 16 = 68 => 72
                , table_feature_properties::prop_apply_setfield{
                      oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_DST, true, 12}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_SRC, true, 12}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_ETH_TYPE, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_VID, true, 4}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_VLAN_PCP, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_DSCP, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_ECN, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IP_PROTO, false, 1}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_SRC, true, 8}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_IPV4_DST, true, 8}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_SRC, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_TCP_DST, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_SRC, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_UDP_DST, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_SRC, false, 2}
                    , oxm_id{protocol::OFPXMC_OPENFLOW_BASIC, protocol::OFPXMT_OFB_SCTP_DST, false, 2}
                  } // 4 + 4 * 16 = 68 => 72
              } // 40 + 16 + 72 + 72 + 88 + 72 + 72 = 432
        }; // 64 + 432 = 496
    }

    table_features features0 = create_features(0);
    table_features features1 = create_features(1);
    table_features features2 = create_features(2);
    table_features features3 = create_features(3);
    table_features features4 = create_features(4);
    table_features features5 = create_features(5);
    table_features features6 = create_features(6);
    table_features features7 = create_features(7);
};

BOOST_AUTO_TEST_SUITE(table_features_request_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        auto const sut = table_features_request{};

        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_MULTIPART_REQUEST);
        BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_multipart_request));
        BOOST_CHECK_EQUAL(sut.multipart_type(), protocol::OFPMP_TABLE_FEATURES);
        BOOST_CHECK_EQUAL(sut.flags(), 0);
    }

    BOOST_FIXTURE_TEST_CASE(constructor_test, table_features_fixture)
    {
        auto const sut = table_features_request{
            features0, features1, features2, features3, features4, features5, features6, features7
        };

        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_MULTIPART_REQUEST);
        BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_multipart_request) + 496 * 8);
        BOOST_CHECK_EQUAL(sut.multipart_type(), protocol::OFPMP_TABLE_FEATURES);
        BOOST_CHECK_EQUAL(sut.flags(), 0);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

struct encode_decode_fixture : table_features_fixture {
    table_features_request const sut = table_features_request{
        features0, features1, features2, features3, features4, features5, features6, features7
    };
    std::vector<std::uint8_t> buffer{};
};
BOOST_FIXTURE_TEST_SUITE(encode_decode_test, encode_decode_fixture)

    BOOST_AUTO_TEST_CASE(encode_test)
    {
        sut.encode(buffer);

        BOOST_CHECK_EQUAL(buffer.size(), sut.length());
    }

BOOST_AUTO_TEST_SUITE_END() // encode_decode_test

BOOST_AUTO_TEST_SUITE_END() // table_features_request_test


BOOST_AUTO_TEST_SUITE(table_features_reply_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

    BOOST_AUTO_TEST_CASE(default_constructor_test)
    {
        auto const sut = table_features_reply{{}};

        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_MULTIPART_REPLY);
        BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_multipart_reply));
        BOOST_CHECK_EQUAL(sut.multipart_type(), protocol::OFPMP_TABLE_FEATURES);
        BOOST_CHECK_EQUAL(sut.flags(), 0);
    }

    BOOST_FIXTURE_TEST_CASE(constructor_test, table_features_fixture)
    {
        auto const sut = table_features_reply{
            features0, features1, features2, features3, features4, features5, features6, features7
        };

        BOOST_CHECK_EQUAL(sut.version(), protocol::OFP_VERSION);
        BOOST_CHECK_EQUAL(sut.type(), protocol::OFPT_MULTIPART_REPLY);
        BOOST_CHECK_EQUAL(sut.length(), sizeof(v13_detail::ofp_multipart_reply) + 496 * 8);
        BOOST_CHECK_EQUAL(sut.multipart_type(), protocol::OFPMP_TABLE_FEATURES);
        BOOST_CHECK_EQUAL(sut.flags(), 0);
    }

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

struct encode_decode_fixture : table_features_fixture { // TODO
    table_features_request const request = table_features_request{
        features0, features1, features2, features3, features4, features5, features6, features7
    };
    table_features_reply const sut = table_features_reply{
        features0, features1, features2, features3, features4, features5, features6, features7
    };
    std::vector<std::uint8_t> buffer = request.encode();
};
BOOST_FIXTURE_TEST_SUITE(encode_decode_test, encode_decode_fixture)

    BOOST_AUTO_TEST_CASE(decode_test)
    {
        // TODO
        buffer[1] = protocol::OFPT_MULTIPART_REPLY;
        auto it = buffer.begin();

        auto const decoded_msg = table_features_reply::decode(it, buffer.end());

        BOOST_REQUIRE(it == buffer.end());
        BOOST_CHECK_EQUAL(decoded_msg.version(), sut.version());
        BOOST_CHECK_EQUAL(decoded_msg.type(), sut.type());
        BOOST_CHECK_EQUAL(decoded_msg.length(), sut.length());
        BOOST_CHECK_EQUAL(decoded_msg.xid(), sut.xid() - 1); // TODO
        BOOST_CHECK_EQUAL(decoded_msg.multipart_type(), sut.multipart_type());
        BOOST_CHECK_EQUAL(decoded_msg.flags(), sut.flags());
    }

BOOST_AUTO_TEST_SUITE_END() // encode_decode_test

BOOST_AUTO_TEST_SUITE_END() // table_features_reply_test

BOOST_AUTO_TEST_SUITE_END() // table_features_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
