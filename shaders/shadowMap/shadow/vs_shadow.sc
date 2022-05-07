$input a_position
$output v_depth

#include "../../common/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
#if BGFX_SHADER_LANGUAGE_GLSL!=0
	v_depth = vec3_splat((gl_Position.z / gl_Position.w + 1.0)/2.0);
#endif
#if BGFX_SHADER_LANGUAGE_HLSL!=0
	v_depth = vec3_splat(gl_Position.z / gl_Position.w);
#endif
}