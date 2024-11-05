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
    template<typename T>
    T get() {
        T res;
        std::istringstream iss(str_);
        iss >> res;
        return res;
    }


    template<typename T>
    void set(T value) {
        std::stringstream ss;
        ss << value;
        str_ = ss.str();
    }

    std::string str() {
        return str_;
    }

public:
    std::string str_;  
};


class Key
{
public:
    Key() : value_(std::make_shared<Value>()) {}
    Key(const std::string& key_name, const std::string& comment = std::string(), const std::string& right_comment = std::string()) 
        : key_name_(key_name), comment_(comment), right_comment_(right_comment), value_(std::make_shared<Value>()) {
        
    }

    std::shared_ptr<Value> value() {
        return value_;
    }

    void set_key_name(const std::string& key_name) {
        key_name_ = key_name;
    }

    void set_comment(const std::string& comment) {
        comment_ = comment;
    }

    void set_right_comment(const std::string& right_comment) {
        right_comment_ = right_comment;
    }

    std::string key_name() {
        return key_name_;
    }

    std::string comment() {
        return comment_;
    }

    std::string right_comment() {
        return right_comment_;
    }


private:
    std::string key_name_;
    std::string comment_;
    std::string right_comment_;
    std::shared_ptr<Value> value_;
};


class Section
{
public:
    Section() {}
    Section(const std::string& section_name, const std::string& comment = std::string(), const std::string& right_comment = std::string()) 
        : section_name_(section_name), comment_(comment), right_comment_(right_comment) {

    }

    int has_key(const std::string& key_name) {
        auto it = key_map_.find(key_name);
        if (it == key_map_.end()) {
            return -1;
        }
        return it->second;
    }

    int insert_key(const std::string& key_name, std::shared_ptr<Key> key) {
        int key_id = has_key(key_name);
        if (key_id >= 0) {
            LOG(WARNING) << "already has " << section_name_ << "." << key_name << ", can't insert same key in one section !";
            return -1;
        }

        int vec_size = key_vec_.size();
        key_vec_.push_back(key);
        key_map_[key_name] = vec_size;
        return 0;
    }

    void delete_key(const std::string& key_name) {
        int key_id = has_key(key_name);
        if (key_id < 0) {
            return;
        }
        auto it_vec = key_vec_.begin() + key_id;
        key_vec_.erase(it_vec);
        auto it_map = key_map_.find(key_name);
        if (it_map != key_map_.end()) {
            key_map_.erase(it_map);
        }
    }

    std::shared_ptr<Key> key(const std::string& key_name) {
        int key_id = has_key(key_name);
        if (key_id < 0) {
            return std::shared_ptr<Key>();
        }
        return key_vec_[key_id];
    }


    void set_section_name(const std::string& section_name) {
        section_name_ = section_name;
    }

    void set_comment(const std::string& comment) {
        comment_ = comment;
    }

    void set_right_comment(const std::string& right_comment) {
        right_comment_ = right_comment;
    }

    std::string section_name() {
        return section_name_;
    }

    std::string comment() {
        return comment_;
    }

    std::string right_comment() {
        return right_comment_;
    }

    std::vector<std::shared_ptr<Key> >& key_vec() {
        return key_vec_;
    }

private:
    std::string section_name_;
    std::string comment_;
    std::string right_comment_;
    std::vector<std::shared_ptr<Key> > key_vec_;
    std::map<std::string, int> key_map_;
};


class Root
{
public:

    int load(const std::string& file_name) {

        section_vec_.clear();
        section_map_.clear();
        std::ifstream ifs(file_name);
        if (ifs.fail()) {
            LOG(WARNING) << "Failed to load ini file: " << file_name << "!";
            return -1;
        }

        std::string line;
        std::string comment; 
        std::string section_name;
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
                comment += line;
                comment += '\n';
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
                std::string right_comment = trim(line.substr(pos + 1));

                std::shared_ptr<Section> section = std::make_shared<Section>(section_name, comment, right_comment);

                insert_section(section_name, section);
       
                comment.clear();

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
                std::string sub_str = line.substr(pos + 1);

                pos = sub_str.find_first_of('#');
                std::string value_str = sub_str.substr(0, pos - 1);
                value_str = trim(value_str);
                std::string right_comment = sub_str.substr(pos);
                // std::cout << "value_str=" << value_str << std::endl;
                // std::cout << "right_comment=" << right_comment << std::endl;

                std::shared_ptr<Key> key = std::make_shared<Key>(key_name, comment, right_comment);
                key->value()->set(value_str.c_str());

                std::shared_ptr<Section> section = get_section(section_name);
                section->insert_key(key_name, key);
                
                comment.clear();
            }
        }

        return 0;
    }


    int save(const std::string& file_name) {

        std::ofstream ofs(file_name.c_str());
        if (ofs.fail())
        {
            LOG(WARNING) << "Failed to save ini file: " << file_name << "!";
            return -1;
        }
        ofs << str();
        ofs.close();

        return 0;
    }

    std::string str() {
        std::stringstream ss;
        for (size_t i = 0; i < section_vec_.size(); i++)
        {
            std::shared_ptr<Section> section = section_vec_[i];
      
            ss << section->comment();
            ss << "[" << section->section_name() << "]" << section->right_comment() << std::endl;
            auto& key_vec = section->key_vec();
            for (size_t i = 0; i < key_vec.size(); i++) {
                std::shared_ptr<Key> key = key_vec[i];

                ss << key->comment();
                ss << key->key_name() << " = " << key->value()->str() << key->right_comment() << std::endl;
            }
            ss << std::endl;
        }

        return ss.str();
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



    int has_section(const std::string& section_name) {
        auto it = section_map_.find(section_name);
        if (it == section_map_.end()) {
            return -1;
        }
        return it->second;
    }

    int insert_section(const std::string& section_name, std::shared_ptr<Section> section) {
        int section_id = has_section(section_name);
        if (section_id >= 0) {
            LOG(WARNING) << "already have [" << section_name << "] section, can't insert same section !";
            return -1;
        }

        int vec_size = section_vec_.size();
        section_vec_.push_back(section);
        section_map_[section_name] = vec_size;
        return 0;
    }

    void delete_section(const std::string& section_name) {
        int section_id = has_section(section_name);
        if (section_id < 0) {
            return;
        }
        auto it_vec = section_vec_.begin() + section_id;
        section_vec_.erase(it_vec);
        auto it_map = section_map_.find(section_name);
        if (it_map != section_map_.end()) {
            section_map_.erase(it_map);
        }
    }

    std::shared_ptr<Section> get_section(const std::string& section_name) {
        int section_id = has_section(section_name);
        if (section_id < 0) {
            return std::shared_ptr<Section>();
        }
        return section_vec_[section_id];
    }

    template<typename T>
    T get(const std::string& section_name, const std::string& key_name, T default_value) {

        std::shared_ptr<Section> section = get_section(section_name);
        if (!section) {
            LOG(WARNING) << "ini don't have section: [" << section_name << "] !";
            return default_value;
        }

        std::shared_ptr<Key> key = section->key(key_name);
        if (!key) {
            LOG(WARNING) << "ini file don't have key: " << section_name << "." << key_name << "!";
            return default_value;
        } 

        std::shared_ptr<Value> value = key->value();
        return value->get<T>();
    }

    template<typename T>
    void set(const std::string& section_name, const std::string& key_name, T data) {
        int ret;
        std::shared_ptr<Section> section = get_section(section_name);
        if (!section) {
            section = std::make_shared<Section>(section_name);
            ret = insert_section(section_name, section);
            CHECK(ret == 0) << "logic error !"; 
        }

        std::shared_ptr<Key> key = section->key(key_name); 
        if (!key) {
            key = std::make_shared<Key>(key_name);
            ret = section->insert_key(key_name, key);
            CHECK(ret == 0) << "logic error !"; 
        }

        std::shared_ptr<Value> value = key->value();
        value->set<T>(data);

    }

    

private: 
    std::vector<std::shared_ptr<Section> > section_vec_;
    std::map<std::string, int> section_map_;
};




}//namespace ini
}//namespace duck










