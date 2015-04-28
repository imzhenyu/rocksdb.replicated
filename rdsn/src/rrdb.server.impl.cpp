# include "rrdb.server.impl.h"


namespace dsn {
    namespace apps {
        rrdb_service_impl::rrdb_service_impl(::dsn::replication::replica* replica, ::dsn::configuration_ptr& config)
            : rrdb_service(replica, config)
        {
            _is_open = false;
            memset(&_wt_opts, 0, sizeof(_wt_opts));
            memset(&_rd_opts, 0, sizeof(_rd_opts));
        }

        void rrdb_service_impl::on_put(const update_request& update, ::dsn::service::rpc_replier<int>& reply)
        {
            if (!_is_open)
            {
                rocksdb::Slice skey(update.key.data(), update.key.length());
                rocksdb::Slice svalue(update.value.data(), update.value.length());
                rocksdb::Status status = _db->Put(_wt_opts, skey, svalue);
                reply(status.code());
            }
            else
            {
                reply(ERR_SERVICE_NOT_ACTIVE);
            }
        }

        void rrdb_service_impl::on_remove(const ::dsn::blob& key, ::dsn::service::rpc_replier<int>& reply)
        {
            if (!_is_open)
            {
                rocksdb::Slice skey(key.data(), key.length());
                rocksdb::Status status = _db->Delete(_wt_opts, skey);
                reply(status.code());
            }
            else
            {
                reply(ERR_SERVICE_NOT_ACTIVE);
            }
        }

        void rrdb_service_impl::on_merge(const update_request& update, ::dsn::service::rpc_replier<int>& reply)
        {
            if (!_is_open)
            {
                rocksdb::Slice skey(update.key.data(), update.key.length());
                rocksdb::Slice svalue(update.value.data(), update.value.length());
                rocksdb::Status status = _db->Merge(_wt_opts, skey, svalue);
                reply(status.code());
            }
            else
            {
                reply(ERR_SERVICE_NOT_ACTIVE);
            }
        }

        void rrdb_service_impl::on_get(const ::dsn::blob& key, ::dsn::service::rpc_replier<::dsn::blob>& reply)
        {
            if (!_is_open)
            {
                rocksdb::Slice skey(key.data(), key.length());
                std::string value;
                rocksdb::Status status = _db->Get(_rd_opts, skey, &value);
                if (status.ok())
                {
                    // copy value to resp
                }

                // TODO: add error code to response type
                //reply(status.code());
            }
            else
            {
                //reply(ERR_SERVICE_NOT_ACTIVE);
            }
        }

        int  rrdb_service_impl::open(bool create_new)
        {
            if (_is_open)
                return ERR_SERVICE_ALREADY_RUNNING;

            rocksdb::Options opts;
            opts.create_if_missing = create_new;
            opts.error_if_exists = create_new;

            auto status = rocksdb::DB::Open(opts, dir() + "/rdb", &_db);
            if (status.ok())
                _is_open = true;

            return status.code();
        }

        int  rrdb_service_impl::close(bool clear_state)
        {
            if (!_is_open)
                return ERR_SERVICE_NOT_ACTIVE;

            _is_open = false;
            delete _db;
            _db = nullptr;
            return 0;
        }

        int  rrdb_service_impl::compact(bool force)
        {
            if (!_is_open)
                return ERR_SERVICE_NOT_ACTIVE;

            // TODO: rocksdb::DB::Compact()
            return 0;
        }

        void rrdb_service_impl::prepare_learning_request(__out_param blob& learnRequest)
        {

        }

        int  rrdb_service_impl::get_learn_state(::dsn::replication::decree start, const blob& learnRequest, __out_param::dsn::replication::learn_state& state)
        {
            return 0;
        }

        int  rrdb_service_impl::apply_learn_state(::dsn::replication::learn_state& state)
        {
            return 0;
        }
    }
}
