# pragma once
# include <dsn/dist/replication.h>
# include "rrdb.code.definition.h"
# include <iostream>

namespace dsn { namespace apps { 
class rrdb_client 
	: public ::dsn::replication::replication_app_client_base
{
public:
	rrdb_client(
		const std::vector<end_point>& meta_servers,
        const char* app_name)
		: ::dsn::replication::replication_app_client_base(meta_servers, app_name) 
	{
	}
	
	virtual ~rrdb_client() {}
	
	// from requests to partition index
	// PLEASE DO RE-DEFINE THEM IN A SUB CLASS!!!
	virtual int get_partition_index(const update_request& key) { return 0;};
	virtual int get_partition_index(const ::dsn::blob& key) { return 0;};

	// ---------- call RPC_RRDB_RRDB_PUT ------------
	// - synchronous 
	::dsn::error_code put(
		const update_request& update, 
		__out_param int& resp, 
		int timeout_milliseconds = 0, 
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		auto resp_task = ::dsn::replication::replication_app_client_base::read<update_request, int>(
            get_partition_index(update),
            RPC_RRDB_RRDB_PUT,
            update,
            nullptr,
            nullptr,
			nullptr,
            timeout_milliseconds,
			0, 
			read_semantic 
            );
		resp_task->wait();
		if (resp_task->error() == ::dsn::ERR_SUCCESS)
		{
			unmarshall(resp_task->get_response()->reader(), resp);
		}
		return resp_task->error();
	}
	
	// - asynchronous with on-stack update_request and int 
	::dsn::rpc_response_task_ptr begin_put(
		const update_request& update,
		void* context = nullptr,
		int timeout_milliseconds = 0, 
		int reply_hash = 0,  
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		return ::dsn::replication::replication_app_client_base::read<rrdb_client, update_request, int>(
            get_partition_index(update),
            RPC_RRDB_RRDB_PUT, 
            update,
            this,
            &rrdb_client::end_put, 
			context,
            timeout_milliseconds,
			reply_hash, 
			read_semantic 
            );
	}

	virtual void end_put(
		::dsn::error_code err, 
		const int& resp,
		void* context)
	{
		if (err != ::dsn::ERR_SUCCESS) std::cout << "reply RPC_RRDB_RRDB_PUT err : " << err.to_string() << std::endl;
		else
		{
			std::cout << "reply RPC_RRDB_RRDB_PUT ok" << std::endl;
		}
	}
	
	// - asynchronous with on-heap std::shared_ptr<update_request> and std::shared_ptr<int> 
	::dsn::rpc_response_task_ptr begin_put2(
		std::shared_ptr<update_request>& update, 		
		int timeout_milliseconds = 0, 
		int reply_hash = 0, 
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		return ::dsn::replication::replication_app_client_base::read<rrdb_client, update_request, int>(
            get_partition_index(*update),
            RPC_RRDB_RRDB_PUT,
            update,
            this,
            &rrdb_client::end_put2, 
            timeout_milliseconds,
			reply_hash, 
			read_semantic 
            );
	}

	virtual void end_put2(
		::dsn::error_code err, 
		std::shared_ptr<update_request>& update, 
		std::shared_ptr<int>& resp)
	{
		if (err != ::dsn::ERR_SUCCESS) std::cout << "reply RPC_RRDB_RRDB_PUT err : " << err.to_string() << std::endl;
		else
		{
			std::cout << "reply RPC_RRDB_RRDB_PUT ok" << std::endl;
		}
	}
	

	// ---------- call RPC_RRDB_RRDB_REMOVE ------------
	// - synchronous 
	::dsn::error_code remove(
		const ::dsn::blob& key, 
		__out_param int& resp, 
		int timeout_milliseconds = 0, 
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		auto resp_task = ::dsn::replication::replication_app_client_base::read<::dsn::blob, int>(
            get_partition_index(key),
            RPC_RRDB_RRDB_REMOVE,
            key,
            nullptr,
            nullptr,
			nullptr,
            timeout_milliseconds,
			0, 
			read_semantic 
            );
		resp_task->wait();
		if (resp_task->error() == ::dsn::ERR_SUCCESS)
		{
			unmarshall(resp_task->get_response()->reader(), resp);
		}
		return resp_task->error();
	}
	
	// - asynchronous with on-stack ::dsn::blob and int 
	::dsn::rpc_response_task_ptr begin_remove(
		const ::dsn::blob& key,
		void* context = nullptr,
		int timeout_milliseconds = 0, 
		int reply_hash = 0,  
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		return ::dsn::replication::replication_app_client_base::read<rrdb_client, ::dsn::blob, int>(
            get_partition_index(key),
            RPC_RRDB_RRDB_REMOVE, 
            key,
            this,
            &rrdb_client::end_remove, 
			context,
            timeout_milliseconds,
			reply_hash, 
			read_semantic 
            );
	}

	virtual void end_remove(
		::dsn::error_code err, 
		const int& resp,
		void* context)
	{
		if (err != ::dsn::ERR_SUCCESS) std::cout << "reply RPC_RRDB_RRDB_REMOVE err : " << err.to_string() << std::endl;
		else
		{
			std::cout << "reply RPC_RRDB_RRDB_REMOVE ok" << std::endl;
		}
	}
	
	// - asynchronous with on-heap std::shared_ptr<::dsn::blob> and std::shared_ptr<int> 
	::dsn::rpc_response_task_ptr begin_remove2(
		std::shared_ptr<::dsn::blob>& key, 		
		int timeout_milliseconds = 0, 
		int reply_hash = 0, 
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		return ::dsn::replication::replication_app_client_base::read<rrdb_client, ::dsn::blob, int>(
            get_partition_index(*key),
            RPC_RRDB_RRDB_REMOVE,
            key,
            this,
            &rrdb_client::end_remove2, 
            timeout_milliseconds,
			reply_hash, 
			read_semantic 
            );
	}

	virtual void end_remove2(
		::dsn::error_code err, 
		std::shared_ptr<::dsn::blob>& key, 
		std::shared_ptr<int>& resp)
	{
		if (err != ::dsn::ERR_SUCCESS) std::cout << "reply RPC_RRDB_RRDB_REMOVE err : " << err.to_string() << std::endl;
		else
		{
			std::cout << "reply RPC_RRDB_RRDB_REMOVE ok" << std::endl;
		}
	}
	

	// ---------- call RPC_RRDB_RRDB_MERGE ------------
	// - synchronous 
	::dsn::error_code merge(
		const update_request& update, 
		__out_param int& resp, 
		int timeout_milliseconds = 0, 
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		auto resp_task = ::dsn::replication::replication_app_client_base::read<update_request, int>(
            get_partition_index(update),
            RPC_RRDB_RRDB_MERGE,
            update,
            nullptr,
            nullptr,
			nullptr,
            timeout_milliseconds,
			0, 
			read_semantic 
            );
		resp_task->wait();
		if (resp_task->error() == ::dsn::ERR_SUCCESS)
		{
			unmarshall(resp_task->get_response()->reader(), resp);
		}
		return resp_task->error();
	}
	
	// - asynchronous with on-stack update_request and int 
	::dsn::rpc_response_task_ptr begin_merge(
		const update_request& update,
		void* context = nullptr,
		int timeout_milliseconds = 0, 
		int reply_hash = 0,  
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		return ::dsn::replication::replication_app_client_base::read<rrdb_client, update_request, int>(
            get_partition_index(update),
            RPC_RRDB_RRDB_MERGE, 
            update,
            this,
            &rrdb_client::end_merge, 
			context,
            timeout_milliseconds,
			reply_hash, 
			read_semantic 
            );
	}

	virtual void end_merge(
		::dsn::error_code err, 
		const int& resp,
		void* context)
	{
		if (err != ::dsn::ERR_SUCCESS) std::cout << "reply RPC_RRDB_RRDB_MERGE err : " << err.to_string() << std::endl;
		else
		{
			std::cout << "reply RPC_RRDB_RRDB_MERGE ok" << std::endl;
		}
	}
	
	// - asynchronous with on-heap std::shared_ptr<update_request> and std::shared_ptr<int> 
	::dsn::rpc_response_task_ptr begin_merge2(
		std::shared_ptr<update_request>& update, 		
		int timeout_milliseconds = 0, 
		int reply_hash = 0, 
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		return ::dsn::replication::replication_app_client_base::read<rrdb_client, update_request, int>(
            get_partition_index(*update),
            RPC_RRDB_RRDB_MERGE,
            update,
            this,
            &rrdb_client::end_merge2, 
            timeout_milliseconds,
			reply_hash, 
			read_semantic 
            );
	}

	virtual void end_merge2(
		::dsn::error_code err, 
		std::shared_ptr<update_request>& update, 
		std::shared_ptr<int>& resp)
	{
		if (err != ::dsn::ERR_SUCCESS) std::cout << "reply RPC_RRDB_RRDB_MERGE err : " << err.to_string() << std::endl;
		else
		{
			std::cout << "reply RPC_RRDB_RRDB_MERGE ok" << std::endl;
		}
	}
	

	// ---------- call RPC_RRDB_RRDB_GET ------------
	// - synchronous 
	::dsn::error_code get(
		const ::dsn::blob& key, 
		__out_param read_response& resp, 
		int timeout_milliseconds = 0, 
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		auto resp_task = ::dsn::replication::replication_app_client_base::read<::dsn::blob, read_response>(
            get_partition_index(key),
            RPC_RRDB_RRDB_GET,
            key,
            nullptr,
            nullptr,
			nullptr,
            timeout_milliseconds,
			0, 
			read_semantic 
            );
		resp_task->wait();
		if (resp_task->error() == ::dsn::ERR_SUCCESS)
		{
			unmarshall(resp_task->get_response()->reader(), resp);
		}
		return resp_task->error();
	}
	
	// - asynchronous with on-stack ::dsn::blob and read_response 
	::dsn::rpc_response_task_ptr begin_get(
		const ::dsn::blob& key,
		void* context = nullptr,
		int timeout_milliseconds = 0, 
		int reply_hash = 0,  
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		return ::dsn::replication::replication_app_client_base::read<rrdb_client, ::dsn::blob, read_response>(
            get_partition_index(key),
            RPC_RRDB_RRDB_GET, 
            key,
            this,
            &rrdb_client::end_get, 
			context,
            timeout_milliseconds,
			reply_hash, 
			read_semantic 
            );
	}

	virtual void end_get(
		::dsn::error_code err, 
		const read_response& resp,
		void* context)
	{
		if (err != ::dsn::ERR_SUCCESS) std::cout << "reply RPC_RRDB_RRDB_GET err : " << err.to_string() << std::endl;
		else
		{
			std::cout << "reply RPC_RRDB_RRDB_GET ok" << std::endl;
		}
	}
	
	// - asynchronous with on-heap std::shared_ptr<::dsn::blob> and std::shared_ptr<read_response> 
	::dsn::rpc_response_task_ptr begin_get2(
		std::shared_ptr<::dsn::blob>& key, 		
		int timeout_milliseconds = 0, 
		int reply_hash = 0, 
		::dsn::replication::read_semantic_t read_semantic = ::dsn::replication::read_semantic_t::ReadLastUpdate 
		)
	{
		return ::dsn::replication::replication_app_client_base::read<rrdb_client, ::dsn::blob, read_response>(
            get_partition_index(*key),
            RPC_RRDB_RRDB_GET,
            key,
            this,
            &rrdb_client::end_get2, 
            timeout_milliseconds,
			reply_hash, 
			read_semantic 
            );
	}

	virtual void end_get2(
		::dsn::error_code err, 
		std::shared_ptr<::dsn::blob>& key, 
		std::shared_ptr<read_response>& resp)
	{
		if (err != ::dsn::ERR_SUCCESS) std::cout << "reply RPC_RRDB_RRDB_GET err : " << err.to_string() << std::endl;
		else
		{
			std::cout << "reply RPC_RRDB_RRDB_GET ok" << std::endl;
		}
	}
	
};

} } 