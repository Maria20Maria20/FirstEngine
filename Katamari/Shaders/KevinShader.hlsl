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
    PointLight pointLights[6];
};


Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0)
{
    AddressU = WRAP;
    AddressV = WRAP;
};

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

float4 calcDirectLight(float3 wPos, float3 normal, float3 toEye, Material mat, DirectionalLight dirLight)
{
    float4 dl_ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 dl_diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 dl_spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
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
    
    return float4(saturate(dl_ambient + dl_spec + dl_diffuse).rgb, 1.0f);
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

float4 PSMain(PS_IN input) : SV_Target
{
    float4 pixelColor = DiffuseMap.Sample(Sampler, (input.texCoord.xy));
    
    Material mat =
    {
        pixelColor,
        pixelColor,
        pixelColor * 0.2f,
        { 0.0f, 0.0f, 0.0f, 0.0f }
    };
    
    float3 normal = normalize(input.normal);
    float3 toEye = normalize(cam_pos - input.wPos);
 
    float4 dirLightCol = calcDirectLight(input.wPos, normal, toEye, mat, dLight);

    float4 pointLightSum = { 0, 0, 0, 0 };

    for (int i = 0; i < 6; i++)
    {
        pointLightSum += calcPointLight(input.wPos, normal, toEye, mat, pointLights[i]);
    }
    
    return float4(saturate(dirLightCol + pointLightSum).rgb, 0.3f);
    
}