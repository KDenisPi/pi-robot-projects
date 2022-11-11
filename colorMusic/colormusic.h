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
        data = std::make_shared<CMusicData>(150);
        recv = std::make_shared<Receiver>(data, filename);
        sendr = std::make_shared<Sender>(data);
    }

    virtual ~ColorMusic(){
        stop();
    }

    void stop(){
        sendr->stop();
        recv->stop();
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool start(){
        sendr->start();
        return recv->start();
    }

    void wait(){
        recv->wait();
    }


    std::shared_ptr<CMusicData> data;
    std::shared_ptr<Receiver> recv;
    std::shared_ptr<Sender> sendr;
};

}//namespace
#endif