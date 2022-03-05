#include "effects.h"

#include "core.h"
#include <DxErr.h>
#include <iterator>

#define CHECKD3DERR(expr) ErrorHandler(expr, __FILE__, __LINE__, __func__, #expr)

inline bool Effects::ErrorHandler(HRESULT hr, const char *file, unsigned line, const char *func, const char *expr) const
{
    if (hr != D3D_OK && hr != S_FALSE)
    {
        core.Trace("[%s:%s:%d] %s: %s (%s) (%.*s)", file, func, line, DXGetErrorString(hr), DXGetErrorDescription(hr),
                   expr, debugMsg_.size(), debugMsg_.data());
        return true;
    }

    return false;
}

Effects::Effects(IDirect3DDevice9 *d3dDevice) : device_(d3dDevice), currentTechnique_(nullptr), currentPass_(0u)
{
}

Effects::~Effects()
{
    release();
}

void Effects::setDevice(IDirect3DDevice9 *device)
{
    device_ = device;
}

void Effects::compile(const char *fxPath)
{
    debugMsg_ = fxPath;
    ID3DXEffect *fx;
    ID3DXBuffer *errors = nullptr;
    std::wstring _fxPath = utf8::ConvertUtf8ToWide(fxPath);
    CHECKD3DERR(D3DXCreateEffectFromFile(device_, _fxPath.c_str(), nullptr, nullptr, D3DXSHADER_OPTIMIZATION_LEVEL3,
                                         nullptr, &fx, &errors));

    if (errors)
    {
        core.Trace(static_cast<const char *>(errors->GetBufferPointer()));
        return;
    }

    effects_.push_back(fx);

    D3DXHANDLE technique = nullptr;
    CHECKD3DERR(fx->FindNextValidTechnique(nullptr, &technique));
    while (technique != nullptr)
    {
        D3DXTECHNIQUE_DESC desc;
        CHECKD3DERR(fx->GetTechniqueDesc(technique, &desc));

        // transform to lowercase to be compliant with the original code
        std::string name_in_lowercase;
        const auto len = strlen(desc.Name);
        name_in_lowercase.reserve(len);
        std::transform(desc.Name, desc.Name + len, std::back_inserter(name_in_lowercase), tolower);

        if (techniques_.count(name_in_lowercase) > 0)
        {
            core.Trace("Warning: duplicate technique (%s)", desc.Name);
        }
        else
        {
            techniques_.emplace(std::move(name_in_lowercase), Technique(fx, technique, desc));
        }

        CHECKD3DERR(fx->FindNextValidTechnique(technique, &technique));
    }
}

void Effects::release()
{
    for (auto *fx : effects_)
        fx->Release();
    effects_.clear();
    techniques_.clear();
    currentTechnique_ = nullptr;
}

bool Effects::begin(const std::string &techniqueName)
{
    // transform to lowercase to be compliant with the original code
    std::string name_in_lowercase;
    name_in_lowercase.reserve(techniqueName.length());
    std::transform(std::begin(techniqueName), std::end(techniqueName), std::back_inserter(name_in_lowercase), tolower);

    debugMsg_ = name_in_lowercase;
    const auto technique = techniques_.find(name_in_lowercase);
    if (technique == techniques_.end())
    {
        core.Trace("Warning: technique (%s) not found!", name_in_lowercase.c_str());
        return false;
    }

    currentTechnique_ = &technique->second;
    auto *fx = currentTechnique_->fx;
    CHECKD3DERR(fx->SetTechnique(currentTechnique_->handle));

    UINT passes = 0;
    CHECKD3DERR(fx->Begin(&passes, 0));
    if (passes == 0)
    {
        core.Trace("Warning: empty technique (%s)!", name_in_lowercase.c_str());
        return false;
    }

    currentPass_ = 0;
    CHECKD3DERR(fx->BeginPass(currentPass_++));
    return true;
}

bool Effects::next()
{
    if (currentTechnique_)
    {
        debugMsg_ = currentTechnique_->desc.Name;
        auto *fx = currentTechnique_->fx;
        CHECKD3DERR(fx->EndPass());

        if (currentPass_ < currentTechnique_->desc.Passes)
        {
            CHECKD3DERR(fx->BeginPass(currentPass_++));
            return true;
        }

        CHECKD3DERR(fx->End());
        currentTechnique_ = nullptr;
    }
    return false;
}

ID3DXEffect *Effects::getEffectPointer(const std::string &techniqueName)
{
    // transform to lowercase to be compliant with the original code
    std::string name_in_lowercase;
    name_in_lowercase.reserve(techniqueName.length());
    std::transform(std::begin(techniqueName), std::end(techniqueName), std::back_inserter(name_in_lowercase), tolower);

    const auto technique = techniques_.find(name_in_lowercase);
    return technique != techniques_.end() ? technique->second.fx : nullptr;
}
