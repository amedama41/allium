INCLUDES = -I../../include
LDFLAGS = -lboost_unit_test_framework-mt -lboost_system-mt
CXX = clang++
# CXX = g++-4.9
CXXFLAGS = -std=c++11 -stdlib=libc++ -Wall -pedantic $(INCLUDES)
# CXXFLAGS = -std=c++11 -Wall -pedantic $(INCLUDES)

UTILITY_TEST_SRCS = byteorder_test.cpp \
					action_list_test.cpp \
					action_set_test.cpp \
					any_action_test.cpp \
					any_oxm_match_field_test.cpp \
					instruction_set_test.cpp \
					oxm_match_test.cpp \
					packet_queue_test.cpp \
					port_test.cpp
UTILITY_TEST_OBJS = $(UTILITY_TEST_SRCS:.cpp=.o)

MESSAGE_TEST_SRCS = hello_test.cpp hello_elements_test.cpp \
					group_mod_test.cpp \
					table_feature_property_test.cpp \
					table_features_test.cpp
MESSAGE_TEST_OBJS = $(MESSAGE_TEST_SRCS:.cpp=.o)

ACTION_TEST_SRCS = output_test.cpp \
				   copy_ttl_in_test.cpp copy_ttl_out_test.cpp \
				   set_mpls_ttl_test.cpp decrement_mpls_ttl_test.cpp \
				   push_vlan_test.cpp pop_vlan_test.cpp \
				   push_mpls_test.cpp pop_mpls_test.cpp \
				   set_queue_test.cpp group_test.cpp \
				   set_nw_ttl_test.cpp decrement_nw_ttl_test.cpp \
				   set_field_test.cpp \
				   push_pbb_test.cpp pop_pbb_test.cpp
ACTION_TEST_OBJS = $(ACTION_TEST_SRCS:.cpp=.o)

INSTRUCTION_TEST_SRCS = goto_table_test.cpp \
						write_metadata_test.cpp \
						write_actions_test.cpp \
						apply_actions_test.cpp \
						clear_actions_test.cpp \
						meter_test.cpp
INSTRUCTION_TEST_OBJS = $(INSTRUCTION_TEST_SRCS:.cpp=.o)

OXM_MATCH_FIELD_TEST_SRCS = oxm_in_port_test.cpp \
							oxm_eth_dst_test.cpp \
							oxm_vlan_vid_test.cpp
OXM_MATCH_FIELD_TEST_OBJS = $(OXM_MATCH_FIELD_TEST_SRCS:.cpp=.o)

SRCS = $(UTILITY_TEST_SRCS) \
	   $(ACTION_TEST_SRCS) \
	   $(MESSAGE_TEST_SRCS) \
	   $(INSTRUCTION_TEST_SRCS) \
	   $(OXM_MATCH_FIELD_TEST_SRCS)
OBJS = $(SRCS:.cpp=.o)

TARGET = all_test

ALL_TESTS = $(OBJS:.o=)

DEPENDS = $(addprefix .,$(OBJS:.o=.depends))

define build_test
$(CXX) -DBOOST_TEST_MODULE=$@ $(CXXFLAGS) -c ../driver.cpp -o ../driver.o
$(CXX) $(CXXFLAGS) -o $@ ../driver.o $^ $(LDFLAGS)
endef

.PHONY: all clean run run_each

all: $(DEPENDS) $(TARGET)

.%.depends: %.cpp
	$(CXX) -MM $(CXXFLAGS) $< > $@

%: %.o
	$(build_test)

$(TARGET): $(OBJS)
	$(build_test)

utility_test: $(UTILITY_TEST_OBJS)
	$(build_test)

message_test: $(MESSAGE_TEST_OBJS)
	$(build_test)

oxm_match_field_test: $(OXM_MATCH_FIELD_TEST_OBJS)
	$(build_test)

action_test: $(ACTION_TEST_OBJS)
	$(build_test)

run: all
	./$(TARGET)

run_each: $(ALL_TESTS)
	for test in $(ALL_TESTS); do \
		echo ========= $$test ========= ; \
		./$$test ; \
		echo ; \
	done

clean:
	-rm *.o $(DEPENDS) $(ALL_TESTS) $(TARGET) utility_test message_test oxm_match_field_test action_test 2> /dev/null

-include $(DEPENDS)

