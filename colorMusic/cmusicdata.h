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
    using RawData = std::shared_ptr<T[]>;

    CMusicData(const int bsize) : _bsize(bsize) {
        buff[0] = RawData(new T[_bsize]);
        buff[1] = RawData(new T[_bsize]);
    }

    virtual ~CMusicData(){
    }

    const int get_size() const {
        return _bsize;
    }

    const void clear(const int index) {
        for(int i=0; i<get_size(); i++)
            buff[index][i] = 0;
    }

    /**
     * @brief
     *
     * @param idx
     * @return const std::shared_ptr<T[]>
     */
    const RawData get(const int index){
        return buff[index];
    }

    std::atomic<int> idx;

private:
    RawData buff[2];
    int _bsize;
};

using CrossData = CMusicData<double>;
using CrossDataPtr = std::shared_ptr<CrossData>;

}

#endif
