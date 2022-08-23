Texture2D Texure : register(t0);
SamplerState TextureSampler : register(s0);

struct VertexOutput {
	float4 Position: SV_POSITION;
	float2 Tex: TEXCOORD;
};

float4 main(VertexOutput Out) : SV_TARGET
{
	return Texure.Sample(TextureSampler, Out.Tex);
}