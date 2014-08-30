#ifndef CANARD_UNIT_TEST_HPP
#define CANARD_UNIT_TEST_HPP

#include <boost/test/unit_test.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#define CANARD_FIXTURE_TEST_CASE(test_name, F) \
    BOOST_FIXTURE_TEST_CASE(test_name, BOOST_PP_TUPLE_ENUM(2, (BOOST_AUTO_TEST_CASE_FIXTURE, F)))

#define CANARD_CHECK_EQUAL_COLLECTIONS(collection1, collection2) \
    do { \
        auto&& lhs = collection1; \
        auto&& rhs = collection2; \
        using std::begin; \
        using std::end; \
        BOOST_CHECK_EQUAL_COLLECTIONS(begin(lhs), end(lhs), begin(rhs), end(rhs)); \
    } while (false)

#endif // CANARD_UNIT_TEST_HPP
