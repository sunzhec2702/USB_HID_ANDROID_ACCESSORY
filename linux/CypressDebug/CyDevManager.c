#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include "CyDevManager.h"
#include "../../common/header/CyUSBSerial.h"

static bool cyLibInitFlag = false;
static CY_DEVICE_STRUCT cyDevices[CY_MAX_DEVICES];
static int cyDeviceNum = 0;

static int selectedCyDev = 0;

CY_RETURN_STATUS initCyDevices() {
    CY_RETURN_STATUS rStatus;
    rStatus = CyLibraryInit();
    if (rStatus != CY_SUCCESS) {
        printf("CY:Error in Doing library init Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    cyLibInitFlag = true;
    searchCyDevice();
}

int getCyDeviceNum() {
    return cyDeviceNum;
}

CY_DEVICE_STRUCT* getCydevStruct() {
    return cyDevices;
}

CY_RETURN_STATUS setSelectedCyDev(unsigned int index) {
    if (index >= cyDeviceNum) {
        return CY_ERROR_INVALID_PARAMETER;
    }
    selectedCyDev = index;
    return CY_SUCCESS;
}

CY_HANDLE getCyHandle(CY_DEVICE_TYPE type) {
    CY_RETURN_STATUS rStatus;
    CY_HANDLE handle;
    int interfaceIndex = -1;
    switch (type) {
        case CY_TYPE_I2C:
        break;
        case CY_TYPE_SPI:
        break;
        default:
        interfaceIndex = cyDevices[selectedCyDev].numInterface - 1;
        break;
    }
    rStatus = CyOpen(deviceNum, interfaceIndex, &handle);
    if (rStatus == CY_SUCCESS) {
        return handle;
    } else {
        return NULL;
    }
}

void freeCyHandle(CY_HANDLE handle) {
    CyClose(handle);
}

bool isCypressDevice(int deviceNum, int interfaceIndex) {
    CY_HANDLE handle;
    unsigned char sig[6];
    CY_RETURN_STATUS rStatus;
    rStatus = CyOpen(deviceNum, interfaceIndex, &handle);
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

void resetCyDevStruct() {
    // Clear
    for (int i = 0; i < numDevices; i++) {
        cyDevices[i].deviceNumber = -1;
        cyDevices[i].isI2c = false;
        cyDevices[i].isSpi = false;
        cyDevices[i].isUart = false;
        for (j = 0; j < CY_MAX_INTERFACES; j++) {
            cyDevices[i].interfaceFunctionality[j] = -1;
        }
    }
    cyDeviceNum = 0;
}

CY_RETURN_STATUS searchCyDevice() {
    CY_RETURN_STATUS rStatus;
    if (cyLibInitFlag == false) {
        return CY_ERROR_DRIVER_INIT_FAILED;
    }
    int numDevices = 0;
    CyGetListofDevices(&numDevices);
    resetCyDevStruct();
    for (int devNum = 0; devNum < numDevices; devNum++) {
        CY_DEVICE_INFO deviceInfo;
        rStatus = CyGetDeviceInfo(devNum, &deviceInfo);
        if (!rStatus) {
            if (!isCypressDevice(devNum, deviceInfo.numInterfaces - 1)) {
                continue;
            }
            cyDevices[cyDeviceNum].numInterface = deviceInfo.numInterfaces;
            cyDevices[cyDeviceNum].deviceNumber = devNum;
            // enumerate Interface
            for (int interfaceIndex = 0; interfaceIndex < cyDevices[cyDeviceNum].numInterface; interfaceIndex++) {
                if (deviceInfo.deviceClass[interfaceIndex] == CY_CLASS_VENDOR) {
                    switch (deviceInfo.deviceType[interfaceIndex]) {
                        case CY_TYPE_I2C:
                            cyDevices[devNum].interfaceFunctionality[interfaceIndex] = CY_TYPE_I2C;
                            cyDevices[devNum].isI2c = true;
                            break;
                        case CY_TYPE_SPI:
                            cyDevices[devNum].interfaceFunctionality[interfaceIndex] = CY_TYPE_SPI;
                            cyDevices[devNum].isSpi = true;
                            break;
                        default:
                            break;    
                    }
                } else if (deviceInfo.deviceClass[interfaceIndex] == CY_CLASS_CDC) {
                    cyDevices[devNum].isUart = true;
                }
            }
            cyDeviceNum++;
        }
    }
}