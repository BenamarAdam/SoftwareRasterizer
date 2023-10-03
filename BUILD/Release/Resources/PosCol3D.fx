// ===== ===== Variables ===== =====
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WorldMatrix;
float4x4 gViewInverseMatrix : ViewInverseMatrix;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

float3 gLightDirection : LightDirection = float3(0.577f, -0.577f, 0.577f);
float gPI					= 3.1415f;
float gLightIntensity		= float(7.f);
float gShininess			= float(25.f);

RasterizerState gRasterizerState
{
	CullMode = back;
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
    DepthWriteMask = true;
    DepthFunc = less;
    StencilEnable = false;

    StencilReadMask = 0x0F;
    StencilWriteMask = 0x0F;

    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;

    FrontFaceStencilDepthFail = keep;
    BackFaceStencilDepthFail = keep;

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
	float4 Worldposition : WORLDPOSITION;
};


// ===== ===== Vertex Shader ===== =====

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.UV = input.UV;
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	output.Worldposition = mul(input.Position, gWorldMatrix);
	return output;
};


// ===== ===== Pixel Shader Functions ===== =====

float CalculatePhong(float3 specular, float phongExponent, float3 view, float3 normal)
{
    float3 reflect								= gLightDirection - 2.f * dot(normal, gLightDirection) * normal;
    float angleBetweenReflectAndLightDirection	= abs(dot(reflect, view));
    float phongValue							= specular.r * pow(angleBetweenReflectAndLightDirection, phongExponent);
    return phongValue;
}
float3 Phong(SamplerState state, VS_OUTPUT input)
{
	float3 binormal 							= normalize(cross(input.Normal, input.Tangent));
	float3x3 tangentSpaceAxis 					= float3x3(normalize(input.Tangent),binormal,input.Normal);
	float3 normalMapSample 						= gNormalMap.Sample(state, input.UV).xyz;
	float3 newNormal 							= 2.f * normalMapSample - 1.f;
	float3 tangentSpaceNormal					= normalize(mul(newNormal, tangentSpaceAxis));
	
	float observedArea							= dot(-tangentSpaceNormal, gLightDirection);
	observedArea								= saturate(observedArea);
	
    float phongValue = CalculatePhong(gSpecularMap.Sample(state, input.UV).xyz, gGlossinessMap.Sample(state, input.UV).x * gShininess,
					normalize(input.Worldposition.xyz - gViewInverseMatrix[3].xyz), tangentSpaceNormal);
	
    return float3(gLightIntensity * (observedArea / gPI) * (gDiffuseMap.Sample(state, input.UV).xyz + phongValue));
};

// ===== ===== Pixel Shader ===== =====

float4 PS_POINT(VS_OUTPUT input) : SV_TARGET
{
	return float4(Phong(samplePoint, input), 1.f);
};

float4 PS_LINEAR(VS_OUTPUT input) : SV_TARGET
{
	return float4(Phong(sampleLinear, input), 1.f);
};

float4 PS_ANISOTROPIC(VS_OUTPUT input) : SV_TARGET
{
	return float4(Phong(sampleAnisotropic, input), 1.f);
};


// ===== ===== Techniques ===== =====

technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
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
        SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
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
        SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ANISOTROPIC()));
	}
};