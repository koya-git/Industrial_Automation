#ifndef PLC_CONNECTION_MODBUS_H
#define PLC_CONNECTION_MODBUS_H

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <memory>
#include <thread>
#include <iomanip>
#include <cstdint>
#include "plc_utils.h"
#include "VirtualCallback_function.h"
#include <functional>
#include <map>

class MODBUS_SERVICE{
    private:
        std::map<MODBUS_tcpip::MB_FUNCTIONS_e, MODBUS_tcpip::modbus_frame_t> _modbus_func_contaier;
    public:
        void register_modbus_elements(MODBUS_tcpip::MB_FUNCTIONS_e FC_n,uint16_t _s, uint16_t length);
        int get_container_length(){ return _modbus_func_contaier.size(); }
        std::map<MODBUS_tcpip::MB_FUNCTIONS_e, MODBUS_tcpip::modbus_frame_t> get_container(){ return _modbus_func_contaier; }
};


class tcp_connection{
    static tcp_connection* _tcp_instance;
    private:
        plc_handler_info_t _handler{0};
        bool isInitialconfigDone{false};
        tcp_connection(){ }
    public:
        /* creation design pattern : signgletone class model */
        static tcp_connection& getTCP_ConnectionInstance(){
            if(_tcp_instance == nullptr){
                _tcp_instance = new tcp_connection();
            }
            return (*_tcp_instance);
        }

        /* PLC connection type : MODBUS tcp/ip */
        void connect_MODBUS_SERVER(std::string, int);
        void start_process(std::function<void(void)> funcPointer,MODBUS_SERVICE* obj);
        ~tcp_connection(){}
};

#endif