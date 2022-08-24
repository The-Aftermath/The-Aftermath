SamplerState TextureSampler : register(s0);

struct VertexOutput {
	float4 Position: SV_POSITION;
	float2 Tex: TEXCOORD;
};

float4 main(VertexOutput Out) : SV_TARGET
{
	Texture2D<float4> myTexture = ResourceDescriptorHeap[0];
	return myTexture.Sample(TextureSampler, Out.Tex);
}