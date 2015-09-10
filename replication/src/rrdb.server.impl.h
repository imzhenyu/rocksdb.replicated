# pragma once

# include "rrdb.server.h"
# include <rocksdb/db.h>

namespace dsn {
    namespace apps {
        class rrdb_service_impl : public rrdb_service
        {
        public:
            rrdb_service_impl(::dsn::replication::replica* replica);

            virtual void on_put(const update_request& update, ::dsn::rpc_replier<int>& reply);
            virtual void on_remove(const ::dsn::blob& key, ::dsn::rpc_replier<int>& reply);
            virtual void on_merge(const update_request& update, ::dsn::rpc_replier<int>& reply);
            virtual void on_get(const ::dsn::blob& key, ::dsn::rpc_replier<read_response>& reply);

            virtual int  open(bool create_new);
            virtual int  close(bool clear_state);
            virtual int  flush(bool wait);
            virtual void on_empty_write();
            virtual void prepare_learning_request(/*out*/ blob& learn_req);
            virtual int  get_learn_state(::dsn::replication::decree start,
                    const blob& learn_req, /*out*/ ::dsn::replication::learn_state& state);
            virtual int  apply_learn_state(::dsn::replication::learn_state& state);
            virtual ::dsn::replication::decree last_durable_decree() const;

        private:
            rocksdb::DB           *_db;
            rocksdb::WriteOptions _wt_opts;
            rocksdb::ReadOptions  _rd_opts;

            std::atomic<bool>     _is_open;

        };
    }
}
