#pragma once

#include <SFML/System/Err.hpp>

#include <any>
#include <memory>
#include <unordered_map>

#ifndef NDEBUG
#define RES_DEBUG sf::err() << "[ResMgr|D] "
#else
#include <fstream>
#define RES_DEBUG std::ofstream __debug; __debug.setstate(std::ios_base::badbit); __debug
#endif
#define RES_WARN sf::err() << "[ResMgr|W] "
#define RES_ERROR sf::err() << "[ResMgr|E] "

class ResourceManager
{
public:
    struct ResourceDefinition
    {
        std::string Name, Path;
        std::any ExtraData;
        const std::type_info& Type;
    };

    ResourceManager();
    ~ResourceManager();

    template<typename T>
    std::shared_ptr<T> load(const std::string& aName) {
        if (m_registeredResources.count(aName) == 0)
        {
            RES_ERROR << "Tried to load unregistered resource " << aName << std::endl;
            return std::shared_ptr<T>();
        }

        const auto& def = m_registeredResources.at(aName);
        if (def.Type != typeid(T))
        {
            RES_ERROR << "Resource " << aName << " is not of type " << def.Type.name() << std::endl;
            return std::shared_ptr<T>();
        }

        if (m_loadedResources.count(aName) > 0)
        {
            auto& weakptr = m_loadedResources.at(aName);
            std::shared_ptr<void*> loaded;
            if (!weakptr.expired())
                loaded = weakptr.lock();

            if (loaded)
            {
                RES_DEBUG << "Resource " << aName << " already loaded, returning" << std::endl;
                return std::reinterpret_pointer_cast<T>(loaded);
            }
        }

        auto loaded(loadDefinition<T>(def));

        auto voidp = std::reinterpret_pointer_cast<void>(loaded);
        m_loadedResources[aName] = voidp;

        return loaded;
    }

    template<typename T>
    void addResource(const std::string& aName, const std::string& aPath, std::any aExtraData = std::any()) {
        addResource({ aName, aPath, aExtraData, typeid(T) });
    }

private:
    void addResource(ResourceDefinition&& aResource);

    template<typename T>
    std::shared_ptr<T> loadDefinition(const ResourceDefinition& aFile);

    std::unordered_map<std::string, ResourceDefinition> m_registeredResources;
    std::unordered_map<std::string, std::weak_ptr<void *>> m_loadedResources;
};
