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
#include "pi-main/pi-main.h"

#include "state_factory.h"
#include "web_settings.h"

namespace dust {

class DustControl : public pimain::PiMain {
public:
    DustControl() : pimain::PiMain(std::string("dust")){
        std::cout <<  "----------- Created ----" << std::endl;
    }

    virtual std::shared_ptr<smachine::StateFactory> factory(const std::string& firstState) override {
        std::cout <<  "----------- My factory ----" << std::endl;

        if(_factory){
            return _factory;
        }

        std::cout <<  "----------- My factory 2 ----" << std::endl;
        return std::make_shared<dust::StFactory>();
    }

    /*
    *
    */
   virtual std::shared_ptr<http::web::WebSettings> web(const uint16_t port, std::shared_ptr<smachine::StateMachineItf> itf) override{
        std::cout <<  "----------- My factory 3 ----" << std::endl;
       return std::shared_ptr<dust::web::WebDust>();
   }
};

}//namespace dust

#endif
