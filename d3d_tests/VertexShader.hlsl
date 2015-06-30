
cbuffer ObjectData
{
    matrix viewProjectionMatrix;
}

void main(
    in float3 pos : POSITION,
    in float3 normal : NORMAL,
    in float4 color : COLOR,
    out float4 outPos : SV_POSITION,
    out float4 outColor : COLOR)
{
    outPos = mul(float4(pos, 1.0f), viewProjectionMatrix);

    // Pass along the color
    outColor = color;
}
