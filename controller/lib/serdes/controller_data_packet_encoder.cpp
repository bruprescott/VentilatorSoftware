#include <pb_encode.h>
#include <pb_common.h>
#include "network_protocol.pb.h"


struct mydata_t {
  size_t len;
  uint8_t *buf;
};

static bool bytes_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
    mydata_t *data = (mydata_t*) *arg;
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, data->buf, data->len);
}

bool serdes_encode_data_packet(uint8_t *data, uint8_t data_len, uint8_t *tx_buffer, uint8_t tx_buffer_len, size_t *encoded_len) {
    pb_ostream_t stream = pb_ostream_from_buffer(tx_buffer, tx_buffer_len);
    
    Packet packet = Packet_init_zero;
    packet.which_payload = Packet_data_tag;
    packet.payload.data.msg_type = ControllerMsgType_DATA;
    
    mydata_t tmp = {data_len, data};
    packet.payload.data.data.funcs.encode = bytes_callback;
    packet.payload.data.data.arg = &tmp;

    bool status = pb_encode(&stream, Packet_fields, &packet);
    *encoded_len = stream.bytes_written;

    return status;
}
