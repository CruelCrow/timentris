#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 48) out; //points

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 MVP; //TODO: do we need MVP?

in float gCubeType[];

out float fCubeType;

void main() {
	/*//Pass-through
	fCubeType = gCubeType[0];
	for ( int i = 0; i < gl_in.length (); i++ ) {
        gl_Position = gl_in [i].gl_Position;
        EmitVertex ();
    }
    EndPrimitive();*/
	fCubeType = gCubeType[0];
	vec4 center = gl_in[0].gl_Position; //cube center
	//back
	gl_Position = center + vec4(-0.5, -0.5, -0.5, 0); EmitVertex();
	gl_Position = center + vec4(-0.5, +0.5, -0.5, 0); EmitVertex();
	gl_Position = center + vec4(+0.5, -0.5, -0.5, 0); EmitVertex();
	gl_Position = center + vec4(+0.5, +0.5, -0.5, 0); EmitVertex();
	EndPrimitive();
	//left
	gl_Position = center + vec4(-0.5, -0.5, -0.5, 0); EmitVertex();
	gl_Position = center + vec4(-0.5, -0.5, +0.5, 0); EmitVertex();
	gl_Position = center + vec4(-0.5, +0.5, -0.5, 0); EmitVertex();
	gl_Position = center + vec4(-0.5, +0.5, +0.5, 0); EmitVertex();
	EndPrimitive();
	/*gl_Position = gl_in[0].gl_Position - vec4(1,0,0,0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position - vec4(0,1,0,0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position - vec4(0,0,1,0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position - vec4(2,0,0,0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position - vec4(0,2,0,0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position - vec4(0,0,2,0);
	EmitVertex();
	EndPrimitive();*/
}

//TODO: draw farther sides first
//TODO: draw axises
//TODO: try instancing?