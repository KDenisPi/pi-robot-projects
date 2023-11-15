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
#include "consumer_html.h"

namespace cmusic {

class Sender : public piutils::Threaded {
public:
    Sender(const std::shared_ptr<CMusicData>& data, const int sleds=300) : _data(data){
    }
    virtual ~Sender() {
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

    const int get_size() const{
        return _data->get_size();
    }

    static void worker(Sender* psend){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Started");

        int i_idx = 0;      //Index for received values
        const int data_count = 0; //Number of received values
        int data_idx = 0;
        int load_loops = 0; //The number of loops necessary for destination array filling

        std::shared_ptr<cmusic::CmrHtml> chtml = std::make_shared<cmusic::CmrHtml>();

        auto fn_event = [psend](int idx) { return ((idx == psend->_data->idx.load()) && !psend->is_stop_signal());};
        for(;;){

            while(fn_event(i_idx))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            i_idx = psend->_data->idx;

            logger::log(logger::LLOG::DEBUG, "sendr", std::string(__func__) + " Data index: " + std::to_string(i_idx) + " or finish " + std::to_string(psend->is_stop_signal()));

            if(psend->is_stop_signal()){
                break;
            }

            data_idx = (i_idx==0 ? 0 : psend->get_size());
            logger::log(logger::LLOG::DEBUG, "sendr", std::string(__func__) + " Data " + std::to_string(i_idx) + " Data idx: " + std::to_string(data_idx));

            chtml->process(&psend->_data->buff[data_idx] , psend->get_size());
        }

        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Finished");
    }

public:
    std::shared_ptr<CMusicData> _data;
};

}
#endif
