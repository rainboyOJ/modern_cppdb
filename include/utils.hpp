#pragma once
#include <string>
#include <ctime>
#include <sstream>
#include <map>
#include <cstring>
#include <mutex>
#include <chrono>
#include <cxxabi.h>

#include "errors.hpp"

#define GET_TYPE_NAME(type) abi::__cxa_demangle(typeid(type).name(),0,0,0)

namespace cppdb {

    using TIME_Pt_str = std::string;
    using TIME_Pt = std::chrono::time_point<std::chrono::system_clock>;

    /// \brief parse a string as time value.
    std::tm parse_time(char const *value);
    std::tm parse_time(std::string const &v);

    /// \brief format a string as time value.
    std::string format_time(std::tm const &v);

    std::string format_time(
            const TIME_Pt & _time
            ) {
        auto in_time_t = std::chrono::system_clock::to_time_t(_time);
        return format_time(*std::localtime(&in_time_t));
    }

    inline auto now() {
        return std::chrono::system_clock::now();
    }


    //@desc 解析连接型的字符串
    /// \verbatim  driver:[key=value;]*  \endverbatim 
    // \@ 特殊的key
    // \verbatim   mysql:username= root;password = 'asdf''5764dg';database=test;@use_prepared=off' \endverbatim 
    void parse_connection_string(    std::string const &cs,
                        std::string &driver_name,
                        std::map<std::string,std::string> &props);

    std::string format_time(std::tm const &v)
    {
        char buf[64]= {0};
        strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",&v);
        return std::string(buf);
    }

    std::tm parse_time(std::string const &v)
    {
        if(strlen(v.c_str())!=v.size())
            throw bad_value_cast(__FILE__,__LINE__);
        return parse_time(v.c_str());
    }
    std::tm parse_time(char const *v)
    {
        std::tm t=std::tm();
        int n;
        double sec = 0;
        n = sscanf(v,"%d-%d-%d %d:%d:%lf",
            &t.tm_year,&t.tm_mon,&t.tm_mday,
            &t.tm_hour,&t.tm_min,&sec);
        if(n!=3 && n!=6) 
        {
            std::cout << v << std::endl;
            throw bad_value_cast(__FILE__,__LINE__);
        }
        t.tm_year-=1900;
        t.tm_mon-=1;
        t.tm_isdst = -1;
        t.tm_sec=static_cast<int>(sec);
        if(mktime(&t)==-1)
            throw bad_value_cast(__FILE__,__LINE__);
        return t;
    }

    namespace {
        bool is_blank_char(char c)
        {
            return c==' ' || c=='\t' || c=='\r' || c=='\n' || c=='\f';
        }
        std::string trim(std::string const &s)
        {
            if(s.empty())
                return s;
            size_t start=0,end=s.size()-1;
            while(start < s.size() && is_blank_char(s[start])) {
                start++;
            }
            while(end > start && is_blank_char(s[end])) {
                end--;
            }
            return s.substr(start,end-start+1);
        }
    }


    void parse_connection_string(std::string const &connection_string,
            std::string &driver,
            std::map<std::string,std::string> &params)
    {
        params.clear();
        size_t p = connection_string.find(':');
        if( p == std::string::npos )
            throw cppdb_error("cppdb::Invalid connection string - no driver given");
        driver = connection_string.substr(0,p);
        p++;
        while(p<connection_string.size()) {
            size_t n=connection_string.find('=',p);
            if(n==std::string::npos)
                throw cppdb_error("Invalid connection string - invalid property");
            std::string key = trim(connection_string.substr(p,n-p));
            p=n+1;
            std::string value;
            while(p<connection_string.size() && is_blank_char(connection_string[p]))
            {
                ++p;
            }
            if(p>=connection_string.size()) {
                /// Nothing - empty property
            }
            else if(connection_string[p]=='\'') {
                p++;
                while(true) {
                    if(p>=connection_string.size()) {
                        throw cppdb_error("Invalid connection string unterminated string");
                    }
                    if(connection_string[p]=='\'') {
                        if(p+1 < connection_string.size() && connection_string[p+1]=='\'') {
                            value+='\'';
                            p+=2;
                        }
                        else {
                            p++;
                            break;
                        }
                    }
                    else {
                        value+=connection_string[p];
                        p++;
                    }
                }
            }
            else {
                size_t n=connection_string.find(';',p);
                if(n==std::string::npos) {
                    value=trim(connection_string.substr(p));
                    p=connection_string.size();
                }
                else {
                    value=trim(connection_string.substr(p,n-p));
                    p=n;
                }
            }
            if(params.find(key)!=params.end()) {
                throw cppdb_error("cppdb::invalid connection string duplicate key");
            }
            params[key]=value;
            while(p<connection_string.size()) {
                char c=connection_string[p];
                if(is_blank_char(c))
                    ++p;
                else if(c==';') {
                    ++p;
                    break;
                }
            }
        }
    } //


struct LOG {
public:
    template<typename... T>
    static void log(T&&... args){
        std::lock_guard<std::mutex> lck(mtx_);
        ( (std::cout << args << " "),...) << '\n';
    }
private:
    static std::mutex mtx_;
};

std::mutex LOG::mtx_;

#ifdef MODERN_CPPDB_DEBUG
    #define cppdb_log(...) LOG::log(__FILE__,__LINE__,__VA_ARGS__)
#else
    #define cppdb_log(...)
#endif


} // end namespace cppdb

