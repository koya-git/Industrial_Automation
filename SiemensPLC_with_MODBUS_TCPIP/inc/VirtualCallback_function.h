#ifndef virtual_callback_fucntion
#define virtual_callback_fucntion

#include "plc_utils.h"
#define __cb        void __attribute__((weak))

/* 
    callback invoked automatically : if connection established 
    args : handler    
*/
__cb onPLC_Connected_callback(plc_handler_info_t&);

/* 
    callback invoked automatically : if Error Occure 
    args : handler, error_message, error_code
*/
__cb onPLC_Error_callback(plc_handler_info_t&, const std::string, const plc_error_e);

/* 
    callback invoked automatically : if thread exit/closed 
    args : ?
*/
__cb onTHREAD_EXIT_callback();

/* 
    callback invoked automatically : On Buffer Chnage Data Event 
    args : ?
*/
__cb onMODBUS_Data_Change_Event_Callback(MODBUS_tcpip::MB_FUNCTIONS_e, int indx, uint16_t data);


#endif