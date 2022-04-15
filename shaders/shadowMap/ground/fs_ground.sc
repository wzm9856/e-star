$input v_wpos, v_viewdir, v_lightdir, v_normal, v_color0

#include "../../common/common.sh"

uniform vec4 u_lightRGB;

void main()
{
	vec3 normal_lightdir = normalize(v_lightdir);
	vec3 normal_viewdir = normalize(v_viewdir);
	
	//bling-phong
	float ambientStrenth = 1f;
	vec3 ambient = v_color0 * vec3(ambientStrenth);
	
	float r2 = dot(v_lightdir, v_lightdir);
	vec3 diffuse = v_color0 * u_lightRGB.xyz / r2 * max(0, dot(v_normal,normal_lightdir));
	
	vec3 half_vec = (normal_lightdir + normal_viewdir) / length(normal_lightdir + normal_viewdir);
	vec3 specular = u_lightRGB.xyz / r2 * max(0, pow(dot(v_normal,half_vec),100.0));
	
	gl_FragColor = vec4(ambient + diffuse + specular, 0.0);
	//gl_FragColor = vec4(v_color0, 0);
	//gl_FragColor = vec4(0.5,0.5,0.5,0);
}