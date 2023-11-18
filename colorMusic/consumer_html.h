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
    /**
     * @brief Construct a new Cmr Html object
     *
     * @param ln_pfile
     * @param rdir
     */
    CmrHtml(const int ln_pfile = 200, const std::string rdir = "./") : Consumer(200, false), _max_lines_per_file(ln_pfile), _root_dir(rdir){

        logger::log(logger::LLOG::INFO, "CmrHtml", std::string(__func__));
        open(0);

    }

    virtual ~CmrHtml() {
        logger::log(logger::LLOG::INFO, "~CmrHtml", std::string(__func__));
        close();
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool start(){
        logger::log(logger::LLOG::INFO, "consm", std::string(__func__));
        return piutils::Threaded::start<CmrHtml>(this);
    }


    /**
     * @brief
     *
     * @return true
     * @return false
     */
    virtual const bool is_ready(){
        return _fd.is_open();
    }

    /**
     *
    */
    static void worker(CmrHtml* p){
        logger::log(logger::LLOG::INFO, "html", std::string(__func__) + " Started");

        auto fn = [p]{return (p->is_stop_signal() || p->is_busy());};
        for(;;){
            {
                std::unique_lock<std::mutex> lk(p->cv_m);
                p->cv.wait(lk, fn);
            }

            if(p->is_stop_signal()){
                logger::log(logger::LLOG::INFO, "html", std::string(__func__) + " Stop signal detected");
                break;
            }

            auto tp_start = p->processing_start();

            int not_empty_counter = p->process_data();

            logger::log(logger::LLOG::DEBUG, "html", std::string(__func__) + " Processed for (ms): " + p->processing_end(tp_start) + " Values: " + std::to_string(not_empty_counter));

            p->set_busy(false);
        }

        logger::log(logger::LLOG::INFO, "html", std::string(__func__) + " Finished");
    }

    /**
     * @brief
     *
     * @return const int
     */
    const int process_data(){
        int not_empty_counter = 0;
        //start line
        _fd << "<tr>" << std::endl;
        _fd << "<td>" << std::to_string(_line_count) << "</td>" << std::endl;

        int color;
        for(int i=0; i<items_count(); i++){
            _fd << "<td style=\"background-color:#" << std::hex << _data[i] << ";\">" << std::to_string(color) <<"</td>" ;
            if(_data[i] != ldata::color_black)
                not_empty_counter++;
        }
        _fd << "</tr>"<< std::endl;

        _line_count++;

        if(_line_count>=_max_lines_per_file){
            reopen();
        }

        return not_empty_counter;
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
            for(int i=0; i<items_count(); i++){
                _fd << "<th>" << std::to_string(i) << "</th>";
            }
            _fd << "</tr>\n";
        }

        logger::log(logger::LLOG::INFO, "CmrHtml", std::string(__func__) + " status: "  + std::to_string(_fd.is_open()));
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

    int _line_count = 0;
    int _part = 0;

    const char* _header = "<!DOCTYPE html>\n<html>\n<head><title>Sound HTML presentation</title></head>\n<body>";
    const char* _table_b = "<table>\n";
    const char* _table_e = "</table>\n";
    const char* _footer = "</body>\n</html>";
};

} //namespace

#endif