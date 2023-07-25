#include "config_library.hpp"

#include "core.h"
#include "istring.hpp"
#include "s_import_func.h"
#include "toml_config.hpp"
#include "v_s_stack.h"
#include "vma.hpp"

namespace storm {

namespace {

using Config = ConfigLibrary;

CREATE_SCRIPTLIBRIARY(Config)

uint32_t LoadConfigImpl(VS_STACK *pS) {
    auto *pString = (VDATA *)pS->Pop();
    auto* objectPtr = pS->Pop();
    const char *file_path_ptr = pString->GetString();
    if (file_path_ptr) {
        std::string file_path = file_path_ptr;
        auto *var = objectPtr->GetVarPointer();
        entid_t entity_id = var->GetEntityID();
        bool success = LoadConfig(var->GetAClass(), file_path, entity_id);
        return success ? IFUNCRESULT_OK : IFUNCRESULT_FAILED;
    }
    else {
        return IFUNCRESULT_FAILED;
    }
}

uint32_t ListFilesImpl(VS_STACK *pS) {
    auto *pString = (VDATA *)pS->Pop();
    auto* objectPtr = pS->Pop();
    const char *root_dir_ptr = pString->GetString();
    if (root_dir_ptr) {
        auto *var = objectPtr->GetVarPointer();

        std::string file_path = fio->ConvertPathResource(root_dir_ptr);
        std::error_code ec;
        std::filesystem::directory_iterator it(file_path);

        auto *attr = var->GetAClass();
        auto *list_attr = attr->CreateSubAClass(attr, "files");
        size_t index = 0;
        for (const auto &entry : it) {
            std::string index_string = std::to_string(index++);
            std::u8string path_str_u8 = entry.path().u8string();
            std::string path_str(path_str_u8.begin(), path_str_u8.end());
            list_attr->SetAttribute(index_string, path_str);
        }

        return ec ? IFUNCRESULT_FAILED : IFUNCRESULT_OK;
    }
    else {
        return IFUNCRESULT_FAILED;
    }
}

} // namespace

bool ConfigLibrary::Init()
{
    IFUNCINFO sIFuncInfo{};
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncName = "LoadConfig";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = LoadConfigImpl;
    core.SetScriptFunction(&sIFuncInfo);

    sIFuncInfo = {};
    sIFuncInfo.nArguments = 2;
    sIFuncInfo.pFuncName = "ListFiles";
    sIFuncInfo.pReturnValueName = "void";
    sIFuncInfo.pFuncAddress = ListFilesImpl;
    core.SetScriptFunction(&sIFuncInfo);

    return true;
}

} // namespace storm
