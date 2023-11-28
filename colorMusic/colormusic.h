/**
 * @file colormusic.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-07
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CMUSIC_COLORMUSIC
#define CMUSIC_COLORMUSIC

#include <string>
#include <memory>

#include "logger.h"
#include "cmusicdata.h"
#include "receiver.h"
#include "sender.h"

namespace cmusic {

class ColorMusic{
public:
    /**
     * @brief Construct a new Color Music object
     *
     * @param filename
     */
    ColorMusic(const std::string& filename){
        _data = std::make_shared<CrossData>(FftProc::chunk_size()); //2000
        _recv = std::make_shared<Receiver>(_data, filename);
        _sendr = std::make_shared<Sender>(_data);
    }

    virtual ~ColorMusic(){
        stop();
    }

    void stop(){
        _sendr->stop();
        _recv->stop();
    }

    /**
     * @brief Start Sender and receiver
     *
     * @return true
     * @return false
     */
    bool start(){
        _sendr->start();
        return _recv->start();
    }

    /**
     * @brief Wait untill we have something to process
     *
     */
    void wait(){
        _recv->wait();
    }

private:

    CrossDataPtr _data;
    std::shared_ptr<Receiver> _recv;
    std::shared_ptr<Sender> _sendr;
};

}//namespace
#endif