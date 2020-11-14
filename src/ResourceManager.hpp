#pragma once

#include <SFML/System/Err.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/core/type_info.hpp>
#include <entt/resource/cache.hpp>

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

namespace sf { class Font; class Image; class Shader; class Texture; class Music; class SoundBuffer; }

class ResourceManager
{
public:
    struct ResourceDefinition
    {
        std::string Name, Path;
        std::any ExtraData;
        entt::type_info Type;
    };

    ResourceManager();
    ~ResourceManager();

    template<typename T>
    entt::resource_handle<T> load(entt::hashed_string aName, bool aFatal = false) {
        if (m_registeredResources.count(aName) == 0)
        {
            RES_ERROR << "Tried to load unregistered resource " << std::string(aName.data()) << std::endl;
            if (aFatal)
                throw std::runtime_error("Tried to load unregistered resource " + std::string(aName.data()));
            return entt::resource_handle<T>();
        }

        const auto& def = m_registeredResources.at(aName);
        if (def.Type != entt::type_id<T>())
        {
            RES_ERROR << "Resource " << def.Name << " is not of type " << def.Type.name() << std::endl;
            if (aFatal)
                throw std::runtime_error("Resource " + def.Name + " is not of type " + std::string(def.Type.name()));
            return entt::resource_handle<T>();
        }

        return loadDefinition<T>(def, aFatal);
    }

    template<typename T>
    void addResource(const std::string& aName, const std::string& aPath, std::any aExtraData = std::any()) {
        addResource({ aName, aPath, aExtraData, entt::type_id<T>() });
    }

private:
#define LOADER(TYPE) \
struct TYPE##Loader final : entt::resource_loader<TYPE##Loader, sf::TYPE> { \
    std::shared_ptr<sf::TYPE> load(const ResourceDefinition& aDef, bool aFatal) const; \
} \

LOADER(Font);
LOADER(Image);
LOADER(Shader);
LOADER(Texture);
LOADER(Music);
LOADER(SoundBuffer);

#undef LOADER

private:
    entt::resource_cache<sf::Font> mFontCache;
    entt::resource_cache<sf::Image> mImageCache;
    entt::resource_cache<sf::Shader> mShaderCache;
    entt::resource_cache<sf::Texture> mTextureCache;
    entt::resource_cache<sf::Music> mMusicCache;
    entt::resource_cache<sf::SoundBuffer> mSoundBufferCache;

    void addResource(ResourceDefinition&& aResource);

    template<typename T>
    entt::resource_handle<T> loadDefinition(const ResourceDefinition& aFile, bool aFatal);

    std::unordered_map<entt::hashed_string::hash_type, ResourceDefinition> m_registeredResources;
};
