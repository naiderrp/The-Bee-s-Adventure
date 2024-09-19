struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    //return float4(0.25, 0.9, 0.25, 1.0);
    return input.color;
}