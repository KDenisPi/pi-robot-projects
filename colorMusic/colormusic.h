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
     * @param filename
     * @param params
     * @param dbg_out
     */
    ColorMusic(const std::string& filename, const ConsumerSettings& params, const bool dbg_out = false){
        logger::log(logger::LLOG::INFO, "cmusic", std::string(__func__));

        _data = std::make_shared<CrossData>(FftProc::chunk_size()); //2000
        _recv = std::make_shared<Receiver>(_data, filename);
        _sendr = std::make_shared<Sender>(_data);

        logger::log(logger::LLOG::INFO, "cmusic", std::string(__func__) + params.to_string());

        if(cmusic::is_real_hardware()){
            _gpio_provider = std::make_shared<pirobot::gpio::GpioProviderSimple>();
            _sendr->add_consumer<cmusic::CmrWS2801>(ws2801_leds(), true, _gpio_provider, params);
        }

        if(!cmusic::is_real_hardware() || dbg_out){
            logger::log(logger::LLOG::INFO, "cmusic", std::string(__func__) + "Debug output: " + std::to_string(dbg_out));

            _sendr->add_consumer<cmusic::CmrHtml>(FftProc::freq_interval(), false, _gpio_provider, params);
            _sendr->add_consumer<cmusic::CmrHtml>(ws2801_leds(), true,  _gpio_provider, params);
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
