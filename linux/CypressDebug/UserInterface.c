#include "UserInterface.h"
#include "CyDevManager.h"

void processCommand(char *command) {

}

void printDevices() {
    int cyDevNum = getCyDeviceNum();
    if (cyDevNum == 0) {
        printf("Cannot find pm342 device.  Either need root access or no devices found.\n");
        return;
    }
    CY_DEVICE_STRUCT *cyDevices = getCydevStruct();
    printf("CyDevIndex        CyDevInterface        CyDevFunction");
    for (int i = 0; i < cyDevNum; i++) {
        for (int j = 0; j < cyDevices[i].interfaceNum; j++) {
            if (cyDevices[i].isUart == true) {
                printf("%d                 %s                     %s\n", i, "NA", "CDC");
                break;
            } else {
                if (cyDevices[i].interfaceFunctionality[j] == CY_TYPE_I2C) {
                    printf("%d                 %d                     %s\n", i, j, "I2C");
                } else if (cyDevices[i].interfaceFunctionality[j] == CY_TYPE_SPI) {
                    printf("%d                 %d                     %s\n", i, j, "SPI");
                }
            }
        }
    }
}

void printMessage() {
    return;
}