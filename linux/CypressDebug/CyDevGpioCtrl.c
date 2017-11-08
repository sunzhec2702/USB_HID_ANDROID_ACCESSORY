#include "CyDevGpioCtrl.h"



CY_RETURN_STATUS cyToggleKey(CYKEYS keys) {
    if(keys & KEY_RECOVERY) {

    }
    return CY_SUCCESS;
}

CY_RETURN_STATUS cySetKey(CYKEYS keys, int state) {
    if (keys & KEY_RECOVERY) {
        
    }
        return CY_SUCCESS;
}

CY_RETURN_STATUS cyResetKeys() {
    
        return CY_SUCCESS;
}
