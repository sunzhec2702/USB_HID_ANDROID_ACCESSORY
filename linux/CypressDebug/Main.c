#include "Main.h"
#include "CyDevManager.h"
#include "UserInterface.h"

int main(int argc, char **argv) {
    initCyDevices();
    exportEnvParam();
    return processCommand(argc, argv);
}