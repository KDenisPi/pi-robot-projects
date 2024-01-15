/**
 * @file stream_info.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-12-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <atomic>
#include <functional>
#include <memory>

#include "logger.h"
#include "colormusic.h"
#include "fft_processor.h"

using namespace std;

int main (int argc, char* argv[])
{
    bool success = true;
    std::shared_ptr<cmusic::FftProc> fft_proc = std::make_shared<cmusic::FftProc>();

    std::cout << "--- Stream parameters ---" << std::endl;
    std::cout << "Frequency:  " << fft_proc->freqency() << std::endl;
    std::cout << "Chunk size: " << fft_proc->chunk_size() << std::endl;
    std::cout << "Freq precision : " << fft_proc->freq_precision() << std::endl;
    std::cout << "Chunk interval : " << fft_proc->chunk_interval() << "ms [" <<  (int)(1000/fft_proc->chunk_interval()) << "Hz]"<< std::endl;
    std::cout << "Freq per Koef  : " << fft_proc->freq_per_coefficient() << std::endl;
    std::cout << "Chunks per MrI : " << fft_proc->chunks_to_measurement_interval() << std::endl;


    std::cout << "Finished " << success << std::endl;
    exit( (success ? EXIT_SUCCESS : EXIT_FAILURE));
}