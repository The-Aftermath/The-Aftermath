#define GBufferRoot "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED)"

struct VertexInput {
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 UV0 : TEXCOORD0;
	uint VertexID : ID;
};

struct VertexOutput {
	float4 Pos : SV_POSITION;
	float3 WNormal : NORMAL;
	float2 UV0 : TEXCOORD;
	uint VertexID : ID;
};

cbuffer GBufferCB {
	matrix V;
	matrix P;
	float4 Light;
};

[RootSignature(GBufferRoot)]
VertexOutput main(VertexInput pIn)
{
	VertexOutput vertex;
	vertex.Pos = float4(pIn.Pos, 1.0f);
	return vertex;
}