// Copyright © 2014-2015 Jay R. Wren <jrwren@xmtp.net>.
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#ifndef _DNSIMPLE_HPP_
#define _DNSIMPLE_HPP_

#include<iostream>
#include<string>
#include<json/json.h>
#include"curlpp11.hpp"

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

namespace dnsimple {
    class record {
        public:
            std::string id;
            std::string name;
            std::string record_type;
            std::string content;
    };
    class domain {
        public:
            int id;
            int user_id;
            int registrant_id;
            std::string name;
            std::string unicode_name;
            std::string token;
            std::string state;
            std::string language;
            bool lockable;
            bool auto_renew;
            bool whois_protected;
            int record_count;
            int service_count;
            std::string expires_on;
            std::string created_at;
            std::string updated_at;
    };
    class change_record {
        public:
            std::string name;
            std::string value;
            std::string type;
            std::string ttl;
    };
    class client {
        public:
        client(std::string email, std::string token) :
            token{std::move(token)},
            email{std::move(email)},
            url{"https://api.dnsimple.com/v1"}
        {
        }
        std::string get(std::string endpoint) {
            std::string results;
            curl::Easy c;
            c.url((url + endpoint).c_str())
                .header(("X-DNSimple-Token: " + email + ":" + token).c_str())
                .header("Accept: application/json")
                .perform(results);
            return results;
        }
        std::vector<domain> get_domains() {
            std::vector<domain> domains;
            std::string results = get("/domains");
//            std::cout << results << std::endl;

            Json::Value root;
            Json::Reader reader;
            bool parsingSuccessful = reader.parse(results, root);
            if (!parsingSuccessful) {
                std::cerr << "Failed to parse response\n"
                    << reader.getFormattedErrorMessages();
                return domains;
            }
            if (root.isObject()) {
                std::cerr << "Unexpected response:" << results << std::endl;
                return domains;
            }
            int i = 0;
/*            for (auto item = root.begin();
                    item!=root.end();
                    item++) { */
            int size = root.size();
            std:: cout << "root size: " << size << std::endl;
            for (int index = 0 ; index < size ; ++index) {
                auto item = root[index]["domain"];
                auto members = item.getMemberNames();
/*                std::cout << i++ << ": has name:";
                std::cout << item.isMember("domain") << " ";
                std::cout << item["domain"] << " ";
                for (auto member = members.begin();
                        member != std::end(members);
                        member++)
                    std::cout << *member << ", " ;
                std::cout << std::endl;*/
                domain d;
                d.id = item["id"].asInt();
                d.user_id = item["user_id"].asInt();
                d.registrant_id = item["registrant_id"].asInt();
                d.name = item["name"].asString();
                d.unicode_name = item["unicode_name"].asString();
                d.token = item["token"].asString();
                d.state = item["state"].asString();
                d.language = item["language"].asString();
                d.lockable = item["lockable"].asBool();
                d.auto_renew = item["auto_renew"].asBool();
                d.whois_protected = item["whois_protected"].asBool();
                d.record_count = item["record_count"].asInt();
                d.service_count = item["service_count"].asInt();
                d.expires_on = item["expires_on"].asString();
                d.created_at = item["created_at"].asString();
                d.updated_at = item["updated_at"].asString();
                d.id = item["id"].asInt();
                domains.push_back(d);
            }

            return domains;
        }
        std::tuple<std::vector<record>, int> get_records(std::string domain) {
            std::vector<record> records;
            std::ostringstream os;
            //os << cURLpp::Options::Url(url + "/domains/" + domain + "/records");
            std::cout << os.str();
            return std::make_tuple(records, 0);
        }
        std::tuple<std::string, int> create_record(std::string domain, change_record *rchange) {
            std::string id;
            return std::make_tuple(id, 0);
        }
        std::tuple<std::string, int> update_record(std::string domain, change_record *change) {
            std::string id;
            return std::make_tuple(id, 0);
        }
        int destroy_record(std::string domain, std::string id) {
            return 0;
        }
        std::string token;
        std::string email;
        std::string url;

    };
}

#endif
