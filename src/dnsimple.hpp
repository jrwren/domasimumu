// Copyright © 2014-2017 Jay R. Wren <jrwren@xmtp.net>.
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#ifndef _DNSIMPLE_HPP_
#define _DNSIMPLE_HPP_

#include <iostream>
#include <string>
#include <json/json.h>
#include "curlpp11.hpp"

using namespace std;

namespace dnsimple {
class record {
public:
  string id;
  string name;
  string record_type;
  string content;
};
class domain {
public:
  int id;
  int user_id;
  int registrant_id;
  string name;
  string unicode_name;
  string token;
  string state;
  string language;
  bool lockable;
  bool auto_renew;
  bool whois_protected;
  int record_count;
  int service_count;
  string expires_on;
  string created_at;
  string updated_at;
};
class change_record {
public:
  string name;
  string value;
  string type;
  string ttl;
};
class client {
public:
  client(string email, string token)
      : token(token), email(email),
        url{"https://api.dnsimple.com/v1"} { }
  string get(string endpoint) const {
    string results;
    curl::Easy c;
    c.url((url + endpoint).c_str())
        .header(("X-DNSimple-Token: " + email + ":" + token).c_str())
        .header("Accept: application/json")
        .perform(results);
    return results;
  }
  vector<domain> get_domains() const {
    vector<domain> domains;
    string results = get("/domains");
    //            cout << results << endl;

    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(results, root);
    if (!parsingSuccessful) {
      cerr << "Failed to parse response\n"
                << reader.getFormattedErrorMessages();
      return domains;
    }
    if (root.isObject()) {
      cerr << "Unexpected response:" << results << endl;
      return domains;
    }
    /*            for (auto item = root.begin();
                        item!=root.end();
                        item++) { */
    int size = root.size();
    cout << "root size: " << size << endl;
    for (int index = 0; index < size; ++index) {
      auto item = root[index]["domain"];
      auto members = item.getMemberNames();
      /*                cout << i++ << ": has name:";
                      cout << item.isMember("domain") << " ";
                      cout << item["domain"] << " ";
                      for (auto member = members.begin();
                              member != end(members);
                              member++)
                          cout << *member << ", " ;
                      cout << endl;*/
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
  tuple<vector<record>, int> get_records(string domain) const {
    vector<record> records;
    ostringstream os;
    // os << cURLpp::Options::Url(url + "/domains/" + domain + "/records");
    cout << os.str();
    return make_tuple(records, 0);
  }
  tuple<string, int> create_record(string domain,
                                             change_record *rchange) const {
    string id;
    return make_tuple(id, 0);
  }
  tuple<string, int> update_record(string domain,
                                             change_record *change) const {
    string id;
    return make_tuple(id, 0);
  }
  int destroy_record(string domain, string id) const { return 0; }
  const string token;
  const string email;
  const string url;
};
}

#endif
