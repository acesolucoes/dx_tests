// ShaderLoader.cpp

#include "ShaderLoader.h"

void ShaderLoader::CompileShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& shaderModel, ID3DBlob** blobOut) {
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(fileName.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), shaderModel.c_str(), shaderFlags, 0, blobOut, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
    }
    if (errorBlob) errorBlob->Release();
}
