struct VS_INPUT
{
    float2 position : POSITION;
    float2 uv       : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 0.0, 1.0); 
    output.uv = input.uv;
    
    return output;
}