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
        _data = OutData(new MeasData[_items_count]);
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
    virtual bool process(const OutData& data, const int d_size, const double pwr_avg){

        if(is_has_job()) //consumer process the privious data
            return false;

        bool ready = is_ready();
        if(!ready) //Consumer object cound not start
            return false;

        bool trans = transformate_data(data, d_size);
        if(trans){
            bool fr2col = freq_to_color((uint32_t) pwr_avg);

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
    const bool is_extend_data() const{
        return _extend_data;
    }

    void set_entend_data(const bool extend_data){
        _extend_data = extend_data;
    }

    const int items_count() const {
        return _items_count;
    }

    virtual const bool is_ready(){
        return true;
    }

    /**
     * @brief
     *
     */
    virtual void process_data() = 0;

    /**
     * @brief Get the color interval by freqency
     *
     * @param freq
     * @return const int color set index
     */
    virtual const int get_interval_by_freq(const int freq) const{
        const int intervals = sizeof(cmusic::ldata::col_intervals)/sizeof(int);
        for(int i=0; i<intervals; i++){
            if(freq <= ldata::col_intervals[i]) return i;
        }

        return (intervals-1);
    }

    /**
     * @brief Get the color by power level
     *
     * @param power - power level
     * @param average_level - avarage power level
     * @return const int color index
     */
    virtual const int get_color_by_power(const int power, const int average_level) const {
        const int quoter = average_level/2;
        return (power < quoter ? 0 : 1);
    }


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
        if(d_size == items_count()){
            for(int i=0; i<items_count(); i++){
                _data[i] = data[i];
            }

            return true;
        }

        //
        if(d_size > items_count()){
            const uint32_t idx = d_size/items_count();   //group values by
            int j = 0;
            MeasData val = {0,0};

            while(j < (items_count()-1)){
                val = {0,0};
                for(int kg_idx = j*idx; kg_idx<((j+1)*idx); kg_idx++){
                    if(data[kg_idx].first > val.first)
                        val = data[kg_idx];
                }
                _data[j] = val;
                j++;
            }

            val = {0,0};
            for(int kg_idx = j*idx; kg_idx<d_size; kg_idx++){
                if(data[kg_idx].first > val.first)
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
    virtual bool freq_to_color(const uint32_t pwr_avg){
        int color;
        int j = 0; //for extend data

        for(int i=0; i<items_count(); i++){
            const auto freq_idx = get_interval_by_freq(_data[i].second);
            const auto pwr_idx = get_color_by_power(_data[i].first, pwr_avg);
            const auto color = freq_idx*ldata::pal_colors_per_block + pwr_idx;
/*
            std::cout << i << " Freq: " << std::dec << _data[i].second << " Idx: " << freq_idx << " Pwr: " << _data[i].first << " Idx: " << pwr_idx << " Avg: " << pwr_avg/2
                << " color idx: " << color << " color: " << std::hex << ldata::colors_blocks[color] << std::endl;
*/
            if(!is_extend_data()) //easy way
                _data[i].first = (_data[i].first == 0 ? ldata::color_black : ldata::colors_blocks[color]);
            else {
                if(_data[i].first > 0){
                     _data[j++].first = ldata::colors_blocks[color];
                }
            }
        }

        if(is_extend_data() && j>0 && j<items_count()){
            int k=0;
            for(int i=j; i<items_count(); i++){
                _data[i].first = _data[k++].first;
                if(k>=j) k=0;
            }
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
