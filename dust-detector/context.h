/*
 * context.h
 *
 *  Created on: Aug 30, 2020
 *      Author: Denis Kudia
 */

#ifndef DUST_CONTEXT_H_
#define DUST_CONTEXT_H_

#include "Environment.h"

namespace dust {

class Context : public smachine::Environment {
public:
    /**
    *
    */
    Context(const std::string& ver = "1.0") : _version(ver), _ip4_address(""), _ip6_address(""), _density(0) {

    }

    virtual ~Context() noexcept {}

    // Version
    std::string _version;
    std::string _ip4_address;
    std::string _ip6_address;

   uint16_t  _density;
};

}//namespace dust
#endif
