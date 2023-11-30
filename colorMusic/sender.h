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
#include "fft_processor.h"

#include "consumer_html.h"
#include "consumer_ws2801.h"

namespace cmusic {

class Sender : public piutils::Threaded {
public:
    Sender(const CrossDataPtr& data, const int sleds=300) : _data(data){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Data size: " + std::to_string(FftProc::freq_interval()));

        data_out = OutData(new uint32_t[FftProc::freq_interval()]);
        data_out_len = FftProc::freq_interval();
    }

    virtual ~Sender() {
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
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
        int load_loops = 0; //The number of loops necessary for destination array filling

        std::shared_ptr<cmusic::FftProc> fft_proc = std::make_shared<cmusic::FftProc>();
        std::shared_ptr<cmusic::CmrHtml> chtml = std::make_shared<cmusic::CmrHtml>();
        std::shared_ptr<cmusic::CmrWS2801> cmrWs2801 = std::make_shared<cmusic::CmrWS2801>(32);

        chtml->start();
        cmrWs2801->start();

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

            auto rawdata = psend->_data->get(i_idx);
            fft_proc->process(rawdata, psend->get_size(), psend->data_out, psend->d_size());

            chtml->process(psend->data_out , psend->d_size());
        }

        chtml->stop();
        cmrWs2801->stop();

        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Finished");
    }

public:

    const int d_size() const {
        return data_out_len;
    }

    CrossDataPtr _data;  //raw data received from received
    OutData data_out;     //data prepeared for output
    int data_out_len;
};

}
#endif
