Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
};

struct PSOutput
{
    float4 Normal : SV_Target0;
    float4 Albedo : SV_Target1;
    float4 Specular : SV_Target2;
    float4 WorldPos : SV_Target3;
};

PSOutput PSMain(PS_IN input)
{
    PSOutput output;
    output.Normal = float4(normalize(input.normal), 1.0);
    output.Albedo = DiffuseMap.Sample(Sampler, input.texCoord);
    output.Specular = float4(0.9, 20, 0, 1);
    output.WorldPos = float4(input.wPos, 1.0f);
    return output;
}