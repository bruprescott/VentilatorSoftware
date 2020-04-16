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
    // int *t = (int*) *arg;
    // printf("!!!decode %d\n", (*t));
    printf("!!!decode\n");
    return true;//pb_decode_string(stream, data->buf, data->len);
}


void test_Serialization() {
    uint8_t readingsData[16];
    readingsData[0] = 1;
    readingsData[1] = 2;
    readingsData[2] = 3;
    readingsData[3] = 4;

    readingsData[4] = 5;
    readingsData[5] = 6;
    readingsData[6] = 7;
    readingsData[7] = 8;

    readingsData[8]  = 9;
    readingsData[9]  = 10;
    readingsData[10] = 11;
    readingsData[11] = 12;

    readingsData[12] = 13;
    readingsData[13] = 14;
    readingsData[14] = 15;
    readingsData[15] = 16;

    size_t encoded_data_length;
    bool status = serdes_encode_data_packet(readingsData, 16, tx_buffer, PACKET_LEN_MAX, &encoded_data_length);
    for(int i = 0; i < PACKET_LEN_MAX; i++) {
    	printf("%d - %d\n", i, tx_buffer[i]);
    }

    printf("packet length: %d\n", (unsigned int) encoded_data_length);

	TEST_ASSERT_EQUAL_INT16(true, status);
	TEST_ASSERT_EQUAL_INT16(24, encoded_data_length);

    pb_istream_t stream = pb_istream_from_buffer(tx_buffer, PACKET_LEN_MAX);
    
    Packet packet = Packet_init_zero;
    packet.payload.data.data.funcs.decode = bytes_callback;

    status = pb_decode(&stream, Packet_fields, &packet);

	TEST_ASSERT_EQUAL_INT16(true, status);
	TEST_ASSERT_EQUAL_INT16(3, packet.which_payload);
    TEST_ASSERT_EQUAL_INT16(ControllerMsgType_DATA, packet.payload.data.msg_type);

}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_Serialization);
  return UNITY_END();
}
