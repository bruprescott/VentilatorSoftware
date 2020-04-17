#include <pb_encode.h>
#include <pb_decode.h>
#include <pb_common.h>
#include "serdes.h"
bool serdes_encode_status_packet(uint64_t time, float pressure, float volume, float flow, uint8_t *tx_buffer, uint8_t tx_buffer_len, size_t *encoded_len) {
    pb_ostream_t stream = pb_ostream_from_buffer(tx_buffer, tx_buffer_len);
    
    ControllerStatus controller_status = ControllerStatus_init_zero;
    controller_status.time = time;
    controller_status.pressure = pressure;
    controller_status.volume = volume;
    controller_status.flow = flow;

    ControllerData cdp = ControllerData_init_zero;
    cdp.msg_type = ControllerMsgType_DATA;
    cdp.response_to_cmd = CommandType_NONE;
    cdp.which_payload = ControllerData_status_tag;
    cdp.payload.status = controller_status;

    Packet packet = Packet_init_zero;
    packet.which_payload = Packet_data_tag;
    packet.payload.data = cdp;

    bool status = pb_encode(&stream, Packet_fields, &packet);

    *encoded_len = stream.bytes_written;

    return status;
}


bool serdes_decode_incomming_packet(uint8_t *rx_buffer, uint8_t rx_buffer_len, size_t encoded_len, GuiAckHandler_t ack_handler, CommandHandler_t command_handler) {
    pb_istream_t stream = pb_istream_from_buffer(rx_buffer, rx_buffer_len);
    Packet packet = Packet_init_zero;
    bool status = pb_decode(&stream, Packet_fields, &packet);
    if(Packet_cmd_tag == packet.which_payload) {
        command_handler(packet.payload.cmd);
    } else if(Packet_gui_ack_tag == packet.which_payload) {
        ack_handler(packet.payload.gui_ack);
    } else {
        return false;
    }

    return status;
}
