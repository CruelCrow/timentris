#version 330 core

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MV;
uniform mat4 MVP;

in vec3 vPosition;

out vec4 fPosition;

void main() {
	gl_Position =  MVP * vec4(vPosition, 1.0);

	fPosition = MV * gl_Position;
}