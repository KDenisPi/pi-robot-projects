/**
 * @file sender.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CMUSIC_SENDER
#define CMUSIC_SENDER

#include <string>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "Threaded.h"
#include "cmusicdata.h"

namespace cmusic {

class Sender : public piutils::Threaded {
public:
    Sender(const std::shared_ptr<CMusicData>& data) : _data(data) {}
    virtual ~Sender() {}

    bool start(){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
        return piutils::Threaded::start<Sender>(this);
    }

    void stop(){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
        piutils::Threaded::stop();
    }

    void wait(){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
        piutils::Threaded::wait();
    }

    static void worker(Sender* p){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Started");
        int i_idx = 0;

        auto fn = [p, i_idx]{return (p->is_stop_signal() || (i_idx != p->_data->idx.load()));};
        for(;;){

            {
                std::unique_lock<std::mutex> lk(p->cv_m);
                p->cv.wait(lk, fn);
            }
/*
            while((i_idx == p->_data->idx.load()) && !p->is_stop_signal())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
*/

            logger::log(logger::LLOG::DEBUG, "sendr", std::string(__func__) + " Data " + std::to_string(p->_data->idx) + " or finish " + std::to_string(p->is_stop_signal()));

            i_idx = p->_data->idx;
            if(p->is_stop_signal()){
                break;
            }
        }

        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Finished");
    }


    std::shared_ptr<CMusicData> _data;
};

}
#endif
