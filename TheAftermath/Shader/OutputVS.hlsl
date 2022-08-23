struct VertexOutput {
	float4 Position: SV_POSITION;
	float2 Tex: TEXCOORD;
};

#define MyRoot "RootFlags(CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | SAMPLER_HEAP_DIRECTLY_INDEXED),DescriptorTable(SRV(t0)),StaticSampler(s0)"
[RootSignature(MyRoot)]
VertexOutput main(uint VertexID : SV_VertexID) {
	VertexOutput Out;
	Out.Tex = float2((VertexID << 1) & 2, VertexID & 2);
	Out.Position = float4(Out.Tex * float2(0.5f, -0.5f) + float2(-0.5f, 0.5f), 0.0f, 1.0f);
	return Out;
}