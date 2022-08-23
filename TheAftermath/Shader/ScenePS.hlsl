
Texture2D<float4> Tex[] : register(t0);
SamplerState MeshTextureSampler:register(s2);

cbuffer SceneCB : register(b0) {
	matrix MVP;
	float4 Light;
};

struct VertexOutput {
	float4 Position: SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV0: TEXCOORD;
	uint Vertex_id: VERTEX_ID;
};

float4 main(VertexOutput vOut) : SV_TARGET
{
	return Tex[NonUniformResourceIndex(vOut.Vertex_id)].Sample(MeshTextureSampler,float2(vOut.UV0));
}