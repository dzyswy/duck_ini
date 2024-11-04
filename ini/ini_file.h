#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <typeinfo>
#include <iomanip>
#include <glog/logging.h>

namespace duck {

namespace ini {

class Value
{
public:
    Value() : str_(std::string()), type_(std::string()) {}
    Value(const std::string& value) : str_(value), type_(std::string()) {

    }

    template<typename T>
    T get() {
        T res;
 
        if (!type_.empty()) {
            CHECK(typeid(res).name() == type_) << "data type: " << typeid(res).name() << " is wrong, expect " << type_ << " !";
        }

        std::istringstream iss(str_);
        iss >> res;
        return res;
    }


    template<typename T>
    void set(T value) {
        str_ = std::to_string(value); 
        type_ = typeid(value).name();
    }

    void set(const char* value) { 
        str_ = value; 
        type_ = typeid(str_).name();
    }

    void set(const std::string& value) {
        str_ = value; 
        type_ = typeid(str_).name();
    }

    std::string str() {
        return str_;
    }

private:
    std::string str_; 
    std::string type_;
};





class Root
{
public:

    int load(const std::string& file_name) {

        root_map_.clear();
        std::ifstream ifs(file_name);
        if (ifs.fail()) {
            LOG(WARNING) << "Failed to load ini file: " << file_name << "!";
            return -1;
        }
 
        std::string line;
        std::string section_name = "";
        size_t line_num = 0;
        while(std::getline(ifs, line))
        {
            line_num++;
            line = trim(line);
            if (line.empty()) {
                continue;
            }
            if ((line[0] == '#') || (line[0] == ';')) //it's comment
            {
                continue;
            }
            if (line[0] == '[') // it's section
            {
                auto pos = line.find_first_of(']');
                if (pos == std::string::npos) {
                    LOG(WARNING) << file_name << " line: " << line_num << " section format error, missing ] !";
                    continue;
                }
                section_name = trim(line.substr(1, pos - 1));
            }
            else //it's key = value
            {
                auto pos = line.find_first_of('=');
                if (pos == std::string::npos) {
                    LOG(WARNING) << file_name << " line: " << line_num << " key-value format error, missing = !";
                    continue;
                }

                if (section_name.empty()) {
                    LOG(WARNING) << file_name << ": missing section !";
                    return -1;
                }

                std::string key_name = line.substr(0, pos);
                key_name = trim(key_name);
                std::string value_str = line.substr(pos + 1);
                value_str = trim(value_str);

                Value value(value_str);
                root_map_[section_name][key_name] = value;
            }
        }

        return 0;
    }


    int save(const std::string& file_name) {

        return 0;
    }

    std::string trim(std::string s)
    {
        if (s.empty())
        {
            return s;
        }
        s.erase(0, s.find_first_not_of(" \r\n"));
        s.erase(s.find_last_not_of(" \r\n") + 1);
        return s;
    }

    void print_root_map() {

        std::cout << std::left << std::setw(40) << std::setfill('-')<< "ini root begin"  << std::setfill(' ') << std::endl;  
        std::cout << std::left << std::setw(20) << "key_name" << std::setw(20) << "value" << std::endl;

        for (const auto& section_pair :  root_map_) {
            std::string section_name = section_pair.first;
            auto& section_map = section_pair.second;
            for (const auto& key_pair : section_map) {
                std::string key_name = key_pair.first;
                Value value = key_pair.second;

                std::string info_name = section_name + "." + key_name;
                std::cout << std::left << std::setw(20) <<  info_name << std::setw(20) << value.str() << std::endl;

            }
        }

        std::cout << std::left << std::setw(40) << std::setfill('-')<< "ini root end"  << std::setfill(' ') << std::endl;  
        std::cout << std::endl;
    }

    bool has_value(const std::string& section, const std::string& key) {
        auto section_pair = root_map_.find(section);
        if (section_pair == root_map_.end()) {
            return false;
        }
        auto& section_map = section_pair->second;
        auto value_pair = section_map.find(key);
        if (value_pair == section_map.end()) {
            return false;
        }
        return true;
    }

    template<typename T>
    T get(const std::string& section, const std::string& key, T default_value) {
        if (!has_value(section, key)) {
            LOG(WARNING) << "Failed to find ini value: " << section << "." << key << "!";
            return default_value;
        }
        Value value = root_map_[section][key];
        return value.get<T>();
    }

    template<typename T>
    void set(const std::string& section, const std::string& key, T value) {
        Value data;
        data.set(value);
        root_map_[section][key] = data;
    }

    std::string str() {

        std::stringstream ss;
        for (const auto& section_pair :  root_map_) {
            std::string section_name = section_pair.first;
            ss << "[" << section_name << "]" << std::endl;
            auto& section_map = section_pair.second;
            for (const auto& key_pair : section_map) {
                std::string key_name = key_pair.first;
                Value value = key_pair.second;

                ss << key_name << " = " << value.str() << std::endl;

            }
            ss << std::endl;
        }

        return ss.str();
    }
 

private: 

    std::map<std::string, std::map<std::string, Value > > root_map_;

    std::map<std::string, std::string> section_comment_;
    std::map<std::string, std::string> key_comment_;
    std::map<std::string, std::string> value_comment_;
};


}//namespace ini
}//namespace duck










