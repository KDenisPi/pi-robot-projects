/**
 * @file cmusicdata.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-11-27
 *
 * @copyright Copyright (c) 2023
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
template<class T>
class CMusicData {
public:
    CMusicData(const int bsize) : _bsize(bsize) {
        buff = new T[_bsize*2];
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
        std::memset(&buff[index*get_size()], 0x00, get_size()*sizeof(T));
    }

    const int get_idx() const {
        return idx;
    }

    std::atomic<int> idx;
    T* buff = nullptr;
    int _bsize;
};

using CrossData = CMusicData<double>;
using CrossDataPtr = std::shared_ptr<CrossData>;

}

#endif
