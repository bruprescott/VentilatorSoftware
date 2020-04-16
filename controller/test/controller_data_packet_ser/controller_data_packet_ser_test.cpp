#include "unity.h"

#include <stdio.h>
#include <pb_encode.h>
#include <pb_common.h>
#include "serdes.h"

#define PACKET_LEN_MAX (32)
uint8_t tx_buffer[PACKET_LEN_MAX];


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

    size_t tx_data_length;
    bool status = serdes_encode_data_packet(readingsData, 16, tx_buffer, PACKET_LEN_MAX, &tx_data_length);
    for(int i = 0; i < PACKET_LEN_MAX; i++) {
    	printf("%d - %d\n", i, tx_buffer[i]);
    }
    printf("packet length: %d\n", tx_data_length);

	TEST_ASSERT_EQUAL_INT16(status, 1);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_Serialization);
  return UNITY_END();
}
