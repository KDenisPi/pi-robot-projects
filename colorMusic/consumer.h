/**
 * @file consumer.h
 * @author your name (you@domain.com)
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

namespace cmusic {

using Tp = std::chrono::time_point<std::chrono::system_clock>;

class Consumer {
public:
    Consumer() {}
    virtual ~Consumer() {}

    /**
     * @brief
     *
     * @param data
     * @param d_size
     */
    virtual void process(const uint32_t* data, const int d_size) = 0;

    const Tp processing_start() {
        return std::chrono::system_clock::now();
    }

    const std::string processing_end(const Tp tp_start){
        auto tp_end = std::chrono::system_clock::now();
        return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(tp_end - tp_start).count());
    }

};

}
#endif
