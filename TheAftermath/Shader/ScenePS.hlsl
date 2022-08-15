
cbuffer SceneCB : register(b0) {
	matrix MVP;
	float4 Light;
};

float4 main() : SV_TARGET
{
	return Light;
}