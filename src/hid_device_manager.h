#ifndef __HID_DEVICE_MANAGER_H
#define __HID_DEVICE_MANAGER_H
#include "common.h"

#define MAX_HID_DEVICES	10

typedef enum
{
    HID_MOUSE = 0,
    HID_KEYBOARD,
    HID_NUM,
} HID_TYPE;

/* Structures */
typedef struct {
	libusb_device *usb_device;
	struct libusb_device_handle *handle;
	unsigned char descriptor[256];
	int endpoint_in;
	int packet_size;
	int descriptor_size;
	pthread_t rx_thread;
} hid_device;

int hid_manager_init(void);

// Get the information of the HID device.
hid_device get_hid_device(HID_TYPE type);

#endif