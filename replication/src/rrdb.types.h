# pragma once

//
// uncomment the following line if you want to use 
// data encoding/decoding from the original tool instead of rDSN
// in this case, you need to use these tools to generate
// type files with --gen=cpp etc. options
//
// !!! WARNING: not feasible for replicated service yet!!! 
//
// # define DSN_NOT_USE_DEFAULT_SERIALIZATION

# include <dsn/service_api_cpp.h>

# ifdef DSN_NOT_USE_DEFAULT_SERIALIZATION

# include <dsn/thrift_helper.h>
# include "rrdb_types.h" 

namespace dsn { namespace apps { 
    // ---------- update_request -------------
    inline void marshall(::dsn::binary_writer& writer, const update_request& val)
    {
        boost::shared_ptr<::dsn::binary_writer_transport> transport(new ::dsn::binary_writer_transport(writer));
        ::apache::thrift::protocol::TBinaryProtocol proto(transport);
        ::dsn::marshall_rpc_args<update_request>(&proto, val, &update_request::write);
    };

    inline void unmarshall(::dsn::binary_reader& reader, __out_param update_request& val)
    {
        boost::shared_ptr<::dsn::binary_reader_transport> transport(new ::dsn::binary_reader_transport(reader));
        ::apache::thrift::protocol::TBinaryProtocol proto(transport);
        ::dsn::unmarshall_rpc_args<update_request>(&proto, val, &update_request::read);
    };

    // ---------- read_response -------------
    inline void marshall(::dsn::binary_writer& writer, const read_response& val)
    {
        boost::shared_ptr<::dsn::binary_writer_transport> transport(new ::dsn::binary_writer_transport(writer));
        ::apache::thrift::protocol::TBinaryProtocol proto(transport);
        ::dsn::marshall_rpc_args<read_response>(&proto, val, &read_response::write);
    };

    inline void unmarshall(::dsn::binary_reader& reader, __out_param read_response& val)
    {
        boost::shared_ptr<::dsn::binary_reader_transport> transport(new ::dsn::binary_reader_transport(reader));
        ::apache::thrift::protocol::TBinaryProtocol proto(transport);
        ::dsn::unmarshall_rpc_args<read_response>(&proto, val, &read_response::read);
    };

} } 


# else // use rDSN's data encoding/decoding

namespace dsn { namespace apps { 
    // ---------- update_request -------------
    struct update_request
    {
        ::dsn::blob key;
        ::dsn::blob value;
    };

    inline void marshall(::dsn::binary_writer& writer, const update_request& val)
    {
        marshall(writer, val.key);
        marshall(writer, val.value);
    };

    inline void unmarshall(::dsn::binary_reader& reader, __out_param update_request& val)
    {
        unmarshall(reader, val.key);
        unmarshall(reader, val.value);
    };

    // ---------- read_response -------------
    struct read_response
    {
        int error;
        std::string value;
    };

    inline void marshall(::dsn::binary_writer& writer, const read_response& val)
    {
        marshall(writer, val.error);
        marshall(writer, val.value);
    };

    inline void unmarshall(::dsn::binary_reader& reader, __out_param read_response& val)
    {
        unmarshall(reader, val.error);
        unmarshall(reader, val.value);
    };

} } 

#endif 
