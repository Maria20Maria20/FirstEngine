
cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 world; // Мировые преобразования
    // row_major float4x4 view; //Вид камеры
    // row_major float4x4 projection; // Проекция
};

struct VS_IN
{
	float4 pos : POSITION0;
	float4 col : COLOR0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float4 col : COLOR;
};

PS_IN VSMain( VS_IN input )
{
    PS_IN output = (PS_IN) 0;

    output.pos = mul(input.pos, world);
      // Полная цепочка преобразований
    // float4 worldPos = mul(input.pos, world);
    // float4 viewPos = mul(worldPos, view);
    //output.pos = mul(viewPos, projection);
    output.col = input.col;

    return output;
}

float4 PSMain( PS_IN input ) : SV_Target
{
	float4 col = input.col;
//#ifdef TEST
//	if (input.pos.x > 400) col = TCOLOR;
//#endif
	return col;
}