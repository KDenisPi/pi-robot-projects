#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "control.h"

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;

    dust::DustControl* dctrl = new dust::DustControl();

    dctrl->load_configuration(argc, argv);
    dctrl->set_conf_main("/home/deniskudia/sources/pi-robot-projects/dust-detector/dust-detector.json");
    dctrl->set_debug_mode(false);
    dctrl->set_use_http(false);
    dctrl->set_daemon_mode(true);

    std::cout << "Starting " << success << std::endl;
    dctrl->run();

    std::cout << "Waiting " << success << std::endl;
    sleep(1);

    delete dctrl;
    std::cout << "Finished " << success << std::endl;

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
