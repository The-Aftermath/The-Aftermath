SamplerState TextureSampler : register(s0);

struct VertexOutput {
	float4 Position: SV_POSITION;
	float2 Tex: TEXCOORD;
};

cbuffer OutputCB : register(b0) {
	uint index;
};

float4 main(VertexOutput Out) : SV_TARGET
{
	Texture2D<float4> myTexture = ResourceDescriptorHeap[index];
	return myTexture.Sample(TextureSampler, Out.Tex);
}