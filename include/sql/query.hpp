#pragma once

#include <array>
#include <string>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <vector>

#include "cexpr/string.hpp"

#include "pool_manager.hpp"

//#include "sql/column.hpp"
//#include "sql/tokens.hpp"
//#include "sql/predicate.hpp"
//#include "sql/row.hpp"

namespace cppdb
{

struct __exec {};
const constexpr auto exec = __exec();

/**
 * schema 的类型有三种
 * int,std::string,单个类型
 * row类型
 * schema
 */
template <cexpr::string Str, typename Schema>
class query {
public:
    
    constexpr query(std::string_view qstr)
        :query_str_{qstr},mark_size_{get_mark_size(qstr)}
    {
        params_.reserve(mark_size_);
        binders_.reserve(mark_size_);
    }

    constexpr auto params_size() const { return mark_size_; }

    template<typename Input>
    query& operator<<(Input && in){
        bind(std::forward<Input>(in));
        return this;
    }

    void reset(){
        cols = 0;
        params_.clear();
        params_.resize(mark_size_);
    }

    auto operator<<(__exec const &) const{
        return this->exec();
    }

    std::string & at(int col) const{
        if(col < mark_size_)
            return params_[col];
        else
            throw invalid_column( "overload cols size :" + std::to_string(mark_size_));
    }

    template<typename Input>
    void bind(Input&& in) const{
        using TYPE = typename std::remove_const_t< std::remove_cv_t<Input> >;
        if constexpr ( std::is_fundamental_v<TYPE> && (! std::is_same_v<TYPE, char *>)  ) // # 基础的类型
        {
            at(cols) = std::to_string(in);
            ++cols;
            return;
        }
        
        if constexpr ( std::is_same_v<TYPE,TIME_Pt >)   // # 时间
        {
            std::string &s = at(cols);
            s.clear();
            s.reserve(30);
            s+='\'';
            s+=cppdb::format_time(in);
            s+='\'';
            ++cols;
            return;
        }

        //if constexpr (std::is_same_v<TYPE, char *>) {
            //std::vector<char> buf(2*(e-b)+1);
            //size_t len = mysql_real_escape_string(conn_,&buf.front(),b,e-b);
            //std::string &s=at(col);
            //s.clear();
            //s.reserve(e-b+2);
            //s+='\'';
            //s.append(&buf.front(),len);
            //s+='\'';
            //return;
        //}
        //if constexpr (std::is_same_v<TYPE, std::string> || std::is_same_v<TYPE, std::string_view>) {
            //return;
        //}


        throw std::invalid_argument(std::string("Do not supporte type!") + typeid(Input).name());
    }

    auto exec() const{
        return "123";
    }

private:
    constexpr int get_mark_size(std::string_view s){
        int cnt{0};
        for (const auto& e : s) {
            if(e == '?' ) cnt++;
        }
        return cnt;
    }

    mutable std::vector<std::string> params_;
    mutable std::vector<std::size_t> binders_;
    mutable int cols{0};
    const   int mark_size_;
    std::string_view query_str_;

};





} // namespace cppdb
