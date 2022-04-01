$input v_wpos, v_viewdir, v_lightdir, v_normal, v_texcoord0

#include "./common/common.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);
uniform vec4 u_lightRGB;

void main()
{
	vec3 normal_lightdir = normalize(v_lightdir);
	vec3 normal_viewdir = normalize(v_viewdir);
	
	vec3 color = texture2D(s_texColor, v_texcoord0).xyz;
	
	//bling-phong
	vec3 ambient = color * vec3(1.0f, 1.0f, 1.0f);
	
	float r2 = dot(v_lightdir, v_lightdir);
	vec3 diffuse = color * u_lightRGB.xyz / r2 * max(0, dot(v_normal,normal_lightdir));
	
	vec3 half_vec = (normal_lightdir + normal_viewdir) / length(normal_lightdir + normal_viewdir);
	vec3 specular = u_lightRGB.xyz / r2 * max(0, pow(dot(v_normal,half_vec),100.0));
	
	gl_FragColor = vec4(ambient + diffuse + specular, 0.0);
	//gl_FragColor = vec4(v_texcoord0,0,0);
}