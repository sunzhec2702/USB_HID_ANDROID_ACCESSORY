#include "UserInterface.h"
#include "CyDevManager.h"
#include "string.h"
#include "CyDevGpioCtrl.h"



int processCommand(int argc, char **argv) {
    // CDC mode.
    if (argc != 2) {
        printHelp();
        return -1;
    }

    if(strcmp(argv[1], "reset") == 0) {
        cyToggleKey(KEY_RESET, KEY_ON, 100000);
    } else if (strcmp(argv[1], "reset_down") == 0) {
        cySetKey(KEY_RESET, KEY_ON);
    } else if (strcmp(argv[1], "reset_up") == 0) {
        cySetKey(KEY_RESET, KEY_OFF);
    } else if (strcmp(argv[1], "onkey") == 0) {
        cyToggleKey(KEY_ONKEY, KEY_ON, 100000);
    } else if (strcmp(argv[1], "onkey_down") == 0) {
        cySetKey(KEY_ONKEY, KEY_ON);
    } else if (strcmp(argv[1], "onkey_up") == 0) {
        cySetKey(KEY_ONKEY, KEY_OFF);
    } else if (strcmp(argv[1], "onkey_hold") == 0) {
        cyToggleKey(KEY_ONKEY, KEY_ON, 12000000);
    } else if (strcmp(argv[1], "reset_recovery") == 0) {
        cySetKey(KEY_RECOVERY, KEY_ON);
        usleep(100000);
        cyToggleKey(KEY_RESET, KEY_ON, 100000);
        usleep(100000);
        cySetKey(KEY_RECOVERY, KEY_OFF);
    } else if (strcmp(argv[1], "recovery_down") == 0) {
        cySetKey(KEY_RECOVERY, KEY_ON);
    } else if (strcmp(argv[1], "recovery_up") == 0) {
        cySetKey(KEY_RECOVERY, KEY_OFF);
    } else if (strcmp(argv[1], "list") == 0) {
        printDevices();
    }
}

void exportEnvParam() {
    char param;
    int cyIndex;
    if ((param = getenv("CYDEV_INDEX"))) {
        cyIndex = atoi(param);
    }
    if (setSelectedCyDev(cyIndex) != CY_SUCCESS) {
        printf("CYDEV_INDEX %d is invalid\n");
    }
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

void printHelp() {
    // TODO: If there is I2C interface, change to I2C instruction.
    printf("Unknown argument! Acceptable args: 
    [reset|reset_down|reset_up|onkey|onkey_down|onkey_up|onkey_hold|
    reset_recovery|recovery_down|recovery_up|list]");
    return;
}