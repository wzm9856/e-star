$input v_position

#include "../common/common.sh"

SAMPLERCUBE(s_texSkyLod, 0);

void main(){
	//gl_FragColor = vec4(v_position,0);
	gl_FragColor = toLinear(textureCubeLod(s_texSkyLod, v_position, 0.0));
}