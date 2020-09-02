/*
 * state_init.h
 *
 *  Created on: Aug 30, 2020
 *      Author: Denis Kudia
 */

#ifndef DUST_STINITIALIZATION_H_
#define DUST_STINITIALIZATION_H_

#include "StateMachine.h"
#include "context.h"

namespace dust {

class StInitialization : public smachine::state::State {
public:
    StInitialization(const std::shared_ptr<smachine::StateMachineItf> itf) : smachine::state::State(itf, "StInitialization"){
        logger::log(logger::LLOG::DEBUG, "DustInit", std::string(__func__) + " Started");
    }

    virtual ~StInitialization(){
        logger::log(logger::LLOG::DEBUG, "DustInit", std::string(__func__) + " Started");
    }

    /**
    *
    */
    virtual void OnEntry() override {
        logger::log(logger::LLOG::DEBUG, "DustInit", std::string(__func__) + " Started");
        auto ctxt = get_env<dust::Context>();

        ctxt->_dust_value = 0;

        CHANGE_STATE("StMeasurement");
    }
};

}//namespace dust
#endif
