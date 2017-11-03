// Copyright © 2014-2017 Jay R. Wren <jrwren@xmtp.net>.
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#include <string>
#include <sstream>
#include <iostream>
#include <boost/program_options.hpp>
#include "curlpp11.hpp"
#include "cpptoml.h"
#include <cstdlib>
#include <boost/filesystem.hpp>
#include "dnsimple.hpp"

using namespace std;

static bool verbose = false;

class config {
public:
  string user;
  string token;
};

int get_creds(string *user, string *token) {
  char *conf_env = getenv("DOMASIMU_CONF");
  string configFileName;
  if (conf_env != NULL) {
    configFileName = conf_env;
  }
  if (configFileName == "") {
    configFileName =
        boost::filesystem::absolute(".domasimurc", getenv("HOME")).string();
  }
  config c;
  try {
    auto g = cpptoml::parse_file(configFileName);

    if (g->contains("user"))
      // c->user = g.get("user")->as<std::string>()->value();
      c.user = *g->get_as<string>("user");
    else
      return 2;
    if (g->contains("token"))
      // c->token = g.get("token")->as<std::string>()->value();
      c.token = *g->get_as<string>("token");
    else
      return 3;
  } catch (const cpptoml::parse_exception &e) {
    cerr << "Failed to parse " << configFileName << ": " << e.what() << endl;
    return 1;
  }
  *user = c.user;
  *token = c.token;
  return 0;
}

tuple<string, int>
get_record_id_by_value(dnsimple::client const &client, string domain,
                       dnsimple::change_record *change_record) {
  auto resp = client.get_records(domain);
  auto records = get<0>(resp);
  auto err = get<1>(resp);
  string id;
  if (err != 0) {
    return tuple<string, int>(id, err);
  }
  for (auto record = records.begin(); record != records.end(); record++) {
    if (record->name == change_record->name &&
        record->record_type == change_record->type &&
        record->content == change_record->value) {
      id = record->id;
      break;
    }
  }
  return make_tuple(id, 0);
}

tuple<string, int> create_or_update(dnsimple::client const &client,
                                    string message) {
  string domain;
  istringstream msg_stream(message);
  dnsimple::change_record change_record;
  msg_stream >> domain >> change_record.name >> change_record.type >>
      change_record.value;
  dnsimple::change_record new_record = change_record;
  msg_stream >> new_record.value >> new_record.ttl;
  string id;
  int err;
  tie(id, err) = get_record_id_by_value(client, domain, &change_record);
  if (err != 0) {
    return tuple<string, int>(0, err);
  }
  string resp_id;
  if (id.empty()) {
    tie(resp_id, err) = client.create_record(domain, &new_record);
  } else {
    tie(resp_id, err) = client.update_record(domain, &new_record);
  }
  if (err != 0) {
    return tuple<string, int>(0, err);
  }
  return tuple<string, int>(resp_id, 0);
}

tuple<string, int> delete_record(unique_ptr<dnsimple::client> const &client,
                                 string message) {
  string domain;
  istringstream msg_stream(message);
  dnsimple::change_record change_record;
  msg_stream >> domain >> change_record.name >> change_record.type >>
      change_record.value;
  string id;
  int err;
  tie(id, err) = get_record_id_by_value(*client, domain, &change_record);
  if (err != 0) {
    return tuple<string, int>(id, err);
  }
  if (id.empty()) {
    return make_tuple(id, 4);
  }
  err = client->destroy_record(domain, id);
  return tuple<string, int>(id, 0);
}
/*
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}*/

int main(int argc, char **argv) {
  curl::GlobalSentry curl;
  namespace po = boost::program_options;
  po::options_description desc("Options");
  desc.add_options()("help,h", "Print help message")(
      "verbose,v", "Use verbose output")("list,l", "List domains")(
      "update,u", po::value<string>(), "Update or create record")(
      "delete,d", po::value<string>(),
      "Delete record")("domain", po::value<vector<string>>(), "domain");

  po::positional_options_description p;
  p.add("domain", -1);

  po::variables_map vm;

  // po::store(po::parse_command_line(argc, argv, desc), vm);
  po::store(
      po::command_line_parser(argc, argv).options(desc).positional(p).run(),
      vm);
  notify(vm);

  if (vm.count("help")) {
    cout << desc << ".domasimurc config file example:" << endl;
    return 0;
  }

  if (vm.count("verbose")) {
    verbose = true;
    cerr << "verbose mode enabled" << endl;
  }

  string user, token;
  int err = get_creds(&user, &token);
  if (err != 0) {
    cerr << "could not read config:" << err << endl;
    return err;
  }

  // auto client = make_unique<dnsimple::client>(user, token);
  unique_ptr<dnsimple::client> client(new dnsimple::client(user, token));

  if (vm.count("list")) {
    auto domains = client->get_domains();
    /*        for(auto domain = domains.begin();
                    domain != domains.end();
                    domain++) {*/
    for (auto domain : domains) {
      if (verbose) {
        cout << domain.name << " " << domain.expires_on << endl;
      } else {
        cout << domain.name << endl;
      }
    }
  }
  if (vm.count("update")) {
    string id;
    int err;
    tie(id, err) = create_or_update(*client, vm["update"].as<string>());
    if (err) {
      cerr << "could not create or update:" << err << endl;
    } else {
      cout << "record written with id " << id << endl;
    }
  }
  if (vm.count("delete")) {
    string id;
    int err;
    tie(id, err) = delete_record(client, vm["delete"].as<string>());
    if (err) {
      cerr << "could not delete:" << err << endl;
    } else {
      cout << "record deleted with id " << id << endl;
    }
  }
  if (vm.count("domain")) {
    for (auto domain = vm["domain"].as<vector<string>>().begin();
         domain != vm["domain"].as<vector<string>>().end(); domain++) {
      vector<dnsimple::record> records;
      tie(records, err) = client->get_records(*domain);
      cout << *domain << endl;
      //            cout << records << endl;
    }
  }
  return 0;
}
