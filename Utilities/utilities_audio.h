#pragma once

#include <SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

class SoundManager
{
public:
    //
    // Returns the singleton instance and registers the channel finish callback
    //
    static SoundManager &Instance();

    // register a sound
    // id: unique identifier for the sound
    // path: file path to load the sound from (relative to Assets/Sounds/)
    bool RegisterSound(const std::string &id, const std::string &path);

    // unregister a sound
    // id: unique identifier of the sound to remove
    void UnregisterSound(const std::string &id);

    // play a sound
    // id: unique identifier of the registered sound
    // loop: true to loop indefinitely
    // volume: volume level in [0.0, 1.0] (0=mute, 1=max)
    void PlaySound(const std::string &id, bool loop = false, float volume = 1.0f);

    // stop a sound
    // id: unique identifier of the sound to stop
    void StopSound(const std::string &id);

    // stop all sounds
    void StopAll();

    // clean up all sounds; call before Mix_CloseAudio()
    void CleanUp();

private:
    SoundManager() {}
    ~SoundManager() {}
    SoundManager(const SoundManager &) = delete;
    SoundManager &operator=(const SoundManager &) = delete;

    std::unordered_map<std::string, Mix_Chunk *> m_Sounds;
    std::unordered_map<std::string, std::vector<int>> m_Channels;

    // callback when a channel finishes playing
    static void ChannelFinishedCallback(int channel);
    void OnChannelFinished(int channel);
};

// Definitions

inline SoundManager &SoundManager::Instance()
{
    static bool callbackRegistered = false;
    static SoundManager instance;
    if (!callbackRegistered)
    {
        Mix_ChannelFinished(ChannelFinishedCallback);
        callbackRegistered = true;
    }
    return instance;
}

inline bool SoundManager::RegisterSound(const std::string &id, const std::string &path)
{
    if (m_Sounds.count(id))
    {
        Debug::Error("RegisterSound: ID already exists: " + id);
        return false;
    }
    std::string p = std::string("Assets/Sounds/") + path;
    Mix_Chunk *chunk = Mix_LoadWAV(p.c_str());
    if (!chunk)
    {
        Debug::Error("RegisterSound: Failed to load sound " + p + ": " + Mix_GetError());
        return false;
    }
    m_Sounds[id] = chunk;
    Debug::Log("RegisterSound: Sound registered with ID " + id);
    return true;
}

inline void SoundManager::UnregisterSound(const std::string &id)
{
    StopSound(id);
    auto it = m_Sounds.find(id);
    if (it != m_Sounds.end())
    {
        Mix_FreeChunk(it->second);
        m_Sounds.erase(it);
        m_Channels.erase(id);
        Debug::Log("UnregisterSound: Sound unregistered with ID " + id);
    }
    else
    {
        Debug::Error("UnregisterSound: ID not found: " + id);
    }
}

inline void SoundManager::PlaySound(const std::string &id, bool loop, float volume)
{
    auto it = m_Sounds.find(id);
    if (it == m_Sounds.end())
    {
        Debug::Error("PlaySound: ID not found: " + id);
        return;
    }

    int loops = loop ? -1 : 0;
    int channel = Mix_PlayChannel(-1, it->second, loops);
    if (channel == -1)
    {
        Debug::Error("PlaySound: Failed to play sound " + id + ": " + Mix_GetError());
        return;
    }

    // Clamp volume
    volume = std::clamp(volume, 0.0f, 1.0f);

    // Set volume (0-128)
    int vol = static_cast<int>(volume * MIX_MAX_VOLUME);
    Mix_Volume(channel, vol);

    m_Channels[id].push_back(channel);
}

inline void SoundManager::StopSound(const std::string &id)
{
    auto it = m_Channels.find(id);
    if (it != m_Channels.end())
    {
        for (int channel : it->second)
            Mix_HaltChannel(channel);
        it->second.clear();
        Debug::Log("StopSound: Stopped sound ID " + id);
    }
    else
    {
        Debug::Error("StopSound: No active channel for ID " + id);
    }
}

inline void SoundManager::StopAll()
{
    Mix_HaltChannel(-1);
    m_Channels.clear();
    Debug::Log("StopAll: All sounds stopped");
}

inline void SoundManager::CleanUp()
{
    StopAll();
    for (auto &p : m_Sounds)
        Mix_FreeChunk(p.second);
    m_Sounds.clear();
    Debug::Log("CleanUp: All sounds cleaned up");
}

inline void SoundManager::ChannelFinishedCallback(int channel)
{
    SoundManager::Instance().OnChannelFinished(channel);
}

inline void SoundManager::OnChannelFinished(int channel)
{
    for (auto &kv : m_Channels)
    {
        auto &vec = kv.second;
        vec.erase(std::remove(vec.begin(), vec.end(), channel), vec.end());
    }
}
