# include "rrdb.server.impl.h"
# include <boost/filesystem.hpp>

namespace dsn {
    namespace apps {
        rrdb_service_impl::rrdb_service_impl(::dsn::replication::replica* replica, ::dsn::configuration_ptr& config)
            : rrdb_service(replica, config)
        {
            _is_open = false;

            //
            // disable write ahead logging as replication handles logging instead now
            //
            _wt_opts.disableWAL = true;
        }

        void rrdb_service_impl::on_empty_write()
        {
            update_request update;
            update.key = blob("empty-0xdeadbeef", 0, 16);
            update.value = blob("empty", 0, 5);

            ::dsn::message_ptr nil = nullptr;
            ::dsn::service::rpc_replier<int> reply(nil, nil);

            on_put(update, reply);
        }

        void rrdb_service_impl::on_put(const update_request& update, ::dsn::service::rpc_replier<int>& reply)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());
            
            rocksdb::WriteOptions opts = _wt_opts;
            opts.given_sequence_number = static_cast<rocksdb::SequenceNumber>(_last_committed_decree + 1);

            rocksdb::Slice skey(update.key.data(), update.key.length());
            rocksdb::Slice svalue(update.value.data(), update.value.length());
            rocksdb::Status status = _db->Put(opts, skey, svalue);
            reply(status.code());

            ++_last_committed_decree;
        }

        void rrdb_service_impl::on_remove(const ::dsn::blob& key, ::dsn::service::rpc_replier<int>& reply)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());
            
            rocksdb::WriteOptions opts = _wt_opts;
            opts.given_sequence_number = static_cast<rocksdb::SequenceNumber>(_last_committed_decree + 1);

            rocksdb::Slice skey(key.data(), key.length());
            rocksdb::Status status = _db->Delete(opts, skey);
            reply(status.code());

            ++_last_committed_decree;
        }

        void rrdb_service_impl::on_merge(const update_request& update, ::dsn::service::rpc_replier<int>& reply)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());
            
            rocksdb::WriteOptions opts = _wt_opts;
            opts.given_sequence_number = static_cast<rocksdb::SequenceNumber>(_last_committed_decree + 1);

            rocksdb::Slice skey(update.key.data(), update.key.length());
            rocksdb::Slice svalue(update.value.data(), update.value.length());
            rocksdb::Status status = _db->Merge(opts, skey, svalue);
            reply(status.code());

            ++_last_committed_decree;
        }

        void rrdb_service_impl::on_get(const ::dsn::blob& key, ::dsn::service::rpc_replier<read_response>& reply)
        {
            read_response resp;

            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());

            rocksdb::Slice skey(key.data(), key.length());
            rocksdb::Status status = _db->Get(_rd_opts, skey, &resp.value);
            resp.error = status.code();

            reply(resp);
        }

        int  rrdb_service_impl::open(bool create_new)
        {
            dassert(!_is_open, "rrdb service %s is already opened", data_dir().c_str());

            rocksdb::Options opts;
            opts.create_if_missing = create_new;
            opts.error_if_exists = create_new;
            opts.write_buffer_size = 40 * 1024; // 40 K for testing now

            auto status = rocksdb::DB::Open(opts, data_dir() + "/rdb", &_db);
            if (status.ok())
            {
                _is_open = true;
                _last_committed_decree = last_durable_decree();
            }

            return status.code();
        }

        int  rrdb_service_impl::close(bool clear_state)
        {
            if (!_is_open)
                return 0;

            _is_open = false;
            delete _db;
            _db = nullptr;

            if (clear_state)
            {
                boost::filesystem::path lp = data_dir();
                ::boost::filesystem::remove_all(lp);
                ::boost::filesystem::create_directory(lp);
            }
            return 0;
        }

        int  rrdb_service_impl::flush(bool force)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());

            rocksdb::FlushOptions opts;
            opts.wait = force;

            auto status = _db->Flush(opts);
            return status.code();
        }

        void rrdb_service_impl::prepare_learning_request(__out_param blob& learn_req)
        {
            // nothing to do
        }

        int  rrdb_service_impl::get_learn_state(
            ::dsn::replication::decree start, 
            const blob& learn_req, 
            __out_param::dsn::replication::learn_state& state)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());

            rocksdb::SequenceNumber start0 = start;
            rocksdb::SequenceNumber end;
            std::string mem_state;
            std::string edit;
            
            auto status = _db->GetLearningState(start0, end, mem_state, edit, state.files);
            if (status.ok())
            {
                binary_writer writer;
                writer.write(start0);
                writer.write(end);
                writer.write(edit);
                writer.write(mem_state);

                printf("GetLearningState result size = %d\n", writer.total_size());

                state.meta.push_back(writer.get_buffer());
            }

            return status.code();
        }

        int  rrdb_service_impl::apply_learn_state(::dsn::replication::learn_state& state)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());

            binary_reader reader(state.meta[0]);

            printf("ApplyLearningState result size = %d\n", reader.total_size());

            rocksdb::SequenceNumber start;
            rocksdb::SequenceNumber end;
            std::string edit;
            std::string mem_state;

            reader.read(start);
            reader.read(end);
            reader.read(edit);
            reader.read(mem_state);
                        
            if (mem_state.size() == 0 && state.files.size() == 0)
                return 0;
            else
            {
                if (start == 0)
                {
                    close(true);
                    open(true);
                }

                for (auto &f : state.files)
                {
                    boost::filesystem::path old_p = learn_dir() + f;
                    boost::filesystem::path new_p = data_dir() + f;

                    // create directory recursively if necessary
                    boost::filesystem::path path = new_p;
                    path = path.remove_filename();
                    if (!boost::filesystem::exists(path))
                        boost::filesystem::create_directories(path);

                    boost::filesystem::rename(old_p, new_p);
                }

                auto status = _db->ApplyLearningState(start, mem_state, edit);
                if (status.ok())
                {
                    _last_committed_decree = end;
                    printf("ApplyLeraningState lastcommitted in DB %s, <C,D> to <%lld, %lld> with <start,end> as <%lld, %lld>\n",
                        data_dir().c_str(),
                        static_cast<long long int>(last_committed_decree()),
                        static_cast<long long int>(last_durable_decree()),
                        static_cast<long long int>(start),
                        static_cast<long long int>(end)
                        );
                }                    
                return status.code();
            }   
        }
                
        ::dsn::replication::decree rrdb_service_impl::last_durable_decree() const
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());

            return _db->GetLatestDurableSequenceNumber();
        }
    }
}
