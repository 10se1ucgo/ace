#pragma once
#include "curl/curl.h"
#include "common.h"
#include <functional>

namespace ace { namespace net {
    struct URLClient;
    

    struct URLRequest {
        URLRequest(URLClient &client, const std::string &url);
//        URLRequest(URLRequest &&that) noexcept;
        ~URLRequest();
//        URLRequest &operator=(URLRequest &&that) noexcept;
        ACE_NO_COPY_MOVE(URLRequest)

        template<typename TFunc>
        URLRequest &on_downloaded(TFunc &&func) {
            this->_on_downloaded = std::forward<TFunc>(func); // am i doing this right
            return *this;
        }

        template<typename TFunc>
        URLRequest &on_data_received(TFunc &&func) {
            this->_on_data_received = std::forward<TFunc>(func); // am i doing this right
            return *this;
        }

        std::string data;
    private:
        // the callback (CURLOPT_WRITEFUNCTION) sent to libcurl
        static size_t write_callback(char *ptr, size_t size, size_t nmemb, URLRequest *thisptr);
        void release();
        void done();

        CURL *handle;
        std::reference_wrapper<URLClient> client;
        std::function<void(URLRequest &)> _on_downloaded, _on_data_received;

        friend URLClient;
    };

    struct URLClient {
        URLClient();
        ~URLClient();
        ACE_NO_COPY_MOVE(URLClient)

        void update(double dt);
        URLRequest &get(const std::string &url);


        std::vector<std::unique_ptr<URLRequest>> requests;
        CURLM *handle;
    };
}}
