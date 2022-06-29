#include <iostream>
#include "utils.hpp"
#include "connection_info.hpp"
#include "sql/query.hpp"

int main(){

    std::string connection_info_ ="mysql:host=127.0.0.1;port=3306;user=root;password=root;database=girls;";
    cppdb::pool_manager::init(connection_info_);

    //mysql_query(con, "INSERT INTO boys(boyName,userCP) VALUES('Audi',300)");
    //insert
    cppdb::query<
        "insert into boys(boyName,userCP) VALUES('?',?);"
        , void> q;

    q << "audi2" << 300 << cppdb::exec;
    //std::cout << "res1 :" << res1 << std::endl;
    //query
    //del
    //update

    return 0;
}
