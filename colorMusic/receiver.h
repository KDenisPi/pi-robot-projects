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

        while(!p->is_stop_signal()){
            int rcv_offset = rcv_index*p->_data->get_size();

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
                if(out[j][0]==0 && out[j][1]==0){
                    continue;
                }

                const double val = 10*log10(out[j][0]*out[j][0]+out[j][1]*out[j][1]);


                if(out[j][0]!=0 || out[j][1]!=0){
                    const double val = 10*log10(out[j][0]*out[j][0]+out[j][1]*out[j][1]);
                    printf("%d, %4.2f\n", j, val);
                    res += val;
                }

                if(j>0 && (j%p->freq_interval)==0){
                    if(res>0)
                        printf("%d, %4.2f, %d\n", j, res, (j/p->freq_interval));

                    res = (res/p->freq_interval) + p->amp_level;
                    logger::log(logger::LLOG::DEBUG, "recv", std::string(__func__) + " value: " + std::to_string(res));
                    //Set minimum level and do not use value less than it
                    if(res>0){
                        idx = (j/p->freq_interval) - 1;
                        int vol_idx = (int)((res/10)<3 ? (res/10) : 2);
                        p->_data->buff[data_idx++] = ldata::colors[idx + vol_idx];
                    }
                    res = 0.0;
                }
            }

            res = (res/p->freq_interval) + p->amp_level;
            //Set minimum level and do not use value less than it
            if(res>0){
                idx = (j/p->freq_interval) - 1;
                int vol_idx = (int)((res/10)<3 ? (res/10) : 2);
                p->_data->buff[data_idx++] = ldata::colors[idx + vol_idx];
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
    const double level_correction() const {
        return _amp_level;
    }

    void set_level_correction(const double amp_level){
        _amp_level = amp_level;
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

private:
    static const int _freq = 40000;     //Base frequency. Constant (samples/sec) - 40000
    static const int _n = 2000;         //Number samples for one time processing - 2000
                                        //Time interval covered by this number of samples: 40000/2000=20; 1000ms/20 = 50ms
    static const int _freqence_precision = 100; //100Hz
    static const int _freq_interval = (_n/2)/_freqence_precision;

private:
    double _amp_level = 0.0;    //Amplitude change used for alignment values

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