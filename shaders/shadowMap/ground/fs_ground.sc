$input v_wpos, v_viewdir, v_lightdir, v_normal, v_color0

#include "../../common/common.sh"

uniform vec4 u_lightRGB;
uniform mat4 u_lightMtx;
SAMPLER2DSHADOW(s_shadowMap, 0);

void main()
{

#if BGFX_SHADER_LANGUAGE_HLSL!=0
	u_lightMtx = transpose(u_lightMtx);
#endif
	vec3 normal_lightdir = normalize(v_lightdir);
	vec3 normal_viewdir = normalize(v_viewdir);
	float NdL = dot(v_normal,normal_lightdir);
	
	//bling-phong
	float ambientStrenth = 1.0;
	vec3 ambient = v_color0 * vec3_splat(ambientStrenth);
	
	float r2 = dot(v_lightdir, v_lightdir);
	vec3 diffuse = v_color0 * u_lightRGB.xyz / r2 * max(0, NdL);
	
	vec3 half_vec = (normal_lightdir + normal_viewdir) / length(normal_lightdir + normal_viewdir);
	vec3 specular = u_lightRGB.xyz / r2 * max(0, pow(dot(v_normal,half_vec),100.0));
	
	gl_FragColor = vec4(ambient + diffuse + specular, 0.0);
	//gl_FragColor = vec4(v_color0, 0);
	//gl_FragColor = vec4(0.5,0.5,0.5,0);
	
	//float bias = 0.005;
	float bias = NdL>0.0 ? 0.03*(1.1-NdL) : 0.005;
	vec4 shadowCoord = mul(u_lightMtx, vec4(v_wpos, 1.0));
	vec3 shadowCoordNDC = shadowCoord.xyz / shadowCoord.w;
	shadowCoordNDC = shadowCoordNDC*0.5+0.5;
	
	float t = 1.0 / 512.0; //t->texelSize
	vec4 lightIntensity = vec4_splat(0.0);
	lightIntensity += vec4_splat(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2(-t,-t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4_splat(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( 0,-t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4_splat(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( t,-t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4_splat(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2(-t, t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4_splat(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( 0, t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4_splat(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( t, t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4_splat(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2(-t, 0), shadowCoordNDC.z-bias)));
	lightIntensity += vec4_splat(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( 0, 0), shadowCoordNDC.z-bias)));
	lightIntensity += vec4_splat(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( t, 0), shadowCoordNDC.z-bias)));
	lightIntensity /= 9.0;
	
	float squareDistance = dot(shadowCoordNDC.xy*2.0-1.0, shadowCoordNDC.xy*2.0-1.0);
	lightIntensity.x *= squareDistance>0.05 ? 1.1 - squareDistance*2.0 : 1.0; // 聚光灯效果
	lightIntensity.x = max(lightIntensity.x, 0.0);
	gl_FragColor = vec4(vec3_splat(lightIntensity.x), 0);
}