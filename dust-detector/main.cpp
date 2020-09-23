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

    std::cout << "Starting " << success << std::endl;
    dctrl->run();

    std::cout << "Waiting " << success << std::endl;
    sleep(1);

    delete dctrl;
    std::cout << "Finished " << success << std::endl;

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
