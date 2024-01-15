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
#include <memory>

#include "logger.h"
#include "colormusic.h"

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;

    logger::log_init("/var/log/pi-robot/cmusic_log");
    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " cmusic");

    int loop_skip = -1;
    for(int i=0; i<argc; i++){
        if( std::string(argv[i]) == "--loop_skip"){
            if(i+1 < argc){
                try{
                    const int val = std::stoi(std::string(argv[i+1]));
                    if(val>0 and val<6)
                        loop_skip = val;
                }
                catch(const std::invalid_argument& ia){
                    std::clog << " Invalid value for loop skip counter: " << std::string(argv[i+1]) << std::endl;
                }
                break;
            }
        }
    }

    std::shared_ptr<cmusic::ColorMusic> cmusic = std::make_shared<cmusic::ColorMusic>(argc==1 ? std::string() : std::string(argv[1]), loop_skip);

    if(cmusic->start()){
        cmusic->wait();
    }

    cmusic->stop();

    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " finished");
    std::clog << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}