#define BOOST_TEST_DYN_LINK
#include <canard/integer_sequence.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(integer_sequence_test)

BOOST_AUTO_TEST_CASE(zero)
{
    using sut = canard::make_index_sequence<0>;
    using expected = canard::index_sequence<>;
    BOOST_TEST((std::is_same<sut, expected>::value));
}

BOOST_AUTO_TEST_CASE(one)
{
    using sut = canard::make_index_sequence<1>;
    using expected = canard::index_sequence<0>;
    BOOST_TEST((std::is_same<sut, expected>::value));
}

BOOST_AUTO_TEST_CASE(two)
{
    using sut = canard::make_index_sequence<2>;
    using expected = canard::index_sequence<0, 1>;
    BOOST_TEST((std::is_same<sut, expected>::value));
}

BOOST_AUTO_TEST_CASE(odd)
{
    using sut = canard::make_index_sequence<3>;
    using expected = canard::index_sequence<0, 1, 2>;
    BOOST_TEST((std::is_same<sut, expected>::value));
}

BOOST_AUTO_TEST_CASE(even)
{
    using sut = canard::make_index_sequence<4>;
    using expected = canard::index_sequence<0, 1, 2, 3>;
    BOOST_TEST((std::is_same<sut, expected>::value));
}

BOOST_AUTO_TEST_CASE(big)
{
    using sut = canard::make_index_sequence<127>;
    using expected = canard::index_sequence<
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
        47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
        62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
        77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
        92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
        105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
        117, 118, 119, 120, 121, 122, 123, 124, 125, 126
    >;
    BOOST_TEST((std::is_same<sut, expected>::value));
}

BOOST_AUTO_TEST_SUITE_END() // integer_sequence_test
