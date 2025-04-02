cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 wvpMat;
    row_major float4x4 wMat;
    row_major float4x4 wMatInvTranspose;
    float3 cam_pos;
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
    float3 cam_pos : POSITION;
};

PS_IN VSMain( VS_IN input )
{
    PS_IN output = (PS_IN) 0;

    // output.pos = input.pos;
    output.pos = mul(input.pos, wvpMat);
    output.col = input.col;
    output.texCoord = input.texCoord;
    output.normal = input.normal;
    output.cam_pos = cam_pos;
  
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
    float3 pixelColor = input.col;
    
    float3 toEye = normalize(cam_pos - input.pos.xyz);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Material mat;
    mat.Ambient = float4(0.48f, 0.77f, 0.46f, 1.0f);
    mat.Diffuse = float4(0.48f, 0.77f, 0.46f, 1.0f);
    mat.Specular = float4(0.2f, 0.2f, 0.2f, 16.0f);
    
    DirectionalLight L;
    L.Ambient = float4(0.2f, 0.2f, 0.2f, 1.0f);
    L.Diffuse = float4(0.5f, 0.5f, 0.5f, 1.0f);
    L.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
    L.Direction = float4(0.0f, -1.0f, 0.0f, 1.0f);

    
    // The light vector aims opposite the direction the light rays travel.
    float3 lightVec = -L.Direction;
    // Add ambient term.
    ambient = mat.Ambient * L.Ambient;
    // Add diffuse and specular term, provided the surface is in
    // the line of site of the light.
    float diffuseFactor = dot(lightVec, input.normal);
    // Flatten to avoid dynamic branching.
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, input.normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }
    
    return float4(ambient + spec + diffuse);
}