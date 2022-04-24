#pragma once

#include <memory>

#include "backend/mysql_backend.hpp"

namespace cppdb {
    

class connection :public std::enable_shared_from_this<connection> {
public:
    connection(connection_info const & ci_) 
        :last_used{std::chrono::system_clock::now()}
    {
        conn_ = std::make_shared<backend::connection>(ci_);
    }

    connection(std::string const & str)
        :connection(connection_info(str))
    {}

    ~connection(){};

    std::shared_ptr<backend::connection> conn(){
        return conn_;
    }

    std::shared_ptr<backend::result> exec(std::string_view s){
        conn_->exec(s);
        return std::make_shared<backend::result>(conn_.get());
    }

    inline auto get_last_used() const{ return  last_used; }

private:
    std::chrono::time_point<std::chrono::system_clock> last_used;
    std::shared_ptr<backend::connection> conn_ = nullptr;
    //std::shared_ptr<backend::result> result_   = nullptr;
};

} // end namespace cppdb
