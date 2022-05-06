$input a_position, a_tangent, a_normal, a_texcoord0
$output v_wpos, v_viewdir, v_lightdir, v_tangent, v_normal, v_texcoord0

#include "../common/common.sh"

uniform vec4 u_lightPos;
uniform vec4 u_eyePos;
uniform mat3 u_normalMtx;

void main()
{
	v_wpos = mul(u_model[0], vec4(a_position, 1.0)).xyz;
	v_viewdir = u_eyePos.xyz - v_wpos;
	v_lightdir = u_lightPos.xyz - v_wpos;
	v_tangent = normalize(mul(u_normalMtx, a_tangent));
	v_normal = normalize(mul(u_normalMtx, a_normal));
	v_texcoord0 = vec2(a_texcoord0.x, -a_texcoord0.y);
	gl_Position = mul(u_viewProj, vec4(v_wpos, 1.0));
}