/**
 * @file cmusicdata.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CMUSIC_CMUSICDATA
#define CMUSIC_CMUSICDATA

#include <unistd.h>
#include <atomic>

namespace cmusic {

/**
 * @brief Service class for saving chunk processing
 *
 */
class CMusicData {
public:
    CMusicData(const int bsize = 150) : _bsize(bsize) {
        buff = new uint32_t[_bsize*2];
    }

    virtual ~CMusicData(){
        delete[] buff;
    }

    const int offset() const{
        return _bsize*idx;
    }

    const int get_size() const {
        return _bsize;
    }

    std::atomic<int> idx;
    uint32_t* buff = nullptr;
    int _bsize = 150;
};

}

#endif
