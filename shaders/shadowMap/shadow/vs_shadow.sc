$input a_position
$output v_depth

#include "../../common/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_depth = (vec3_splat(gl_Position.z / gl_Position.w)+vec3_splat(1.0))/vec3_splat(2.0);
}