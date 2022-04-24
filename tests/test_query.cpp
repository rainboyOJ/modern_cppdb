#include <iostream>
#include "sql/query.hpp"

int main(){
    using myschema = cppdb::schema<"myschema", 
          cppdb::column<"int", int>
          >;
    cppdb::query<"select 1+?", myschema> q;

    std::cout << q.mark_size_ << std::endl;

    std::cout << q.at(0) << std::endl;
    q << 123;
    std::cout << q.at(0) << std::endl;

    return 0;
}
