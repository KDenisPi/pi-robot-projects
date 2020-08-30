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
    StInitialization(smachine::StateMachineItf* itf) : smachine::state::State(itf, "StInitialization") {
    }

    virtual ~StInitialization(){}

    /**
    *
    */
    virtual void OnEntry() override {

    }
};

}//namespace dust
#endif
