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
    CMusicData(const int bsize) : _bsize(bsize) {
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

    const void clear(const int index) {
        std::memset(&buff[index*get_size()], 0x00, get_size()*sizeof(uint32_t));
    }

    const int get_idx() const {
        return idx;
    }

    std::atomic<int> idx;
    uint32_t* buff = nullptr;
    int _bsize;
};

}

#endif
