#include "vhost/vhost-factory.hh"

#include "vhost/vhost-static-file.hh"

namespace http
{
    shared_vhost VHostFactory::Create(VHostConfig config)
    {
        auto v = new VHostStaticFile(config);
        return std::shared_ptr<VHostStaticFile>(v);
    }
} // namespace http
