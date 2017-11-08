#ifndef __CY_DEV_MANAGER_H
#define __CY_DEV_MANAGER_H

#include <unistd.h>

#define CY_MAX_DEVICES 30
#define CY_MAX_INTERFACES 4

typedef struct _CY_DEVICE_STRUCT
{
    int deviceNumber;
    int interfaceFunctionality[CY_MAX_INTERFACES];
    bool isI2c;
    bool isSpi;
    bool isUart;
    int numInterface;
} CY_DEVICE_STRUCT;


CY_RETURN_STATUS initCyDevices();
CY_DEVICE_STRUCT* getCydevStruct();
int getCyDeviceNum();    
bool isCypressDevice(int deviceNum, int interfaceIndex);
void resetCyDevStruct();
CY_RETURN_STATUS searchCyDevice(bool isprint);

#endif