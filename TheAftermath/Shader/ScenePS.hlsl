
cbuffer SkyLight : register(b1) {
	float4 Light;
};

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}