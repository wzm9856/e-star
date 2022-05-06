$input v_wpos, v_viewdir, v_lightdir, v_tangent, v_normal, v_texcoord0

#include "../common/common.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);
uniform vec4 u_lightRGB;

void main()
{
	vec3 normal		= normalize(v_normal);
	vec3 tangent 	= normalize(v_tangent);
	vec3 lightdir 	= normalize(v_lightdir);
	vec3 viewdir 	= normalize(v_viewdir);
	
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);
	vec3 normalTex = texture2D(s_texNormal, v_texcoord0).xyz;
	normal = normalize(TBN * normalTex);
	
	vec3 color = toLinear(texture2D(s_texColor, v_texcoord0).xyz);
	
	//bling-phong
	vec3 ambient = 0.1 * color * vec3(1.0f, 1.0f, 1.0f);
	
	float r2 = dot(v_lightdir, v_lightdir);
	vec3 diffuse = 0.8 * color * u_lightRGB.xyz / r2 * max(0, dot(normal,lightdir));
	
	vec3 half_vec = (lightdir + viewdir) / length(lightdir + viewdir);
	vec3 specular = 0.1 * u_lightRGB.xyz / r2 * max(0, pow(dot(normal,half_vec),50.0));
	
	gl_FragColor = toGamma(vec4(ambient + diffuse + specular, 0.0));
	//gl_FragColor = vec4(v_texcoord0,0,0);
}