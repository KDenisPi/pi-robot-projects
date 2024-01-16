/**
 * @file main.cpp
 * @author Denis Kudia (dkudja@gmail.com)
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

    //
    //Command line parameters
    //
    int loop_skip = 3;
    std::string filename;
    bool dbg_out = false;
    for(int i=1; i<argc; i++){
        const std::string prm = std::string(argv[i]);

        //define how many measurements shoud be ignored for ooutput
        if( prm == "--loop_skip"){
            if(i+1 < argc){
                try{
                    const int val = std::stoi(std::string(argv[i+1]));
                    if(val>=0 and val<6)
                        loop_skip = val;
                }
                catch(const std::invalid_argument& ia){
                    std::clog << " Invalid value for loop skip counter: " << std::string(argv[i+1]) << std::endl;
                }
            }
        }

        //input filename
        if(prm == "--file"){
            if(i+1 < argc){
              filename  = std::string(argv[i+1]);
            }
        }

        //include HTML output
        if(prm == "--html"){
            dbg_out = true;
        }

        if(prm == "--help" || prm == "-h"){
            std::cout << "cmusicd [--file input_file.raw] [--loop_skip n] [--html] [--help]" << std::endl;
            exit(EXIT_SUCCESS);
        }
    }

    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " loop skip: " + std::to_string(loop_skip));
    std::clog << "Loops skip " << loop_skip << " Filename: " << filename << " Use HTML: " << dbg_out << std::endl;

    std::shared_ptr<cmusic::ColorMusic> cmusic = std::make_shared<cmusic::ColorMusic>(filename, loop_skip, dbg_out);

    if(cmusic->start()){
        cmusic->wait();
    }

    cmusic->stop();

    logger::log(logger::LLOG::INFO, "main", std::string(__func__) + " finished");
    std::clog << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}
