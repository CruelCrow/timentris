#version 330 core

uniform vec4 shapeColor;

in vec4 fPosition;

out vec4 color;

void main() {
	color = shapeColor;
}

//TODO: detect distance to camera for lighting