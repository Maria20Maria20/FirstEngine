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

cbuffer ShadowCBuf : register(b2) // per frame
{
    row_major float4x4 lightView;
    row_major float4x4 lightProj;
    row_major float4x4 shadowTransform;
};

SamplerComparisonState samShadow : register(s0)
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};

Texture2D shadowMap : register(t1);
SamplerState shadowSampler : register(s1);



static const float SMAP_SIZE = 512.0f;
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
                       Texture2D shadowMap,
                       float4 shadowPosH)
{
  // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;
  
  // Depth in NDC space.
    float depth = shadowPosH.z;

    // if (samShadow.Sample())
  
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
        shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit /= 9.0f;
}


float4 PSMain(PS_IN input) : SV_Target
{
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

    float4 shPos = mul(float4(input.wPos, 1.0), shadowTransform);
    shPos = shPos / shPos.w;
    
    if ((shPos.x >= 0) && (shPos.y >= 0) && (shPos.z >= 0) && (shPos.x <= 1) && (shPos.y <= 1) && (shPos.z <= 1))
    {
        float shZ = shadowMap.Sample(shadowSampler, shPos.xy).r;
        
        float shadowFactor = CalcShadowFactor(samShadow, shadowMap, shPos);
        dirLightCol = saturate(dl_ambient + shadowFactor * (dl_diffuse + dl_spec));
        /*
        if (shZ < shPos.z)
        {
            // For debugging
            // dirLightCol = float4(((floor(shPos.x * SMAP_SIZE) + floor(shPos.y * SMAP_SIZE)) % 2).xxx, 1.0f);

            dirLightCol = saturate(dl_ambient);
        }
        else
            dirLightCol = saturate(dl_ambient + dl_diffuse + dl_spec);
        */
    }
    else
        dirLightCol = saturate(dl_ambient + dl_spec + dl_diffuse);

    
    float4 pointLightSum = { 0, 0, 0, 0 };

    for (int i = 0; i < 10; i++)
    {
        pointLightSum += calcPointLight(input.wPos, normal, toEye, mat, pointLights[i]);
    }
    
    return saturate(dirLightCol + pointLightSum);
    
}