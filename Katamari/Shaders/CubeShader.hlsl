struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflect;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Att;
    float pad;
};

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Direction;
    float pad;
};


cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 wvpMat;
    row_major float4x4 wMat;
    row_major float4x4 wMatInvTranspose;
    float3 cam_pos;
};

cbuffer LightBuffer : register(b1) // per frame
{
    DirectionalLight dLight;
    PointLight pointLights[10];
};

struct ShadowTransforms
{
    row_major float4x4 lightView;
    row_major float4x4 lightProj;
    row_major float4x4 shadowTransform;
};

cbuffer CascadeCBuf : register(b2) // per frame
{
    ShadowTransforms shTransforms[4];
    float4 distances;
};

Texture2DArray shadowMap : register(t0);
SamplerState shadowSampler : register(s0);
SamplerComparisonState samShadow : register(s1)
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(1.0f, 1.0f, 1.0f, 0.0f);

    ComparisonFunc = LESS;
};


static const float SMAP_SIZE = 4096.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL0;
    float3 wPos : POSITION;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;

    output.pos = mul(input.pos, wvpMat);
    float4 _wPos = mul(input.pos, wMat);
    output.wPos = _wPos.xyz / _wPos.w;
    
    output.col = input.col;
    output.texCoord = input.texCoord;
    
    output.normal = normalize(mul(float4(input.normal, 0), wMatInvTranspose));
  
    return output;
}

void calcDirectionalLight(float3 wPos, float3 normal, float3 toEye, Material mat, DirectionalLight dirLight,
    out float4 dl_ambient,
    out float4 dl_diffuse,
    out float4 dl_spec)
{
    dl_ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    dl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    dl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    dl_ambient = mat.Ambient * dirLight.Ambient;
    
    {
        float3 lightVec = -dirLight.Direction;
        float diffuseFactor = dot(lightVec, normal);
        
        [flatten]
        if (diffuseFactor > 0.0f)
        {
            float3 v = reflect(-lightVec, normal);
            float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
            dl_diffuse = diffuseFactor * mat.Diffuse * dirLight.Diffuse;
            dl_spec = specFactor * mat.Specular * dirLight.Specular;
        }
    }
}

float4 calcPointLight(float3 wPos, float3 normal, float3 toEye, Material mat, PointLight pointLight)
{
    float4 pl_ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 pl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 pl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightVec = pointLight.Position - wPos;
    float d = length(lightVec);
       
    if (d > pointLight.Range)
        return float4(0, 0, 0, 0);
    lightVec /= d;
    pl_ambient = mat.Ambient * pointLight.Ambient;
    float diffuseFactor = dot(lightVec, normal);
        
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        pl_diffuse = diffuseFactor * mat.Diffuse * pointLight.Diffuse;
        pl_spec = specFactor * mat.Specular * pointLight.Specular;
    }

    float att = 1.0f / dot(pointLight.Att, float3(1.0f, d, d * d));
    pl_diffuse *= att;
    pl_spec *= att;
        
    return saturate(pl_ambient + pl_diffuse + pl_spec);
}

float CalcShadowFactor(SamplerComparisonState samShadow,
                       Texture2DArray shadowMap,
                       float4 shadowPosH,
                       int layer)
{
  // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;
  
    // Depth in NDC space.
    float depth = shadowPosH.z;

    //return shadowMap.SampleCmpLevelZero(samShadow, shadowPosH.xy, depth).r;
  
    // Texel size.
    const float dx = SMAP_DX;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
        float3(shadowPosH.xy + offsets[i], layer), depth).r;
    }

    return percentLit /= 9.0f;
}

static const float4 colorValue[4] =
{
    float4(1, 0, 1, 1),
        float4(0, 1, 0, 1),
        float4(0, 0, 1, 1),
        float4(1, 0, 0, 1)
};

float4 PSMain(PS_IN input) : SV_Target
{
    /*
    if (input.pos.x < 0.5 * SMAP_SIZE && input.pos.y < 0.5 * SMAP_SIZE)
    {
        float val = shadowMap.Sample(shadowSampler, float3(input.pos.xy, 0)).r;
        val = shadowMap.Load(float4(input.pos.xy * 2, 1, 0)).r;
        return float4(val.rrr, 1.0f);
    }
    */
    
    float4 pixelColor = input.col;
    
    Material mat =
    {
        pixelColor,
        pixelColor,
        pixelColor * 0.5f,
        { 0.0f, 0.0f, 0.0f, 0.0f }
    };
    
    float3 normal = normalize(input.normal);
    float3 toEye = normalize(cam_pos - input.wPos);
    
    float4 dl_ambient;
    float4 dl_diffuse;
    float4 dl_spec;
    
    float4 dirLightCol;
    
    calcDirectionalLight(input.wPos, normal, toEye, mat, dLight, dl_ambient, dl_diffuse, dl_spec);

    float4 lightViewPos = mul(float4(input.wPos, 1.0), shTransforms[0].lightView);
    lightViewPos = lightViewPos / lightViewPos.w;
    
    
    int layer = 1;
    
    static float cascadeDistances[4] = (float[4])distances;
    
    for (int i = 0; i < 4; ++i)
    {
        if (lightViewPos.z < cascadeDistances[i])
        {
            layer = i;
            break;
        }
    }
    
    float4 shPos = mul(float4(input.wPos, 1.0), shTransforms[layer].shadowTransform);
    shPos = shPos / shPos.w;
    
    /*
    if (input.pos.x < 0.5 * SMAP_SIZE && input.pos.y < 0.5 * SMAP_SIZE)
    {
        float val = shadowMap.Sample(shadowSampler, float3(input.pos.xy, 0)).r;
        val = shadowMap.Load(float4(input.pos.xy * 2, 1, 0)).r;
        return float4(val.rrr, 1.0f);
    }
    */
    
    if ((shPos.x >= 0) && (shPos.y >= 0) && (shPos.z >= 0) && (shPos.x <= 1) && (shPos.y <= 1) && (shPos.z <= 1))
    {
        // float shZ = shadowMap.Sample(shadowSampler, shPos.xy, 0).r;
        //return saturate(float4(float(lightViewPos.z * 1.0f / 200).rrr, 1.0f));
        
        
        float shadowFactor = CalcShadowFactor(samShadow, shadowMap, shPos, layer);
        dirLightCol = saturate(dl_ambient + shadowFactor * (dl_diffuse + dl_spec));
    }
    else
        dirLightCol = saturate(dl_ambient + dl_spec + dl_diffuse);
    
        float4 pointLightSum = { 0, 0, 0, 0 };

    for (int i = 0; i < 10; i++)
    {
        pointLightSum += calcPointLight(input.wPos, normal, toEye, mat, pointLights[i]);
    }    
    
    return saturate(dirLightCol + pointLightSum) * colorValue[layer];
    //return saturate(dirLightCol + pointLightSum) * float4(float(layer * 1.0 / 3).xxx, 1.0f);
    //layer = layer + 1; // 1, 2, 3, 4
    //                    // 001, 010, 011, 100
    //return saturate(dirLightCol + pointLightSum) * float4((layer & 1), (layer & 2) >> 1, (layer & 4) >> 2, 1);

}