struct VertexOutput {
	float4 Pos : SV_POSITION;
	float3 WNormal : NORMAL;
	float2 UV0 : TEXCOORD;
	uint VertexID : ID;
};

cbuffer GBufferCB : register(b0) {
	matrix V;
	matrix P;
	float4 Light;
};

float4 main(VertexOutput vOut) : SV_TARGET
{
	return float4(1,1,1,1);
}