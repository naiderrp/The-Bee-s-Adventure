TextureCube CubeMap : register(t0, space0);
SamplerState linearWrapSamplerState : register(s0, space0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    return CubeMap.Sample(linearWrapSamplerState, input.texCoord);
    //return float4(0.0, 1.0, 1.0, 1.0);
}