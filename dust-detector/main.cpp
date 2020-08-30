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

    dust::DustControl dctrl();


    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Dust. Start procedure finished");
    std::cout << "Finished " << success << std::endl;

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
