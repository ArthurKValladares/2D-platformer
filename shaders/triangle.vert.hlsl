struct VSInput {
    float3 Pos : POSITION0;
    float3 Color : COLOR0;
};

struct VSOutput {
	float4 Pos : SV_POSITION;
    float3 Color : COLOR0;
};

VSOutput main(VSInput input) {
	VSOutput output;
	output.Color = input.Color;
	output.Pos = float4(input.Pos.xyz, 1.0);
	return output;
}