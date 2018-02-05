#version 330 core

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MV;
uniform mat4 MVP;
uniform vec4 cubeCenter;
uniform float isLine;

in vec3 vPosition;
in vec3 vLinePosition;

out vec4 fPosition;

void main() {
	//gl_Position =  MVP * vec4(vPosition, 1.0);
	if (isLine == 1) {
		gl_Position =  MVP * vec4(vLinePosition, 1.0);
	} else {
		gl_Position =  MVP * vec4(vPosition, 1.0);
	}

	fPosition = MV * gl_Position;
}