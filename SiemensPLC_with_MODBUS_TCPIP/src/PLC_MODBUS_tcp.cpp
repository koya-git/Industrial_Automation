#include "PLC_MODBUS_tcp.h"

tcp_connection* tcp_connection::_tcp_instance = nullptr;
/* virtual function defination */
__cb onPLC_Connected_callback(plc_handler_info_t&){}
__cb onPLC_Error_callback(plc_handler_info_t&, const std::string, const plc_error_e){}
__cb onTHREAD_EXIT_callback(){}
__cb onMODBUS_Data_Change_Event_Callback(MODBUS_tcpip::MB_FUNCTIONS_e, int indx, uint16_t data){}
void tcp_connection::connect_MODBUS_SERVER(std::string remote_ip, int _remote_port=502){
    if(!isInitialconfigDone){
        /* reset Handler : null zero */
        memset(&_handler, 0, sizeof(_handler));
        /* convert string ip address format to network structure format : uint32_t LE format */
        inet_pton(AF_INET, remote_ip.c_str(), &(_handler.serv_addr.sin_addr));
        /* convert int port to network format : Uint16_t */
        _handler.serv_addr.sin_port = htons(_remote_port);
        _handler.serv_addr.sin_family = AF_INET;
        isInitialconfigDone = true;
    }
    if(!_handler.IsSOCKET_CREATED && (isInitialconfigDone)){
        if ((_handler.sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            _handler.IsSOCKET_CREATED = false;
            onPLC_Error_callback(_handler,"failed to create socket_fd",plc_error_unknown);
            return;
        }
        _handler.IsSOCKET_CREATED = true;       //flag : socket created & got new socket fd
        _handler.IsCONNECTED = false;           //flag : define no server conncted yet 
        struct timeval timeout;
        timeout.tv_sec = 4;  // 4 seconds timeout
        timeout.tv_usec = 0;
        setsockopt(_handler.sock_id, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
    }
    if(_handler.IsCONNECTED){
        /* already server is connected */
        onPLC_Connected_callback(_handler);
        return;
    }
    /* ================== do connection established  process ======================= */
    if (connect(_handler.sock_id, (struct sockaddr*)&_handler.serv_addr,sizeof(_handler.serv_addr)) < 0) {
         /* serever connection failed */
        _handler.IsCONNECTED = false;
        onPLC_Error_callback(_handler,"failed to connect plc "+remote_ip + " - " + std::to_string(_remote_port),plc_error_connection_failed);
        return;
    }
    /* server connected sucessfully */
    _handler.IsCONNECTED = true;
    onPLC_Connected_callback(_handler);
}


void tcp_connection::start_process(std::function<void(void)> funcPointer,MODBUS_SERVICE* obj){
    if(!_handler.IsCONNECTED){
        onPLC_Error_callback(_handler,"failed to start std::thread ? plc is not connected yet.",plc_error_connection_failed);
        return;
    }
    /* PLC is connected ready to start thread */
    std::thread t([&](){
        funcPointer();
        uint16_t bufferX[2][256];
        while(_handler.IsCONNECTED){
            for(auto& frame : obj->get_container()){
                if(_handler.buff_len){ memset(&_handler.buff[0],0,_handler.buff_len+1); }
                int ret = send(_handler.sock_id, &frame.second, sizeof(MODBUS_tcpip::modbus_frame_t),0);
                _handler.buff_len = recv(_handler.sock_id, &_handler.buff[0],1023,0);
                if(_handler.buff_len){
                    MODBUS_tcpip::modbus_frame_decode_t* recv_frame = (MODBUS_tcpip::modbus_frame_decode_t*)&(_handler.buff[0]);   
                    if(recv_frame->FC_n == MODBUS_tcpip::MB_INPUT_REGISTER_READ || recv_frame->FC_n == MODBUS_tcpip::MB_HOLDING_REGISTER_READ){
                        /* this is 16bit */
                        for(int i=0,ix=0;i<recv_frame->_recvData_length;i+=2,ix++){ 
                             bufferX[0][ix] = (recv_frame->_recvByte[i] << 8) | (recv_frame->_recvByte[i+1]); }
                        for(int i=0;i<(recv_frame->_recvData_length/2);i++){
                            if(bufferX[0][i] !=bufferX[1][i]){
                                /* notify If data change Occure */
                                onMODBUS_Data_Change_Event_Callback(recv_frame->FC_n,i, bufferX[0][i]);
                            }
                        }
                        /* copy current data into previous data */
                        memcpy(&bufferX[1][0], &bufferX[0][0], recv_frame->_recvData_length);
                    }
                    else if(recv_frame->FC_n == MODBUS_tcpip::MB_COILS_READ || recv_frame->FC_n == MODBUS_tcpip::MB_INPUT_READ){
                        /* this is 1 Bit */
                    }
                }
                else{
                    onPLC_Error_callback(_handler, "data received Error For Function="+ std::to_string(static_cast<int>(frame.first)), plc_error_unknown);
                    _handler.IsCONNECTED = false;
                    /* closed socket Connection*/
                    close(_handler.sock_id);
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        /* this will triger once Exit from Thread */
        onTHREAD_EXIT_callback();
    });
    t.join();
    /* this will triger once Exit from Thread */
    onTHREAD_EXIT_callback();
}

/* this will register service to fectch data on preodically */
void MODBUS_SERVICE::register_modbus_elements(MODBUS_tcpip::MB_FUNCTIONS_e FC_n,uint16_t _s, uint16_t length){
    if(FC_n <=0){ return; }
    MODBUS_tcpip::modbus_frame_t* _newFrame = new MODBUS_tcpip::modbus_frame_t();
    _newFrame->_transation = (0x01 << 8) | 0x00;
    _newFrame->_length = (0x06 << 8) | 0x00 ;
    _newFrame->FC_n = FC_n;
    _newFrame->_deviceID = 0x40;     //SLAVE ID = 0d64
    _newFrame->_startRegister_FirstAddress = _s;
    _newFrame->_reg_length = ((length) << 8) | length >> 8 ;
    memcpy(&_modbus_func_contaier[FC_n],_newFrame, sizeof(MODBUS_tcpip::modbus_frame_t));
    delete _newFrame;
}