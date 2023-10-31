/**
 * @file consumer_html.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CMUSIC_CONSUMER_HTML
#define CMUSIC_CONSUMER_HTML

#include "consumer.h"

namespace cmusic {

class CmrHtml : public Consumer {
public:
    CmrHtml() {}
    virtual ~CmrHtml() {}

    virtual void process(const uint32_t* data, const int d_size);

private:
    int _fd = -1;

};

} //namespace

#endif