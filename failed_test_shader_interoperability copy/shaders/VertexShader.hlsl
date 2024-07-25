// VertexShader.hlsl

cbuffer ConstantBuffer : register(b0)
{
    matrix worldViewProj;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Col : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Col : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), worldViewProj);
    output.Col = input.Col;
    return output;
}