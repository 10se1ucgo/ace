#include <algorithm>
#include "net/url.h"
#include "util/except.h"

namespace ace { namespace net {
    URLRequest::URLRequest(URLClient &client, const std::string &url): handle(curl_easy_init()), client(client) {
        curl_easy_setopt(this->handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(this->handle, CURLOPT_WRITEFUNCTION, &URLRequest::write_callback);
        curl_easy_setopt(this->handle, CURLOPT_WRITEDATA, this);
        curl_multi_add_handle(this->client.get().handle, this->handle);
    }

//    URLRequest::URLRequest(URLRequest &&that) noexcept:
//        data(std::move(that.data)),
//        handle(that.handle),
//        client(that.client),
//        _on_downloaded(std::move(that._on_downloaded)) {
//        that.handle = nullptr;
//    }
//
//    URLRequest &URLRequest::operator=(URLRequest &&that) noexcept {
//        if (this != &that) {
//            this->release();
//            std::swap(this->handle, that.handle);
//            std::swap(this->client, that.client);
//            std::swap(this->data, that.data);
//            std::swap(this->_on_data_received, that._on_data_received);
//            std::swap(this->_on_downloaded, that._on_downloaded);
//        }
//        return *this;
//    }

    URLRequest::~URLRequest() {
        this->release();
    }

    void URLRequest::done() {
        if(this->_on_downloaded) this->_on_downloaded(*this);
    }

    size_t URLRequest::write_callback(char *ptr, size_t size, size_t nmemb, URLRequest *thisptr) {
        thisptr->data.append(ptr, size * nmemb);
        if(thisptr->_on_data_received) thisptr->_on_data_received(*thisptr);
        return size * nmemb;
    }

    void URLRequest::release() {
        if (this->handle == nullptr) return;
        curl_multi_remove_handle(this->client.get().handle, this->handle);
        curl_easy_cleanup(this->handle);
        this->handle = nullptr;
    }

    URLClient::URLClient(): handle((curl_global_init(CURL_GLOBAL_DEFAULT), curl_multi_init())) {
    }

    URLClient::~URLClient() {
        this->requests.clear();
        curl_multi_cleanup(this->handle);
        curl_global_cleanup();
    }

    void URLClient::update(double dt) {
        int running_handles = 0;
        CURLMcode err;
        do {
            err = curl_multi_perform(this->handle, &running_handles);
//            fmt::print("HANDLES: {} vs {}\n", running_handles, this->requests.size());
            if(err != CURLM_OK && err != CURLM_CALL_MULTI_PERFORM) {
                THROW_ERROR("ERROR IN URL REQUEST: {}\n", curl_multi_strerror(err));
            }
        } while (false /* err == CURLM_CALL_MULTI_PERFORM */ /* CURLM_CALL_MULTI_PERFORM deprecated? cURL libtest ignores it */);

        CURLMsg *message;
        do {
            int msgq = 0;
            message = curl_multi_info_read(this->handle, &msgq);
            if (message && (message->msg == CURLMSG_DONE)) {
                CURL *handle = message->easy_handle;

                const auto it = std::find_if(this->requests.begin(), this->requests.end(), [handle](const std::unique_ptr<URLRequest> &p) {
                    return p->handle == handle;
                });
                (*it)->done();
                this->requests.erase(it);
            }
        } while (message);
    }

    URLRequest &URLClient::get(const std::string &url) {
        this->requests.emplace_back(std::make_unique<URLRequest>(*this, url));
        return *this->requests.back();
    }
}}
