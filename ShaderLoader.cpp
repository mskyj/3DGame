#include "ShaderLoader.h"

// 適当シェーダー
const GLchar *vtxSrc = {
	"#version 120\n"
	"void main(void)"
	"{"
	"	vec3 position = vec3(gl_ModelViewMatrix * gl_Vertex);"
	"	vec3 normal = gl_NormalMatrix * gl_Normal;"
	"	vec3 lightVec = gl_LightSource[0].position.xyz - position;"
	"	float dis = length(lightVec);"
	"	lightVec = normalize(lightVec);"
	"    float attenuation = 1.0 / (gl_LightSource[0].constantAttenuation +"
	"       gl_LightSource[0].linearAttenuation * dis +"
	"       gl_LightSource[0].quadraticAttenuation * dis * dis);"
	"	vec3 normal_n = normalize(normal);"
	"	vec3 light = normalize(gl_LightSource[0].position.xyz);"
	"	float lgtdot = dot(light, normal_n);"
	"    vec3 eye = - normalize(position);"
	"	float eyedot = dot(eye, normal_n);"
	"	gl_Position = ftransform();"
	"	gl_TexCoord[0] = gl_MultiTexCoord0;"
	"    gl_TexCoord[1].s = eyedot;"
	"    gl_TexCoord[1].t = lgtdot;"
	"}"
};

const GLchar *frgSrc = {
	"#version 120\n"
	"uniform sampler2D texture1;"
	"uniform sampler2D texture2;"
	"uniform sampler2D toontex;"
	"uniform float drawType;"
	"void main (void)"
	"{"
	"	vec4 color = gl_FrontMaterial.diffuse;"
	"	float temp = drawType;"
	"	if( temp >= 8.0 )"
	"	{"
	"		vec4 tColor = texture2D(toontex, gl_TexCoord[1].st);"
	"		color.r *= tColor.r;"
	"		color.g *= tColor.g;"
	"		color.b *= tColor.b;"
	"		temp -= 8.0;"
	"	}"
	"	if( temp >= 4.0 )"
	"	{"
	"		vec4 sColor = texture2DProj(texture2, gl_TexCoord[0]);"
	"		color.r += sColor.r*sColor.a;"
	"		if( color.r > 1.0 )"
	"			color.r = 1.0;"
	"		color.g += sColor.g*sColor.a;"
	"		if( color.g > 1.0 )"
	"			color.g = 1.0;"
	"		color.b += sColor.b*sColor.a;"
	"		if( color.b > 1.0 )"
	"			color.b = 1.0;"
	"		temp -= 4.0;"
	"	}"
	"	if( temp >= 2.0 )"
	"	{"
	"		vec4 sColor = texture2DProj(texture2, gl_TexCoord[0]);"
	"		color.r *= sColor.r*sColor.a;"
	"		color.g *= sColor.g*sColor.a;"
	"		color.b *= sColor.b*sColor.a;"
	"		temp -= 2.0;"
	"	}"
	"	if( temp == 1.0 )"
	"	{"
	"		vec4 bColor = texture2DProj(texture1, gl_TexCoord[0]);"
	"		color.r *= bColor.r*bColor.a;"
	"		color.g *= bColor.g*bColor.a;"
	"		color.b *= bColor.b*bColor.a;"
	"		color.a *= bColor.a;"
	"	}"
	"	gl_FragColor = color.rgba;"
	"}"
};

bool LoadShaderFile( GLuint handle, const char** src )
{
	glShaderSource( handle, 1, src, NULL );

	if ( glGetError() != GL_NO_ERROR ) {
		cout << "Shader Loader Error: cannot create shader object" << endl;
		return false;
	}

	glCompileShader( handle );
	GLint result;
	glGetShaderiv( handle, GL_COMPILE_STATUS, &result );

	if ( glGetError() != GL_NO_ERROR || result == GL_FALSE )
	{
		cout << "Shader Loader Error: cannot compile shader object" << endl;

		int	length;
		glGetShaderiv( handle, GL_INFO_LOG_LENGTH, &length );
		if ( length > 0 )
		{
			int	l;
			GLcharARB *info_log = new GLcharARB[ length ];
			glGetShaderInfoLog( handle, length, &l, info_log );
			cout << info_log << endl;
			delete [] info_log;
		}
		return false;
	}

	return true;
}

bool LoadShader( GLuint* program )
{
	GLuint vtxShader;
    GLuint frgShader;
	GLuint prog;
    GLint linked;

	// シェーダオブジェクトの作成
	vtxShader = glCreateShader(GL_VERTEX_SHADER);
    frgShader = glCreateShader(GL_FRAGMENT_SHADER);

	// バーテックスシェーダのロードとコンパイル
	if( !LoadShaderFile( vtxShader, &vtxSrc ) )
		return false;

	// フラグメントシェーダのロードとコンパイル
	if( !LoadShaderFile( frgShader, &frgSrc ) )
		return false;

	// プログラムオブジェクトの作成
	prog = glCreateProgram();

	// シェーダオブジェクトのシェーダプログラムへの登録
	glAttachShader(prog, vtxShader);
    glAttachShader(prog, frgShader);

	// シェーダオブジェクトの削除
	glDeleteShader(vtxShader);
    glDeleteShader(frgShader);

	// シェーダプログラムのリンク
	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &linked);
	if ( glGetError() != GL_NO_ERROR || linked == GL_FALSE )
	{
		cout << "Shader Loader Error: cannot link shader object" << endl;

		int	length;
		glGetShaderiv( prog, GL_INFO_LOG_LENGTH, &length );
		if ( length > 0 )
		{
			int	l;
			GLcharARB *info_log = new GLcharARB[ length ];
			glGetShaderInfoLog( prog, length, &l, info_log );
			cout << info_log << endl;
			delete [] info_log;
		}
		return false;
	}

	*program = prog;

	return true;
}