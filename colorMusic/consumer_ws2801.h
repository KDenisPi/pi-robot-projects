/**
 * @file consumer_ws2801.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-11-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CMUSIC_CONSUMER_WS2801
#define CMUSIC_CONSUMER_WS2801

#include <string>

#include "GpioProviderSimple.h"
#include "SPI.h"

#include "logger.h"
#include "consumer.h"
#include "colors.h"

namespace cmusic {

using pspi = std::shared_ptr<pirobot::spi::SPI>;
using pspi_cfg = pirobot::spi::SPI_config;
using provider = std::shared_ptr<pirobot::gpio::GpioProviderSimple>;
using gpio = std::shared_ptr<pirobot::gpio::Gpio>;

using LedData = std::unique_ptr<uint8_t>;


/**
 * @brief
 *
 */
class CmrWS2801 : public Consumer {
public:
    /**
     * @brief Construct a new consumer for LED WS2801 object
     *
     * @param leds_count
     */
    CmrWS2801(const int leds_count) : Consumer(leds_count, false)
    {
        logger::log(logger::LLOG::INFO, "ws2801", std::string(__func__));

        pspi_cfg cfg;
        cfg.speed[0] = pirobot::spi::SPI_SPEED::MHZ_2; // 25 Mhz
        cfg.speed[1] = pirobot::spi::SPI_SPEED::MHZ_2; // 25 Mhz

        g_prov = std::make_shared<pirobot::gpio::GpioProviderSimple>();
        logger::log(logger::LLOG::INFO, "ws2801", std::string(__func__) + " GPIO Provider: " + g_prov->printConfig());

        p_gpio_ce0 = std::make_shared<pirobot::gpio::Gpio>(19, pirobot::gpio::GPIO_MODE::OUT, g_prov, pirobot::gpio::PULL_MODE::PULL_OFF, pirobot::gpio::GPIO_EDGE_LEVEL::EDGE_NONE);
        p_gpio_ce1 = std::make_shared<pirobot::gpio::Gpio>(18, pirobot::gpio::GPIO_MODE::OUT, g_prov, pirobot::gpio::PULL_MODE::PULL_OFF, pirobot::gpio::GPIO_EDGE_LEVEL::EDGE_NONE);

        p_pspi = std::make_shared<pirobot::spi::SPI>(std::string("PI_SPI"), cfg, p_gpio_ce0, p_gpio_ce1);


        led_data_size = leds_count*3;
        ledData = LedData(new uint8_t(led_data_size));
    }

    /**
     * @brief Destroy the CmrWS2801 object
     *
     */
    virtual ~CmrWS2801() {
        logger::log(logger::LLOG::INFO, "ws2801", std::string(__func__));

        p_pspi.reset();

        p_gpio_ce1.reset();
        p_gpio_ce0.reset();

        g_prov.reset();
    }

    bool start(){
        logger::log(logger::LLOG::INFO, "ws2801", std::string(__func__));
        return piutils::Threaded::start<CmrWS2801>(this);
    }

    /*
    * ON for SPI based device
    */
    void SPI_On(){
        p_pspi->set_channel_on(pirobot::spi::SPI_CHANNELS::SPI_0);
    }

    /*
    * OFF for SPI base device
    */
    void SPI_Off(){
        p_pspi->set_channel_off(pirobot::spi::SPI_CHANNELS::SPI_0);
    }


    /**
     * @brief
     *
     * @param p
     */
    static void worker(CmrWS2801* p){
        logger::log(logger::LLOG::INFO, "ws2801", std::string(__func__) + " Started");

        auto fn = [p]{return (p->is_stop_signal() || p->is_has_job());};
        for(;;){
            {
                std::unique_lock<std::mutex> lk(p->cv_m);
                p->cv.wait(lk, fn);
            }

            if(p->is_stop_signal()){
                logger::log(logger::LLOG::INFO, "ws2801", std::string(__func__) + " Stop signal detected");
                break;
            }

            auto tp_start = p->processing_start();

            int not_empty_counter = p->process_data();

            logger::log(logger::LLOG::DEBUG, "ws2801", std::string(__func__) + " Processed for (ms): " + p->processing_end(tp_start) + " Values: " + std::to_string(not_empty_counter));

            p->set_busy(false);
        }

        logger::log(logger::LLOG::INFO, "ws2801", std::string(__func__) + " Finished");
    }

    /**
     * @brief
     *
     * @return const int
     */
    virtual const int process_data() override{
        logger::log(logger::LLOG::DEBUG, "ws2801", std::string(__func__) + " Started" );

        SPI_On();

        //init output data
        clear_data();

        for( std::size_t lidx = 0; lidx < items_count(); lidx++ ){

            const int idx = lidx*3;
            ledData.get()[idx + 0] = (_data[lidx] & 0xFF);
            ledData.get()[idx + 1] = ((_data[lidx] >> 8 ) & 0xFF);
            ledData.get()[idx + 2] = ((_data[lidx] >> 16 ) & 0xFF);
        }

        if(!p_pspi->data_rw(ledData.get(), led_data_size)){
            logger::log(logger::LLOG::ERROR, "ws2801", std::string(__func__) + " Could not write data");
        }

        //switch off stransmission channel
        SPI_Off();
    }

private:

    void clear_data(){
        std::memset( (void*)ledData.get(), 0, led_data_size);
    }

    provider g_prov;
    gpio p_gpio_ce0;
    gpio p_gpio_ce1;
    pspi p_pspi;

    LedData ledData;
    std::size_t led_data_size;
};

} //namespace

#endif //CMUSIC_CONSUMER_WS2801