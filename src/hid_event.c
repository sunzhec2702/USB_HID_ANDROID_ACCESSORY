#include "hid_event.h"
#include "hid_device_manager.h"
#include "accessory_manager.h"

#define DEFAULT_HIDRAW_TARGET "hidraw0"

	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	static void milli_sleep(int millis)
	{
	    struct timespec tm;

	    tm.tv_sec = 0;
	    tm.tv_nsec = millis * 1000000;
	    nanosleep(&tm, NULL);
	}

	void hid_raw_device_init()
	{
	    char path[100];
	    char tmp[256];
	    struct hidraw_devinfo info;
	    int desc_length;
	    int ret;
	    struct hidraw_report_descriptor desc;

	    snprintf(path, sizeof(path), "/dev/%s", DEFAULT_HIDRAW_TARGET);
	    int fd = open(path, O_RDWR);
	    if (fd < 0)
	    {
		printf("Cannot open %s\n", DEFAULT_HIDRAW_TARGET);
		return;
	    }
	    if (ioctl(fd, HIDIOCGRDESCSIZE, &desc_length))
	    {
		printf("HIDIOCGRDESCSIZE failed\n");
		close(fd);
		return;
	    }
	    printf("desc_length is %d\n", desc_length);

	    desc.size = HID_MAX_DESCRIPTOR_SIZE - 1;
	    if (ioctl(fd, HIDIOCGRDESC, &desc))
	    {
		printf("HIDIOCGRDESC failed\n");
		close(fd);
		return;
	    }
	    printf("desc done\n");

	    if ((ret = ioctl(fd, HIDIOCGRAWNAME(256), tmp)) < 0)
	    {
		printf("HIDIOCGRAWNAME failed, ret %d\n", ret);
		close(fd);
		return;
	    }
	    printf("HIDIOCGRAWNAME is %s\n", tmp);

	    if ((ret = ioctl(fd, HIDIOCGRAWPHYS(256), tmp)) < 0)
	    {
		printf("HIDIOCGRAWPHYS failed, ret %d\n", ret);
		close(fd);
		return;
	    }
	    printf("HIDIOCGRAWPHYS is %s\n", tmp);
	    register_hid_accessory(&desc, desc_length, 0);
	    accessory_t *acc_device = get_acc_device();
    while (1)
    {
        memset(tmp, 0, 256);
        ret = read(fd, tmp, 256);
        if (ret < 0)
        {
            printf("read failed, errno: %d, fd: %d\n", errno, fd);
            break;
        }
        printf("ret is %d\n", ret);
        for (int i = 0; i < 256; i++)
        {
            printf("0x%x ", tmp[i]);
        }
        printf("\n");

        ret = libusb_control_transfer(acc_device->handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                    ACCESSORY_SEND_HID_EVENT, 0, 0, tmp, ret, 1000);
        if (ret < 0 && errno != EPIPE)
        {
            printf("ACCESSORY_SEND_HID_EVENT returned %d errno: %d\n", ret, errno);
        }
    }
    return;
}

void register_hid_accessory(struct hidraw_report_descriptor* desc, int desc_length, int index)
{
    int ret;
    //HID related information.
    int hid_number = get_hid_device_number();
    printf("register_hid_accessory, the hid number is %d\n", hid_number);
    if (index >= hid_number)
    {
        printf("Index %d is invalid\n", hid_number);
    }
    hid_device *registed_device = get_hid_device(index);
    // accessory related information.
    accessory_t *acc_device = get_acc_device();
    //register hid accessory

    ret = libusb_control_transfer(acc_device->handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
            ACCESSORY_REGISTER_HID, 0, desc_length, NULL, 0, 1000);
    printf("ACCESSORY_REGISTER_HID returned %d\n", ret);
    // send hid descriptor length
    for (int offset = 0; offset < desc_length;)
    {
        int count = desc_length - offset;
        if (count > registed_device->packet_size)

            count = registed_device->packet_size;

        printf("sending ACCESSORY_SET_HID_REPORT_DESC offset: %d count: %d desc_length: %d\n",
                offset, count, desc_length);
        ret = libusb_control_transfer(acc_device->handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                          ACCESSORY_SET_HID_REPORT_DESC, 0, offset, &desc->value[offset], count, 1000);
        printf("ACCESSORY_SET_HID_REPORT_DESC returned %d errno %d\n", ret, errno);
        offset += count;
    }


    printf("sending ACCESSORY_START\n");
    ret = libusb_control_transfer(acc_device->handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR,
                                      ACCESSORY_START_ACCESSORY, 0, 0, NULL, 0, 1000);
    printf("did ACCESSORY_START\n");
    return;
}

static void *hid_event_thread(void *arg)
{
}

void init_hid()
{
    pthread_t th;
    pthread_create(&th, NULL, hid_event_thread, NULL);
}
