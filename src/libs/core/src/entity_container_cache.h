#pragma once

#include <unordered_map>
#include <vector>

#include "entity.h"

class EntityContainerCache
{
  public:
    bool Contains(hash_t hash) const;

    void Add(hash_t hash, entid_t id);
    void UpdateAdd(hash_t hash, entid_t id);
    void UpdateErase(hash_t hash, entid_t id);
    void Clear();
    entity_container_cref Get(hash_t hash);

private:
    std::unordered_map<hash_t, std::vector<entid_t>> cache_;
};
