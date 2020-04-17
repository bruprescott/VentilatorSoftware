#include <Arduino.h>

#include "comms.h"
#include "serdes.h"
#include "serialIO.h"
#include "version.h"
#include "network_protocol.pb.h"

#define PACKET_LEN_MAX (32)
uint8_t tx_buffer[PACKET_LEN_MAX];
uint16_t tx_data_length;
bool output_buffer_ready = false;

uint8_t rx_buffer[PACKET_LEN_MAX];
uint16_t rx_data_length;
uint8_t rx_idx = 0;
uint64_t last_rx = 0;
uint8_t rx_in_progress = false;
#define RX_TIMEOUT 1

void comms_init() {
    serialIO_init();
}

void command_handler(Command) {
}

void ack_handler(GuiAck) {
}

inline static bool is_time_process_packet() {
	return millis() - last_rx > RX_TIMEOUT;
}

void comms_sendResetState() {
}

void comms_sendPeriodicReadings(float pressure, float volume, float flow) {
    //TODO solve tx overflow 
    if(output_buffer_ready) {
    	return;
    }
    uint64_t time;
    time = millis();
    bool status = serdes_encode_status_packet(time, pressure, volume, flow, tx_buffer, PACKET_LEN_MAX, &tx_data_length);
    if(status) {
    	output_buffer_ready = true;
    }
}

//NOTE this is work in progress. Proper framing incomming. 
//TODO run this via DMA to free up resources for control loops
void process_tx() {
	if(output_buffer_ready) {
		for(uint8_t i = 0; i < tx_data_length; i++) {
			serialIO_send(tx_buffer[i]);
		}
		output_buffer_ready = false;
	}
}

void process_rx() {
	while(serialIO_dataAvailable()) {
		rx_in_progress = true;
		char b;
		serialIO_readByte(&b);
		rx_buffer[rx_idx++] = (uint8_t) b;
		if(rx_idx >= PACKET_LEN_MAX) {
			rx_idx = 0;
			break;
		}
		last_rx = millis();
	}

	//timeouts are lameeeee
	if(rx_in_progress && is_time_process_packet()) {
		serdes_decode_incomming_packet(rx_buffer, PACKET_LEN_MAX, rx_idx - 1, ack_handler, command_handler);
		rx_idx = 0;
		rx_in_progress = false;
	}
}

void comms_handler() {
	process_tx();
	process_rx();
}
