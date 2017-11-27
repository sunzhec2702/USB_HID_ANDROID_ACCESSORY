#include "accessory_manager.h"

// Fix device information.
static accessory_t acc_device = {
    .device = "12d1:107e",
    .manufacturer = "Google, Inc.",
    .model = "DemoDarren",
    .description = "Demo HID",
    .version = "0.1",
    .url = "https://github.com/darrens",
    .serial = "0001234567890123",
};

accessory_t* get_accessory_device()
{
    return &acc_device;
}

static int check_accessory_status()
{
    struct libusb_device_handle *handle;
    uint16_t vid = ACCESSORY_VID;
    uint16_t pid = 0;

    /* Trying to open all the AOA IDs possible */
    pid = ACCESSORY_PID;
    handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (handle != NULL)
        goto claim;

    pid = ACCESSORY_ADB_PID;
    handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (handle != NULL)
        goto claim;

    pid = AUDIO_PID;
    handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (handle != NULL)
        goto claim;

    pid = AUDIO_ADB_PID;
    handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (handle != NULL)
        goto claim;

    pid = ACCESSORY_AUDIO_PID;
    handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (handle != NULL)
        goto claim;

    pid = ACCESSORY_AUDIO_ADB_PID;
    handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (handle != NULL)
        goto claim;

    return 0;

claim:
    printf("Found accessory %4.4x:%4.4x\n", vid, pid);
    acc_device.handle = handle;
    acc_device.vid = vid;
    acc_device.pid = pid;
    return 1;
}

accessory_t* get_acc_device(void)
{
    return &acc_device;
}

int accessory_device_init()
{
    int ret = 0;
    uint8_t buffer[2];
    char *tmp;

    /* Check if device is not already in accessory mode */
    if (check_accessory_status())
    {
        printf("The device has been in Accessory mode\n");
        return 0;
    }

    /* Getting product and vendor IDs */
    uint16_t vid = (uint16_t)strtol(acc_device.device, &tmp, 16);
    uint16_t pid = (uint16_t)strtol(tmp + 1, &tmp, 16);
    printf("Looking for device %4.4x:%4.4x\n", vid, pid);

    /* Trying to open it */
    acc_device.handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (acc_device.handle == NULL)
    {
        printf("Unable to open device...\n");
        return -1;
    }

    /* Now asking if device supports Android Open Accessory protocol */
    ret = libusb_control_transfer(acc_device.handle, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR, ACCESSORY_GET_PROTOCOL, 0, 0, buffer, sizeof(buffer), 0);
    if (ret < 0)
    {
        printf("Error getting protocol...\n");
        return ret;
    }
    else
    {
        acc_device.aoa_version = ((buffer[1] << 8) | buffer[0]);
        printf("Device supports AOA %d.0!\n", acc_device.aoa_version);
    }

	/* Some Android devices require a waiting period between transfer calls */
	usleep(10000);

    /* In case of a no_app accessory, the version must be >= 2 */
    if ((acc_device.aoa_version < 2) && !acc_device.manufacturer)
    {
        printf("Connecting without an Android App only for AOA 2.0\n");
        return -1;
    }
    usleep(10000);
    printf("Sending identification to the device\n");
    if (acc_device.manufacturer)
    {
        printf("Sending manufacturer: %s\n", acc_device.manufacturer);
        ret = libusb_control_transfer(acc_device.handle,
                                      LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                      ACCESSORY_SEND_IDENT, 0,
                                      ACCESSORY_STRING_MAN_ID,
                                      (uint8_t *)acc_device.manufacturer,
                                      strlen(acc_device.manufacturer) + 1, 0);
        if (ret < 0)
            goto error;
    }

    if (acc_device.model)
    {
        printf("Sending model: %s, len is %d\n", acc_device.model, strlen(acc_device.model));
        ret = libusb_control_transfer(acc_device.handle,
                                      LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                      ACCESSORY_SEND_IDENT, 0,
                                      ACCESSORY_STRING_MOD_ID,
                                      (uint8_t *)acc_device.model,
                                      strlen(acc_device.model) + 1, 0);
        if (ret < 0)
            goto error;
    }

        printf("Sending description: %s\n", acc_device.description);
        ret = libusb_control_transfer(acc_device.handle,
                                    LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                    ACCESSORY_SEND_IDENT, 0, ACCESSORY_STRING_DSC_ID,
                                    (uint8_t *)acc_device.description,
                                    strlen(acc_device.description) + 1, 0);
        if (ret < 0)
            goto error;

    

    printf("Sending version: %s\n", acc_device.version);
    ret = libusb_control_transfer(acc_device.handle,
                                  LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                  ACCESSORY_SEND_IDENT, 0, ACCESSORY_STRING_VER_ID,
                                  (uint8_t *)acc_device.version,
                                  strlen(acc_device.version) + 1, 0);
    if (ret < 0)
        goto error;

    printf("Sending url: %s\n", acc_device.url);
    ret = libusb_control_transfer(acc_device.handle,
                                  LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                  ACCESSORY_SEND_IDENT, 0, ACCESSORY_STRING_URL_ID,
                                  (uint8_t *)acc_device.url,
                                  strlen(acc_device.url) + 1, 0);
    if (ret < 0)
        goto error;

    printf("Sending serial number: %s\n", acc_device.serial);
    ret = libusb_control_transfer(acc_device.handle,
                                  LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                  ACCESSORY_SEND_IDENT, 0, ACCESSORY_STRING_SER_ID,
                                  (uint8_t *)acc_device.serial,
                                  strlen(acc_device.serial) + 1, 0);
    if (ret < 0)
        goto error;

    printf("Turning the device in Accessory mode\n");
    ret = libusb_control_transfer(acc_device.handle,
                                  LIBUSB_ENDPOINT_OUT |
                                      LIBUSB_REQUEST_TYPE_VENDOR,
                                  ACCESSORY_START_ACCESSORY, 0, 0, NULL, 0, 0);
    if (ret < 0)
        goto error;
    
    sleep(5);
    /* Check if device is not already in accessory mode */
    if (check_accessory_status())
    {
        printf("The device is in Accessory mode right now\n");
        return 0;
    }

error:
    printf("Accessory init failed: %d\n", ret);
    return -1;
}