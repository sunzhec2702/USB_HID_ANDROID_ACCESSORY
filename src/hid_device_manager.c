#include "hid_device_manager.h"

//record all the hid devices.
static int hid_device_number = 0;
static hid_device hid_devices[MAX_HID_DEVICES];

int get_hid_device_number()
{
    return hid_device_number;
}

hid_device *get_hid_device(int index)
{
    return &hid_devices[index];
}

static void update_hid_devices(
    libusb_device *usb_device,
    struct libusb_device_descriptor *usb_descriptor,
    int interface_index,
    unsigned char endpoint,
    unsigned int packet_size)
{
    hid_device *current_hid_device = &hid_devices[hid_device_number++];
    
    int usb_desc_size = 0, ret = 0;
    unsigned char usb_desc[512];
    libusb_device_handle *usb_handle = NULL;

    libusb_ref_device(usb_device);
    libusb_open(usb_device, &usb_handle);
    if (usb_handle == NULL)
    {
        printf("libusb open failed\n");
        goto error_open;
    }

    ret = libusb_kernel_driver_active(usb_handle, interface_index);
    if (ret == 1)
    {
        printf("Kernel Driver Active, Deattaching\n");
        if (libusb_detach_kernel_driver(usb_handle, interface_index)) {
			printf("Unable to grab usb device\n");
			usb_handle = NULL;
            goto error_open;
		}
    }
    ret = libusb_claim_interface(usb_handle, interface_index);
	if (ret) {
		printf("Failed to claim interface %d.\n", interface_index);
		libusb_attach_kernel_driver(usb_handle, interface_index);
        usb_handle = NULL;
        goto error_open;
	}

    usb_desc_size = libusb_control_transfer(usb_handle, LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_INTERFACE,
                                                   LIBUSB_REQUEST_GET_DESCRIPTOR,
                                                   LIBUSB_DT_REPORT << 8, 0,
                                                   usb_desc, 512, 0);
    if (usb_desc_size < 0)
    {
        printf("usb_desc_size %d < 0\n", usb_desc_size);
        goto error_desc;
    }
    /*
    memcpy(&current_hid_device->usb_device, current_device, sizeof(libusb_device));
    */
    memcpy(current_hid_device->descriptor, usb_desc, 256);
    current_hid_device->descriptor_size = usb_desc_size;
    current_hid_device->packet_size = usb_descriptor->bMaxPacketSize0;
    if (endpoint != 0 && packet_size != 0)
    {
        current_hid_device->endpoint_in = endpoint;
    }
    else
    {
        current_hid_device->endpoint_in = -1;
        
    }
    printf("hid_device_number %d, vid 0x%x, pid 0x%x\n", hid_device_number - 1, usb_descriptor->idVendor, usb_descriptor->idProduct);
    printf("endpoint is %d, packet_size is %d, descriptor size is %d\n",
            current_hid_device->endpoint_in, current_hid_device->packet_size, current_hid_device->descriptor_size);
    for (int i = 0; i < current_hid_device->descriptor_size; i++)
    {
        printf("0x%x ", current_hid_device->descriptor[i]);
    }
    printf("\n");
    printf("release interface\n");
    libusb_release_interface(usb_handle, interface_index);
    printf("Reattaching the kernel driver\n");
    libusb_attach_kernel_driver(usb_handle, interface_index);
error_desc:
    libusb_close(usb_handle);
error_open:
    libusb_unref_device(usb_device);
    return;
}

int hid_manager_init(void)
{
    libusb_device **list;
    int usb_device_number = 0, ret = 0;
    /* List every USB device attached */

    usb_device_number = libusb_get_device_list(NULL, &list);
    if (usb_device_number < 0)
    {
        printf("Usb Device List is NULL or USB Device Number is 0\n");
        libusb_free_device_list(list, 1);
        libusb_exit(NULL);
        return -1;
    }
    for (int i = 0; i < usb_device_number; i++)
    {

        libusb_device *current_device = list[i];
        struct libusb_device_descriptor current_desc;
        ret = libusb_get_device_descriptor(current_device, &current_desc);
        if (ret < 0)
        {
            printf("Device Descriptor is NULL, continue\n");
            continue;
        }
        if (current_desc.bDeviceClass == LIBUSB_CLASS_HID)
        {
            printf("Found LIBUSB_CLASS_HID\n");
            //update_hid_devices(current_device, &current_desc, -1, 0, 0);
        }
        else if (current_desc.bDeviceClass == LIBUSB_CLASS_PER_INTERFACE)
        {
            printf("LIBUSB_CLASS_PER_INTERFACE\n");
            struct libusb_config_descriptor *current_config;
            ret = libusb_get_active_config_descriptor(current_device, &current_config);
            if (ret < 0)
            {
                printf("Get Active Config Descriptor failed\n");
                continue;
            }
            for (int j = 0; j < current_config->bNumInterfaces; j++)
            {
                for (int k = 0; k < current_config->interface[j].num_altsetting; k++)
                {
                    if (current_config->interface[j].altsetting[k].bInterfaceClass == LIBUSB_CLASS_HID)
                    {
                        printf("Found InterfaceClass is LIBUSB_CLASS_HID\n");
                        update_hid_devices(current_device, &current_desc, j, current_config->interface[j].altsetting[k].endpoint[0].bEndpointAddress, current_config->interface[j].altsetting[k].endpoint[0].wMaxPacketSize);
                    }
                }
            }
            libusb_free_config_descriptor(current_config);
        }
    }
    printf("Found %d hid device\n", hid_device_number);
    libusb_free_device_list(list, 1);
    return 0;
}