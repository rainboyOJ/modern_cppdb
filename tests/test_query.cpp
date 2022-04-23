#include <string_view>
#include "sql/query.hpp"
#include <iostream>


int main(){
    using namespace std::literals;
    const cppdb::query<int> q(
            "selet a from ?,(?),?"
            "selet a from ?,(?),?"
            "selet a from ?,(?),?"
            "a from ?,(?),?"sv);
    std::cout << q.params_size() << std::endl;
    auto t = q << cppdb::exec;
    std::cout << t << std::endl;
    return 0;
}
