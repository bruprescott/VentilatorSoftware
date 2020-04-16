#include <Arduino.h>

#include "comms.h"
#include "serdes/serdes.h"

#define PACKET_LEN_MAX (32)
uint8_t tx_buffer[PACKET_LEN_MAX];

void comms_init() {

}

void comms_handler() {

}

void comms_sendResetState() {

}


void comms_sendPeriodicReadings(float pressure, float volume, float flow) {
    uint8_t readingsData[16];
    uint32_t time;

    time = millis();

    readingsData[0] = (time >> 24) & 0xFF;
    readingsData[1] = (time >> 16) & 0xFF;
    readingsData[2] = (time >> 8) & 0xFF;
    readingsData[3] = time & 0xFF;

    readingsData[4] = (((uint32_t) pressure) >> 24) & 0xFF;
    readingsData[5] = (((uint32_t) pressure) >> 16) & 0xFF;
    readingsData[6] = (((uint32_t) pressure) >> 8) & 0xFF;
    readingsData[7] = ((uint32_t) pressure) & 0xFF;

    readingsData[8]  = (((uint32_t) volume) >> 24) & 0xFF;
    readingsData[9]  = (((uint32_t) volume) >> 16) & 0xFF;
    readingsData[10] = (((uint32_t) volume) >> 8) & 0xFF;
    readingsData[11] = ((uint32_t) volume) & 0xFF;

    readingsData[12] = (((uint32_t) flow) >> 24) & 0xFF;
    readingsData[13] = (((uint32_t) flow) >> 16) & 0xFF;
    readingsData[14] = (((uint32_t) flow) >> 8) & 0xFF;
    readingsData[15] = ((uint32_t) flow) & 0xFF;

    serdes_encode_data_packet(readingsData, 16, tx_buffer, PACKET_LEN_MAX);
}
