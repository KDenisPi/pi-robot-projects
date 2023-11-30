/**
 * @file consumer.h
 * @author your name (you@domain.com)
 * @brief  Consumer base class. Transform general data to view can be used for different ways.
 * @version 0.1
 * @date 2023-10-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CMUSIC_CONSUMER
#define CMUSIC_CONSUMER

#include <cstdint>
#include <memory>
#include "Threaded.h"

namespace cmusic {

using Tp = std::chrono::time_point<std::chrono::system_clock>;

/**
 * @brief
 *
 */
class Consumer : public piutils::Threaded {
public:
    /**
     * @brief Construct a new Consumer object
     *
     * @param items_count
     * @param extend_data
     */
    Consumer(const int items_count, const bool extend_data) : _items_count(items_count), _extend_data(extend_data) {
        logger::log(logger::LLOG::INFO, "consm", std::string(__func__) + " Items count: " + std::to_string(items_count) + " Extend: " + std::to_string(extend_data));

        assert(items_count > 0 && items_count <1000);
        _data = OutData(new uint32_t[_items_count]);
    }

    /**
     * @brief Destroy the Consumer object
     *
     */
    virtual ~Consumer() {
        logger::log(logger::LLOG::INFO, "consm", std::string(__func__));
    }

    /**
     * @brief
     *
     */
    void stop(){
        logger::log(logger::LLOG::INFO, "consm", std::string(__func__));
        piutils::Threaded::stop();
    }

    /**
     * @brief
     *
     */
    void wait(){
        logger::log(logger::LLOG::INFO, "consm", std::string(__func__));
        piutils::Threaded::wait();
    }

    /**
     * @brief
     *
     * @param data
     * @param d_size
     */
    virtual bool process(const OutData& data, const int d_size){

        if(is_has_job()) //consumer process the privious data
            return false;

        bool ready = is_ready();
        if(!ready) //Consumer object cound not start
            return false;

        bool trans = transformate_data(data, d_size);
        if(trans){
            bool fr2col = freq_to_color();

            set_busy(true);
            cv.notify_one();
        }

        return true;
    }

    /**
     * @brief
     *
     * @return const Tp
     */
    const Tp processing_start() {
        return std::chrono::system_clock::now();
    }

    /**
     * @brief
     *
     * @param tp_start
     * @return const std::string
     */
    const std::string processing_end(const Tp tp_start){
        auto tp_end = std::chrono::system_clock::now();
        return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(tp_end - tp_start).count());
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    const bool is_has_job() const {
        return _busy;
    }

    /**
     * @brief Mark consumer as busy
     *
     * @param busy
     */
    void set_busy(const bool busy = true) {
        _busy = busy;
    }

    /**
     * @brief Extend values if original data do not contain enough
     *
     * @return true
     * @return false
     */
    const bool extend_data() const{
        return _extend_data;
    }

    const int items_count() const {
        return _items_count;
    }

    virtual const bool is_ready(){
        return true;
    }

    virtual void process_data() = 0;

private:
    /**
     * @brief Tranformate input data in depends on consumer parameters
     *
     * @param data - input array
     * @param d_size - input array size
     *
     * @return true - input data were tranformed successfully
     * @return false - could not transform input data to consumer view
     */
    virtual bool transformate_data(const OutData& data, const int d_size){
        //std::cout << "Input size: " << d_size << " Internal size: " << items_count() << std::endl;

        //the simplest scenario - inpur and cunsumer data have the same size and no data extension
        if(d_size ==items_count()){
            for(int i=0; i<items_count(); i++){
                _data[i] = data[i];
            }

            return true;
        }

        //
        if(d_size > items_count()){
            const uint32_t idx = d_size/items_count();   //group values by
            int j = 0;
            uint32_t val = 0;

            while(j < (items_count()-1)){
                val = 0;
                for(int kg_idx = j*idx; kg_idx<((j+1)*idx); kg_idx++){
                    if(data[kg_idx]>val)
                        val = data[kg_idx];
                }
                _data[j] = val;
                j++;
            }

            val = 0;
            for(int kg_idx = j*idx; kg_idx<d_size; kg_idx++){
                if(data[kg_idx]>val)
                    val = data[kg_idx];
            }
            _data[j] = val;

            return true;
        }

        return false;
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    virtual bool freq_to_color(){
        int color;
        for(int i=0; i<items_count(); i++){
            if(items_count() == ldata::pal_size_32)
                color = i;
            else
                color = ((i/color_ratio()) < ldata::pal_size_32 ? i/color_ratio() : ldata::pal_size_32-1);

            _data[i] = (_data[i]==0 ? ldata::color_black : ldata::colors32[color]);
        }
        return true;
    }

    /**
     * @brief Color scale ratio.
     * It is 200/32=6 for now (200 number of intervalls, 32 palette size)
     *
     * @return const int
     */
    const int color_ratio() const {
        return FftProc::freq_interval()/ldata::pal_size_32;
    }

    bool _busy = false;
    bool _extend_data = false;

protected:
    int _items_count;      //number of output items supported by this consumer
    OutData _data;

};

}
#endif
