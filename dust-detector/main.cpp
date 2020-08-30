#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"
#include "pi-main/pi-main.h"

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;
    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Dust. Starting");

    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " Dust. Start procedure finished");
    std::cout << "Finished " << success << std::endl;

    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
