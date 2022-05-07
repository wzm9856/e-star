vec3 a_position : POSITION;
vec3 a_normal 	: NORMAL;
vec4 a_color0	: COLOR;

vec3 v_wpos		: TEXCOORD0;
vec3 v_viewdir	: TEXCOORD1;
vec3 v_lightdir	: TEXCOORD2 = vec3(5.0, 5.0, 5.0);
vec3 v_normal	: NORMAL;
vec3 v_color0	: COLOR;