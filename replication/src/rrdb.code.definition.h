# pragma once
# include <dsn/service_api.h>
# include "rrdb.types.h"

namespace dsn { namespace apps { 
    // define RPC task code for service 'rrdb'
    DEFINE_TASK_CODE_RPC(RPC_RRDB_RRDB_PUT, ::dsn::TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
    DEFINE_TASK_CODE_RPC(RPC_RRDB_RRDB_REMOVE, ::dsn::TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
    DEFINE_TASK_CODE_RPC(RPC_RRDB_RRDB_MERGE, ::dsn::TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
    DEFINE_TASK_CODE_RPC(RPC_RRDB_RRDB_GET, ::dsn::TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
    // test timer task code
    DEFINE_TASK_CODE(LPC_RRDB_TEST_TIMER, ::dsn::TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
} } 
