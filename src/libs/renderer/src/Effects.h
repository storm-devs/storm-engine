#pragma once

#include <cstdint>
#include <d3d9.h>
#include <d3dx9.h>
#include <unordered_map>
#include <vector>

class Effects final
{
  private:
    struct Technique
    {
        Technique(ID3DXEffect *fx, D3DXHANDLE handle, D3DXTECHNIQUE_DESC desc) : fx(fx), handle(handle), desc(desc)
        {
        }

        ID3DXEffect *fx;
        D3DXHANDLE handle;
        D3DXTECHNIQUE_DESC desc;
    };

    IDirect3DDevice9 *device_;

    std::vector<ID3DXEffect *> effects_;
    std::unordered_map<std::string, Technique> techniques_;

    const Technique *currentTechnique_;
    uint32_t currentPass_;

    std::string_view debugMsg_;

    inline bool ErrorHandler(HRESULT hr, const char *file, unsigned line, const char *func, const char *expr) const;

  public:
    Effects(Effects &) = delete;
    Effects(Effects &&) = delete;
    Effects &operator=(Effects &) = delete;
    Effects &operator=(Effects &&) = delete;

    explicit Effects(IDirect3DDevice9 *d3dDevice = nullptr);
    ~Effects();
    // Set device
    void setDevice(IDirect3DDevice9 *device);
    // Compile effect by path
    void compile(const char *fxPath);
    // Release all effects
    void release();
    // Begin technique
    bool begin(const std::string &techniqueName);
    // Execute next technique
    bool next();
    // Get effect pointer by technique name
    ID3DXEffect *getEffectPointer(const std::string &techniqueName);
};
