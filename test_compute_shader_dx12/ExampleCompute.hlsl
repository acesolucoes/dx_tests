// compute.hlsl
RWStructuredBuffer<float> Result : register(u0);
[numthreads(1, 1, 1)]
void main() {
    Result[0] = 42.0f; // Write a constant value to the output buffer
}
