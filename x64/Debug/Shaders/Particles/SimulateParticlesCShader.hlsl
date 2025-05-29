
float3 mod289(float3 x)
{
    return x - floor(x / 289.0) * 289.0;
}

float4 mod289(float4 x)
{
    return x - floor(x / 289.0) * 289.0;
}

float4 permute(float4 x)
{
    return mod289((x * 34.0 + 1.0) * x);
}

float4 taylorInvSqrt(float4 r)
{
    return 1.79284291400159 - r * 0.85373472095314;
}

float snoise(float3 v)
{
    const float2 C = float2(1.0 / 6.0, 1.0 / 3.0);

    // First corner
    float3 i = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);

    // Other corners
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1.0 - g;
    float3 i1 = min(g.xyz, l.zxy);
    float3 i2 = max(g.xyz, l.zxy);

    // x1 = x0 - i1  + 1.0 * C.xxx;
    // x2 = x0 - i2  + 2.0 * C.xxx;
    // x3 = x0 - 1.0 + 3.0 * C.xxx;
    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy;
    float3 x3 = x0 - 0.5;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    float4 p =
        permute(permute(permute(i.z + float4(0.0, i1.z, i2.z, 1.0))
            + i.y + float4(0.0, i1.y, i2.y, 1.0))
            + i.x + float4(0.0, i1.x, i2.x, 1.0));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float4 j = p - 49.0 * floor(p / 49.0); // mod(p,7*7)

    float4 x_ = floor(j / 7.0);
    float4 y_ = floor(j - 7.0 * x_); // mod(j,N)

    float4 x = (x_ * 2.0 + 0.5) / 7.0 - 1.0;
    float4 y = (y_ * 2.0 + 0.5) / 7.0 - 1.0;

    float4 h = 1.0 - abs(x) - abs(y);

    float4 b0 = float4(x.xy, y.xy);
    float4 b1 = float4(x.zw, y.zw);

    //float4 s0 = float4(lessThan(b0, 0.0)) * 2.0 - 1.0;
    //float4 s1 = float4(lessThan(b1, 0.0)) * 2.0 - 1.0;
    float4 s0 = floor(b0) * 2.0 + 1.0;
    float4 s1 = floor(b1) * 2.0 + 1.0;
    float4 sh = -step(h, 0.0);

    float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

    float3 g0 = float3(a0.xy, h.x);
    float3 g1 = float3(a0.zw, h.y);
    float3 g2 = float3(a1.xy, h.z);
    float3 g3 = float3(a1.zw, h.w);

    // Normalise gradients
    float4 norm = taylorInvSqrt(float4(dot(g0, g0), dot(g1, g1), dot(g2, g2), dot(g3, g3)));
    g0 *= norm.x;
    g1 *= norm.y;
    g2 *= norm.z;
    g3 *= norm.w;

    // Mix final noise value
    float4 m = max(0.6 - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    m = m * m;

    float4 px = float4(dot(x0, g0), dot(x1, g1), dot(x2, g2), dot(x3, g3));
    return 42.0 * dot(m, px);
}


#include "ParticlesGlobals.h"

cbuffer sceneConstantBuffer : register(b0)
{
    float4 camPosition;
    float dt;
    float rngSeed;
    
    float2 padding;
};

cbuffer aliveListCountConstantBuffer : register(b1)
{
    uint nbAliveParticles;

    uint3 aliveListPadding;
};


cbuffer simulateParticlesConstantBuffer : register(b2)
{
    float4 force;
}

RWBuffer<uint> indirectDrawArgs : register(u0);
AppendStructuredBuffer<ParticleIndexElement> aliveParticleIndex : register(u1);
AppendStructuredBuffer<uint> deadParticleIndex : register(u2);
RWStructuredBuffer<Particle> particleList : register(u3);
ConsumeStructuredBuffer<ParticleIndexElement> aliveParticleIndexIn : register(u4);
RWBuffer<uint> indirectDispatchArgs : register(u5);

float3 snoiseVec3(float3 x)
{

    float s = snoise(float3(x));
    float s1 = snoise(float3(x.y - 19.1, x.z + 33.4, x.x + 47.2));
    float s2 = snoise(float3(x.z + 74.2, x.x - 124.5, x.y + 99.4));
    float3 c = float3(s, s1, s2);
    return c;

}

float3 curlNoise(float3 p)
{
    float epsilon = 0.1;
    float3 dx = float3(epsilon, 0.0, 0.0);
    float3 dy = float3(0.0, epsilon, 0.0);
    float3 dz = float3(0.0, 0.0, epsilon);

    float3 p_x0 = snoiseVec3(p - dx);
    float3 p_x1 = snoiseVec3(p + dx);
    float3 p_y0 = snoiseVec3(p - dy);
    float3 p_y1 = snoiseVec3(p + dy);
    float3 p_z0 = snoiseVec3(p - dz);
    float3 p_z1 = snoiseVec3(p + dz);

    float x = p_y1.z - p_y0.z - p_z1.y + p_z0.y;
    float y = p_z1.x - p_z0.x - p_x1.z + p_x0.z;
    float z = p_x1.y - p_x0.y - p_y1.x + p_y0.x;

    float divisor = 1.0 / (2.0 * epsilon);

    return normalize(float3(x, y, z) * divisor);
}

// Particle Bounces

Texture2D NormalMap : register(t0);
Texture2D WorldPosMap : register(t1);
SamplerState Sam : register(s0);

cbuffer viewProjectionBuffer : register(b3)
{
    row_major float4x4 vMat;
    row_major float4x4 pMat;
    bool hasBounce;
}

//256 particles per thread group
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID, uint groupId : SV_GroupIndex) //SV_GroupIndex is a flatenned index
{
    //first thread to initialise the arguments
    if (id.x == 0)
    {
        indirectDrawArgs[0] = 0; // Number of primitives reset to zero
        indirectDrawArgs[1] = 1; // Number of instances is always 1
        indirectDrawArgs[2] = 0;
        indirectDrawArgs[3] = 0;
        indirectDrawArgs[4] = 0;

        indirectDispatchArgs[0] = 0;
        indirectDispatchArgs[1] = 1;
        indirectDispatchArgs[2] = 1;
    }

    // Wait after draw args are written so no other threads can write to them before they are initialized
    GroupMemoryBarrierWithGroupSync();

    ParticleIndexElement particle = aliveParticleIndexIn.Consume();
    Particle p = particleList[particle.index];
    if (p.age > 0)
    {
        
        //lifeSpan < 0.0 => invulnerable
        if (p.lifeSpan >= 0.0)
        {
            p.age -= dt;
        }
        
        float4 acceleration = force / p.mass;
            
        
        p.velocity.xyz += acceleration * dt;
        p.position.xyz += p.velocity.xyz * dt;
        
        p.screenSpin += p.screenSpinSpeed * dt;
        
        
        float4 p_viewPos_prev = mul(float4(p.position.xyz, 1.0f), vMat);
        p_viewPos_prev = p_viewPos_prev / p_viewPos_prev.w;
        float4 p_projPos_prev = mul(p_viewPos_prev, pMat);
        p_projPos_prev = p_projPos_prev / p_projPos_prev.w;
        
        
        if (abs(p_projPos_prev.x) < 1 && abs(p_projPos_prev.y) < 1
            && p_projPos_prev.z > 0 && p_projPos_prev.z < 1
            )
        {
            int2 prevSample = int2(float2(p_projPos_prev.x + 1, 1 - p_projPos_prev.y) * 0.5 * float2(1000, 800));
            float3 worldMapValue_prev = WorldPosMap[prevSample].xyz;
            float4 viewValue_prev = mul(float4(worldMapValue_prev, 1.0f), vMat);
            viewValue_prev = viewValue_prev / viewValue_prev.w;
        
            float deltaZ_prev = p_viewPos_prev.z - viewValue_prev.z;
        
            p.velocity.xyz += acceleration * dt;
            p.position.xyz += p.velocity.xyz * dt;
            float4 p_position_2x = float4(p.position.xyz + p.velocity.xyz * 4 * dt, 1.0f);
        
            float4 p_viewPos_next = mul(p_position_2x, vMat);
            p_viewPos_next = p_viewPos_next / p_viewPos_next.w;
            float4 p_projPos_next = mul(p_viewPos_next, pMat);
            p_projPos_next = p_projPos_next / p_projPos_next.w;
            
            int2 nextSample = int2(float2(p_projPos_next.x + 1, 1 - p_projPos_next.y) * 0.5 * float2(1000, 800));
            float3 worldMapValue_next = WorldPosMap[nextSample].xyz;
            float4 viewValue_next = mul(float4(worldMapValue_next, 1.0f), vMat);
            viewValue_next = viewValue_next / viewValue_next.w;
        
            float deltaZ_next = p_viewPos_next.z - viewValue_next.z;
        
        
            if ((deltaZ_prev < 0) && (deltaZ_next > 0)
                //&& (deltaZ_next > -0.5)
                )
            {
                float4 normal = normalize(NormalMap[nextSample]);
                if (dot(normal.xyz, p.velocity.xyz) < 0)
                {
                    float3 newVelocity = reflect(p.velocity.xyz, normal.xyz);
                    newVelocity += 4 * (newVelocity * normal.xyz) * normal.xyz;
                
                    
                    float alpha = 1 - smoothstep(0.0, abs(p.lifeSpan), p.age);
                    float p_size = p.sizeStart + alpha * (p.sizeEnd - p.sizeStart);
                    
                    p.position.xyz += newVelocity.xyz * dt + normal.xyz * p_size;
                    p.velocity = float4(newVelocity, 0);
                }
            
            }
            else if ((deltaZ_prev > 0) && (deltaZ_next < 0))
            {
                float4 normal = normalize(NormalMap[prevSample]);
                if (dot(normal.xyz, p.velocity.xyz) > 0)
                {
                    float3 newVelocity = reflect(p.velocity.xyz, -normal.xyz);
                
                    float alpha = 1 - smoothstep(0.0, abs(p.lifeSpan), p.age);
                    float p_size = p.sizeStart + alpha * (p.sizeEnd - p.sizeStart);
                    
                    p.position.xyz += newVelocity.xyz * dt - normal.xyz * p_size;
                    p.velocity = float4(newVelocity, 0);
                }
            
            }
        }
        else
        {
            p.velocity.xyz += acceleration * dt;
            p.position.xyz += p.velocity.xyz * dt;
        }
        
        
        
        if (p.age > 0)
        {
            particle.distance = length(p.position - camPosition);
            aliveParticleIndex.Append(particle);

            InterlockedAdd(indirectDrawArgs[0], 1);
            InterlockedAdd(indirectDispatchArgs[0], 1);
            
        }
        else
        {
            p.age = -1.0;
            deadParticleIndex.Append(particle.index);
        }
    }

    particleList[particle.index] = p;
}
