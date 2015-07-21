
# include <dsn/tool/simulator.h>
# include "rrdb.check.h"

namespace dsn { namespace apps { class rrdb_checker : public ::dsn::tools::checker
{
public:
    rrdb_checker(const char* name) : ::dsn::tools::checker(name)
    {
        for (auto& app : _apps)
        {
            // TODO: identify your own type of service apps
            //auto meta = dynamic_cast<meta_service_app *>(app.second);
            //if (meta != nullptr) _meta_servers.push_back(meta);
        }
    }

    virtual void check() override
    {
        // nothing to check
        //if (_meta_servers.size() == 0)
        //    return;

        // check all invariances
        /*
        auto meta = meta_leader();
        if (!meta) return;

        for (auto& r : _replica_servers)
        {
            if (!r->is_started())
                continue;

            auto ep = r->primary_address();
            if (!meta->_service->_failure_detector->is_worker_connected(ep))
            {
                dassert(!r->_stub->is_connected(), "when meta server says a replica is dead, it must be dead");
            }
        }
        */
    }

private:
    //std::vector<meta_service_app*>        _meta_servers;
};

void install_checkers(configuration_ptr config)
{
    auto sim = dynamic_cast<::dsn::tools::simulator*>(::dsn::tools::get_current_tool());
    if (nullptr == sim)
        return;

    sim->add_checker(new rrdb_checker("rrdb.global-checker"));
}

} } 