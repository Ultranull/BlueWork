#include "Transform.h"

 glm::mat4 Transform::Model() {
	return T() * R() * S();
}

 glm::mat4 Transform::FinalTransform(bool force, glm::mat4 parent) {
    if (!ValidFinalTransform || force) {
        _FinalTransform = parent * Model();
        ValidFinalTransform = true;
    }

    return _FinalTransform;
 }

 glm::mat4 Transform::T() {
	return glm::translate(_Position);
}

 glm::mat4 Transform::S() {
	return glm::scale(_Scale);
}

 glm::mat4 Transform::R() {
	return glm::toMat4(_Rotation);
}

 glm::vec3 Transform::Up() {
     glm::vec4 unitAxis(0, 1, 0, 1);
     return glm::normalize(glm::vec3(R() * unitAxis));
 }

 glm::vec3 Transform::Forward() {
     glm::vec4 unitAxis(0, 0, 1, 1);
     return glm::normalize(glm::vec3(R() * unitAxis));
 }

 glm::vec3& Transform::Position(){
     ValidFinalTransform = false;
     return _Position;
 }

 glm::vec3& Transform::Scale() {
     ValidFinalTransform = false;
     return _Scale;
 }

 glm::quat& Transform::Rotation() {
     ValidFinalTransform = false;
     return _Rotation;
 }

 const glm::vec3& Transform::Position() const {
     return _Position;
 }

 const glm::vec3& Transform::Scale() const {
     return _Scale;
 }

 const glm::quat& Transform::Rotation() const {
     return _Rotation;
 }


 void Transform::rotateBy(glm::quat b) {
     ValidFinalTransform = false;
	 _Rotation = b * _Rotation;
 }

 void Transform::rotate(float rads, glm::vec3 axis) {
     ValidFinalTransform = false;
	 _Rotation = glm::angleAxis(rads, axis);
 }

 void Transform::translate(glm::vec3 t) {
     ValidFinalTransform = false;
	 _Position += t;
 }

 void Transform::scale(glm::vec3 t) {
     ValidFinalTransform = false;
	 _Scale = t;
 }


 bool Transform::IsValid(void) {
     return ValidFinalTransform;
 }