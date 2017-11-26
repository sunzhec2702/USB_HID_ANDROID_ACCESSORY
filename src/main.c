#include "main.h"
#include "hid_device_manager.h"
#include "accessory_manager.h"
#include "hid_event.h"
#include <signal.h>

static void show_version(char *name)
{
    printf("%s v%s\nreport bugs to %s\n", name, VERSION,
           BUG_TO_MAIL);
    return;
}

static void signal_handler(int signo)
{
    printf("SIGINT: Closing accessory\n");
}

int main(int argc, char *argv[])
{
    show_version("Darren");
    /*
    if (signal(SIGINT, signal_handler) == SIG_ERR)
        printf("Cannot setup a signal handler...\n");
    */
    int ret = 0;
    ret = hid_manager_init();
    if (ret != 0)
    {
        printf("hid manager init failed\n");
        exit(1);
    }

    hid_raw_device_init();
    while(1)
    {

    }

    /*
    ret = accessory_device_init();
    if (ret != 0)
    {
        printf("accessory device init failed\n");
        exit(1);
    }
    */
    /*
    printf("Starting to sending event to accessory device\n");
    hid_manager_start_send_hid_event(HID_MOUSE);
    while(stop_flag == 1)
    {
        exit(0);
    }
    */
    return 0;
}