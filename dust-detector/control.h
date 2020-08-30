/*
 * control.h
 *
 * Main class for application creating
 *
 * Created on: Aug 30, 2020
 *      Author: Denis Kudia
 */

#ifndef DUST_APPLICATION_
#define DUST_APPLICATION_

#include "StateMachine.h"
#include "logger.h"

#include "state_factory.h"
#include "web_settings.h"

namespace dust {

class DustControl :: public pimain::PiMain {
public:

    DustControl() : PiMain("dust"){

    }

    virtual std::shared_ptr<smachine::StateFactory> factory(const std::string& firstState) override {
        if(_factory){
            return _factory;
        }

        return std::make_shared<dust::StateFactory>();
    }

    /*
    *
    */
   virtual std::shared_ptr<http::web::WebSettings> web(const uint16_t port, std::shared_ptr<smachine::StateMachineItf> itf) override{
       return std::shared_ptr<dust::web::WebSettings>();
   }


};

}//namespace dust

#endif