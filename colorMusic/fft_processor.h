/**
 * @file fft_processor.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-11-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CMUSIC_FFT
#define CMUSIC_FFT

namespace cmusic {

#include <time.h>
#include <chrono>
#include <ctime>
#include <memory>
#include <fftw3.h>

#include "logger.h"

using OutData = std::unique_ptr<uint32_t[]>;

class FftProc {
public:
    FftProc() {
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));
        /*
        FFT data (In, Out, Plan)
        */
        buff_in = (double*) fftw_malloc(sizeof(double) * FftProc::chunk_size());
        buff_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * chunk_size());

        logger::log(logger::LLOG::INFO, "fftp", std::string(__func__) + " Chunks per interval: " + std::to_string(chunks_to_mitv));
    }

    virtual ~FftProc() {
        logger::log(logger::LLOG::INFO, "sendr", std::string(__func__));

        fftw_free(buff_in);
        fftw_free(buff_out);
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

        if(chunk_size()<d_size_in){
            logger::log(logger::LLOG::ERROR, "fftp", std::string(__func__) + " Too much input data: " + std::to_string(d_size_in));
            return false;
        }

        tp_start = std::chrono::system_clock::now();

        for(int i=0; i<d_size_in; i++){
            buff_in[i] = data_in[i];

        }

        my_plan = fftw_plan_dft_r2c_1d(chunk_size(), buff_in, buff_out, FFTW_ESTIMATE);
        fftw_execute(my_plan);
        set_power_correction(0.0);
        for(j=0; j<chunk_size()/2; j++){

            /*
            1. Ignore empty value
            2. Ignore negative values (?)
            */
            if(buff_out[j][0]!=0 || buff_out[j][1]!=0){
                const double val = 10*log10(buff_out[j][0]*buff_out[j][0]+buff_out[j][1]*buff_out[j][1]);
                if(val>res)
                    res = val;

                if(j==0 && val>0){
                    set_power_correction(val);
                }
            }

            if(j>0 && (j%chunks_to_mitv)==0){
                const int i_idx = j/chunks_to_mitv;
                if(res>0 && i_idx <= d_size_out){
                    data_out[i_idx-1] = (uint32_t)round(res);
                    not_empty_counter++;
                }
                res = 0.0;
            }
        }

        const int i_idx = j/chunks_to_mitv;
        if(res>0 && i_idx <= d_size_out){
            data_out[i_idx-1] = (uint32_t)round(res);
            not_empty_counter++;
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
    static const int freq_chunk(){
        return _freq/_n;
    }

    /**
     * @brief
     *
     * @return const int
     */
    static const int chunk_interval() {
        return _chunk_interval;
    }

private:
    static const int _freq = 40000;     //Base frequency. Constant (samples/sec) - 40000

    static const int _chunk_interval = 50; //milliseconds

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

    double* buff_in;
    fftw_complex* buff_out;
    fftw_plan my_plan;

    //chunks per measurement interval
    const int chunks_to_mitv = freq_precision()/freq_chunk();
    std::chrono::time_point<std::chrono::system_clock> tp_start, tp_end;

};

}

#endif
