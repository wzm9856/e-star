$input a_position
$output v_depth

#include "../../common/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_depth = (vec3(gl_Position.z / gl_Position.w)+vec3(1.0))/vec3(2.0);
}