#include "hid_event.h"

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
    if (fd < 0){
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

    while(1)
    {
        memset(tmp, 0, 256);
        ret = read(fd, tmp, 256);
        if (ret < 0) 
        {
            printf("read failed, errno: %d, fd: %d\n", errno, fd);
            break;
        }
        for (int i = 0; i < 256; i++)
        {
            printf("0x%x ", tmp[i]);
        }
        printf("\n");
    }
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
