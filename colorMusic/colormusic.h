/**
 * @file colormusic.h
 * @author Denis Kudia (dkudja@gmail.com)
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

#include "version.h"
#include "logger.h"
#include "cmusicdata.h"
#include "receiver.h"
#include "sender.h"
#include "colormusic_hw.h"

namespace cmusic {

class ColorMusic{
public:
    /**
     * @brief Construct a new Color Music object
     *
     * @param filename - filename for data source, if empty standard input is used
     * @param skip_loops - number of loops should skipped for output (from 0 [out all] to 6 [out one of 6])
     * @param dbg_out - make ouput created for test purposes (to HTML files) for reasl hardware too
     */
    ColorMusic(const std::string& filename, const int skip_loops = 3, const bool dbg_out = false, const int pal_index = 0){
        logger::log(logger::LLOG::INFO, "cmusic", std::string(__func__));

        _data = std::make_shared<CrossData>(FftProc::chunk_size()); //2000
        _recv = std::make_shared<Receiver>(_data, filename);
        _sendr = std::make_shared<Sender>(_data);

        if(cmusic::is_real_hardware()){
            _gpio_provider = std::make_shared<pirobot::gpio::GpioProviderSimple>();
            _sendr->add_consumer<cmusic::CmrWS2801>(ws2801_leds(), true, _gpio_provider, skip_loops, pal_index);
        }

        if(!cmusic::is_real_hardware() || dbg_out){
            logger::log(logger::LLOG::INFO, "cmusic", std::string(__func__) + "Debug output: " + std::to_string(dbg_out));

            _sendr->add_consumer<cmusic::CmrHtml>(FftProc::freq_interval(), false, _gpio_provider);
            _sendr->add_consumer<cmusic::CmrHtml>(ws2801_leds(), true,  _gpio_provider, skip_loops);
        }
    }

    /**
     * @brief
     *
     * @return const int
     */
    const int ws2801_leds() const {
        return 63;
    }

    /**
     * @brief Destroy the Color Music object
     *
     */
    virtual ~ColorMusic(){
        logger::log(logger::LLOG::INFO, "cmusic", std::string(__func__));
        stop();
    }

    /**
     * @brief
     *
     */
    void stop(){
        logger::log(logger::LLOG::INFO, "cmusic", std::string(__func__));

        _recv->stop();
        _sendr->stop();
    }

    /**
     * @brief Start Sender and receiver
     *
     * @return true
     * @return false
     */
    bool start(){
        logger::log(logger::LLOG::INFO, "cmusic", std::string(__func__));

        _sendr->start();
        return _recv->start();
    }

    /**
     * @brief
     *
     */
    void stop_receiver(){
        _recv->set_stop_signal(true);
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

    gpio_provider _gpio_provider;
};

std::shared_ptr<cmusic::ColorMusic> cmusic;

}//namespace
#endif
