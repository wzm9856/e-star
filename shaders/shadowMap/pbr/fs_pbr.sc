$input v_wpos, v_viewdir, v_lightdir, v_normal, v_texcoord0, v_tangent

#include "../common/common.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);
SAMPLER2D(s_texAORM, 2);
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
	vec3 lightdir 	= normalize(v_lightdir);
	vec3 viewdir 	= normalize(v_viewdir);
	vec3 half_vec	= normalize(lightdir + viewdir);
	vec3 normal 	= normalize(v_normal);
	vec3 tangent 	= normalize(v_tangent);
	
	vec3 color 	= texture2D(s_texColor, v_texcoord0).xyz;
	vec3 albedo = pow(color, vec3(2.2));
	vec4 aorm 	= texture2D(s_texAORM, v_texcoord0);
	float ao 		= aorm.x;
	float rough 	= aorm.y;
	float matallic 	= aorm.z;
	
	vec3 F0 = vec3(0.04);
	F0      = mix(F0, albedo, matallic); //金属的反射光有颜色，非金属就是白的
	vec3 F  = fresnelSchlick(max(dot(half_vec, viewdir), 0.0), F0);
	float D = DistributionGGX(normal, half_vec, rough);
	float G = GeometrySmith(normal, viewdir, lightdir, rough);
	vec3 specular = D * G * F / (4.0 * max(dot(normal, viewdir), 0.0) * max(dot(normal, lightdir), 0.0) + 0.001); 
	
	vec3 kD = (vec3(1.0) - F) * (1.0 - matallic);
	
    gl_FragColor = vec4((kD * albedo / PI + specular) * u_lightRGB.xyz / r2 * max(dot(normal, lightdir), 0.0) + vec3(0.03) * albedo * ao,0);
	//gl_FragColor = vec4(ambient + diffuse + specular, 0.0);
	//gl_FragColor = vec4(matallic,0,0,0);
}