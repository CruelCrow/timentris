#version 330 core

in vec3 fragmentColor;
in float fragmentAlpha;
out vec4 color;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	//gl_PointCoord
	//gl_FragCoord
	color = vec4(fragmentColor, fragmentAlpha);
}

/*
in vec3 fragmentColor;

out vec4 color;

void main(void) {
	color = vec4(fragmentColor, 1.0);
}
*/