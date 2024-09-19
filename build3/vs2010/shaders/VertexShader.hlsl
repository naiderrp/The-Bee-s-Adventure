struct VS_INPUT
{
    float3 position : POSITION;
    float4 color    : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

struct PassData
{
    float4x4 MVP;
    float4 color;
};

ConstantBuffer<PassData> gPassData : register(b0);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float r = max(input.color.x, gPassData.color.x);
    float g = max(input.color.y, gPassData.color.y);
    float b = min(input.color.z, gPassData.color.z);
    float opacityFactor = gPassData.color.w;
    float a = input.color.w * opacityFactor;

    output.color = float4(r, g, b, a); 
    output.position = mul(gPassData.MVP, float4(input.position, 1.0f));
    
    return output;
}