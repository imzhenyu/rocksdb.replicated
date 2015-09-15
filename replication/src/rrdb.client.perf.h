# pragma once

# include "rrdb.client.h"

namespace dsn { namespace apps { class rrdb_perf_test_client
    : public rrdb_client, public ::dsn::service::perf_client_helper
{
public:
    rrdb_perf_test_client(
        const std::vector<::dsn::rpc_address>& meta_servers,
        const char* app_name)
        : rrdb_client(meta_servers, app_name)
    {
    }

    void start_test()
    {
        perf_test_suite s;
        std::vector<perf_test_suite> suits;

        s.name = "rrdb.put";
        s.config_section = "task.RPC_RRDB_RRDB_PUT";
        s.send_one = [this](int payload_bytes){this->send_one_put(payload_bytes); };
        s.cases.clear();
        load_suite_config(s);
        suits.push_back(s);
        
        s.name = "rrdb.remove";
        s.config_section = "task.RPC_RRDB_RRDB_REMOVE";
        s.send_one = [this](int payload_bytes){this->send_one_remove(payload_bytes); };
        s.cases.clear();
        load_suite_config(s);
        suits.push_back(s);
        
        /*
        s.name = "rrdb.merge";
        s.config_section = "task.RPC_RRDB_RRDB_MERGE";
        s.send_one = [this](int payload_bytes){this->send_one_merge(payload_bytes); };
        s.cases.clear();
        load_suite_config(s);
        suits.push_back(s);
        */
        
        s.name = "rrdb.get";
        s.config_section = "task.RPC_RRDB_RRDB_GET";
        s.send_one = [this](int payload_bytes){this->send_one_get(payload_bytes); };
        s.cases.clear();
        load_suite_config(s);
        suits.push_back(s);
        
        start(suits);
    }                

    void send_one_put(int payload_bytes)
    {
        void* ctx = prepare_send_one();
        if (!ctx)
            return;

        update_request req;
        
        auto rs = random64(0, 10000000);
        binary_writer writer(payload_bytes < 128 ? 128 : payload_bytes);
        writer.write("key.", 4);
        writer.write(rs);
        req.key = writer.get_buffer();

        while (writer.total_size() < payload_bytes)
            writer.write(req.key);
        req.value = writer.get_buffer();

        begin_put(req, ctx, _timeout_ms);
    }

    virtual void end_put(
        ::dsn::error_code err,
        const int& resp,
        void* context) override
    {
        end_send_one(context, err);
    }

    void send_one_remove(int payload_bytes)
    {
        void* ctx = prepare_send_one();
        if (!ctx)
            return;

        ::dsn::blob req;

        auto rs = random64(0, 10000000);
        binary_writer writer;
        writer.write("key.", 4);
        writer.write(rs);
        req = writer.get_buffer();
        
        begin_remove(req, ctx, _timeout_ms);
    }

    virtual void end_remove(
        ::dsn::error_code err,
        const int& resp,
        void* context) override
    {
        end_send_one(context, err);
    }

    void send_one_merge(int payload_bytes)
    {
        void* ctx = prepare_send_one();
        if (!ctx)
            return;

        update_request req;

        auto rs = random64(0, 10000000);
        binary_writer writer(payload_bytes < 128 ? 128 : payload_bytes);
        writer.write("key.", 4);
        writer.write(rs);
        req.key = writer.get_buffer();

        while (writer.total_size() < payload_bytes)
            writer.write(req.key);
        req.value = writer.get_buffer();
        
        begin_merge(req, ctx, _timeout_ms);
    }

    virtual void end_merge(
        ::dsn::error_code err,
        const int& resp,
        void* context) override
    {
        end_send_one(context, err);
    }

    void send_one_get(int payload_bytes)
    {
        void* ctx = prepare_send_one();
        if (!ctx)
            return;

        ::dsn::blob req;

        auto rs = random64(0, 10000000);
        binary_writer writer;
        writer.write("key.", 4);
        writer.write(rs);
        req = writer.get_buffer();
        
        begin_get(req, ctx, _timeout_ms);
    }

    virtual void end_get(
        ::dsn::error_code err,
        const read_response& resp,
        void* context) override
    {
        end_send_one(context, err);
    }
};

} } 
