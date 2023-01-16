/*[DirectX9]*/
matrix MVP;
int filterMin = 2;
int filterMag = 2;
int filterMip = 2;
texture texture1;
float4	uvSO;
float2	R;
sampler sampler1 =	sampler_state
{
	Texture		= <texture1>;
	MinFilter	=  <filterMin>;
	MagFilter	=  <filterMag>;
	MipFilter	=  <filterMip>;
};

struct VInput{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
};
struct VOutput{
	float4 Pos		: POSITION;
	float4 Color	: TEXCOORD1;
};

VOutput Vshader(VInput input)
{
	VOutput output;
	float4 Pos		=	input.Pos;
	Pos.x			=	(Pos.x*uvSO.x + uvSO.z - 0.5)*R.x;
	Pos.y			=	(0.5 - (Pos.y*uvSO.y + uvSO.w))*R.y;
	output.Pos		=	mul(Pos, MVP);
	output.Color	=	input.Color;
	return output;
}
float4	Pshader(VOutput output) : COLOR0
{
	return output.Color;
}

technique main
{
	pass P
	{
		VertexShader = compile vs_2_0 Vshader();
		PixelShader  = compile ps_2_0 Pshader();
	}
}
/*[/DirectX9]*/

/*[OpenglES_Vertex]*/
attribute vec4	inPosition;
attribute vec4	inColor;
varying vec4	passColor0;
uniform vec4	uvSO;
uniform vec2	R;
uniform mat4	MVP;
void main()
{
	vec4 Pos		=	inPosition;
	Pos.x			=	(Pos.x*uvSO.x + uvSO.z - 0.5)*R.x;
	Pos.y			=	(0.5 - (Pos.y*uvSO.y + uvSO.w))*R.y;
	passColor0		=	vec4(inColor.b, inColor.g, inColor.r, inColor.a);
	gl_Position	=	MVP * Pos;
}

/*[/OpenglES_Vertex]*/
/*[OpenglES_Pixel]*/

precision lowp float;
varying vec4 passColor0;
void main()
{
	gl_FragColor = passColor0;
}

/*[/OpenglES_Pixel]*/
