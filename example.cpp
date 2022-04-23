#include <iostream>
#include "utils.hpp"
#include "connection_info.hpp"

int main(){
    cppdb::connection_info csi("mysql:username=test;password=pass;port=123;");
    std::cout << csi.driver << std::endl;
    std::cout << csi.has("username") << std::endl;
    std::cout << csi.get("username") << std::endl;
    std::cout << csi.get("not_found","default_value") << std::endl;

    return 0;
}
