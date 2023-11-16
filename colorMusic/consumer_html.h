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
#include "colors.h"

namespace cmusic {

class CmrHtml : public Consumer {
public:
    CmrHtml(const int ln_pfile = 200, const std::string rdir = "./") :
        _max_lines_per_file(ln_pfile), _root_dir(rdir) {

        logger::log(logger::LLOG::INFO, "CmrHtml", std::string(__func__));
        open(0);

    }

    virtual ~CmrHtml() {
        logger::log(logger::LLOG::INFO, "~CmrHtml", std::string(__func__));
        close();
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

        auto tp_start = processing_start();

        int not_empty_counter = 0;

        _fd << "<tr>" << std::endl;
        _fd << "<td>" << std::to_string(_line_count) << "</td>" << std::endl;
        for(int i=0; i<_chunk_number; i++){
            if(i<d_size){
                int color = ((i/6)<32 ? i/6 : 31);
                _fd << "<td style=\"background-color:#" << std::hex << (data[i]==0 ? ldata::color_black : ldata::colors32[color]) << ";\">" << std::to_string(color) <<"</td>" ;
                if(data[i] > 0)
                    not_empty_counter++;
            }
            else
                _fd << "<td>" << "na" << "</td>";
        }
        _fd << "</tr>"<< std::endl;

        logger::log(logger::LLOG::DEBUG, "html", std::string(__func__) + " Processed for (ms): " + processing_end(tp_start) + " Values: " + std::to_string(not_empty_counter));
        _line_count++;

        if(_line_count>=_max_lines_per_file){
            reopen();
        }
    }

private:

    /**
     *
    */
    void open(const int part = 0){
        const std::string fname = _root_dir + "colormusic_" + std::to_string(part) + ".html";
        logger::log(logger::LLOG::INFO, "CmrHtml", std::string(__func__) + " filename: " + fname);

        if(is_ready()){
            close();
        }

        _fd.open(fname, std::ios::out | std::ios::trunc);
        if(is_ready()){
            _fd << _header;
            _fd << _table_b;
            _fd << "<tr>\n";
            _fd << "<th>" << "chunk" << "</th>";
            for(int i=0; i<_chunk_number; i++){
                _fd << "<th>" << std::to_string(i) << "</th>";
            }
            _fd << "</tr>\n";
        }
    }

    /**
     *
    */
    void close(){
        logger::log(logger::LLOG::INFO, "~CmrHtml", std::string(__func__));

        if(is_ready()){
            _fd << _table_e;
            _fd << _footer;
            _fd.close();
        }
    }

    void reopen(){
        logger::log(logger::LLOG::INFO, "~CmrHtml", std::string(__func__));

        close();
        open(++_part);
        _line_count = 0;
    }

    std::ofstream _fd;
    int _max_lines_per_file = 200;  //maximum number lines per file
    std::string _root_dir = "./";     //folder for file generating

    const int _chunk_number = 200;

    int _line_count = 0;
    int _part = 0;

    const char* _header = "<!DOCTYPE html>\n<html>\n<head><title>Sound HTML presentation</title></head>\n<body>";
    const char* _table_b = "<table>\n";
    const char* _table_e = "</table>\n";
    const char* _footer = "</body>\n</html>";
};

} //namespace

#endif