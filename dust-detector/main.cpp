#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "logger.h"


using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
