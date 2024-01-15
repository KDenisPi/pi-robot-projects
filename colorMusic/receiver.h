/**
 * @file receiver.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CMUSIC_RECEIVER
#define CMUSIC_RECEIVER

#include <string>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#include <math.h>
#include <time.h>
#include <chrono>
#include <ctime>
#include <thread>

#include "Threaded.h"
#include "cmusicdata.h"
#include "colors.h"
#include "fft_processor.h"

namespace cmusic {

/*
arecord --file-type raw --channels=1 --rate=40000 --format=FLOAT_LE -D pulse  --buffer-size=0
*/

class Receiver : public piutils::Threaded {
public:
    Receiver(const CrossDataPtr& data, const std::string& filename = "") : _filename(filename), _data(data) {
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Source: " + filename);
        _fd = (filename.empty() ? dup(STDIN_FILENO) : open(filename.c_str(), O_RDONLY|O_SYNC));
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Source FD: " + std::to_string(_fd));

        std::cout << "File descriptor: " << _fd << std::endl;
    }


    virtual ~Receiver(){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));

        if(_fd>=0){
            close(_fd);
            _fd = -1;
        }
    }

    bool start(){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));
        return piutils::Threaded::start<Receiver>(this);
    }

    void stop(){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));
        piutils::Threaded::stop();
    }

    void wait(){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));
        piutils::Threaded::wait();
    }

    /**
     * @brief
     *
     */
    using fload = union {
        float fl;
        uint8_t ch[sizeof(float)];
    };

    /**
     * @brief Worker
     *
     * @param p
     */
    static void worker(Receiver* p){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Started: " + std::to_string(p->fd()));
        if(!p->is_ready()){
            logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " Finished. Wrong file descriptior.");
            return;
        }

        if(!p->check_source()){
            logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " Finished. Source is not ready");
            return;
        }

        fload buff;
        int rcv_index = 0;
        bool success = true;
        std::chrono::time_point<std::chrono::system_clock> tp_start, tp_end;

        while(!p->is_stop_signal()){
            p->_data->clear(rcv_index);
            auto rdata = p->_data->get(rcv_index);
            tp_start = std::chrono::system_clock::now();
            for(int i=0; i < FftProc::chunk_size(); i++){
                size_t read_bytes = read(p->fd(), buff.ch, sizeof(buff.fl));
                if(read_bytes==0){ //EOF
                    logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " Unexpected EOF");
                    success = false;
                    break;
                }
                else if(read_bytes<0){
                    logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " File read error Error: " + std::to_string(errno));
                    success = false;
                    break;
                }
                else if(read_bytes<sizeof(buff.fl)){
                    logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " File read less than should: " + std::to_string(read_bytes));
                }

                rdata[i] = buff.fl;
            }
            tp_end = std::chrono::system_clock::now();
            auto e_time = std::chrono::duration_cast<std::chrono::milliseconds>(tp_end - tp_start).count();

            if(!success){
                break;
            }

            //update atomic value - start send thread
            p->_data->idx = rcv_index;

            logger::log(logger::LLOG::DEBUG, "recv", std::string(__func__) + " Processed (ms): " + std::to_string(e_time) + " Index: " + std::to_string(rcv_index));

            rcv_index = (rcv_index==1? 0 : 1);

            /*
                Now let's emulate delay if data loaded from the file
            */
           if(!p->if_stdin() && e_time < FftProc::chunk_interval()){
                logger::log(logger::LLOG::DEBUG, "recv", std::string(__func__) + " Emulated delay for (ms): " + std::to_string(FftProc::chunk_interval()-e_time));
                std::this_thread::sleep_for (std::chrono::milliseconds(FftProc::chunk_interval()-e_time));
           }

        }

        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Finished");
    }

    /**
     * @brief Check source - STDIN only for now
     *
     * @return true
     * @return false
     */
    const bool check_source() const{
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));

        if(!if_stdin())
            return true;

        fd_set readfds;
        struct timeval timeout;

        timeout.tv_sec = 4;
        timeout.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(_fd, &readfds);

        int ret = select(_fd+1, &readfds, NULL, NULL, &timeout);
        if(ret<0){
            logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " STDIN Error: " + std::to_string(errno));
            return false;
        }
        if(ret==0){
            logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " STDIN not ready. Timeout.");
            return false;
        }

        return true;
    }

    /**
     * @brief
     *
     * @return const int
     */
    const int fd() const{
        return _fd;
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    const bool is_ready() const {
        return (_fd>=0);
    }

    const bool if_stdin() const {
        return _filename.empty();
    }

    CrossDataPtr _data;

private:
    int _fd;
    std::string _filename;

};

}//namespace

#endif
