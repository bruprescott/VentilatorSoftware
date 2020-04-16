#ifndef __SERDES_H
#define __SERDES_H

bool serdes_encode_data_packet(uint8_t *data, uint8_t data_len, uint8_t *tx_buffer, uint8_t tx_buffer_len);

#endif