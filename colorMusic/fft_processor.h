/**
 * @file fft_processor.h
 * @author Denis Kudia (dkudja@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CMUSIC_FFT
#define CMUSIC_FFT

#include <time.h>
#include <chrono>
#include <ctime>
#include <memory>
#include <tuple>
#include <fftw3.h>
#include <cstdint>
#include <functional>

namespace cmusic {

#include "logger.h"
#include "cmusicdata.h"

using MeasData = std::tuple<uint32_t, uint32_t, uint32_t>;
using OutData = std::unique_ptr<MeasData[]>;
using FftDouble = std::unique_ptr<double, std::function<void(double*)>>;
using FftComplex = std::unique_ptr<fftw_complex, std::function<void(fftw_complex*)>>;

class FftProc {
public:
    FftProc() {
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));

        /*
        FFT data (In, Out, Plan)
        */
        buff_in = FftDouble((double*)fftw_malloc(sizeof(double) * FftProc::chunk_size()), [](double* ptr) { fftw_free(ptr); });
        buff_out = FftComplex((fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FftProc::chunk_size()), [](fftw_complex* ptr) { fftw_free(ptr); });

        logger::log(logger::LLOG::INFO, "fftp", std::string(__func__) + " Chunks per interval: " + std::to_string(chunks_to_mitv));
    }

    virtual ~FftProc() {
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
    }

    /**
     * @brief Number of output Frequence intervals
     *
     * @return const int
     */
    static const int freq_interval() {
        return _freq_interval;
    }

    /**
     * @brief
     *
     * @param data_in
     * @param data_out
     * @param d_size
     * @return true
     * @return false
     */
    bool process(const CrossData::RawData& data_in, const int d_size_in, OutData& data_out, const int d_size_out){

        double res = 0.0;
        int j, idx;
        int not_empty_counter = 0;
        double val_min = 0.0, val_max = 0.0;

        if(chunk_size()<d_size_in){
            logger::log(logger::LLOG::ERROR, "fftp", std::string(__func__) + " Too much input data: " + std::to_string(d_size_in));
            return false;
        }

        tp_start = std::chrono::system_clock::now();

        for(int i=0; i<d_size_in; i++){
            buff_in.get()[i] = data_in[i];
        }

        my_plan = fftw_plan_dft_r2c_1d(chunk_size(), buff_in.get(), buff_out.get(), FFTW_ESTIMATE);
        fftw_execute(my_plan);
        set_power_correction(0.0);
        for(j=0; j<chunk_size()/2; j++){

            /*
            1. Ignore empty value
            2. Ignore negative values (?)
            */
            const fftw_complex* cpx = buff_out.get();
            if(cpx[j][0]!=0 || cpx[j][1]!=0){
                const double val = 10*log10(cpx[j][0]*cpx[j][0]+cpx[j][1]*cpx[j][1]);

                if(val < val_min)
                    val_min = val;
                if(val > val_max)
                    val_max = val;

                if(j==0 && val>0){
                    set_power_correction(val);
                }

                if(val+power_correction() > res)
                    res = val + power_correction();
            }

            if(j>0 && (j%chunks_to_mitv)==0){
                const int i_idx = j/chunks_to_mitv;
                if(i_idx <= d_size_out){
                    if(res>0){
                        data_out[i_idx-1] = std::make_tuple((uint32_t)round(res), (uint32_t)(j*freq_per_coefficient()), (uint32_t)0);
                        not_empty_counter++;

                        //std::cout << "x: " << i_idx-1 << " Fst: " << std::dec << std::get<0>(data_out[i_idx-1]) << " Scd: " << std::dec << std::get<1>(data_out[i_idx-1]) << " Thrd: 0x" << std::hex << std::get<2>(data_out[i_idx-1]) << std::endl;
                    }
                    else
                        data_out[i_idx-1] = std::make_tuple((uint32_t)0, (uint32_t)(j*freq_per_coefficient()), (uint32_t)0);
                }
                res = 0.0;
            }
        }

        //std::cout << " Power correction:  " << power_correction() << "\t Min: " << val_min << "\t Max: " << val_max << std::endl;

        const int i_idx = j/chunks_to_mitv;
        if(i_idx <= d_size_out){
            if(res>0){
                data_out[i_idx-1] =  std::make_tuple((uint32_t)round(res), j*freq_per_coefficient(), 0);
                not_empty_counter++;
            }
            else
                data_out[i_idx-1] = std::make_tuple(0, j*freq_per_coefficient(), 0);
        }

        fftw_destroy_plan(my_plan);

        tp_end = std::chrono::system_clock::now();
        auto e_time = std::chrono::duration_cast<std::chrono::milliseconds>(tp_end - tp_start).count();
        logger::log(logger::LLOG::DEBUG, "fftp", std::string(__func__) + " Processed for (ms): " + std::to_string(e_time) + " No Empty: " + std::to_string(not_empty_counter));

        return true;
    }

    /**
     * @brief Return base frequesncy
     *
     * @return const int
     */
    const int freqency() const {
        return _freq;
    }

    /**
     * @brief Return number of samples for one chunk.
     *
     * @return const int
     */
    static const int chunk_size(){
        return _n;
    }

    /**
     * @brief Return volume level modifier
     *
     * @return const double
     */
    const double power_correction() const {
        return _power_level;
    }

    void set_power_correction(const double amp_level){
        _power_level = amp_level;
    }

    /**
     * @brief Frequence processing pprecision (100Hz)
     *
     * @return const int
     */
    static const int freq_precision(){
        return _freqence_precision;
    }

    /**
     * @brief
     *
     * @return const int
     */
    static const int chunk_interval() {
        return _chunk_interval;
    }

    static const int freq_per_coefficient() {
        return _freq_per_coefficient;
    }

    const int chunks_to_measurement_interval() {
        return chunks_to_mitv;
    }

private:
    static const int _freq = 44100;     //Base frequency. Constant (samples/sec) - 40000

    static const int _chunk_interval = 40; //milliseconds

    // We will try to process by 50ms chunk
    // 50ms = 1/20 sec; 40000 sm/sec * 0.05 = 2000
    static const int _n = (_freq*_chunk_interval)/1000;  //Number samples for one time processing - 2000

    /*
    The array you are showing is the Fourier Transform coefficients of the audio signal.
    These coefficients can be used to get the frequency content of the audio.
    The FFT is defined for complex valued input functions, so the coefficients you get out will be imaginary numbers
    even though your input is all real values. In order to get the amount of power in each frequency,
    you need to calculate the magnitude of the FFT coefficient for each frequency.
    This is not just the real component of the coefficient, you need to calculate the square root of
    the sum of the square of its real and imaginary components.
    That is, if your coefficient is a + b*j, then its magnitude is sqrt(a^2 + b^2).

    Once you have calculated the magnitude of each FFT coefficient,
    you need to figure out which audio frequency each FFT coefficient belongs to.
    An N point FFT will give you the frequency content of your signal at N equally spaced frequencies, starting at 0.
    Because your sampling frequency is 44100 samples / sec. and the number of points in your FFT is 256,
    your frequency spacing is 44100 / 256 = 172 Hz (approximately)


    My case: 40000 samples/sec, the number of points in FFT is 2000
    40000/2000 = 20 Hz

    The first coefficient in your array will be the 0 frequency coefficient. That is basically the average power level
    for all frequencies. The rest of your coefficients will count up from 0 in multiples of 172 Hz until you get to 128.
    In an FFT, you only can measure frequencies up to half your sample points. Read these links on the Nyquist Frequency and
    Nyquist-Shannon Sampling Theorem if you are a glutton for punishment and need to know why,
    but the basic result is that your lower frequencies are going to be replicated or aliased in the higher frequency buckets.
    So the frequencies will start from 0, increase by 172 Hz for each coefficient up to the N/2 coefficient,
    then decrease by 172 Hz until the N - 1 coefficient.

    My case: It means that we will be able to measure frequence in interval between 20*1000=20000 (0 - 20KHz)


    Note: That is basically the average power level for all frequencies. (!)
    */

   // (freq/n)*(n/2) = freq/2
   // My case: It means that we will be able to measure frequence in interval between 20*1000=20000 (0 - 20KHz)
   static const int _max_frequency = _freq/2;

    //minimum frequency interval we would like to recognize
    static const int _freqence_precision = 100; //100Hz

    //number of intervals
    static const int _freq_interval = _max_frequency/_freqence_precision;

    //which audio frequency each FFT coefficient belongs to (20 Hz)
    static const int _freq_per_coefficient = _freq/_n;

private:
    double _power_level = 0.0;    //Amplitude change used for alignment values        fftw_plan my_plan;


    /*
    Frequesnce interval is th value we racognize as single value.
    Example: we have 1000 (N/2) measurements for 20KHz (each for 20Hz interval) i.e. 20Hz per sample
    id we would like to recognize frequency with prrecisely 100Hz we should analyze 5 samples as one
    (200 visualization intervals)

    Otherwise if we have 50 intervals for visualization interval will be 20000Hz/50 = 400Hz per interval
    i.e. we will recognize 20 measurements as one (400Hz/20Hz=20 samples)
    */

    //double* buff_in;
    //fftw_complex* buff_out;

    FftDouble buff_in;
    FftComplex buff_out;

    fftw_plan my_plan;

    //chunks per measurement interval
    const int chunks_to_mitv = freq_precision()/freq_per_coefficient();
    std::chrono::time_point<std::chrono::system_clock> tp_start, tp_end;

};

}

#endif
