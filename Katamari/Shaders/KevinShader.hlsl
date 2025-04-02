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

cbuffer CBuf
{
    row_major float4x4 wvpMat; // for vertices (world -> view -> projection)
    row_major float4x4 wMat; // for vertices (only world)
    row_major float4x4 wMatInvTranspose; // for normals (only world)
    float3 cam_pos;
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

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};
float4 PSMain(PS_IN input) : SV_Target
{
    // float4 col = input.col;
    // float4 col = DiffuseMap.Sample(Sampler, input.tex.xy);
    //if (input.pos.x > 400)
    //    col = float4(0.0f, 1.0f, 0.0f, 1.0f);

    float4 pixelColor = DiffuseMap.Sample(Sampler, (input.texCoord.xy));
    
    float3 toEye = normalize(cam_pos - input.wPos);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Material mat;
    mat.Ambient = float4(0.88f, 0.86f, 0.86f, 1.0f);
    mat.Diffuse = float4(0.48f, 0.55f, 0.46f, 1.0f);
    mat.Specular = float4(0.7f, 0.7f, 0.7f, 16.0f);
    
    DirectionalLight L;
    L.Ambient = float4(0.5f, 0.5f, 0.5f, 1.0f);
    L.Diffuse = float4(0.5f, 0.5f, 0.5f, 1.0f);
    L.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
    L.Direction = float4(normalize(float3(-1.0f, -1.0f, 0.0f)), 0.0f);

    
    // The light vector aims opposite the direction the light rays travel.
    float3 lightVec = -L.Direction;
    // Add ambient term.
    ambient = mat.Ambient * L.Ambient;
    float3 normal = normalize(input.normal);
    // Add diffuse and specular term, provided the surface is in
    // the line of site of the light.
    float diffuseFactor = dot(lightVec, normal);
    // Flatten to avoid dynamic branching.
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }
    
    return float4(((ambient + spec + diffuse) * pixelColor).rgb, 0.3);
}
