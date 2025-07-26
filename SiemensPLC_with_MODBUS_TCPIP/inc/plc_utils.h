#ifndef PLC_UTILS_H
#define PLC_UTILS_H

#include <stdint.h>
#include <iostream>
#include <netinet/in.h>

typedef enum {
    plc_error_unknown = -1,
    plc_error_connection_failed
}plc_error_e;

typedef struct{
    int sock_id{-1};
    bool IsCONNECTED{false};
    bool IsSOCKET_CREATED{false};
    plc_error_e error_code{plc_error_unknown};
    int buff_len{-1};
    uint8_t buff[1024]{'\0'};
    struct sockaddr_in serv_addr;
}plc_handler_info_t;

namespace MODBUS_tcpip{
    enum MB_FUNCTIONS_e : uint8_t{
        MB_COILS_READ = 0x01,
        MB_INPUT_READ = 0x02,
        MB_HOLDING_REGISTER_READ = 0x03,
        MB_INPUT_REGISTER_READ = 0x04
    };

    typedef struct {
        uint16_t _transation{0};
        uint16_t _protocol_id{0};
        uint16_t _length{0};
        uint8_t _deviceID{0x40}; //default PLC_Hardware ID : 64 = 0x40
        MB_FUNCTIONS_e FC_n;     
        uint16_t _startRegister_FirstAddress{0x0000};
        uint16_t _reg_length{0x0000};
    }modbus_frame_t;

    typedef struct {
        uint16_t _transation{0};
        uint16_t _protocol_id{0};
        uint16_t _length{0};
        uint8_t _deviceID{0}; //default PLC_Hardware ID : 64 = 0x40
        MB_FUNCTIONS_e FC_n;     
        uint8_t _recvData_length;
        uint8_t _recvByte[512];
    }modbus_frame_decode_t;

};

#endif