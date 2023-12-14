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
    Sender(const CrossDataPtr& data, const int sleds=63) : _data(data){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Data size: " + std::to_string(FftProc::freq_interval()));

        data_out = OutData(new MeasData[FftProc::freq_interval()]);

        fft_proc = std::make_shared<cmusic::FftProc>();
        chtml = std::make_shared<cmusic::CmrHtml>(true);
        cmrWs2801 = std::make_shared<cmusic::CmrWS2801>(sleds, true);
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    const bool is_html_consumer() const {
        return false && (chtml);
    }

    /**
     * @brief Destroy the Sender object
     *
     */
    virtual ~Sender() {
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool start(){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
        return piutils::Threaded::start<Sender>(this);
    }

    /**
     * @brief
     *
     */
    void stop(){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
        piutils::Threaded::stop();
    }

    /**
     * @brief
     *
     */
    void wait(){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
        piutils::Threaded::wait();
    }

    const int get_size() const{
        return _data->get_size();
    }

    static void worker(Sender* p){
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Started");

        int i_idx = 0;      //Index for received values
        const int data_count = 0; //Number of received values
        int load_loops = 0; //The number of loops necessary for destination array filling

        if(p->is_html_consumer())
            p->chtml->start();

        p->cmrWs2801->start();

        auto fn_event = [p](int idx) { return ((idx == p->_data->idx.load()) && !p->is_stop_signal());};
        for(;;){

            while(fn_event(i_idx))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            i_idx = p->_data->idx;

            logger::log(logger::LLOG::DEBUG, "sendr", std::string(__func__) + " Data index: " + std::to_string(i_idx) + " or finish " + std::to_string(p->is_stop_signal()));

            if(p->is_stop_signal()){
                break;
            }

            auto rawdata = p->_data->get(i_idx);
            p->fft_proc->process(rawdata, p->get_size(), p->data_out, p->d_size());

            p->cmrWs2801->process(p->data_out , p->d_size(), p->fft_proc->power_correction());

            if(p->is_html_consumer())
                p->chtml->process(p->data_out , p->d_size(), p->fft_proc->power_correction());
        }

        p->cmrWs2801->stop();

        if(p->is_html_consumer())
            p->chtml->stop();

        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__) + " Finished");
    }

public:

    /**
     * @brief
     *
     * @return const int
     */
    const int d_size() const {
        return FftProc::freq_interval();
    }

    CrossDataPtr _data;     //raw data received from received
    OutData data_out;       //data prepeared for output

    std::shared_ptr<cmusic::FftProc> fft_proc;
    std::shared_ptr<cmusic::CmrHtml> chtml;
    std::shared_ptr<cmusic::CmrWS2801> cmrWs2801;
};

}
#endif
