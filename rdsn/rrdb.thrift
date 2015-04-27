
include "dsn.thrift"

namespace cpp dsn.apps

struct update_request
{
    1:dsn.blob      key;
    2:dsn.blob      value;
}

struct read_response
{
    1:int      error;
    2:dsn.blob value;
}

service rrdb
{
    int put(1:update_request update);
    int remove(1:dsn.blob key);
    int merge(1:update_request update);
    read_response get(1:dsn.blob key);
}


