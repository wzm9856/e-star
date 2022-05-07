$input a_position, a_texcoord0
$output v_texcoord0

#include "../../common/common.sh"

void main()
{
	v_texcoord0 = a_texcoord0;
#if BGFX_SHADER_LANGUAGE_HLSL!=0
	a_position.z = a_position.z+1.0;
#endif
	gl_Position = vec4(a_position, 1.0);
}