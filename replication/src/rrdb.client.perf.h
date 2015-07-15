# pragma once

# include "rrdb.client.h"
# include <dsn/internal/perf_test_helper.h>

namespace dsn { namespace apps { class rrdb_perf_test_client
    : public rrdb_client, public ::dsn::service::perf_client_helper<rrdb_perf_test_client>
{
public:
    rrdb_perf_test_client(
        const std::vector<::dsn::end_point>& meta_servers,
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
        s.send_one = [this](){this->send_one_put(); };
        s.cases.clear();
        load_suite_config(s);
        suits.push_back(s);
        
        s.name = "rrdb.remove";
        s.config_section = "task.RPC_RRDB_RRDB_REMOVE";
        s.send_one = [this](){this->send_one_remove(); };
        s.cases.clear();
        load_suite_config(s);
        suits.push_back(s);
        
        s.name = "rrdb.merge";
        s.config_section = "task.RPC_RRDB_RRDB_MERGE";
        s.send_one = [this](){this->send_one_merge(); };
        s.cases.clear();
        load_suite_config(s);
        suits.push_back(s);
        
        s.name = "rrdb.get";
        s.config_section = "task.RPC_RRDB_RRDB_GET";
        s.send_one = [this](){this->send_one_get(); };
        s.cases.clear();
        load_suite_config(s);
        suits.push_back(s);
        
        start(suits);
    }                

    void send_one_put()
    {
        void* ctx = prepare_send_one();
        if (!ctx)
            return;

        update_request req;
        // TODO: randomize the value of req
        // auto rs = random64(0, 10000000);
        // std::stringstream ss;
        // ss << "key." << rs;
        // req = ss.str();
        
        begin_put(req, ctx, _timeout_ms);
    }

    virtual void end_put(
        ::dsn::error_code err,
        const int& resp,
        void* context) override
    {
        end_send_one(context, err, [this](){ send_one_put();});
    }

    void send_one_remove()
    {
        void* ctx = prepare_send_one();
        if (!ctx)
            return;

        ::dsn::blob req;
        // TODO: randomize the value of req
        // auto rs = random64(0, 10000000);
        // std::stringstream ss;
        // ss << "key." << rs;
        // req = ss.str();
        
        begin_remove(req, ctx, _timeout_ms);
    }

    virtual void end_remove(
        ::dsn::error_code err,
        const int& resp,
        void* context) override
    {
        end_send_one(context, err, [this](){ send_one_remove();});
    }

    void send_one_merge()
    {
        void* ctx = prepare_send_one();
        if (!ctx)
            return;

        update_request req;
        // TODO: randomize the value of req
        // auto rs = random64(0, 10000000);
        // std::stringstream ss;
        // ss << "key." << rs;
        // req = ss.str();
        
        begin_merge(req, ctx, _timeout_ms);
    }

    virtual void end_merge(
        ::dsn::error_code err,
        const int& resp,
        void* context) override
    {
        end_send_one(context, err, [this](){ send_one_merge();});
    }

    void send_one_get()
    {
        void* ctx = prepare_send_one();
        if (!ctx)
            return;

        ::dsn::blob req;
        // TODO: randomize the value of req
        // auto rs = random64(0, 10000000);
        // std::stringstream ss;
        // ss << "key." << rs;
        // req = ss.str();
        
        begin_get(req, ctx, _timeout_ms);
    }

    virtual void end_get(
        ::dsn::error_code err,
        const read_response& resp,
        void* context) override
    {
        end_send_one(context, err, [this](){ send_one_get();});
    }
};

} } 