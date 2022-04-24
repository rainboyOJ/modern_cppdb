#pragma once

#include <array>
#include <string>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <vector>

#include "pool_manager.hpp"
#include "cexpr/string.hpp"
#include "sql/schema.hpp"

#include "errors.hpp"
#include "utils.hpp"



namespace cppdb
{

struct __exec {};
const constexpr auto exec = __exec();


template<std::size_t... As,std::size_t... Bs>
constexpr auto operator+(std::index_sequence<As...>,std::index_sequence<Bs...>) 
    -> std::index_sequence<As...,Bs...>
{ return {}; }

template<cexpr::string str>
struct GET_STR_QUESTION_MARK_SEQ {
    static constexpr auto S {str};
    static constexpr std::size_t size__ = S.size();

    static constexpr auto get()
    {
        return  filter(std::make_index_sequence<size__>{});
    }

    template<std::size_t... Is>
    static constexpr auto filter(std::index_sequence<Is...>){
        return (filter_single<Is>()  + ...);
    }

    template <std::size_t Val>
    static constexpr auto filter_single() {
        if constexpr ( S.get(Val) == '?')
            return std::index_sequence<Val> {};
        else
            return std::index_sequence<> {};
    }
    using index_seq = decltype(get());

};

template<typename T>
static constexpr std::size_t get_mark_size(T & s){
    int cnt{0};
    auto b = s.cbegin();
    auto e = s.cend();
    for( ; b!= e; b++)
        if(*b == '?') cnt++;
    return cnt;
}
/**
 * schema 的类型有三种
 * int,std::string,单个类型
 * row类型
 * schema
 */
template <cexpr::string Str, typename Schema>
class query {
public:
    using binders_seq = typename GET_STR_QUESTION_MARK_SEQ<Str>::index_seq;

    static constexpr auto query_str_ {Str};
    static constexpr std::size_t mark_size_ {get_mark_size(Str)};

    constexpr query()
    {
    }

    constexpr auto params_size() const { return mark_size_; }

    template<typename Input>
    query& operator<<(Input && in) {
        bind(std::forward<Input>(in));
        return *this;
    }

    void reset(){
        cols = 0;
        for (auto& e : params_) e.clear();
    }

    auto operator<<(__exec const &) {
        //TODO 执行 得到 backend::result 继续填充schema
        return this->exec();
    }

    std::string & at(int col) {
        if(col < mark_size_)
            return params_[col];
        else
            throw cppdb_error( "overload cols size :" + std::to_string(mark_size_));
    }

    template<typename Input>
    void bind(Input&& in) {
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

    auto exec() {
        if( cols != mark_size_)
            throw cppdb_error("must insert full value_ use << ");

        return "123";
    }


private:

    //mutable std::vector<std::string> params_;
    //std::array<std::string, mark_size_> params_;
    std::string params_[mark_size_];
    int cols{0}; //当前存的数据的行数

};





} // namespace cppdb
