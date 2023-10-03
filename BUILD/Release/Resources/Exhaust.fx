// ===== ===== Variables ===== =====
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = true;
};

BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;
	
	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;
	
	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;
	
	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;
	
	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;
};

// ===== ===== Sample States ===== =====

SamplerState samplePoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border; // or mirror or clamp or border
	AddressV = Clamp; // or mirror or clamp or border
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

SamplerState sampleLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border; // or mirror or clamp or border
	AddressV = Clamp; // or mirror or clamp or border
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

SamplerState sampleAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Border; // or mirror or clamp or border
	AddressV = Clamp; // or mirror or clamp or border
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};


// ===== ===== IO Structs ===== =====

struct VS_INPUT
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};


// ===== ===== Vertex Shader ===== =====

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.UV = input.UV;
	output.Normal = input.Normal;
	output.Tangent = input.Tangent;
	return output;
};


// ===== ===== Pixel Shader ===== =====

float4 PS_POINT(VS_OUTPUT input) : SV_TARGET
{
	return float4(gDiffuseMap.Sample(samplePoint, input.UV));
};

float4 PS_LINEAR(VS_OUTPUT input) : SV_TARGET
{
	return float4(gDiffuseMap.Sample(sampleLinear, input.UV));
};

float4 PS_ANISOTROPIC(VS_OUTPUT input) : SV_TARGET
{
	return float4(gDiffuseMap.Sample(sampleAnisotropic, input.UV));
};


// ===== ===== Techniques ===== =====

technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_POINT()));
	}
};

technique11 LinearTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_LINEAR()));
	}
};

technique11 AnisotropicTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ANISOTROPIC()));
	}
};