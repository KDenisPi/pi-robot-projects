/**
 * @file consumer.h
 * @author Denis Kudia (dkudja@gmail.com)
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
#include <tuple>

#include "logger.h"
#include "Threaded.h"
#include "colormusic_hw.h"

namespace cmusic {

using Tp = std::chrono::time_point<std::chrono::system_clock>;

class ConsumerSettings {
public:
    ConsumerSettings() {}
    ConsumerSettings(const int sl=3, const int pidx=0, const int sitms=0)
        : skip_loops(sl), pal_index(pidx), sft_items(sitms) {}

    /**
     * @brief
     *
     * @return const std::string
     */
    const std::string to_string() const{
        const std::string result = std::string("SLoops: ") + std::to_string(skip_loops) + " PIfx: " + std::to_string(pal_index) + " Shift: "+ std::to_string(sft_items);
        return result;
    }

    /**
     * @brief
     *
     * @param oth
     * @return ConsumerSettings&
     */
    ConsumerSettings& operator=(const ConsumerSettings& oth){
        this->pal_index = oth.pal_index;
        this->sft_items = oth.sft_items;
        this->skip_loops = this->skip_loops;
        return *this;
    }

    const int skiplps() const {
        return skip_loops;
    }

    const int pidx() const {
        return pal_index;
    }

    const int shtitms() const {
        return sft_items;
    }

    const int lnpfile() const {
        return ln_pfile;
    }

    const std::string rdirect() const{
        return rdir;
    }

    void set_pidx(const int pidx){
        pal_index = pidx;
    }

    void set_sloops(const int sloops){
        skip_loops = sloops;
    }

private:
    int skip_loops = 3;     //skip number of input loops for output
    int pal_index = 0;      //index of palette should be used for output
    int sft_items = 0;      //number of items should we shift for output

    int ln_pfile = 200;
    std::string rdir = "./";
};


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
     * @param skip_loops
     */
    Consumer(const int items_count, const bool extend_data, const ConsumerSettings& params)
    : _items_count(items_count), _extend_data(extend_data), cs(params) {
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
     * @return true
     * @return false
     */
    virtual bool start() = 0;

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    const bool is_skip_loop() {
        if(skip_loops() <= 0)
            return false;

        const bool skip = ((_loop_counter >= 0) && (_loop_counter < skip_loops()));

        _loop_counter++;
        if(_loop_counter > skip_loops())
            _loop_counter = 0;

        return skip;
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

        if(!is_ready()) //Consumer object cound not start
            return false;

        if(is_skip_loop()){
            //std::cout << "Skip data " << std::endl;
            return true;
        }

        //std::cout << " Consumer input size: " << d_size << std::endl;

        bool trans = transformate_data(data, d_size, (uint32_t) pwr_avg);
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

    /**
     * @brief Set the entend data object
     *
     * @param extend_data
     */
    void set_entend_data(const bool extend_data){
        _extend_data = extend_data;
    }

    /**
     * @brief
     *
     * @return const int
     */
    const int items_count() const {
        return _items_count;
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
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
        const int intervals = cmusic::ldata::col_intervals_count;
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

    /**
     * @brief
     *
     * @return const int
     */
    const int skip_loops() const {
        return cs.skiplps();
    }

    virtual const std::string to_string() = 0;

    /**
     * @brief
     *
     * @param g_prov
     */
    virtual void hardware_init(const gpio_provider& g_prov) {

    }

    /**
     * @brief
     *
     */
    virtual void hardware_release() {

    }

    /**
     * @brief Get the palette idx object
     *
     * @return const int
     */
    const int get_palette_idx() const {
        return cs.pidx();
    }

    /**
     * @brief Get the shift items object
     *
     * @return const int
     */
    const int get_shift_items() const {
        return cs.shtitms();
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    virtual const bool is_support_shift() {
        return false;
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
    virtual bool transformate_data(const OutData& data, const int d_size, const int average_level){
        //std::cout << "Input size: " << d_size << " Internal size: " << items_count() << std::endl;

        //the simplest scenario - inpur and cunsumer data have the same size and no data extension
        if(d_size == items_count()){
            for(int i=0; i<items_count(); i++){
                _data[i] = data[i];
            }

            //std::cout << "Trasformer Easy way" << std::endl;
            return true;
        }

        //TDOD: probably to make it configured
        constexpr const auto pw_ignore_below = [] (const int pwr_level) { return pwr_level/2;};

        //std::cout << " Ign. below: " << pw_ignore_below(average_level) << std::endl;
        //
        if(d_size > items_count()){
            const uint32_t idx = d_size/items_count();   //group values by
            int j = 0;
            MeasData val = {0,0,0};

            while(j < (items_count()-1)){
                val = data[j*idx];
                for(int kg_idx = j*idx; kg_idx<((j+1)*idx); kg_idx++){
                    if( std::get<0>(data[kg_idx]) > std::get<0>(val))
                        val = data[kg_idx];
                }

                //Ignore values less than some power value - 1/3 of average level now
                if(std::get<0>(val) > pw_ignore_below(average_level))
                    _data[j] =  val;
                else
                    _data[j] = std::make_tuple(0, std::get<1>(val), 0);

                j++;
            }

            val = data[j*idx];
            for(int kg_idx = j*idx; kg_idx<d_size; kg_idx++){
                if(std::get<0>(data[kg_idx]) > std::get<0>(val))
                    val = data[kg_idx];
            }
            if(std::get<0>(val) >= pw_ignore_below(average_level))
                _data[j] = val;
            else
                _data[j] = std::make_tuple(0, std::get<1>(val), 0);

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
        std::vector<int> freq_count = std::vector<int>(color_intervals()*2, 0);

        //std::cout << " items_count: " << items_count() << std::endl;

        const uint32_t* clr_blocks = colors_blocks();
        for(int i=0; i<items_count(); i++){
            const auto val_0 = std::get<0>(_data[i]);
            const auto val_1 = std::get<1>(_data[i]);

            const auto freq_idx = get_interval_by_freq(val_1);
            const auto pwr_idx = get_color_by_power(val_0, pwr_avg);
            const auto color = freq_idx*ldata::pal_colors_per_block + pwr_idx;


            if(!is_extend_data()){ //easy way
                const uint32_t clr_code = (val_0 == 0 ? black_color() : clr_blocks[color]);
                _data[i] = std::make_tuple(val_0, val_1, clr_code);
                continue;
            }

            if(val_0 > 0){
                freq_count[freq_idx*2 + pwr_idx]++;
                j++;
            }
            else{
                _data[i] = std::make_tuple(val_0, val_1, black_color());  //case when we do not have any values
            }

            //std::cout << "i: " << i << " Fst: " << std::dec << std::get<0>(_data[i]) << " Scd: " << std::dec << std::get<1>(_data[i]) << " Thrd: 0x" << std::hex << std::get<2>(_data[i]) << std::endl;
        }

/*
        int freq_count_all = 0;
        for(auto i = 0; i < freq_count.size(); i++){
            if(freq_count[i] > 0){
                std::cout << i << "-" << freq_count[i] << " ";
                freq_count_all += freq_count[i];
            }
        }
        std::cout << std::endl << " ---- " << freq_count_all << std::endl;
*/
        /*
        Extend data in depends on number values for some frequency
        */
        if(is_extend_data() && j>0){
            //j - number of real values
            if((items_count()/j) <= 2){
                int i = 0;
                while(j<items_count()){
                    if(freq_count[i]>0){
                        freq_count[i] += 1;
                        j++;
                    }

                    i = (i < freq_count.size()-1 ? i+1 : 0);
                }
            }
            const int items_per_freq = items_count()/j;

            //std::cout << " items_per_freq " << items_per_freq << " j " << j << std::endl;

            int i_idx = 0;
            int color = -1;
            int freq_count_all = 0;
            for(int i=0; i<freq_count.size(); i++){

                if(freq_count[i]==0)
                    continue;

                const auto pwr_idx = i%2;
                color = (i/2)*ldata::pal_colors_per_block + pwr_idx;

                for(int k=0; k<(freq_count[i]*items_per_freq); k++){
                    _data[i_idx] = std::make_tuple(std::get<0>(_data[i_idx]), std::get<1>(_data[i_idx]), clr_blocks[color]);
                    //_data[i_idx] = std::make_tuple(freq_count[i], i, ldata::colors_blocks[color]);
                    i_idx++;
                }
                freq_count_all += freq_count[i];
            }

            if(color >= 0 && (i_idx < items_count()) ){
                while(i_idx < items_count()){
                    _data[i_idx] = std::make_tuple(std::get<0>(_data[i_idx]), std::get<1>(_data[i_idx]), clr_blocks[color]);
                    //_data[i_idx] = std::make_tuple(j, items_per_freq, ldata::colors_blocks[color]);
                    i_idx++;
                }
                //_data[i_idx] = std::make_tuple(freq_count_all, freq_count_all, ldata::colors_blocks[color]);
            }

        }

        //shift output to around
        //TODO: Copy not just the last value but all shift items
        if(is_support_shift() && get_shift_items() != 0){
            const int shifts = get_items_to_shift();
            //std::cout << " Shift: " << shifts  <<  std::endl;
            for(int itms=0; itms < shifts; itms++){
                auto last_itm = _data[items_count()-1];
                for(int idx=items_count()-2; idx>=0 ; idx--){
                   //std::cout << idx << " ";
                    _data[idx+1] = _data[idx];
                }
                 //std::cout  <<  std::endl;
                _data[0] = last_itm;
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

    /**
     * @brief
     *
     * @return const int
     */
    const int color_intervals() const {
        return cmusic::ldata::col_intervals_count;
    }

    /**
     * @brief
     *
     * @return const uint32_t*
     */
    virtual const uint32_t* colors_blocks(){
        return ldata::colors_blocks_html;
    }

    /**
     * @brief
     *
     * @return const uint32_t
     */
    virtual const uint32_t black_color() {
         return ldata::color_black;
    }

    /**
     * @brief Set the palette idx object
     *
     * @param pal_idx
     */
    virtual void set_palette_idx(const int pal_idx = 0){
        cs.set_pidx(0);
    }

    bool _busy = false;
    bool _extend_data = false;

   const int get_items_to_shift() {
     _shift_counter = _shift_counter + get_shift_items();
     if(_shift_counter >= items_count())
       _shift_counter = 0;

     return _shift_counter;
   }

protected:
    int _items_count;      //number of output items supported by this consumer
    OutData _data;

    int _loop_counter = -1;     //loop countr
    int _shift_counter = 0;

    ConsumerSettings cs;

};

}
#endif
 
