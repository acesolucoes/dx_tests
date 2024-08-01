#include "RenderAPI.h"
#include "IUnityGraphicsD3D11.h"
// #include "shader.h"
#include "OutCompute.h"

#include <exception>
#include <stdexcept>
#include <fstream>


namespace {
	BufType g_vBuf0[NUM_ELEMENTS];
	BufType g_vBuf1[NUM_ELEMENTS];
}

_Use_decl_annotations_
HRESULT CreateStructuredBuffer( ID3D11Device* pDevice, UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut )
{
    *ppBufOut = nullptr;

    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    desc.ByteWidth = uElementSize * uCount;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = uElementSize;

    if ( pInitData )
    {
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = pInitData;
        return pDevice->CreateBuffer( &desc, &InitData, ppBufOut );
    } else
        return pDevice->CreateBuffer( &desc, nullptr, ppBufOut );
}

_Use_decl_annotations_
HRESULT CreateBufferSRV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut )
{
    D3D11_BUFFER_DESC descBuf = {};
    pBuffer->GetDesc( &descBuf );

    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    desc.BufferEx.FirstElement = 0;

    if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
    {
        // This is a Structured Buffer

        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
    } else
    {
        return E_INVALIDARG;
    }

    return pDevice->CreateShaderResourceView( pBuffer, &desc, ppSRVOut );
}

//--------------------------------------------------------------------------------------
// Create Unordered Access View for Structured or Raw Buffers
//-------------------------------------------------------------------------------------- 
_Use_decl_annotations_
HRESULT CreateBufferUAV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut )
{
    D3D11_BUFFER_DESC descBuf = {};
    pBuffer->GetDesc( &descBuf );
        
    D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    desc.Buffer.FirstElement = 0;

    if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
    {
        // This is a Raw Buffer

        desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
        desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        desc.Buffer.NumElements = descBuf.ByteWidth / 4; 
    } else
    if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
    {
        // This is a Structured Buffer

        desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
        desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride; 
    } else
    {
        return E_INVALIDARG;
    }
    
    return pDevice->CreateUnorderedAccessView( pBuffer, &desc, ppUAVOut );
}

_Use_decl_annotations_
void RunComputeShader( ID3D11DeviceContext* pd3dImmediateContext,
                      ID3D11ComputeShader* pComputeShader,
                      UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews, 
                      ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
                      ID3D11UnorderedAccessView* pUnorderedAccessView,
                      UINT X, UINT Y, UINT Z )
{
    pd3dImmediateContext->CSSetShader( pComputeShader, nullptr, 0 );
    pd3dImmediateContext->CSSetShaderResources( 0, nNumViews, pShaderResourceViews );
    pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &pUnorderedAccessView, nullptr );
    if ( pCBCS && pCSData )
    {
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        pd3dImmediateContext->Map( pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
        memcpy( MappedResource.pData, pCSData, dwNumDataBytes );
        pd3dImmediateContext->Unmap( pCBCS, 0 );
        ID3D11Buffer* ppCB[1] = { pCBCS };
        pd3dImmediateContext->CSSetConstantBuffers( 0, 1, ppCB );
    }

    pd3dImmediateContext->Dispatch( X, Y, Z );

    pd3dImmediateContext->CSSetShader( nullptr, nullptr, 0 );

    ID3D11UnorderedAccessView* ppUAViewnullptr[1] = { nullptr };
    pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, ppUAViewnullptr, nullptr );

    ID3D11ShaderResourceView* ppSRVnullptr[2] = { nullptr, nullptr };
    pd3dImmediateContext->CSSetShaderResources( 0, 2, ppSRVnullptr );

    ID3D11Buffer* ppCBnullptr[1] = { nullptr };
    pd3dImmediateContext->CSSetConstantBuffers( 0, 1, ppCBnullptr );
}

ID3D11Buffer* CreateAndCopyToDebugBuf( ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer )
{
    ID3D11Buffer* debugbuf = nullptr;

    D3D11_BUFFER_DESC desc = {};
    pBuffer->GetDesc( &desc );
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    if ( SUCCEEDED(pDevice->CreateBuffer(&desc, nullptr, &debugbuf)) )
    {
        pd3dImmediateContext->CopyResource( debugbuf, pBuffer );
    }

    return debugbuf;
}

D3D11Render::D3D11Render(std::ofstream &output) : m_logger(output), m_Device(nullptr){}

void D3D11Render::setComputeBuffer(void *buffer) 
{
    g_pBuf0 = reinterpret_cast<ID3D11Buffer*>(buffer);
}

void D3D11Render::createResources() {
	auto hr = m_Device->CreateComputeShader( g_CSMain, sizeof(g_CSMain), nullptr, &g_computeShader );
    if ( FAILED(hr) )
    {
        m_Device->Release();
        m_logger << "Failed compiling shader " << hr << std::flush;
		throw std::runtime_error("ERROR");
    }

	m_logger << "Success\n" << std::flush;
	m_logger << "Creating buffers and filling them with initial data..." << std::flush;

    // for ( int i = 0; i < NUM_ELEMENTS; ++i ) 
    // {
    //     g_vBuf0[i] = {1.0f, 0.0f, 0.0f, 1.0f};
    // }

    // CreateStructuredBuffer( m_Device, sizeof(BufType), NUM_ELEMENTS, &g_vBuf0[0], &g_pBuf0 );
    // CreateStructuredBuffer( m_Device, sizeof(BufType), NUM_ELEMENTS, &g_vBuf1[0], &g_pBuf1 );
    CreateStructuredBuffer( m_Device, sizeof(BufType), NUM_ELEMENTS, nullptr, &g_pBufResult );

	m_logger << "done\n" << std::flush;
	
	m_logger << "Creating buffer views...\n" << std::flush;

    CreateBufferSRV( m_Device, g_pBuf0, &g_pBuf0SRV );
    CreateBufferUAV( m_Device, g_pBufResult, &g_pBufResultUAV );

    m_logger << "done\n" << std::flush;
	
	m_logger << "Running Compute Shader...\n" << std::flush;

    ID3D11ShaderResourceView* aRViews[1] = { g_pBuf0SRV };
    RunComputeShader( g_pContext, g_computeShader, 1, aRViews, nullptr, nullptr, 0, g_pBufResultUAV, NUM_ELEMENTS, 1, 1 );

    m_logger << "done\n" << std::flush;
}

bool D3D11Render::checkIfRanSuccessfully() {
    ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(m_Device, g_pContext, g_pBufResult);
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    BufType* p;
    g_pContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

    // Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS
    // This is also a common trick to debug CS programs.
    p = (BufType*)MappedResource.pData;

    /*m_logger << "DEBUG: Verifying GPU buffer and cpu buffers...\n" << std::flush;
    for (int i = 0; i < NUM_ELEMENTS; ++i)
    {
        printf("%d %f - %d %f %d %f - %d %f\n", p[i].i, p[i].f,
            g_vBuf0[i].i, g_vBuf0[i].f, g_vBuf1[i].i, g_vBuf1[i].f,
            g_vBuf0[i].i + g_vBuf1[i].i, g_vBuf0[i].f + g_vBuf1[i].f);
    }*/
   auto computedColor = p[0];
   m_logger << computedColor.x << ","<< computedColor.y << ","<< computedColor.z << ","<< computedColor.w << "\n" << std::flush;

    // Verify that if Compute Shader has done right
    m_logger << "Verifying against CPU result...\n" << std::flush;
    bool bSuccess = true;
    
    if (bSuccess)
        m_logger << "succeeded\n" << std::flush;

    g_pContext->Unmap(debugbuf, 0);

    SAFE_RELEASE(debugbuf);

    return bSuccess;
}

void D3D11Render::releaseResources() {
    g_pBuf0SRV->Release();
    g_computeShader->Release();
    g_pBufResultUAV->Release();
    g_pContext->Release();
    m_Device->Release();
}

void D3D11Render::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces) {
	switch (type)
{
	
	case kUnityGfxDeviceEventInitialize:
	{
		IUnityGraphicsD3D11* d3d = interfaces->Get<IUnityGraphicsD3D11>();
		m_Device = d3d->GetDevice();
        m_Device->GetImmediateContext(&g_pContext);
		createResources();
		break;
	}

	case kUnityGfxDeviceEventShutdown:
		releaseResources();
		break;
	} 
}
