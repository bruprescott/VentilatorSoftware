#include <Arduino.h>

#include "comms.h"
#include "serdes.h"
#include "serialIO.h"
#include "version.h"

#define PACKET_LEN_MAX (32)
uint8_t tx_buffer[PACKET_LEN_MAX];
uint16_t tx_data_length;
bool output_buffer_ready = false;

void comms_init() {
    serialIO_init();
}

void comms_handler() {
	//TODO run this via DMA to free up resources for control loops
	if(output_buffer_ready) {
		for(uint8_t i = 0; i < tx_data_length; i++) {
			serialIO_send(tx_buffer[i]);
		}
		output_buffer_ready = false;
	}
}

void comms_sendResetState() {
}

void comms_sendPeriodicReadings(float pressure, float volume, float flow) {
    uint8_t readingsData[16];
    uint32_t time;

    time = millis();
    bool status = serdes_encode_status_packet(time, pressure, volume, flow, tx_buffer, PACKET_LEN_MAX, &tx_data_length);
    if(status) {
    	output_buffer_ready = true;
    }
}
