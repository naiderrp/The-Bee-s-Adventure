struct PassData
{
    matrix viewProjectionMatrix;
    float4 cameraPosition; // cameraPosition.w is scaleFactor for vertices as we only scale the sphere
};

struct VSInput
{
    float3 position : POSITION;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

ConstantBuffer<PassData> gPassData : register(b0);

PSInput VSMain(VSInput input)
{
    PSInput output;

    output.texCoord = input.position; // store local space position to use for sampling since we created a unit sphere (radius = 0.5)

    float scaleFactor = gPassData.cameraPosition.w;
    float3 scaledVertex = scaleFactor * input.position;
    float4 posW = float4(scaledVertex, 1.0f);
    posW.xyz += gPassData.cameraPosition.xyz; // center around camera

    output.position = mul(gPassData.viewProjectionMatrix, posW).xyww;

    return output;
}
