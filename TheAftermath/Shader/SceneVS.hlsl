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
"CBV(b0),"\
"CBV(b1)"
[RootSignature(SceneRoot)]
float4 main(float4 position : POSITION, float3 normal : NORMAL, float2 UV0 : TEXCOORD) : SV_POSITION
{
	return position;
}