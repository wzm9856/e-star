$input a_position, a_normal, a_color0
$output v_wpos, v_viewdir, v_lightdir, v_normal, v_color0

#include "../../common/common.sh"

uniform vec4 u_lightPos;
uniform vec4 u_eyePos;


void main()
{
	v_wpos = mul(u_model[0], vec4(a_position, 1.0)).xyz;
	v_viewdir = u_eyePos.xyz - v_wpos;
	v_lightdir = u_lightPos.xyz - v_wpos;
	v_normal = normalize(mul(mat3(u_model[0]), a_normal));
	v_color0 = a_color0.xyz;
	gl_Position = mul(u_viewProj, vec4(v_wpos, 1.0));
}