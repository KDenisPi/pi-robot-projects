#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "control.h"

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;
    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Dust. Starting");

    dust::DustControl* dctrl = new dust::DustControl();
    dctrl->set_conf_main("/home/pi/pi-robot-projects/dust-detector/dust-detector.json");
    dctrl->set_debug_mode(true);
    dctrl->run();

    std::cout << "Started " << success << std::endl;
    sleep(4);

    delete dctrl;

    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Dust. Start procedure finished");
    std::cout << "Finished " << success << std::endl;

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
