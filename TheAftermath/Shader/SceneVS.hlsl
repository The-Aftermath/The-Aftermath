struct VertexOutput {
	float4 Position: SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV0: TEXCOORD;
};

cbuffer Global : register(b0) {
	matrix MVP;
};

#define SceneRoot "RootFlags(0),"\
"CBV(b0),"\
"CBV(b1)"
[RootSignature(SceneRoot)]
float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}