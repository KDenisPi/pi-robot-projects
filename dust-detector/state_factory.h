/*
 * state_factory.h
 *
 *  Created on: Aug 30, 2020
 *      Author: Denis Kudia
 */

#ifndef DUST_STATEFACTORY_H_
#define DUST_STATEFACTORY_H_

#include "StateFactory.h"
#include "context.h"
#include "state_init.h"
#include "state_measurement.h"

namespace dust {

class StFactory: public smachine::StateFactory {
public:
	StFactory(const std::string& firstState = "StInitialization") : smachine::StateFactory(firstState) {
		logger::log(logger::LLOG::DEBUG, "StFact", std::string(__func__) + " Set first state:" + firstState);
	}

	virtual ~StFactory() {}

	virtual const std::shared_ptr<smachine::state::State> get_state(const std::string state_name, smachine::StateMachineItf* itf) override{
	    logger::log(logger::LLOG::DEBUG, "StFact", std::string(__func__) + " State:" + state_name);
	    if(state_name.compare("StInitialization") == 0){
		return std::shared_ptr<smachine::state::State>(new dust::StInitialization(itf));
	    }
	    else if(state_name.compare("StMeasurement") == 0){
		return std::shared_ptr<smachine::state::State>(new dust::StMeasurement(itf));
   	    }

	    //return empty object
	    return smachine::StateFactory::get_state(state_name, itf);
        }

	/*
	 * Create Environment object
	 */
	virtual std::shared_ptr<smachine::Environment> get_environment() override {
	    return std::make_shared<Context>();
	}
};

} /* namespace dust */

#endif /* DUST_STATEFACTORY_H_ */

