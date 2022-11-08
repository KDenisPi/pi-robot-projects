/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "logger.h"
#include "receiver.h"

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;

    logger::log_init("/var/log/pi-robot/cmusic_log");
    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " cmusic");


    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " finished");
    std::clog << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}