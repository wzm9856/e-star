vec3 a_position : POSITION;
vec3 a_normal 	: NORMAL;
vec2 a_texcoord0: TEXCOORD0;
vec3 a_tangent	: TANGENT;

vec3 v_wpos		: TEXCOORD0;
vec2 v_texcoord0: TEXCOORD1;
vec3 v_viewdir	: TEXCOORD2;
vec3 v_lightdir	: TEXCOORD3 = vec3(5.0, 5.0, 5.0);
vec3 v_tangent	: TANGENT;
vec3 v_normal	: NORMAL;