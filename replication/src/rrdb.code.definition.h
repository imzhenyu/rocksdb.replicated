# pragma once
# include <dsn/service_api_cpp.h>
# include "rrdb.types.h"

namespace dsn { namespace apps { 
    // define your own thread pool using DEFINE_THREAD_POOL_CODE(xxx)
    // define RPC task code for service 'rrdb'
    DEFINE_TASK_CODE_RPC(RPC_RRDB_RRDB_PUT, TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
    DEFINE_TASK_CODE_RPC(RPC_RRDB_RRDB_REMOVE, TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
    DEFINE_TASK_CODE_RPC(RPC_RRDB_RRDB_MERGE, TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
    DEFINE_TASK_CODE_RPC(RPC_RRDB_RRDB_GET, TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
    // test timer task code
    DEFINE_TASK_CODE(LPC_RRDB_TEST_TIMER, TASK_PRIORITY_COMMON, ::dsn::THREAD_POOL_DEFAULT)
} } 
