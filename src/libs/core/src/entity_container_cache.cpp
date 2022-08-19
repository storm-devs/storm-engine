#include "entity_container_cache.h"

#include <algorithm>
#include <ranges>

void EntityContainerCache::Add(hash_t hash, entid_t id)
{
    // insert ordered
    auto &entry = cache_[hash];
    const auto it = std::ranges::upper_bound(entry, id);
    entry.insert(it, id);
}

void EntityContainerCache::UpdateAdd(hash_t hash, entid_t id)
{
    // check if exists
    if (auto entry = cache_.find(hash); entry != cache_.end())
    {
        // high dword is a timestamp. therefore newly added ent_id should always be greater than existing values
        entry->second.push_back(id);
    }
}

void EntityContainerCache::UpdateErase(hash_t hash, entid_t id)
{
    // check if exists
    if (auto entry = cache_.find(hash); entry != cache_.end())
    {
        // erase ordered
        const auto it = std::ranges::lower_bound(entry->second, id);
        if (it != std::end(entry->second) && *it == id)
        {
            entry->second.erase(it);
        }
    }
}

void EntityContainerCache::Clear()
{
    cache_.clear();
}

bool EntityContainerCache::Contains(hash_t hash) const
{
    return cache_.contains(hash);
}

entity_container_cref EntityContainerCache::Get(hash_t hash)
{
    // no bound check
    return cache_.find(hash)->second;
}
