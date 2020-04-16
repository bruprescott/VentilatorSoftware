#include <pb_encode.h>
#include <pb_common.h>
#include "network_protocol.pb.h"

/* This function is the core of the union encoding process. It handles
 * the top-level pb_field_t array manually, in order to encode a correct
 * field tag before the message. The pointer to MsgType_fields array is
 * used as an unique identifier for the message type.
 */
bool encode_unionmessage(pb_ostream_t *stream, const pb_msgdesc_t *messagetype, void *message)
{
    pb_field_iter_t iter;

    if (!pb_field_iter_begin(&iter, Packet_fields, message))
        return false;

    do
    {
        if (iter.submsg_desc == messagetype)
        {
            /* This is our field, encode the message using it. */
            if (!pb_encode_tag_for_field(stream, &iter))
                return false;
            
            return pb_encode_submessage(stream, messagetype, message);
        }
    } while (pb_field_iter_next(&iter));
    
    /* Didn't find the field for messagetype */
    return false;
}

struct mydata_t {
  size_t len;
  uint8_t *buf;
};

static bool bytes_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
    mydata_t *data = (mydata_t*) *arg;
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, data->buf, data->len);
}

bool serdes_encode_data_packet(uint8_t *data, uint8_t data_len, uint8_t *tx_buffer, uint8_t tx_buffer_len) {
    pb_ostream_t stream = pb_ostream_from_buffer(tx_buffer, tx_buffer_len);
    
    ControllerDataPacket packet = ControllerDataPacket_init_zero;
    packet.msg_type = ControllerMsgType_DATA;
    
    mydata_t tmp = {data_len, data};
    packet.data.funcs.encode = bytes_callback;
    packet.data.arg = &tmp;

    bool status = encode_unionmessage(&stream, ControllerDataPacket_fields, &packet);
    return status;
}
