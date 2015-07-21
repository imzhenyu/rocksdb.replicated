# pragma once
# include <dsn/dist/replication.h>
# include "rrdb.code.definition.h"
# include <iostream>

namespace dsn { namespace apps { 
class rrdb_service 
    : public ::dsn::replication::replication_app_base
{
public:
    rrdb_service(::dsn::replication::replica* replica, ::dsn::configuration_ptr& config) 
        : ::dsn::replication::replication_app_base(replica, config)
    {
        open_service();
    }
    
    virtual ~rrdb_service() 
    {
        close_service();
    }

protected:
    // all service handlers to be implemented further
    // RPC_RRDB_RRDB_PUT 
    virtual void on_put(const update_request& update, ::dsn::service::rpc_replier<int>& reply)
    {
        std::cout << "... exec RPC_RRDB_RRDB_PUT ... (not implemented) " << std::endl;
        int resp;
        reply(resp);
    }
    // RPC_RRDB_RRDB_REMOVE 
    virtual void on_remove(const ::dsn::blob& key, ::dsn::service::rpc_replier<int>& reply)
    {
        std::cout << "... exec RPC_RRDB_RRDB_REMOVE ... (not implemented) " << std::endl;
        int resp;
        reply(resp);
    }
    // RPC_RRDB_RRDB_MERGE 
    virtual void on_merge(const update_request& update, ::dsn::service::rpc_replier<int>& reply)
    {
        std::cout << "... exec RPC_RRDB_RRDB_MERGE ... (not implemented) " << std::endl;
        int resp;
        reply(resp);
    }
    // RPC_RRDB_RRDB_GET 
    virtual void on_get(const ::dsn::blob& key, ::dsn::service::rpc_replier<read_response>& reply)
    {
        std::cout << "... exec RPC_RRDB_RRDB_GET ... (not implemented) " << std::endl;
        read_response resp;
        reply(resp);
    }
    
public:
    void open_service()
    {
        this->register_async_rpc_handler(RPC_RRDB_RRDB_PUT, "put", &rrdb_service::on_put);
        this->register_async_rpc_handler(RPC_RRDB_RRDB_REMOVE, "remove", &rrdb_service::on_remove);
        this->register_async_rpc_handler(RPC_RRDB_RRDB_MERGE, "merge", &rrdb_service::on_merge);
        this->register_async_rpc_handler(RPC_RRDB_RRDB_GET, "get", &rrdb_service::on_get);
    }

    void close_service()
    {
        this->unregister_rpc_handler(RPC_RRDB_RRDB_PUT);
        this->unregister_rpc_handler(RPC_RRDB_RRDB_REMOVE);
        this->unregister_rpc_handler(RPC_RRDB_RRDB_MERGE);
        this->unregister_rpc_handler(RPC_RRDB_RRDB_GET);
    }
};

} } 