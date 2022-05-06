$input a_position, a_normal, a_texcoord0, a_tangent
$output v_wpos, v_viewdir, v_lightdir, v_normal, v_texcoord0, v_tangent

#include "../common/common.sh"

uniform vec4 u_lightPos;
uniform vec4 u_eyePos;


void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(u_model[0])));
	//在世界坐标系算光照
	v_wpos = mul(u_model[0], vec4(a_position, 1.0)).xyz;
	v_viewdir = u_eyePos.xyz - v_wpos;
	v_lightdir = u_lightPos.xyz - v_wpos;
	v_normal = normalize(mul(normalMatrix, a_normal));
	v_tangent = normalize(mul(normalMatrix, a_tangent));
	v_texcoord0 = a_texcoord0;
	gl_Position = mul(u_viewProj, vec4(v_wpos, 1.0));
}