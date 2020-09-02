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
    StMeasurement(const std::shared_ptr<smachine::StateMachineItf> itf) : smachine::state::State(itf, "StMeasurement") {
    }

    virtual ~StMeasurement(){}

    /**
    *
    */
    virtual void OnEntry() override {
        logger::log(logger::LLOG::DEBUG, "DustMg", std::string(__func__) + " Started");
        auto ctxt = get_env<dust::Context>();

        ctxt->_dust_value++;

        get_itf()->timer_start(TIMER_ID::TIMER_MEASUREMENT, MEASURE_INTERVAL);
    }

    /**
    *
    */
    virtual bool OnTimer(const int id) override {
        logger::log(logger::LLOG::DEBUG, "DustMg", std::string(__func__) + " OnTimer ID: " + std::to_string(id));
        auto ctxt = get_env<dust::Context>();

        switch(id){
          case TIMER_ID::TIMER_MEASUREMENT:
          {
            ctxt->_dust_value++;
            logger::log(logger::LLOG::DEBUG, "DustMg", std::string(__func__) + " Counter: " + std::to_string(ctxt->_dust_value));

            TIMER_CREATE(TIMER_ID::TIMER_MEASUREMENT, MEASURE_INTERVAL);
            return true;
          }
          break;
        }

        return false;
    }

};

}//namespace dust
#endif
