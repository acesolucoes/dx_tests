// ShaderLoader.h

#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>

class ShaderLoader {
public:
    static void CompileShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& shaderModel, ID3DBlob** blobOut);
};
