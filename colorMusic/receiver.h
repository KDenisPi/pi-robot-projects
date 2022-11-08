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

#include "logger.h"
#include "Threaded.h"

namespace cmusic {

class Receiver : public piutils::Threaded {
public:
    Receiver(const std::string& filename = "") : _filename(filename) {
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Source: " + filename);
        _fd = (filename.empty() ? dup(STDIN_FILENO) : open(filename.c_str(), O_RDONLY));
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Sourec FD: " + std::to_string(_fd));
    }


    virtual ~Receiver(){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));
        if(_fd>=0){
            close(_fd);
            _fd = -1;
        }
    }

    /**
     * @brief
     *
     */
    using fload = union {
        float fl;
        uint8_t ch[4];
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

private:
    int _fd;
    std::string _filename;
};

}//namespace

#endif