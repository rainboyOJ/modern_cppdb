/**
 * 连接
 */
#include <iostream>
//#include "pool_manager.hpp"
#include "backend/mysql_backend.hpp"
//#include "backend/result_backend.hpp"

using namespace cppdb;

int main(){

    std::string connection_info_ ="mysql:host=127.0.0.1;port=3306;user=root;password=root;database=rojcpp;";
    // 创建pool
    //cppdb::pool_manager::get().create("mysql:host=127.0.0.1;port=3306;user=root;password=root;database=rojcpp;");

    //auto version = 
        //cppdb::pool_manager::get().conn_()->server_version();
    //std::cout << version << std::endl;
    //

    backend::connection myconn(connection_info_);
    //backend::Connection conn(connection_info);
    //conn.driver();
    //std::cout << GET_TYPE_NAME(backend::Connection) << std::endl;

    auto ver = myconn.server_version();
    std::cout << "server_version : " << ver << std::endl;



    return 0;
}
