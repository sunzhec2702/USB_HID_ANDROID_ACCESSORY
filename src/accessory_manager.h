#ifndef __ACC_MANAGER_H_
#define __ACC_MANAGER_H_
#include "common.h"

typedef struct
{
    struct libusb_device_handle *handle;
    struct libusb_transfer *transfer;
    uint32_t aoa_version;
    uint16_t vid;
    uint16_t pid;
    char *device;
    char *manufacturer;
    char *model;
    char *description;
    char *version;
    char *url;
    char *serial;
} accessory_t;

int accessory_device_init(void);
accessory_t* get_acc_device(void);

#endif
