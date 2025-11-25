#ifndef REFUGE_RESTROOMS_H
#define REFUGE_RESTROOMS_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class RefugeRestrooms {
private:
    std::string api_base = "https://www.refugerestrooms.org/api";
    http_client_config client_config;
    
    http_request create_request(const std::string& endpoint, const std::string& method) {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Accept"), U("application/json"));
        request.headers().add(U("Content-Type"), U("application/json"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        return request;
    }
    
    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method) {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }

public:
    RefugeRestrooms() {
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> get_restrooms_by_date(
        int day,
        int month,
        int year,
        int page = 1,
        int per_page = 10,
        int offset = 0,
        bool ada = false,
        bool unisex = false,
        bool updated = false) {
        
        std::map<std::string, std::string> params;
        params["page"] = std::to_string(page);
        params["per_page"] = std::to_string(per_page);
        params["offset"] = std::to_string(offset);
        params["day"] = std::to_string(day);
        params["month"] = std::to_string(month);
        params["year"] = std::to_string(year);
        
        if (ada) params["ada"] = "true";
        if (unisex) params["unisex"] = "true";
        if (updated) params["updated"] = "true";
        
        return make_api_call("/v1/restrooms/by_date" + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_restrooms_by_location(
        double lat,
        double lng,
        int page = 1,
        int per_page = 10,
        int offset = 0,
        bool ada = false,
        bool unisex = false) {
        
        std::map<std::string, std::string> params;
        params["page"] = std::to_string(page);
        params["per_page"] = std::to_string(per_page);
        params["offset"] = std::to_string(offset);
        params["lat"] = std::to_string(lat);
        params["lng"] = std::to_string(lng);
        
        if (ada) params["ada"] = "true";
        if (unisex) params["unisex"] = "true";
        
        return make_api_call("/v1/restrooms/by_location" + build_query_params(params), "GET");
    }

    pplx::task<json::value> search_restrooms(
        const std::string& query,
        int page = 1,
        int per_page = 10,
        int offset = 0,
        bool ada = false,
        bool unisex = false) {
        
        std::map<std::string, std::string> params;
        params["page"] = std::to_string(page);
        params["per_page"] = std::to_string(per_page);
        params["offset"] = std::to_string(offset);
        params["query"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(query)));
        
        if (ada) params["ada"] = "true";
        if (unisex) params["unisex"] = "true";
        
        return make_api_call("/v1/restrooms/search" + build_query_params(params), "GET");
    }

    pplx::task<json::value> get_all_restrooms(
        int page = 1,
        int per_page = 10,
        int offset = 0,
        bool ada = false,
        bool unisex = false) {
        
        std::map<std::string, std::string> params;
        params["page"] = std::to_string(page);
        params["per_page"] = std::to_string(per_page);
        params["offset"] = std::to_string(offset);
        
        if (ada) params["ada"] = "true";
        if (unisex) params["unisex"] = "true";
        
        return make_api_call("/v1/restrooms" + build_query_params(params), "GET");
    }
};
#endif
