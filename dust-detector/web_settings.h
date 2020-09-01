/*
* web_settings.h
*
* Web interface for Dust project
*
*/
#ifndef DUST_WEB_SETTINGS_H
#define DUST_WEB_SETTINGS_H

#include "WebSettings.h"
#include "context.h"
#include "logger.h"

namespace dust {
namespace web {

class WebDust : public http::web::WebSettings {
public:
    WebDust(const uint16_t port, std::shared_ptr<smachine::StateMachineItf> itf) : http::web::WebSettings(port, itf){
        logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__));
    }

    /*
    *
    */
    virtual ~WebDust() {
        logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__));
    }


    /*
    *
    */
    virtual const std::pair<std::string, std::string> get_page(const struct mg_connection *conn) override {
        logger::log(logger::LLOG::DEBUG, "Web", std::string(__func__) + " URI:" + (conn->uri != nullptr ? conn->uri : "Not defined"));

        if(std::strcmp(conn->uri, "/") == 0 || strcmp(conn->uri, "/status.html") == 0 || strcmp(conn->uri, "/default.html") == 0){
           std::string result;
           return std::make_pair(http::web::mime_json, result);
        }

       return std::make_pair("", "");
    }

};

} //namespace web
} //namespace dust

#endif
