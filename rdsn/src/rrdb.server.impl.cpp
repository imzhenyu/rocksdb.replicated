# include "rrdb.server.impl.h"


namespace dsn {
    namespace apps {
        rrdb_service_impl::rrdb_service_impl(::dsn::replication::replica* replica, ::dsn::configuration_ptr& config)
            : rrdb_service(replica, config)
        {
        }

        void rrdb_service_impl::on_put(const update_request& update, ::dsn::service::rpc_replier<int>& reply)
        {
            int resp;
            reply(resp);
        }

        void rrdb_service_impl::on_remove(const ::dsn::blob& key, ::dsn::service::rpc_replier<int>& reply)
        {
            int resp;
            reply(resp);
        }

        void rrdb_service_impl::on_merge(const update_request& update, ::dsn::service::rpc_replier<int>& reply)
        {
            int resp;
            reply(resp);
        }

        void rrdb_service_impl::on_get(const ::dsn::blob& key, ::dsn::service::rpc_replier<::dsn::blob>& reply)
        {
            ::dsn::blob resp;
            reply(resp);
        }

        int  rrdb_service_impl::open(bool create_new)
        {
            return 0;
        }

        int  rrdb_service_impl::close(bool clear_state)
        {
            return 0;
        }

        int  rrdb_service_impl::compact(bool force)
        {
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
