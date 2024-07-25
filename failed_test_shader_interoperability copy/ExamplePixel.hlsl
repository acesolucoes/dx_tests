#pragma kernel CSMain
#pragma target 5.0

RWStructuredBuffer<float4> Result;

[numthreads(1,1,1)]
void CSMain (uint3 id : SV_DispatchThreadID)
{
    Result[0].xyzw = float4(1, 0, 0, 1);
}