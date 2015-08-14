# pragma once
# include "rrdb.client.h"
# include "rrdb.client.perf.h"
# include "rrdb.server.h"

namespace dsn { namespace apps { 
// client app example
class rrdb_client_app : 
    public ::dsn::service_app,
    public virtual ::dsn::servicelet
{
public:
    rrdb_client_app()
    {
        _rrdb_client = nullptr;
    }
    
    ~rrdb_client_app() 
    {
        stop();
    }

    virtual ::dsn::error_code start(int argc, char** argv)
    {
        if (argc < 2)
            return ::dsn::ERR_INVALID_PARAMETERS;

        std::vector<dsn_address_t> meta_servers;
        ::dsn::replication::replication_app_client_base::load_meta_servers(meta_servers);
        
        _rrdb_client = new rrdb_client(meta_servers, argv[1]);
        _timer = ::dsn::tasking::enqueue(LPC_RRDB_TEST_TIMER, this, &rrdb_client_app::on_test_timer, 0, 0, 100);
        return ::dsn::ERR_OK;
    }

    virtual void stop(bool cleanup = false)
    {
        _timer->cancel(true);
 
        if (_rrdb_client != nullptr)
        {
            delete _rrdb_client;
            _rrdb_client = nullptr;
        }
    }

    void on_test_timer()
    {
        // test for service 'rrdb'
        {
            update_request req;
            //sync:
            int resp;
            auto err = _rrdb_client->put(req, resp);
            std::cout << "call RPC_RRDB_RRDB_PUT end, return " << err.to_string() << std::endl;
            //async: 
            //_rrdb_client->begin_put(req);
           
        }
        {
            ::dsn::blob req;
            //sync:
            int resp;
            auto err = _rrdb_client->remove(req, resp);
            std::cout << "call RPC_RRDB_RRDB_REMOVE end, return " << err.to_string() << std::endl;
            //async: 
            //_rrdb_client->begin_remove(req);
           
        }
        {
            update_request req;
            //sync:
            int resp;
            auto err = _rrdb_client->merge(req, resp);
            std::cout << "call RPC_RRDB_RRDB_MERGE end, return " << err.to_string() << std::endl;
            //async: 
            //_rrdb_client->begin_merge(req);
           
        }
        {
            ::dsn::blob req;
            //sync:
            read_response resp;
            auto err = _rrdb_client->get(req, resp);
            std::cout << "call RPC_RRDB_RRDB_GET end, return " << err.to_string() << std::endl;
            //async: 
            //_rrdb_client->begin_get(req);
           
        }
    }

private:
    ::dsn::task_ptr _timer;
    dsn_address_t _server;
    
    rrdb_client *_rrdb_client;
};

class rrdb_perf_test_client_app : 
    public ::dsn::service_app,
    public virtual ::dsn::servicelet
{
public:
    rrdb_perf_test_client_app()
    {
        _rrdb_client = nullptr;
    }

    ~rrdb_perf_test_client_app()
    {
        stop();
    }

    virtual ::dsn::error_code start(int argc, char** argv)
    {
        if (argc < 2)
            return ::dsn::ERR_INVALID_PARAMETERS;

        std::vector<dsn_address_t> meta_servers;
        ::dsn::replication::replication_app_client_base::load_meta_servers(meta_servers);

        _rrdb_client = new rrdb_perf_test_client(meta_servers, argv[1]);
        _rrdb_client->start_test();
        return ::dsn::ERR_OK;
    }

    virtual void stop(bool cleanup = false)
    {
        if (_rrdb_client != nullptr)
        {
            delete _rrdb_client;
            _rrdb_client = nullptr;
        }
    }
    
private:
    rrdb_perf_test_client *_rrdb_client;
};

} } 
