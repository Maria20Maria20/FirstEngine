Texture2D NormalMap : register(t0);
Texture2D DiffuseMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D PositionMap : register(t3);
SamplerState Sampler : register(s0);

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

static const float SCREEN_SIZE_X = 800.0f;
static const float SCREEN_SIZE_Y = 800.0f;

// Borders ....................... float4(x_start, y_start, x_end, y_end)
static const float4 NormalMapPos = float4(0.0f, 0.0f, 120.0f, 130.0f);
static const float4 DiffuseMapPos = float4(130.0f, 0.0f, 250.0f, 130.0f);
static const float4 SpecularMapPos = float4(260.0f, 0.0f, 380.0f, 130.0f);
static const float4 PositionMapPos = float4(390.0f, 0.0f, 510.0f, 130.0f);

float4 PSMain(PS_IN input) : SV_Target
{

    float4 output = float4(0,0,0,0);
    
    if (input.pos.x > NormalMapPos.x && input.pos.x < NormalMapPos.z
        && input.pos.y > NormalMapPos.y && input.pos.y < NormalMapPos.w)
    {
        float x = (input.pos.x - NormalMapPos.x) / (NormalMapPos.z - NormalMapPos.x);
        float y = (input.pos.y - NormalMapPos.y) / (NormalMapPos.w - NormalMapPos.y);
        output = float4(NormalMap.Sample(Sampler, float2(x, y)).rgb, 1.0f);
    }
    else if (input.pos.x > DiffuseMapPos.x && input.pos.x < DiffuseMapPos.z
        && input.pos.y > DiffuseMapPos.y && input.pos.y < DiffuseMapPos.w)
    {
        float x = (input.pos.x - DiffuseMapPos.x) / (DiffuseMapPos.z - DiffuseMapPos.x);
        float y = (input.pos.y - DiffuseMapPos.y) / (DiffuseMapPos.w - DiffuseMapPos.y);
        output = float4(DiffuseMap.Sample(Sampler, float2(x, y)).rgb, 1.0f);
    }
    else if (input.pos.x > SpecularMapPos.x && input.pos.x < SpecularMapPos.z
        && input.pos.y > SpecularMapPos.y && input.pos.y < SpecularMapPos.w)
    {
        float x = (input.pos.x - SpecularMapPos.x) / (SpecularMapPos.z - SpecularMapPos.x);
        float y = (input.pos.y - SpecularMapPos.y) / (SpecularMapPos.w - SpecularMapPos.y);
        output = float4(SpecularMap.Sample(Sampler, float2(x, y)).rg, 0.0f, 1.0f);
    }
    else if (input.pos.x > PositionMapPos.x && input.pos.x < PositionMapPos.z
        && input.pos.y > PositionMapPos.y && input.pos.y < PositionMapPos.w)
    {
        float x = (input.pos.x - PositionMapPos.x) / (PositionMapPos.z - PositionMapPos.x);
        float y = (input.pos.y - PositionMapPos.y) / (PositionMapPos.w - PositionMapPos.y);
        output = float4(PositionMap.Sample(Sampler, float2(x, y)).rgb, 1.0f);
    }
    
    return saturate(output);
}