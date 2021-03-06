#ifdef GL_ES_VERSION_2_0
#version 100  // OpenGL ES 2.0
#else
#version 120  // OpenGL 2.1
#endif
uniform mat4 transform;
attribute vec3 coord3d;
varying vec4 vertex;
void main(void)
{
  gl_Position = transform*vec4(coord3d[0], coord3d[1], coord3d[2], 1.0);
  vertex = vec4(coord3d, 1.0);
}
