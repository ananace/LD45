#include "ResourceManager.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <stdexcept>
ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::addResource(ResourceDefinition&& aResource)
{
    RES_DEBUG << "Registering " << aResource.Name << " as " << aResource.Type.name() << " from " << aResource.Path << std::endl;

    m_registeredResources.emplace(entt::hashed_string(aResource.Name.c_str()).value(), std::move(aResource));
}

#define MANAGED_TYPE(TYPE) \
template<> \
entt::resource_handle<sf::TYPE> ResourceManager::loadDefinition(const ResourceDefinition& aDef, bool aFatal) { \
    return m##TYPE##Cache.load<TYPE##Loader>(entt::hashed_string{aDef.Name.c_str()}, aDef, aFatal); \
} \

MANAGED_TYPE(Font)
MANAGED_TYPE(Image)
MANAGED_TYPE(Shader)
MANAGED_TYPE(Texture)
MANAGED_TYPE(Music)
MANAGED_TYPE(SoundBuffer)

#define DEFAULT_LOAD(TYPE) \
std::shared_ptr<sf::TYPE> ResourceManager::TYPE##Loader::load(const ResourceManager::ResourceDefinition& aDef, bool aFatal) const \
{ \
    const std::string& aFile = aDef.Path; \
    RES_DEBUG << "Loading " << aDef.Name << " (" #TYPE ") from " << aFile << "..." << std::endl; \
    auto ptr = std::shared_ptr<sf::TYPE>(new sf::TYPE(), [&aDef](auto p) { \
        RES_DEBUG << "Unloading " << aDef.Name << std::endl; \
        delete p; \
    }); \
    if (ptr->loadFromFile(aFile)) \
        return ptr; \
    if (aFatal)\
        throw std::runtime_error("Failed to load " + aDef.Name + " (" #TYPE ") from " + aFile); \
    RES_ERROR << "Failed to load " << aDef.Name << " (" #TYPE ") from " << aFile << "..." << std::endl; \
    return std::shared_ptr<sf::TYPE>(); \
} \

#define MODIFIED_LOAD(TYPE, LOAD) \
std::shared_ptr<sf::TYPE> ResourceManager::TYPE##Loader::load(const ResourceManager::ResourceDefinition& aDef, bool aFatal) const \
{ \
    const std::string& aFile = aDef.Path; \
    RES_DEBUG << "Loading " << aDef.Name << " (" #TYPE ") from " << aFile << "..." << std::endl; \
    auto ptr = std::shared_ptr<sf::TYPE>(new sf::TYPE(), [&aDef](auto p) { \
        RES_DEBUG << "Unloading " << aDef.Name << std::endl; \
        delete p; \
    }); \
    if (ptr-> LOAD) \
        return ptr; \
    if (aFatal) \
        throw std::runtime_error("Failed to load " + aDef.Name + " (" #TYPE ") from " + aFile); \
    RES_ERROR << "Failed to load " << aDef.Name << " (" #TYPE ") from " << aFile << "..." << std::endl; \
    return std::shared_ptr<sf::TYPE>(); \
} \

DEFAULT_LOAD(Font)
DEFAULT_LOAD(Image)
MODIFIED_LOAD(Shader, loadFromFile(aFile, std::any_cast<sf::Shader::Type>(aDef.ExtraData)));
DEFAULT_LOAD(Texture)
MODIFIED_LOAD(Music, openFromFile(aFile))
DEFAULT_LOAD(SoundBuffer)
