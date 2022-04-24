//@desc gate For db Conecion
//

#pragma once
#include "pool.hpp"

namespace cppdb {


class pool_manager {

public:
    static auto get() {
        static pool_manager _pm;
        return _pm.conn_();
    }

    void init(std::string const & conn_str){
        pool_ = pool::create(conn_str);
    }

private:

    std::unique_ptr<pool::connection_raii> conn_() {
        if( pool_ != nullptr)
            return pool_->open();
        else throw "should init pool_manager with init()";
    }

    pool_manager() = default;
    pool_manager(const pool_manager &)    = delete;
    void operator=(const pool_manager &)  = delete;
    void operator=(const pool_manager &&) = delete;

    std::shared_ptr<pool> pool_ = nullptr; 

};

    
} // end namespace cppdb
