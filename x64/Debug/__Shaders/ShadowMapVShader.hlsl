struct VS_IN
{
    float4 pos : POSITION0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
};

cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 wvpMat;
    row_major float4x4 wMat;
    row_major float4x4 wMatInvTranspose;
    float3 cam_pos;
};

cbuffer ShadowCBuf : register(b1)
{
    row_major float4x4 lightView;
    row_major float4x4 lightProj;
    row_major float4x4 shadowTransform;
};


PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    output.pos = mul(mul(mul(input.pos, wMat), lightView), lightProj);
    output.pos = output.pos / output.pos.w;
    return output;
}
