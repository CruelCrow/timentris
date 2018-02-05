#version 330 core

uniform vec4 shapeColor;

in vec4 fPosition;

out vec4 color;

void main() {
	color = shapeColor;
	//Detect distance to camera for lighting
	/*float d = distance(fPosition, vec4(0.0, 0.0, 0.0, 1.0));
	float intensity = (d*100) / pow(d,2);
	color = vec4(intensity*shapeColor.xyz, shapeColor.w);*/

	/*if (dist < 5) {
		// color near origin
		color = vec4(0.0, 1.0, 0.0, 1.0);
	} else  {
		// color far from origin
		color = vec4(0.3, 0.3, 0.3, 1.0); 
	}*/

	/*float depth = gl_FragCoord.z / gl_FragCoord.w;
	float d = distance(vec3(512,384,0), vec3(gl_FragCoord.x / gl_FragCoord.w, gl_FragCoord.y / gl_FragCoord.w, gl_FragCoord.z / gl_FragCoord.w) );
	color = vec4(d/1000*shapeColor.xyz, shapeColor.w);*/


	/*float d = distance(vec3(0,0,0), gl_FragCoord.xyz);
	float intensity = 1.0;
	if (d < 2.4) {
		intensity = -0.042 * d + 1;
	} else {
		intensity = 1.49902 * pow(d,-0.568651);
	}*/
	/*if (intensity < 0.1) {
		color = vec4(0,1,0,1);
	} else {
		color = vec4(intensity*shapeColor.xyz, 1.0);
	}*/

	/*color = vec4(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.z, 0.25); return;
	if (fCubeType == 0) { //empty
		color = vec4(1.0, 1.0, 1.0, 0.25);
	} else if (fCubeType == 1) { //brick
		color = vec4(0.0, 1.0, 0.0, 0.8);
	} else if (fCubeType == 2) { //piece
		color = vec4(0.0, 0.0, 1.0, 0.8);
	} else { //unknown
		color = vec4(0.2, 0.3, 0.4, 1); //debug
		//discard;
	}*/
}

//TODO: detect distance to camera for lighting