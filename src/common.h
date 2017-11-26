#ifndef __COMMON_H
#define __COMMON_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#include <libusb.h>

/* Android Open Accessory protocol defines */
#define ACCESSORY_GET_PROTOCOL		51
#define ACCESSORY_SEND_IDENT			52
#define ACCESSORY_START_ACCESSORY		53
#define ACCESSORY_REGISTER_HID		54
#define ACCESSORY_UNREGISTER_HID		55
#define ACCESSORY_SET_HID_REPORT_DESC		56
#define ACCESSORY_SEND_HID_EVENT		57
#define ACCESSORY_AUDIO_SUPPORT		58

/* String IDs */
#define ACCESSORY_STRING_MAN_ID		0
#define ACCESSORY_STRING_MOD_ID		1
#define ACCESSORY_STRING_DSC_ID		2
#define ACCESSORY_STRING_VER_ID		3
#define ACCESSORY_STRING_URL_ID		4
#define ACCESSORY_STRING_SER_ID		5

/* Product IDs / Vendor IDs */
#define ACCESSORY_VID		0x18D1	/* Google */
#define ACCESSORY_PID		0x2D00	/* accessory */
#define ACCESSORY_ADB_PID		0x2D01	/* accessory + adb */
#define AUDIO_PID			0x2D02	/* audio */
#define AUDIO_ADB_PID		0x2D03	/* audio + adb */
#define ACCESSORY_AUDIO_PID		0x2D04	/* accessory + audio */
#define ACCESSORY_AUDIO_ADB_PID	0x2D05	/* accessory + audio + adb */

/* Endpoint Addresses TODO get from interface descriptor */
#define ACCESSORY_EP_IN		0x81
#define ACCESSORY_EP_OUT		0x02
#define ACCESSORY_INTERFACE		0x00

/* App defines */
#define VERSION		"0.1"
#define BUG_TO_MAIL	"sunzhe_c2702@163.com"

#endif
