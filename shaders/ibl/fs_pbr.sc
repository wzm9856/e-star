$input v_wpos, v_viewdir, v_lightdir, v_tangent, v_normal, v_originNormal, v_texcoord0, temp

#include "../common/common.sh"

SAMPLER2D(s_texColor, 1);
SAMPLER2D(s_texNormal, 2);
SAMPLER2DSHADOW(s_shadowMap, 3);
SAMPLERCUBE(s_texSkyLod, 4);
SAMPLERCUBE(s_texSkyIrr, 5);
SAMPLER2D(s_texBrdfLut, 6);
SAMPLER2D(s_texAORM, 7);
uniform vec4 u_lightRGB;

#define PI 3.14159265359

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
void main(){
	
	float r2 = dot(v_lightdir, v_lightdir);
	vec3 lightdir 	= normalize(v_lightdir); //着色点指向相机/光源
	vec3 viewdir 	= normalize(v_viewdir);
	vec3 half_vec	= normalize(lightdir + viewdir);
	vec3 normal 	= normalize(v_normal);
	vec3 tangent 	= normalize(v_tangent);
	
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);
	vec3 normalTex = texture2D(s_texNormal, v_texcoord0).xyz;
	normal = normalize(mul(TBN, normalTex));
	float NdV = dot(normal, viewdir);
	float NdL = dot(normal, lightdir);
	vec3 reflectdir	= normalize(reflect(-viewdir, normal));
	
	vec3 albedo	= toLinear(texture2D(s_texColor, v_texcoord0).xyz);
	vec4 aorm 	= texture2D(s_texAORM, v_texcoord0);
	float ao 		= aorm.x;
	float rough 	= aorm.y-0.002;
	float matallic 	= aorm.z;
	
	vec3 F0	= mix(vec3_splat(0.04), albedo, matallic); //金属的反射光有颜色，非金属就是白的
	vec3 F  = fresnelSchlick(max(dot(half_vec, viewdir), 0.0), F0);
	float D = DistributionGGX(normal, half_vec, rough);
	float G = GeometrySmith(normal, viewdir, lightdir, rough);
	vec3 specular = D * G * F / (4.0 * max(NdV, 0.0) * max(NdL, 0.0) + 0.001); 
	
	vec3 kD = (vec3_splat(1.0) - F) * (1.0 - matallic);
	
	vec3 irradiance = textureCube(s_texSkyIrr,v_originNormal).xyz;
	const float MAXLOD = 6.0;
	vec3 original_reflectdir = mul(transpose(u_model[0]),vec4(reflectdir, 0.0)).xyz;
	vec3 envSpecularColor = textureCubeLod(s_texSkyLod, original_reflectdir, MAXLOD * rough).xyz;
	vec2 brdf = texture2D(s_texBrdfLut, vec2(max(NdV, 0), 1.0-rough)).xy; //这个的uv坐标又是正确的右u上v了
	vec3 envSpecular = envSpecularColor * (F * brdf.x + brdf.y);
	
	vec3 directColor = (kD * albedo / PI + specular) * u_lightRGB.xyz / r2 * max(dot(normal, lightdir), 0);
	vec3 envColor = albedo * irradiance + envSpecular;
	//gl_FragColor = vec4(vec3(4.0 * max(NdV, 0.0) * max(NdL, 0.0)), 0);
	//gl_FragColor = vec4(directColor,0);
	gl_FragColor = vec4(directColor + envColor * ao, 0);
	//gl_FragColor = vec4(1,1,1,0);
	gl_FragColor = toGamma(gl_FragColor);
}