#ifndef __SERDES_H
#define __SERDES_H

bool serdes_encode_status_packet(uint64_t time, float pressure, float volume, float flow, uint8_t *tx_buffer, uint8_t tx_buffer_len, size_t *encoded_len);

#endif