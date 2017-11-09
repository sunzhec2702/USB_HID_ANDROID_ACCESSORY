#include "Main.h"
#include "CyDevManager.h"
#include "UserInterface.h"
#include "signal.h"

int main(int argc, char **argv) {
    initCyDevices();
    exportEnvParam();
    signal(SIGINT, handlIntEvent);
    return processCommand(argc, argv);
}

void handlIntEvent() {
    CY_RETURN_STATUS rStatus;
    if (getCyDeviceNum == 0) {
        return;
    }
    rStatus = cyResetKeys();
    return;
}