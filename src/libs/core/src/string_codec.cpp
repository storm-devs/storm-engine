#include "string_codec.hpp"

namespace
{

constexpr size_t HASH_TABLE_SIZE = 512;          // must be power of 2
constexpr size_t MAX_TABLE_ENTRY_SIZE = 1 << 16; // 0x10000

constexpr uint32_t MakeHashValue(const std::string_view &ps)
{
    uint32_t hval = 0;
    for (char v : ps)
    {
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A'; // case independent
        hval = (hval << 4) + (uint32_t)v;
        uint32_t g = hval & ((uint32_t)0xf << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return hval;
}

uint32_t MakeStringCode(const uint32_t table_index, const uint32_t element_index)
{
    Assert(table_index < HASH_TABLE_SIZE);
    Assert(element_index < MAX_TABLE_ENTRY_SIZE);
    return (table_index << 16) | (element_index & (MAX_TABLE_ENTRY_SIZE - 1));
}

} // namespace

STRING_CODEC::STRING_CODEC() : hashTable_(HASH_TABLE_SIZE)
{
}

STRING_CODEC::~STRING_CODEC() noexcept = default;

uint32_t STRING_CODEC::GetNum() const
{
    return totalStringCount_;
}

uint32_t STRING_CODEC::Convert(const std::string_view &pString)
{
    uint32_t hash_code = MakeHashValue(pString);
    uint32_t table_index = hash_code & (HASH_TABLE_SIZE - 1);

    HTELEMENT &table_entry = hashTable_[table_index];

    for (size_t n = 0; n < table_entry.elements.size(); n++)
    {
        const auto &element = table_entry.elements[n];
        if (element.hashCode == hash_code && storm::iEquals(pString, element.string))
        {
            return MakeStringCode(table_index, n);
        }
    }

    const size_t n = table_entry.elements.size();
    table_entry.elements.push_back(HTSUBELEMENT{
        .string = std::string(pString),
        .hashCode = hash_code,
    });
    totalStringCount_++;
    return MakeStringCode(table_index, n);
}

const char *STRING_CODEC::Convert(uint32_t code) const
{
    uint32_t table_index = code >> 16;
    if (table_index >= HASH_TABLE_SIZE)
    {
        return "ERROR: invalid SCCT index";
    }
    uint32_t n = code & (MAX_TABLE_ENTRY_SIZE - 1);
    if (n >= hashTable_[table_index].elements.size())
    {
        return "INVALID SCC";
    }
    return hashTable_[table_index].elements[n].string.c_str();
}

const char *STRING_CODEC::Get()
{
    tableIndex_ = 0;
    for (tableIndex_ = 0; tableIndex_ < HASH_TABLE_SIZE; tableIndex_++)
    {
        if (!hashTable_[tableIndex_].elements.empty())
        {
            tableElementIndex_ = 0;
            return hashTable_[tableIndex_].elements[tableElementIndex_].string.c_str();
        }
    }
    return nullptr;
}

const char *STRING_CODEC::GetNext()
{
    tableElementIndex_++;
    for (; tableIndex_ < HASH_TABLE_SIZE; tableIndex_++)
    {
        if (hashTable_[tableIndex_].elements.empty())
            continue;
        if (tableElementIndex_ >= hashTable_[tableIndex_].elements.size())
        {
            tableElementIndex_ = 0;
            continue;
        }
        return hashTable_[tableIndex_].elements[tableElementIndex_].string.c_str();
    }
    return nullptr;
}
