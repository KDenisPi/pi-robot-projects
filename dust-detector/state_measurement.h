/*
 * state_measurement.h
 *
 *  Created on: Aug 30, 2020
 *      Author: Denis Kudia
 */

#ifndef DUST_STMEASUREMENT_H_
#define DUST_STMEASUREMENT_H_

#include "StateMachine.h"
#include "context.h"

namespace dust {

enum TIMER_ID : uint16_t {
    TIMER_MEASUREMENT      = 1000,
};

#define MEASURE_INTERVAL   60 //sec


class StMeasurement : public smachine::state::State {
public:
    /**
    *
    */
    StMeasurement(smachine::StateMachineItf* itf) : smachine::state::State(itf, "StMeasurement") {
    }

    virtual ~StMeasurement(){}

    /**
    *
    */
    virtual void OnEntry() override {

    }

    /**
    *
    */
    virtual bool OnTimer(const int id) override {
    }

};

}//namespace dust
#endif
