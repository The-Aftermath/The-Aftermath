struct VertexInput{
	float4 position: POSITION;
	float3 normal : NORMAL;
	float2 UV0: TEXCOORD;
};

struct VertexOutput {
	float4 Position: SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV0: TEXCOORD;
};

cbuffer SceneCB : register(b0) {
	matrix MVP;
	float4 Light;
};

#define SceneRoot "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
"CBV(b0)"
[RootSignature(SceneRoot)]
VertexOutput main(VertexInput pIn)
{
	VertexOutput vertex;
	vertex.Position = mul(pIn.position, MVP);
	vertex.Normal = pIn.normal;
	vertex.UV0 = pIn.UV0;

	return vertex;
}