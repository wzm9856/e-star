$input v_texcoord0

#include "../../common/common.sh"

SAMPLER2D(s_shadowMap, 0);
void main(){
    gl_FragColor = vec4(texture2D(s_shadowMap, v_texcoord0).xyz,0);
}