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
    Sender(const std::shared_ptr<CMusicData>& data, const int sleds=300) : _data(data), sleds_count(sleds) {
        sleds_buff = new uint32_t[sleds_count];
    }
    virtual ~Sender() {
        delete[] sleds_buff;
    }

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
        int i_idx = 0;      //Index for received values
        int data_count = 0; //Number of received values
        int data_idx = 0;
        int load_loops = 0; //The number of loops necessary for destination array filling

        auto fn = [p, i_idx]{return ((i_idx != p->_data->idx.load())|| p->is_stop_signal());};

        for(;;){

            while(((i_idx == p->_data->idx.load()) && !p->is_stop_signal()))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

/*
            {
                std::unique_lock<std::mutex> lk(p->cv_m);
                p->cv.wait(lk, fn);
            }
*/
            i_idx = p->_data->idx;

            logger::log(logger::LLOG::DEBUG, "sendr", std::string(__func__) + " Data " + std::to_string(i_idx) + " or finish " + std::to_string(p->is_stop_signal()));

            data_count = (i_idx%p->_data->get_size());
            data_idx = (i_idx/p->_data->get_size());

            if(p->is_stop_signal()){
                break;
            }

            if(data_count==0){
                logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Nothing to do Data " + std::to_string(i_idx));
                continue;
            }

            load_loops = (p->sleds_count/data_count);

            for(int loop=0; loop<load_loops; loop++){
                std::memcpy((void*)&p->sleds_buff[loop*data_count],  (void*)p->_data->buff, sizeof(uint32_t)*data_count);
            }

            if((p->sleds_count/data_count)>0){
                std::memcpy((void*)&p->sleds_buff[load_loops*data_count],  (void*)p->_data->buff, sizeof(uint32_t)*(p->sleds_count%data_count));
            }

            logger::log(logger::LLOG::DEBUG, "sendr", std::string(__func__) + " Data " + std::to_string(i_idx) + " Data idx: " + std::to_string(data_idx) +
                " Loops: " + std::to_string(load_loops) + " Tail: " + std::to_string(p->sleds_count%data_count));

            /*
                Send data to Arduino
            */

        }

        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Finished");
    }


    std::shared_ptr<CMusicData> _data;
    int sleds_count;
    uint32_t* sleds_buff = nullptr;
};

}
#endif
