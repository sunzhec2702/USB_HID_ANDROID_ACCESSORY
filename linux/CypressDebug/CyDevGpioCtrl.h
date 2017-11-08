#ifndef __CY_DEV_GPIO_CTRL_H
#define __CY_DEV_GPIO_CTRL_H

#include "../../common/header/CyUSBSerial.h"
#define KEY_RECOVERY_GPIO   2
#define KEY_ONKEY_GPIO      15
#define KEY_RESET_GPIO      9

#define KEY_ON              0
#define KEY_OFF             1
typedef enum 
{
    KEY_RECOVERY = 1 << 0,
    KEY_ONKEY = 1 << 1,
    KEY_RESET = 1 << 2,
} CYKEYS;

CY_RETURN_STATUS cyToggleKey(CYKEYS keys);
CY_RETURN_STATUS cySetKey(CYKEYS keys, int state);
CY_RETURN_STATUS cyResetKeys(void);


#endif
