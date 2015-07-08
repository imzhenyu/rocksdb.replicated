# pragma once
# include "rrdb.client.h"
# include "rrdb.server.h"

namespace dsn { namespace apps { 
// client app example
class rrdb_client_app : public ::dsn::service::service_app, public virtual ::dsn::service::servicelet
{
public:
	rrdb_client_app(::dsn::service_app_spec* s) 
		: ::dsn::service::service_app(s) 
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

		std::vector<::dsn::end_point> meta_servers;
		auto cf = ::dsn::service::system::config();
		::dsn::replication::replication_app_client_base::load_meta_servers(cf, meta_servers);
		
		_rrdb_client = new rrdb_client(meta_servers, argv[1]);
		_timer = ::dsn::service::tasking::enqueue(LPC_RRDB_TEST_TIMER, this, &rrdb_client_app::on_test_timer, 0, 0, 1000);
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
			//update_request req;
			//sync:
			//int resp;
			//auto err = _rrdb_client->merge(req, resp);
			//std::cout << "call RPC_RRDB_RRDB_MERGE end, return " << err.to_string() << std::endl;
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
	::dsn::end_point _server;
	
	rrdb_client *_rrdb_client;
};

} } 
