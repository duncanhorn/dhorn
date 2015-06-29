

void main(
    in float3 pos : POSITION,
    in float3 normal : NORMAL,
    in float4 color : COLOR,
    out float4 outPos : SV_POSITION,
    out float4 outColor : COLOR)
{
    // TODO
    outPos = float4(pos, 1.0f);

    // Pass along the color
    outColor = color;
}
