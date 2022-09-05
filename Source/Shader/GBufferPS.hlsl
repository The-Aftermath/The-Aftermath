#define BASECOLOR_INDEX 0
#define GBUFFER_CB_INDEX 1

struct VertexOutput {
	float4 Pos : SV_POSITION;
	float3 WNormal : NORMAL;
	float2 UV0 : TEXCOORD;
	uint VertexID : ID;
};

struct GBufferCB {
	matrix V;
	matrix P;
	float4 Light;
};

struct GBuffer {
	float4 BaseColor : SV_TARGET0;
};

void main(VertexOutput vOut)
{
	ConstantBuffer<GBufferCB> bindless_cbuffers = ResourceDescriptorHeap[GBUFFER_CB_INDEX];
	GBuffer gbuffer;

	gbuffer.BaseColor = float4(1.f, 1.f, 1.f, 1.f);
}