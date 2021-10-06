#include "config.h"
#include <fstream>
#include <iostream> 
#include <map>
#include <string>
#include <cstring>
#include <vector>


std::shared_ptr<config> config::config_ptr_ = nullptr;

config::config() {
    std::ifstream config_file("../config/config.txt");
    if (config_file.is_open() == false) {
        std::cerr << "Open config/config.txt failed." << std::endl;
        exit(errno);
    }

    for (int pp = 0; pp < 10; pp++) {
        std::cout <<  "-";
        if (pp == 9) {
            std::cout << std::endl << std::endl;
        }
    }

    std::string temp_s;
    for (int row_num = 0; getline(config_file, temp_s); row_num++) {
        if (temp_s[0] == '#') {
            continue;
        }

        char *temp_p = new char[temp_s.length() + 1];
        strcpy(temp_p, temp_s.c_str());
        char *p = strtok (temp_p, ", ");

        std::string key;
        std::string value;
        for (int column_num = 0; p != NULL; column_num++) {
            printf ("%s ", p);

            if (column_num == 0) {
                key = std::string(p);
                if (config_.count(key) != 0) {
                    std::cerr << "Repeat config key is illegal!" << std::endl;
                    exit(errno);
                }
            } else if (column_num == 1) {
                value = std::string(p);
                config_[key] = value;
            } else {
                std::cerr << "Each line config cannot exceed two words!" << std::endl;
                break;
            }
            p = strtok(NULL, ", ");
        }
        std::cout << std::endl;
        
        delete temp_p;
    }

    for (int pp = 0; pp < 10; pp++) {
        std::cout <<  "-";
        if (pp == 9) {
            std::cout << std::endl << std::endl;
        }
    }

    if (!judge_config()) {
        exit(errno);
    }
}

void config::init_config_ptr_() {
    config_ptr_.reset(new config());
}

std::shared_ptr<config> config::get_config_ptr() {
    if (!config_ptr_) {
        init_config_ptr_();
    }
    return config_ptr_;
}

bool config::judge_config() {
    std::vector<std::string> nece_strs;

    // server.cc
    nece_strs.push_back("limit_conn_waiting_num");
    nece_strs.push_back("token");
    nece_strs.push_back("limit_online_user_");

    for (auto key : nece_strs) {
         if (!config_.count(key)) {
            std::cerr << "Not found information about " << key << std::endl;
            return false;
         }
    }
    return true;
}