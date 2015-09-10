# include "rrdb.server.impl.h"
# include <boost/filesystem.hpp>

# ifdef __TITLE__
# undef __TITLE__
# endif
#define __TITLE__ "rrdb.server.impl"

namespace dsn {
    namespace apps {
        rrdb_service_impl::rrdb_service_impl(::dsn::replication::replica* replica)
            : rrdb_service(replica)
        {
            _is_open = false;

            // disable write ahead logging as replication handles logging instead now
            _wt_opts.disableWAL = true;
        }

        void rrdb_service_impl::on_empty_write()
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());
            
            update_request update;
            update.key = blob("empty-0xdeadbeef", 0, 16);
            update.value = blob("empty", 0, 5);

            ::dsn::rpc_replier<int> reply(nullptr);

            on_put(update, reply);
        }

        void rrdb_service_impl::on_put(const update_request& update, ::dsn::rpc_replier<int>& reply)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());
            
            rocksdb::WriteOptions opts = _wt_opts;
            opts.given_sequence_number = static_cast<rocksdb::SequenceNumber>(_last_committed_decree + 1);

            rocksdb::Slice skey(update.key.data(), update.key.length());
            rocksdb::Slice svalue(update.value.data(), update.value.length());
            rocksdb::Status status = _db->Put(opts, skey, svalue);
            dassert(status.ok(), status.ToString().c_str());
            dassert(_db->GetLatestSequenceNumber() == _last_committed_decree + 1, "");
            reply(status.code());

            ++_last_committed_decree;
        }

        void rrdb_service_impl::on_remove(const ::dsn::blob& key, ::dsn::rpc_replier<int>& reply)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());
            
            rocksdb::WriteOptions opts = _wt_opts;
            opts.given_sequence_number = static_cast<rocksdb::SequenceNumber>(_last_committed_decree + 1);

            rocksdb::Slice skey(key.data(), key.length());
            rocksdb::Status status = _db->Delete(opts, skey);
            dassert(status.ok(), status.ToString().c_str());
            dassert(_db->GetLatestSequenceNumber() == _last_committed_decree + 1, "");
            reply(status.code());

            ++_last_committed_decree;
        }

        void rrdb_service_impl::on_merge(const update_request& update, ::dsn::rpc_replier<int>& reply)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());
            
            rocksdb::WriteOptions opts = _wt_opts;
            opts.given_sequence_number = static_cast<rocksdb::SequenceNumber>(_last_committed_decree + 1);

            rocksdb::Slice skey(update.key.data(), update.key.length());
            rocksdb::Slice svalue(update.value.data(), update.value.length());
            // TODO(qinzuoyan) To support merging, we need provide a merge operator when opening DB, or it will
            // fail when call Merge().
            rocksdb::Status status = _db->Merge(opts, skey, svalue);
            // Here we should make sure that the Put/Remove/Merge operator is called successfully, so that the
            // last_committed_decree is consistent inside and outside the db.
            dassert(status.ok(), status.ToString().c_str());
            dassert(_db->GetLatestSequenceNumber() == _last_committed_decree + 1, "");
            reply(status.code());

            ++_last_committed_decree;
        }

        void rrdb_service_impl::on_get(const ::dsn::blob& key, ::dsn::rpc_replier<read_response>& reply)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());

            read_response resp;
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
                // because disableWAL is set true when do writes, so after open db,
                // it should satisfy that LatestSequenceNumber == LatestDurableSequenceNumber.
                dassert(_db->GetLatestSequenceNumber() == _db->GetLatestDurableSequenceNumber(), "");
                _is_open = true;
                _last_committed_decree = _db->GetLatestSequenceNumber();

                ddebug("open app: lastC/DDecree = <%llu, %llu>", last_committed_decree(), last_durable_decree());
            }

            return status.code();
        }

        int  rrdb_service_impl::close(bool clear_state)
        {
            if (!_is_open)
            {
                dassert(_db == nullptr, "");
                dassert(!clear_state, ""); // should not be here if do clear
                return 0;
            }

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

        int  rrdb_service_impl::flush(bool wait)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());

            rocksdb::FlushOptions opts;
            opts.wait = wait;

            auto status = _db->Flush(opts);
            return status.code();
        }

        void rrdb_service_impl::prepare_learning_request(/*out*/ blob& learn_req)
        {
            // nothing to do
        }

        int  rrdb_service_impl::get_learn_state(
            ::dsn::replication::decree start, 
            const blob& learn_req, 
            /*out*/ ::dsn::replication::learn_state& state)
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

                ddebug("lastC/DDecree=<%llu,%llu>, meta_size=%d, "
                       "start=%lld, end=%lld, file_count=%d, mem_state_size=%d",
                       last_committed_decree(), last_durable_decree(), writer.total_size(),
                       static_cast<long long int>(start0), static_cast<long long int>(end),
                       state.files.size(), mem_state.size());

                state.meta.push_back(writer.get_buffer());
            }

            return status.code();
        }

        int  rrdb_service_impl::apply_learn_state(::dsn::replication::learn_state& state)
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());

            int err = 0;
            binary_reader reader(state.meta[0]);

            rocksdb::SequenceNumber start;
            rocksdb::SequenceNumber end;
            std::string edit;
            std::string mem_state;

            reader.read(start);
            reader.read(end);
            reader.read(edit);
            reader.read(mem_state);

            ddebug("lastC/DDecree=<%llu,%llu>, meta_size=%d, "
                   "start=%lld, end=%lld, file_count=%d, mem_state_size=%d",
                   last_committed_decree(), last_durable_decree(), reader.total_size(),
                   static_cast<long long int>(start), static_cast<long long int>(end),
                   state.files.size(), mem_state.size());

            if (mem_state.size() == 0 && state.files.size() == 0)
            {
                // nothing to learn
                dassert(end == start - 1, "");
                dassert(end == last_committed_decree(), "");
                return 0;
            }

            if (start == 0)
            {
                // learn from scratch, should clear db firstly and then re-create it
                ddebug("start == 0, learn from scratch, clear db and then re-create it");

                // clear db
                err = close(true);
                if (err != 0)
                {
                    derror("clear db %s failed, err = %d", data_dir().c_str(), err);
                    return err;
                }

                // re-create db
                err = open(true);
                if (err != 0)
                {
                    derror("open db %s failed, err = %d", data_dir().c_str(), err);
                    return err;
                }
            }

            // rename 'learn_dir/xxx.sst' to 'data_dir/xxx.sst.learn' to avoid conflicting
            // with exist files.
            for (auto &f : state.files)
            {
                boost::filesystem::path old_p = learn_dir() + f;
                boost::filesystem::path new_p = data_dir() + f + ".learn";

                // create directory recursively if necessary
                boost::filesystem::path path = new_p;
                path = path.remove_filename();
                if (!boost::filesystem::exists(path))
                    boost::filesystem::create_directories(path);

                try
                {
                    boost::filesystem::rename(old_p, new_p);
                }
                catch (const boost::filesystem::filesystem_error& e)
                {
                    // TODO(qinzuoyan) delete garbage files
                    derror("rename %s to %s failed, err = %s",
                           old_p.string().c_str(), new_p.string().c_str(), e.what());
                    dassert(e.code().value() != 0, "");
                    return e.code().value();
                }
            }

            auto status = _db->ApplyLearningState(start, end, mem_state, edit);
            if (status.ok())
            {
                _last_committed_decree = end;
                ddebug("after ApplyLearningState in DB %s, "
                       "updated <C,D> to <%lld, %lld> with <start,end> as <%lld, %lld>",
                       data_dir().c_str(),
                       static_cast<long long int>(last_committed_decree()),
                       static_cast<long long int>(last_durable_decree()),
                       static_cast<long long int>(start),
                       static_cast<long long int>(end)
                      );
            }

            return status.code();
        }
                
        ::dsn::replication::decree rrdb_service_impl::last_durable_decree() const
        {
            dassert(_is_open, "rrdb service %s is not ready", data_dir().c_str());

            return _db->GetLatestDurableSequenceNumber();
        }
    }
}
