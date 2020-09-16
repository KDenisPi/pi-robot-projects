/*
 * state_measurement.h
 *
 *  Created on: Aug 30, 2020
 *      Author: Denis Kudia
 */

#ifndef DUST_STMEASUREMENT_H_
#define DUST_STMEASUREMENT_H_

#include "StateMachine.h"
#include "State.h"
#include "AnalogMeterSimple.h"
#include "dustSensor.h"

#include "context.h"

namespace dust {

enum TIMER_ID : uint16_t {
    TIMER_MEASUREMENT      = 1000,
};

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

        auto analog_meter = get_item<pirobot::analogmeter::AnalogMeterSimple>("AnalogMeter");
        if(analog_meter){
            logger::log(logger::LLOG::DEBUG, "DustMg", std::string(__func__) + " AnalogMeter Detected");
            ctxt->_density = analog_meter->get_data(0);
        }

        logger::log(logger::LLOG::DEBUG, "DustMg", std::string(__func__) + " Density : " + std::to_string(ctxt->_density));

        get_itf()->timer_start(TIMER_ID::TIMER_MEASUREMENT, measure_interval);
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
            auto analog_meter = get_item<pirobot::analogmeter::AnalogMeterSimple>("AnalogMeter");
            ctxt->_density = analog_meter->get_data(0);

            logger::log(logger::LLOG::DEBUG, "DustMg", std::string(__func__) + " Counter: " + std::to_string(ctxt->_density));
            TIMER_CREATE(TIMER_ID::TIMER_MEASUREMENT, measure_interval);

            return true;
          }
          break;
        }

        return false;
    }

  const int measure_interval = 10;

};

}//namespace dust
#endif
