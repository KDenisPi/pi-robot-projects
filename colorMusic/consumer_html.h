/**
 * @file consumer_html.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CMUSIC_CONSUMER_HTML
#define CMUSIC_CONSUMER_HTML

#include <string>
#include <iostream>
#include <fstream>

#include "logger.h"
#include "consumer.h"

namespace cmusic {

class CmrHtml : public Consumer {
public:
    CmrHtml(const int ln_pfile = 200, const std::string rdir = "./") : 
        _max_lines_per_file(ln_pfile), _root_dir(rdir) {

        logger::log(logger::LLOG::INFO, "CmrHtml", std::string(__func__));

        const std::string fname = _root_dir + "colormusic.html";
        _fd.open(fname, std::ios::out | std::ios::trunc);
        if(is_ready()){
            _fd << _header; 
        }

    }

    virtual ~CmrHtml() {
        logger::log(logger::LLOG::INFO, "CmrHtml", std::string(__func__));

        if(is_ready()){
            _fd << _footer;
            _fd.close();
        }
    }

    const bool is_ready() const {
        return _fd.is_open();
    }

    /**
     * 
    */
    virtual void process(const uint32_t* data, const int d_size){

        if(!is_ready())
            return;

        //Later will do something
        if(_line_count > _max_lines_per_file){
            return;
        }



    }

private:
    std::ofstream _fd;
    int _max_lines_per_file = 200;  //maximum number lines per file
    std::string _root_dir = "./";     //folder for file generating

    int _line_count = 0;

    const char * _header = "<!DOCTYPE html>\n<html>\n<head><title>Sound HTML presentation</title></head>\n<body>"; 
    const char * _footer = "</body>\n</html>"; 
};

} //namespace

#endif