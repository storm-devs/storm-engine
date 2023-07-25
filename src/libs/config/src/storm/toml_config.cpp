#include "toml_config.hpp"
#include "core.h"

#include <toml++/toml.h>

namespace storm
{

namespace
{

void ConvertTable(ATTRIBUTES *target, const toml::table &table, entid_t entity_id);

void ConvertValue(ATTRIBUTES *target, const std::string &key, const toml::node &node, entid_t entity_id)
{
    if (node.is_array())
    {
        const auto *array = node.as_array();
        ATTRIBUTES *new_element = target->VerifyAttributeClass(key);
        const size_t existing_attr_count = new_element->GetAttributesNum();
        size_t offset = 0;
        for (size_t i = 0; i < existing_attr_count; ++i) {
            if (new_element->HasAttribute(std::to_string(i))) {
                ++offset;
            }
            else {
                break;
            }
        }
        array->for_each([&](size_t index, auto &&el) {
            std::string index_key = std::to_string(offset + index);
            ConvertValue(new_element, index_key, el, entity_id);
        });
    }
    else if (node.is_table())
    {
        ConvertTable(&(target->CreateAttribute(key)), *node.as_table(), entity_id);
    }
    else if (node.is_string())
    {
        std::string str = node.value_or<std::string>("");
        if (key == "__value__")
        {
            target->SetValue(str);
            if (entity_id != invalid_entity)
            {
                core.Entity_AttributeChanged(entity_id, target);
            }
            return;
        }
        else
        {
            target->SetAttribute(key, str);
        }
    }
    else if (node.is_floating_point())
    {
        target->SetAttributeUseFloat(key.c_str(), node.value_or<float>(0));
    }
    else if (node.is_integer())
    {
        target->SetAttributeUseDword(key.c_str(), node.value_or<uint32_t>(0));
    }
    else if (node.is_boolean()) {
        target->SetAttribute(key, node.value_or<bool>(false) ? "1" : "0");
    }
    ATTRIBUTES *updated_attr = target->GetAttributeClass(key);
    if (entity_id != invalid_entity && updated_attr != nullptr)
    {
        core.Entity_AttributeChanged(entity_id, updated_attr);
    }
}

void ConvertTable(ATTRIBUTES *target, const toml::table &table, entid_t entity_id)
{
    for (const auto &[key, value] : table)
    {
        ConvertValue(target, std::string(key), value, entity_id);
    }
}

} // namespace

bool LoadConfig(ATTRIBUTES *target, const std::string_view &file_path, entid_t entity_id)
{
    auto table = toml::parse_file(file_path);
    ConvertTable(target, table, entity_id);
    return true;
}

} // namespace storm
