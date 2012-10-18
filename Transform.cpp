// Transform.cpp: implementation of the Transform class.


#include "Transform.h"


// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) {
  // YOUR CODE FOR HW2 HERE
  // Please implement this.  Likely the same as in HW 1.  
  float rads = degrees*(pi/180);
  vec3 axis_norm = glm::normalize(axis);
  float x = axis_norm[0];
  float y = axis_norm[1];
  float z = axis_norm[2];
  //axis angle formula
  mat3 M = cos(rads)*mat3(1, 0, 0,
                          0, 1, 0,
                          0, 0, 1);
  M += (1-cos(rads))*mat3(x*x, x*y, x*z,
                          x*y, y*y, y*z,
                          x*z, y*z, z*z);
  M += sin(rads)*mat3(0, -z, y,
                      z, 0, -x,
                      -y, x, 0);
  return M;

}

void Transform::left(float degrees, vec3& eye, vec3& up) {
  // YOUR CODE FOR HW2 HERE
  // Likely the same as in HW 1.  
  mat3 M = rotate(degrees, up);
  eye = eye * M;
  up = up * M;
}

void Transform::up(float degrees, vec3& eye, vec3& up) {
  // YOUR CODE FOR HW2 HERE 
  // Likely the same as in HW 1.  
  vec3 cross = glm::normalize(glm::cross(eye, up));
  mat3 M = rotate(degrees, cross);
  eye = eye * M;
  up = up * M;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
  // YOUR CODE FOR HW2 HERE
  // Likely the same as in HW 1.  
  vec3 w = glm::normalize(eye);
  vec3 u = glm::normalize(glm::cross(up,w));
  vec3 v = glm::cross(w,u);
  float ex = eye[0];
  float ey = eye[1];
  float ez = eye[2];
  mat4 M = mat4(u[0], u[1], u[2], -ex*u[0]-ey*u[1]-ez*u[2],
                v[0], v[1], v[2], -ex*v[0]-ey*v[1]-ez*v[2],
                w[0], w[1], w[2], -ex*w[0]-ey*w[1]-ez*w[2],
                0, 0, 0, 1);
  return M;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  // YOUR CODE FOR HW2 HERE
  // New, to implement the perspective transform as well.  
  float rads = fovy*(pi/180);
  float f = 1.0/tan(rads/2);
  float z1 = (zFar+zNear)/(zNear-zFar);
  float z2 = (2*zFar*zNear)/(zNear-zFar);
  return mat4(f/aspect, 0, 0, 0,
              0, f, 0, 0,
              0, 0, z1, z2,
              0, 0, -1, 0);
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) {
  // YOUR CODE FOR HW2 HERE
  // Implement scaling 
  return mat4(sx, 0, 0, 0,
              0, sy, 0, 0,
              0, 0, sz, 0,
              0, 0, 0, 1);
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) {
  // YOUR CODE FOR HW2 HERE
  // Implement translation 
  return mat4(1, 0, 0, tx,
              0, 1, 0, ty,
              0, 0, 1, tz,
              0, 0, 0, 1);
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) {
  vec3 x = glm::cross(up,zvec) ; 
  vec3 y = glm::cross(zvec,x) ; 
  vec3 ret = glm::normalize(y) ; 
  return ret ; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
