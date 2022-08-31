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

struct GBuffer {
	float4 BaseColor : SV_TARGET0;
};

void main(VertexOutput vOut)
{
	GBuffer gbuffer;

	gbuffer.BaseColor = float4(1.f, 1.f, 1.f, 1.f);
}