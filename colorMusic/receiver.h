/**
 * @file receiver.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CMUSIC_RECEIVER
#define CMUSIC_RECEIVER

#include <string>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#include <math.h>
#include <time.h>
#include <chrono>
#include <ctime>

#include<fftw3.h>

#include "Threaded.h"
#include "cmusicdata.h"
#include "colors.h"
namespace cmusic {

/*
arecord --file-type raw --channels=1 --rate=40000 --format=FLOAT_LE -D pulse  --buffer-size=0
*/

class Receiver : public piutils::Threaded {
public:
    Receiver(const std::shared_ptr<CMusicData>& data, const std::string& filename = "") : _filename(filename), _data(data) {
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Source: " + filename);
        _fd = (filename.empty() ? dup(STDIN_FILENO) : open(filename.c_str(), O_RDONLY|O_SYNC));
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Sourec FD: " + std::to_string(_fd));
    }


    virtual ~Receiver(){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));
        if(_fd>=0){
            close(_fd);
            _fd = -1;
        }
    }

    bool start(){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));
        return piutils::Threaded::start<Receiver>(this);
    }

    void stop(){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));
        piutils::Threaded::stop();
    }

    void wait(){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));
        piutils::Threaded::wait();
    }

    /**
     * @brief
     *
     */
    using fload = union {
        float fl;
        uint8_t ch[sizeof(float)];
    };

    /**
     * @brief Worker
     *
     * @param p
     */
    static void worker(Receiver* p){
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Started: " + std::to_string(p->fd()));
        if(!p->is_ready()){
            logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " Finished. Wrong file descriptior.");
            return;
        }

        if(!p->check_source()){
            logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " Finished. Source is not ready");
            return;
        }

        fload buff;
        int rcv_index = 0;

        /*
        FFT data (In, Out, Plan)
        */
        double* in = (double*) fftw_malloc(sizeof(double) * p->chunk_size());
        fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * p->chunk_size());
        fftw_plan my_plan;

        std::chrono::time_point<std::chrono::system_clock> tp_start, tp_end;
        bool success = true;

        //chunks per measurement interval
        const int i_chunks = p->freq_precision()/p->freq_chunk();
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Chunks per interval: " + std::to_string(i_chunks));

        while(!p->is_stop_signal()){
            int rcv_offset = rcv_index*p->_data->get_size();
            p->_data->clear(rcv_index);

            tp_start = std::chrono::system_clock::now();
            for(int i=0; i<p->chunk_size(); i++){
                size_t read_bytes = read(p->fd(), buff.ch, sizeof(buff.fl));
                if(read_bytes==0){ //EOF
                    logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " Unexpected EOF");
                    success = false;
                    break;
                }
                else if(read_bytes<0){
                    logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " File read error Error: " + std::to_string(errno));
                    success = false;
                    break;
                }
                else if(read_bytes<sizeof(buff.fl)){
                    logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " File read less than should: " + std::to_string(read_bytes));
                }

                in[i] = buff.fl;
            }
            tp_end = std::chrono::system_clock::now();
            logger::log(logger::LLOG::DEBUG, "recv", std::string(__func__) + " Loaded for (ms): " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(tp_end - tp_start).count()));

            if(!success){
                break;
            }

            my_plan = fftw_plan_dft_r2c_1d(p->chunk_size(), in, out, FFTW_ESTIMATE);
            fftw_execute(my_plan);

            double res = 0.0;
            int j, idx;
            int data_idx = rcv_index * p->_data->get_size(); //index in result array
            for(j=0; j<p->chunk_size()/2; j++){

                /*
                1. Ignore empty value
                2. Ignore negative values (?)
                */
                if(j==0){
                        printf("First value [%4.2f][%4.2f]\n", out[j][0], out[j][1]);
                }

                if(out[j][0]!=0 || out[j][1]!=0){
                    const double val = 10*log10(out[j][0]*out[j][0]+out[j][1]*out[j][1]);
                    if(val>0){ //debug
                        printf("%d, Freq: %d, %4.2f\n", j, j*p->freq_chunk(), val);
                    }

                    if(val>res){
                        res = val;
                    }
                }

                if(j>0 && (j%i_chunks)==0 && res>0){
                    const int i_idx = j/i_chunks;
                    printf("%d, Freq: [%d-%d], Val: %4.2f, %d\n", j, (j-i_chunks)*p->freq_chunk(), j*p->freq_chunk(), res, i_idx);

                    logger::log(logger::LLOG::DEBUG, "recv", std::string(__func__) + " value: " + std::to_string(res));
                    p->_data->buff[i_idx-1] = (uint32_t)round(res);
                    res = 0.0;
                }
            }

            if(res>0){
                const int i_idx = j/i_chunks;
                printf("%d, Freq: [%d-%d], Val: %4.2f, %d\n", j, (j-i_chunks)*p->freq_chunk(), j*p->freq_chunk(), res, i_idx);
                p->_data->buff[i_idx-1] = (uint32_t)round(res);
            }

            //update atomic value - start send thread
            p->_data->idx = data_idx;
            rcv_index = (rcv_index==1? 0 : 1);

            logger::log(logger::LLOG::DEBUG, "recv", std::string(__func__) + " has value: " + std::to_string((data_idx>=150 ? data_idx-150 : data_idx)));

            logger::log(logger::LLOG::DEBUG, "recv", std::string(__func__) + " Processed (ms): " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp_end).count()));

            fftw_destroy_plan(my_plan);
        }

        fftw_free(in);
        fftw_free(out);

        logger::log(logger::LLOG::INFO, "recv", std::string(__func__) + " Finished");
    }

    /**
     * @brief Check source - STDIN only for now
     *
     * @return true
     * @return false
     */
    const bool check_source() const{
        logger::log(logger::LLOG::INFO, "recv", std::string(__func__));

        if(!if_stdin())
            return true;

        fd_set readfds;
        struct timeval timeout;

        timeout.tv_sec = 4;
        timeout.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(_fd, &readfds);

        int ret = select(_fd+1, &readfds, NULL, NULL, &timeout);
        if(ret<0){
            logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " STDIN Error: " + std::to_string(errno));
            return false;
        }
        if(ret==0){
            logger::log(logger::LLOG::ERROR, "recv", std::string(__func__) + " STDIN not ready. Timeout.");
            return false;
        }

        return true;
    }

    /**
     * @brief
     *
     * @return const int
     */
    const int fd() const{
        return _fd;
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    const bool is_ready() const {
        return (_fd>=0);
    }

    const bool if_stdin() const {
        return _filename.empty();
    }

    std::shared_ptr<CMusicData> _data;

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
    const int chunk_size() const {
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
     * @brief Frequence interval (5 samples for 100Hz)
     *
     * @return const int
     */
    static const int freq_interval() {
        return _freq_interval;
    }

    const int freq_chunk() const {
        return _freq/_n;
    }

private:
    static const int _freq = 40000;     //Base frequency. Constant (samples/sec) - 40000
    //
    // We will try to process by 50ms chunk
    // 50ms = 1/20 sec; 40000 sm/sec * 0.05 = 2000
    static const int _n = 2000;         //Number samples for one time processing - 2000

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
    double _power_level = 0.0;    //Amplitude change used for alignment values

    /*
    Frequesnce interval is th value we racognize as single value.
    Example: we have 1000 (N/2) measurements for 20KHz (each for 20Hz interval) i.e. 20Hz per sample
    id we would like to recognize frequency with prrecisely 100Hz we should analyze 5 samples as one
    (200 visualization intervals)

    Otherwise if we have 50 intervals for visualization interval will be 20000Hz/50 = 400Hz per interval
    i.e. we will recognize 20 measurements as one (400Hz/20Hz=20 samples)
    */
private:
    int _fd;
    std::string _filename;

};

}//namespace

#endif