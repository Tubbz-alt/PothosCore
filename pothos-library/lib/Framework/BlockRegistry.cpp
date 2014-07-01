// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework/Block.hpp>
#include <Pothos/Framework/Topology.hpp>
#include <Pothos/Framework/BlockRegistry.hpp>
#include <Pothos/Framework/Exception.hpp>
#include <Pothos/Plugin.hpp>
#include <Poco/Logger.h>

Pothos::BlockRegistry::BlockRegistry(const std::string &path, const Callable &factory)
{
    //check the path
    if (path.empty() or path.front() != '/')
    {
        poco_error_f1(Poco::Logger::get("Pothos.BlockRegistry"), "Invalid path: %s", path);
        return;
    }

    //parse the path
    PluginPath fullPath;
    try
    {
        fullPath = PluginPath("/blocks").join(path.substr(1));
    }
    catch (const PluginPathError &)
    {
        poco_error_f1(Poco::Logger::get("Pothos.BlockRegistry"), "Invalid path: %s", path);
        return;
    }

    //check the factory
    if (factory.type(-1) == typeid(Block*) or factory.type(-1) == typeid(Topology*))
    {
        //register
        PluginRegistry::add(fullPath, factory);
    }

    //otherwise report the error
    else
    {
        poco_error_f1(Poco::Logger::get("Pothos.BlockRegistry"), "Bad Factory, must return Block* or Topology*: %s", factory.toString());
    }
}

Pothos::Callable Pothos::BlockRegistry::lookup(const std::string &path)
{
    return PluginRegistry::get(PluginPath("/blocks").join(path.substr(1))).getObject().extract<Callable>();
}

static Pothos::Object blockRegistryMake(const std::string &path, const Pothos::Object *args, const size_t numArgs)
{
    const auto factory = Pothos::BlockRegistry::lookup(path);

    if (factory.type(-1) == typeid(Pothos::Block*))
    {
        auto element = factory.opaqueCall(args, numArgs).extract<Pothos::Block *>();
        if (element->getName().empty()) element->setName(path); //a better name
        return Pothos::Object(std::shared_ptr<Pothos::Block>(element));
    }

    if (factory.type(-1) == typeid(Pothos::Topology*))
    {
        auto element = factory.opaqueCall(args, numArgs).extract<Pothos::Topology *>();
        if (element->getName().empty()) element->setName(path); //a better name
        return Pothos::Object(std::shared_ptr<Pothos::Topology>(element));
    }

    throw Pothos::IllegalStateException("Pothos::BlockRegistry::make("+path+")", factory.toString());
}

#include <Pothos/Managed.hpp>

static auto managedBlockRegistry = Pothos::ManagedClass()
    .registerClass<Pothos::BlockRegistry>()
    .registerWildcardStaticMethod(&blockRegistryMake)
    .commit("Pothos/BlockRegistry");