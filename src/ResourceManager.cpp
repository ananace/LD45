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

#define DEFAULT_LOAD(T) \
template<> \
std::shared_ptr<T> ResourceManager::loadDefinition<T>(const ResourceDefinition& aDef, bool aFatal) \
{ \
    const std::string& aFile = aDef.Path; \
    RES_DEBUG << "Loading " << aDef.Name << " (" #T ") from " << aFile << "..." << std::endl; \
    auto ptr = std::shared_ptr<T>(new T(), [&aDef](auto p) { \
        RES_DEBUG << "Unloading " << aDef.Name << std::endl; \
        delete p; \
    }); \
    if (ptr->loadFromFile(aFile)) \
        return ptr; \
    if (aFatal)\
        throw std::runtime_error("Failed to load " + aDef.Name + " (" #T ") from " + aFile); \
    RES_ERROR << "Failed to load " << aDef.Name << " (" #T ") from " << aFile << "..." << std::endl; \
    return std::shared_ptr<T>(); \
} \

#define MODIFIED_LOAD(T, LOAD) \
template<> \
std::shared_ptr<T> ResourceManager::loadDefinition<T>(const ResourceDefinition& aDef, bool aFatal) \
{ \
    const std::string& aFile = aDef.Path; \
    RES_DEBUG << "Loading " << aDef.Name << " (" #T ") from " << aFile << "..." << std::endl; \
    auto ptr = std::shared_ptr<T>(new T(), [&aDef](auto p) { \
        RES_DEBUG << "Unloading " << aDef.Name << std::endl; \
        delete p; \
    }); \
    if (ptr-> LOAD) \
        return ptr; \
    if (aFatal) \
        throw std::runtime_error("Failed to load " + aDef.Name + " (" #T ") from " + aFile); \
    RES_ERROR << "Failed to load " << aDef.Name << " (" #T ") from " << aFile << "..." << std::endl; \
    return std::shared_ptr<T>(); \
} \

DEFAULT_LOAD(sf::Font)
DEFAULT_LOAD(sf::Image)
MODIFIED_LOAD(sf::Shader, loadFromFile(aFile, std::any_cast<sf::Shader::Type>(aDef.ExtraData)));
DEFAULT_LOAD(sf::Texture)
MODIFIED_LOAD(sf::Music, openFromFile(aFile))
DEFAULT_LOAD(sf::SoundBuffer)
