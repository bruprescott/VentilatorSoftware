#include "unity.h"

#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <pb_common.h>
#include "serdes.h"
#include "network_protocol.pb.h"

#define PACKET_LEN_MAX (32)
uint8_t tx_buffer[PACKET_LEN_MAX];

struct mydata_t {
  size_t len;
  uint8_t buf[16];
};

static bool bytes_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    printf("!!!decode\n");
    return true;
}


void test_Serialization() {
    size_t encoded_data_length;
    uint64_t time = 1;
    float pressure = 2.2;
    float volume = 3.3;
    float flow = 4.4;

    bool status = serdes_encode_status_packet(time, pressure, volume, flow, tx_buffer, PACKET_LEN_MAX, &encoded_data_length);

	TEST_ASSERT_EQUAL_INT16(true, status);
	TEST_ASSERT_EQUAL_INT16(28, encoded_data_length);

    pb_istream_t stream = pb_istream_from_buffer(tx_buffer, encoded_data_length);
    
    Packet packet = Packet_init_zero;

    status = pb_decode(&stream, Packet_fields, &packet);

	TEST_ASSERT_EQUAL_INT16(true, status);
	TEST_ASSERT_EQUAL_INT16(Packet_data_tag, packet.which_payload);
    TEST_ASSERT_EQUAL_INT16(ControllerMsgType_DATA, packet.payload.data.msg_type);
    TEST_ASSERT_EQUAL_INT16(ControllerData_status_tag, packet.payload.data.which_payload);
    ControllerStatus s = packet.payload.data.payload.status;
    TEST_ASSERT_EQUAL_INT16(s.time, time);
    TEST_ASSERT_EQUAL_INT16(s.pressure, pressure);
    TEST_ASSERT_EQUAL_INT16(s.volume, volume);
    TEST_ASSERT_EQUAL_INT16(s.flow, flow);
    // TEST_ASSERT_EQUAL_INT16(s.alarm_flags, time);
}



int main() {
  UNITY_BEGIN();
  RUN_TEST(test_Serialization);
  return UNITY_END();
}
