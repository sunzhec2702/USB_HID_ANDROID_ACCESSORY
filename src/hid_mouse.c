#include "hid_mouse.h"
#include "accessory_manager.h"

static hid_device hid_dev;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

unsigned char search_hid(hid_device * hid)
{
	libusb_device **list;
	struct libusb_device_descriptor desc;
	ssize_t cnt;
	ssize_t i = 0;
	int j;
	libusb_device *device;

	hid->handle = NULL;

	/* List every USB device attached */
	cnt = libusb_get_device_list(NULL, &list);
	if (cnt < 0)
		goto error0;

	for (i = 0; i < cnt; i++) {
		device = list[i];
		int r;
		r = libusb_get_device_descriptor(device, &desc);
		if (r < 0)
			continue;
		if (desc.bDeviceClass == LIBUSB_CLASS_HID) {
			goto found;
			break;
		} else if (desc.bDeviceClass == LIBUSB_CLASS_PER_INTERFACE) {
			struct libusb_config_descriptor *current_config;
			int r;
			r = libusb_get_active_config_descriptor(device,
								&current_config);
			if (r < 0)
				continue;
			for (j = 0; j < current_config->bNumInterfaces; j++) {
				int k;
				for (k = 0; k < current_config->interface[j].num_altsetting; k++) {
					if (current_config->interface[j].altsetting[k].bInterfaceClass == LIBUSB_CLASS_HID) {
						hid->endpoint_in =
						    current_config->interface
						    [j].
						    altsetting[k].endpoint
						    [0].bEndpointAddress;
						hid->packet_size =
						    current_config->interface
						    [j].
						    altsetting[k].endpoint[0].
						    wMaxPacketSize;
						goto found;
					}
				}
			}
			libusb_free_config_descriptor(current_config);
		}
	}
found:
	if (i == cnt)
		goto error0;
	printf("Found hid device\n");
	hid->descriptor_size = libusb_control_transfer(hid->handle,
						       LIBUSB_ENDPOINT_IN |
						       LIBUSB_RECIPIENT_INTERFACE,
						       LIBUSB_REQUEST_GET_DESCRIPTOR,
						       LIBUSB_DT_REPORT << 8, 0,
						       hid->descriptor, 256, 0);
	if (hid->descriptor_size < 0)
		goto error1;
	libusb_free_device_list(list, 1);
	printf("=> found HID device vid 0x%x pid 0x%x\n", desc.idVendor,
	       desc.idProduct);
	return 0;
error1:
	libusb_close(hid->handle);
error0:
	libusb_free_device_list(list, 1);
	return -1;
}

static void *hid_thread(void *arg)
{
    int fd = (int)arg;
    char buffer[4096];
    int id, ret, offset;
    int desc_length = 0;
    struct hidraw_report_descriptor desc;

    printf("hid_thread start fd: %d\n", fd);

    if (ioctl(fd, HIDIOCGRDESCSIZE, &desc_length))
    {
        printf("HIDIOCGRDESCSIZE failed\n");
        close(fd);
        goto err;
    }

    desc.size = HID_MAX_DESCRIPTOR_SIZE - 1;
    if (ioctl(fd, HIDIOCGRDESC, &desc))
    {
        printf("HIDIOCGRDESC failed\n");
        close(fd);
        goto err;
    }

wait_for_device:
    printf("waiting for device fd: %d\n", fd);
    accessory_t *acc = get_acc_device();
    struct libusb_device_handle *acc_handle = acc->handle;
 
    // FIXME
    sleep(1);

    pthread_mutex_lock(&mutex);
    ret = libusb_control_transfer(acc_handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                  AOA_REGISTER_HID, 0, desc_length, NULL, 0, 1000);
    printf("ACCESSORY_REGISTER_HID returned %d\n", ret);

    // FIXME
    sleep(1);

    ret = libusb_control_transfer(acc_handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                    AOA_SET_HID_REPORT_DESC, id, 0, &desc.value, desc_length+1, 1000);
    printf("DEBUG: desc_length is %d, max_packet is %d\n", desc_length, hid_dev.packet_size);
    for (offset = 0; offset < desc_length;)
    {
        int count = desc_length - offset;
        if (count > hid_dev.packet_size)
            count = hid_dev.packet_size;

        printf("sending ACCESSORY_SET_HID_REPORT_DESC offset: %d count: %d desc_length: %d\n",
               offset, count, desc_length);
        ret = libusb_control_transfer(acc_handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                      AOA_SET_HID_REPORT_DESC, id, offset, &desc.value[offset], count, 1000);
        printf(stderr, "ACCESSORY_SET_HID_REPORT_DESC returned %d errno %d\n", ret, errno);
        offset += count;
    }

    pthread_mutex_unlock(&mutex);

    while (1)
    {
        
        ret = read(fd, buffer, sizeof(buffer));
        if (ret < 0)
        {
            printf("read failed, errno: %d, fd: %d\n", errno, fd);
            break;
        }
        printf("Darren:DEBUG: buffer %s\n", buffer);
        ret = libusb_control_transfer(acc_handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                          AOA_SEND_HID_EVENT, id, 0, buffer, ret, 1000);
        if (ret < 0 && errno != EPIPE)
        {
            printf("ACCESSORY_SEND_HID_EVENT returned %d errno: %d\n", ret, errno);
            goto wait_for_device;
        }
    }

    printf("ACCESSORY_UNREGISTER_HID\n");
    ret = libusb_control_transfer(acc_handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                      AOA_UNREGISTER_HID, id, 0, NULL, 0, 1000);

    printf("hid thread exiting\n");
err:
    return NULL;
}

static void open_hid(const char *name)
{
    char path[100];

    snprintf(path, sizeof(path), "/dev/%s", name);
    int fd = open(path, O_RDWR);
    if (fd < 0)
        return;

    printf("opened /dev/%s\n", name);
    pthread_t th;
    pthread_create(&th, NULL, hid_thread, (void *)fd);
}


void init_hid_mouse()
{
    int ret;
    ret = search_hid(&hid_dev);
    if (ret != 0)
    {
        printf("Search HID failed\n");
        return;
    }
    // hid_dev is updated now.
    open_hid("hidraw0");

}