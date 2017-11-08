#include "CyDevGpioCtrl.h"
#include "CyDevManager.h"
#include "stdio.h"


CY_RETURN_STATUS cyToggleKey(CYKEYS keys, int state, int timeout) {
    CY_RETURN_STATUS rState = cySetKey(keys, state);
    if (rStatus == CY_SUCCESS && timeout > 0) {
        usleep(timeout);
        rState = cySetkey(keys, ~state);
    }
    return rState;
}

CY_RETURN_STATUS cySetKey(CYKEYS keys, int state) {
    int keyGpio = -1;
    CY_RETURN_STATUS rState;
    CY_HANDLE handle = getCyHandle(CY_TYPE_DISABLED);
    if (handle == NULL) {
        printf("Cannot get device\n");
        return CY_ERROR_REQUEST_FAILED;
    }
    if (keys & KEY_RECOVERY) {
        keyGpio = KEY_RECOVERY_GPIO;
    } else if (keys & KEY_ONKEY) {
        keyGpio = KEY_ONKEY_GPIO;        
    } else if (keys & KEY_RESET) {
        keyGpio = KEY_RESET_GPIO;
    } else {
        printf("Key is not supported\n");
        freeCyHandle(handle);
        return CY_ERROR_INVALID_PARAMETER;
    }
    rState = CySetGpioValue(handle, KEY_RECOVERY_GPIO, state);
    if (rState != CY_SUCCESS) {
        printf("CySetGpioValue failed\n");
    }
    freeCyHandle(handle);
    return rState;
}

CY_RETURN_STATUS cyResetKeys() {
    CY_RETURN_STATUS rStatus;
    rStatus = cySetKey(KEY_RECOVERY|KEY_RESET|KEY_ONKEY, KEY_OFF);
    if (rStatus != CY_SUCCESS) {
        printf("Reset Keys failed\n");
    }
    return rStatus;
}
