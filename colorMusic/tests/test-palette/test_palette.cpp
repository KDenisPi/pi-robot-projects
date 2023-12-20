/**
 * @file test_palette.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-12-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>


#include "colors.h"

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;

    const char* _header = "<!DOCTYPE html>\n<html>\n<head><title>Sound HTML presentation</title></head>\n<body>";
    const char* _table_b = "<table>\n";
    const char* _table_e = "</table>\n";
    const char* _footer = "</body>\n</html>";

    std::cout << "Freq intervals " << cmusic::ldata::col_intervals_count << std::endl;

    std::ofstream  fd;
    fd.open("test_palette.html", std::ios::out | std::ios::trunc);
    if(fd.is_open()){
        fd << _header;
        fd << _table_b;

        fd << "<tr>\n";
        for(int i = 0; i < cmusic::ldata::pal_size_block; i++){
            fd << "<td style=\"background-color:#" << std::hex << cmusic::ldata::colors_blocks[i] << ";\">" << "0x" << std::hex << cmusic::ldata::colors_blocks[i] <<"</td>" << std::endl;
            if(i>0 && ((i+1)%cmusic::ldata::pal_colors_per_block)==0){
                fd << "</tr>\n" << i+1 <<"<tr>\n";
            }
        }

        fd << "</tr>\n";

        fd.close();
    }

    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}