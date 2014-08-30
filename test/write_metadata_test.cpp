#define BOOST_TEST_DYN_LINK
#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE write_metadata_test
#else
#undef  BOOST_TEST_MODULE
#endif
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <utility>
#include <vector>
#include <canard/network/protocol/openflow/v13/instruction/write_metadata.hpp>

namespace canard {
namespace network {
namespace openflow {
namespace v13 {

BOOST_AUTO_TEST_SUITE(write_metadata_test)

BOOST_AUTO_TEST_SUITE(instantiation_test)

BOOST_AUTO_TEST_CASE(constructor_test)
{
    auto const metadata = std::numeric_limits<std::uint64_t>::max();

    auto const sut = instructions::write_metadata{metadata};

    BOOST_CHECK_EQUAL(sut.type(), OFPIT_WRITE_METADATA);
    BOOST_CHECK_EQUAL(sut.length(), 24);
    BOOST_CHECK_EQUAL(sut.metadata(), metadata);
    BOOST_CHECK_EQUAL(sut.metadata_mask(), std::numeric_limits<std::uint64_t>::max());
}

BOOST_AUTO_TEST_CASE(constructor_test_when_has_mask)
{
    auto const metadata = std::numeric_limits<std::uint64_t>::max() - 1;
    auto const metadata_mask = 1;

    auto const sut = instructions::write_metadata{metadata, metadata_mask};

    BOOST_CHECK_EQUAL(sut.type(), OFPIT_WRITE_METADATA);
    BOOST_CHECK_EQUAL(sut.length(), 24);
    BOOST_CHECK_EQUAL(sut.metadata(), metadata);
    BOOST_CHECK_EQUAL(sut.metadata_mask(), metadata_mask);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test)
{
    auto sut = instructions::write_metadata{1, 0x00FF00FF00FF00FF};

    auto const copy = sut;

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.metadata(), sut.metadata());
    BOOST_CHECK_EQUAL(copy.metadata_mask(), sut.metadata_mask());
}

BOOST_AUTO_TEST_CASE(move_constructor_test)
{
    auto sut = instructions::write_metadata{0, 0x11FF00FF11FF00FF};

    auto const copy = std::move(sut);

    BOOST_CHECK_EQUAL(copy.type(), sut.type());
    BOOST_CHECK_EQUAL(copy.length(), sut.length());
    BOOST_CHECK_EQUAL(copy.metadata(), sut.metadata());
    BOOST_CHECK_EQUAL(copy.metadata_mask(), sut.metadata_mask());
}

BOOST_AUTO_TEST_SUITE_END() // instantiation_test

BOOST_AUTO_TEST_CASE(encode_decode_test)
{
    auto buffer = std::vector<std::uint8_t>{};
    auto const sut = instructions::write_metadata{1234, 5678};

    sut.encode(buffer);

    BOOST_CHECK_EQUAL(buffer.size(), sut.length());

    auto it = buffer.begin();
    auto const decoded_instruction = instructions::write_metadata::decode(it, buffer.end());

    BOOST_CHECK_EQUAL(decoded_instruction.type(), sut.type());
    BOOST_CHECK_EQUAL(decoded_instruction.length(), sut.length());
    BOOST_CHECK_EQUAL(decoded_instruction.metadata(), sut.metadata());
    BOOST_CHECK_EQUAL(decoded_instruction.metadata_mask(), sut.metadata_mask());
}

BOOST_AUTO_TEST_SUITE_END() // write_metadata_test

} // namespace v13
} // namespace openflow
} // namespace network
} // namespace canard
