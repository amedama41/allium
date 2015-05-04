import sys
from parser import DeclarationCollector
import generator.ofheader as ofheader
import generator.error_code as error_code
import generator.enum_to_string as enum_to_string
import generator.byteorder as byteorder
import generator.fusion_adaptor as fusion_adaptor

MACRO_TYPE_MAP = {
        10: {
            'OFP_VERSION': 'std::uint8_t',
            'OFP_MAX_TABLE_NAME_LEN': 'std::size_t',
            'OFP_MAX_PORT_NAME_LEN' : 'std::size_t',
            'OFP_TCP_PORT': 'std::uint16_t',
            'OFP_SSL_PORT': 'std::uint16_t',
            'OFP_ETH_ALEN': 'std::size_t',
            'OFP_DEFAULT_MISS_SEND_LEN': 'std::uint16_t',
            'OFP_VLAN_NONE': 'std::uint16_t',
            'OFP_DL_TYPE_ETH2_CUTOFF': 'std::uint16_t',
            'OFP_DL_TYPE_NOT_ETH_TYPE': 'std::uint16_t',
            'OFP_FLOW_PERMANENT': 'std::uint16_t',
            'OFP_DEFAULT_PRIORITY': 'std::uint16_t',
            'DESC_STR_LEN': 'std::size_t',
            'SERIAL_NUM_LEN': 'std::size_t',
            'OFPQ_ALL': 'std::uint32_t',
            'OFPQ_MIN_RATE_UNCFG': 'std::uint16_t',
        },
        13: {
            'OFP_VERSION': 'std::uint8_t',
            'OFP_TCP_PORT': 'std::uint16_t',
            'OFP_SSL_PORT': 'std::uint16_t',
            'OFP_DEFAULT_MISS_SEND_LEN': 'std::uint16_t',
            'OFP_FLOW_PERMANENT': 'std::uint16_t',
            'OFP_DEFAULT_PRIORITY': 'std::uint16_t',
            'OFP_NO_BUFFER': 'std::uint32_t',
            'OFPQ_ALL': 'std::uint32_t',
            'OFPQ_MIN_RATE_UNCFG': 'std::uint16_t',
            'OFPQ_MAX_RATE_UNCFG': 'std::uint16_t',
        },
}

IGNORE_TO_STRING_ENUMS = {
        10: {
            'OFPPS_STP_MASK',
            'OFPFW_NW_SRC_SHIFT', 'OFPFW_NW_SRC_BITS',
            'OFPFW_NW_DST_SHIFT', 'OFPFW_NW_DST_BITS',
        },
        13: {},
}

IGNORE_ERROR_TYPE = {
        10: {},
        13: {'OFPET_EXPERIMENTER'},
}

if len(sys.argv) != 2:
    print 'usage {} <openflow.h path>'.format(sys.argv[0])
    sys.exit()

collector = DeclarationCollector()
filename = sys.argv[1]
collector.collect(filename)

dirname = 'of{}'.format(collector.version)
with open(dirname + '/openflow.hpp', 'w') as f:
    f.write(ofheader.generate(collector, MACRO_TYPE_MAP[collector.version]))
with open(dirname + '/error.hpp', 'w') as f:
    f.write(error_code.generate(collector, IGNORE_ERROR_TYPE[collector.version]))
with open(dirname + '/enum_to_string.hpp', 'w') as f:
    f.write(enum_to_string.generate(collector, IGNORE_TO_STRING_ENUMS[collector.version]))
with open(dirname + '/byteorder.hpp', 'w') as f:
    f.write(byteorder.generate(collector))
with open(dirname + '/fusion_adaptor.hpp', 'w') as f:
    f.write(fusion_adaptor.generate(collector))

