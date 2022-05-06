$input v_wpos, v_viewdir, v_lightdir, v_tangent, v_normal, v_texcoord0

#include "../common/common.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);
uniform vec4 u_lightRGB;
uniform mat4 u_lightMtx;
SAMPLER2DSHADOW(s_shadowMap, 0);

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
	
	float bias = dot(normal, lightdir)>0.0 ? 0.03*(1.1-dot(normal, lightdir)) : 0.005;
	vec4 shadowCoord = mul(u_lightMtx, vec4(v_wpos, 1.0));
	vec3 shadowCoordNDC = shadowCoord.xyz / shadowCoord.w;
	shadowCoordNDC = shadowCoordNDC*0.5+0.5;
	
	float t = 1.0 / 512.0; //t->texelSize
	vec4 lightIntensity = vec4(0.0);
	lightIntensity += vec4(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2(-t,-t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( 0,-t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( t,-t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2(-t, t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( 0, t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( t, t), shadowCoordNDC.z-bias)));
	lightIntensity += vec4(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2(-t, 0), shadowCoordNDC.z-bias)));
	lightIntensity += vec4(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( 0, 0), shadowCoordNDC.z-bias)));
	lightIntensity += vec4(shadow2D(s_shadowMap, vec3(shadowCoordNDC.xy+vec2( t, 0), shadowCoordNDC.z-bias)));
	lightIntensity /= 9.0;
	
	float squareDistance = dot(shadowCoordNDC.xy*2.0-1.0, shadowCoordNDC.xy*2.0-1.0);
	lightIntensity.x *= squareDistance>0.05 ? 1.1 - squareDistance*2.0 : 1.0; // 聚光灯效果
	lightIntensity.x = max(lightIntensity.x, 0.0);
	
	gl_FragColor = toGamma(vec4(ambient + (diffuse + specular)*lightIntensity.x, 0.0));
	//gl_FragColor = vec4(v_texcoord0,0,0);
}