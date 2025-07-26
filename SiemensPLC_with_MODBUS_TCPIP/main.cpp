#include "inc/PLC_MODBUS_tcp.h"

void onPLC_Connected_callback(plc_handler_info_t& _handler){
     std::cout << __func__  << std::endl;
}
void onTHREAD_EXIT_callback(){
     std::cout << __func__  << std::endl;
}
void onPLC_Error_callback(plc_handler_info_t& _handler, const std::string msg, const plc_error_e e_code){
    std::cout << __func__  << " -> " << msg << std::endl;
}
void onMODBUS_Data_Change_Event_Callback(MODBUS_tcpip::MB_FUNCTIONS_e func_code, int indx, uint16_t data){
    std::cout << "On Data Change Event [ " << static_cast<int>(func_code) << "] [" << static_cast<int>(indx) << "] = ";
     std::cout << "0x"
              << std::uppercase   // A-F in uppercase
              << std::setfill('0')
              << std::setw(4)
              << std::hex << data
              << std::endl;
}

int main(){
    tcp_connection PLC  = tcp_connection::getTCP_ConnectionInstance();
    PLC.connect_MODBUS_SERVER("192.168.1.10",502);
    MODBUS_SERVICE* service_frame =  new MODBUS_SERVICE();
    PLC.start_process([&](){
        service_frame->register_modbus_elements(MODBUS_tcpip::MB_HOLDING_REGISTER_READ,0,3);
    },service_frame);
}