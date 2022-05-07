vec3 a_position : POSITION;
vec3 a_normal 	: NORMAL;
vec2 a_texcoord0: TEXCOORD0;
vec3 a_tangent	: TANGENT;

vec3 v_wpos		: TEXCOORD1;
vec3 v_position : TEXCOORD2;
vec3 v_viewdir	: TEXCOORD3;
vec3 v_lightdir	: TEXCOORD4 = vec3(5.0, 5.0, 5.0);
vec3 v_tangent	: TANGENT;
vec3 v_normal	: NORMAL;
vec3 v_originNormal: TEXCOORD5;
vec2 v_texcoord0: TEXCOORD6;
vec3 temp		: TEXCOORD7;