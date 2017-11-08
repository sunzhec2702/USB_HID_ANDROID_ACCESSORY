#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <ctype.h>

#include "../../common/header/CyUSBSerial.h"

CY_DEVICE_STRUCT* glDevice;
int i2cDeviceIndex[CY_MAX_DEVICES][CY_MAX_INTERFACES];
int CyDevNumIndex[CY_MAX_DEVICES];
unsigned char* deviceNumber = NULL;

int cyDevices, i2cDevices = 0, numDevices = 0;
int selectedDeviceNum = -1, selectedInterfaceNum = -1;
bool exitApp = false;
unsigned short pageAddress = -1;
short readWriteLength = -1;
bool deviceAddedRemoved = false;

void print_message(int message_id) {
    switch (message_id) {
        case 1:
            printf(
                "Cannot open Cypress device.  Either need root access or no "
                "devices found.\n");
            break;
    }
}

bool isCypressDevice(int deviceNum, int interfaceIndex) {
    CY_HANDLE handle;
    unsigned char interfaceNum = interfaceIndex;
    unsigned char sig[6];
    CY_RETURN_STATUS rStatus;
    rStatus = CyOpen(deviceNum, interfaceNum, &handle);
    if (rStatus == CY_SUCCESS) {
        rStatus = CyGetSignature(handle, sig);
        if (rStatus == CY_SUCCESS) {
            CyClose(handle);
            return true;
        } else {
            CyClose(handle);
            return false;
        }
    } else
        return false;
}

static void list_devices(bool isprint) {
    int index_i = 0, index_j = 0, i, j, countOfDevice = 0, devNum;
    int length, index = 0, numInterfaces, interfaceNum;
    bool set1 = false;

    unsigned char deviceID[CY_MAX_DEVICES];
    CY_DEVICE_INFO deviceInfo;
    CY_DEVICE_CLASS deviceClass[CY_MAX_INTERFACES];
    CY_DEVICE_TYPE deviceType[CY_MAX_INTERFACES];
    CY_RETURN_STATUS rStatus;

    // Retrieve all the USB devices of the Host PC.
    CyGetListofDevices(&numDevices);
    for (i = 0; i < numDevices; i++) {
        for (j = 0; j < CY_MAX_INTERFACES; j++)
            glDevice[i].interfaceFunctionality[j] = -1;
    }

    for (devNum = 0; devNum < numDevices; devNum++) {
        rStatus = CyGetDeviceInfo(devNum, &deviceInfo);
        interfaceNum = 0;
        if (!rStatus) {
            // Only CypressDevice go further. Check VID/PID
            if (!isCypressDevice(devNum, deviceInfo.numInterfaces - 1)) {
                continue;
            }
            glDevice[index].numInterface = deviceInfo.numInterfaces;
            CyDevNumIndex[cyDevices++] = devNum;
        }
    }

    if (isprint) {
        if (cyDevices == 0) {
            print_message(1);
        }
        for (i = 0; i < cyDevices; i++) {
            CyGetDeviceInfo(CyDevNumIndex[i], &deviceInfo);
            printf("Cypress_Index   Cypress_Serial\n");
            printf("%d                %s\n", i, deviceInfo.serialNum);
        }
    }
    return;
}

static bool check_selection(int index) {
    if (index >= cyDevices) {
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    char *p, *cyIndex, *cySerial;
    CY_RETURN_STATUS rStatus;
    glDevice = (CY_DEVICE_STRUCT*)malloc(CY_MAX_DEVICES * sizeof(CY_DEVICE_STRUCT));
    // Initial the evironment
    if (glDevice == NULL) {
        printf("Memory allocation failed ...!! \n");
        return -1;
    }
    rStatus = CyLibraryInit();
    if (rStatus != CY_SUCCESS) {
        printf("CY:Error in Doing library init Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    // Check the selection.
    if ((p = getenv("CYPRESS_INDEX")))
        cyIndex = p;
    if ((p = getenv("CYPRESS_SERIAL")))
        cySerial = p;
    printf("index = \'%s\'\n", cyIndex);
    printf("serial = \'%s\'\n", cySerial);
    // Retieve all the usb devices and cypress devices to data struct.
    list_devices(false);
    if (cyIndex != NULL) {
        int cySelectIndex = atoi(cyIndex);
        if (check_selection(cySelectIndex) == false) {
            print_message(1);
            return -1;
        } else {
            selectedDeviceNum = CyDevNumIndex[cySelectIndex];
        }
    }
    int value1;
    CY_HANDLE handle;
    CyOpen(selectedDeviceNum, 3, &handle);
    // printf("DEBUG1, result %d\n", result);
    CyGetGpioValue(handle, 14, &value1);
    printf("Current value is %d\n", value1);
    CySetGpioValue(handle, 14, 1);
    CyGetGpioValue(handle, 14, &value1);
    printf("Current value is now %d\n", value1);
    CyClose(handle);
    return 0;
}
