#include "BuildingBlocks/Transform.h"

namespace Engine {
	Transform::Transform()
		: _localScale({}),
		_localRotationQuaternion(0, 0, 0, 0),
		_localTranslation({})
	{
	}

	Transform::~Transform() {
	}

	Transform& Transform::rotate(const Vector3D& eulerRotation) {
		Quaternion q = Quaternion::quaternionFromEuler(eulerRotation.x(), eulerRotation.y(), eulerRotation.z());
		return this->rotate(q);
	}

	Transform& Transform::rotate(const Vector3D& axis,
								 const float& phi) {
		Quaternion q = Quaternion::quaternionFromAxisAngle(axis, phi);
		return this->rotate(q);
	}

	Transform& Transform::rotate(const Quaternion& q) {
		_localRotationQuaternion *= q;

		setDirty();
		return *this;
	}

	Transform& Transform::resetRotation(const Vector3D& eulerRotation) {
		return this->resetRotation(Quaternion::quaternionFromEuler(eulerRotation.x(), eulerRotation.y(), eulerRotation.z()));
	}
	Transform& Transform::resetRotation(const Vector3D& axis,
										const float&   phi) {
		return this->resetRotation(Quaternion::quaternionFromAxisAngle(axis, phi));
	}
	Transform& Transform::resetRotation(const Quaternion& quaternionRotation) {
		_localRotationQuaternion = quaternionRotation;

		setDirty();
		return *this;
	}

	Transform& Transform::translate(const Vector3D& translation) {
		_localTranslation += translation;

		setDirty();
		return *this;
	}

	Transform& Transform::resetTranslation(const Vector3D& translation) {
		_localTranslation = translation;

		setDirty();
		return *this;
	}

	Transform& Transform::scale(const float& factor) {
		return this->scale({ factor, factor, factor });
	}

	Transform& Transform::scale(const Vector3D& factors) {
		for (int k=0; k < 3; ++k) {
			_localScale[k] *= factors[k];
		}

		setDirty();
		return *this;
	}

	Transform& Transform::resetScale(const float& factor) {
		return this->resetScale({ factor, factor, factor });
	}

	Transform& Transform::resetScale(const Vector3D& factors) {
		_localScale = factors;

		setDirty();
		return *this;
	}

	Transform& Transform::reset() {
		this->resetScale(1.0);
		this->resetTranslation({ 0, 0, 0 });
		this->resetRotation({ 0, 0, 0 }, 0);

		return *this;
	}

	const Matrix4x4& Transform::local() {
		if (isDirty()) {
			Matrix4x4 S = Matrix4x4::identity();
			S.r00(_localScale.x());
			S.r11(_localScale.y());
			S.r22(_localScale.z());
			S.r33(1.0);

			Matrix4x4 T = Matrix4x4::identity();
			T.r03(_localTranslation.x());
			T.r13(_localTranslation.y());
			T.r23(_localTranslation.z());

			Matrix4x4 R = Matrix4x4::identity();
			R = Quaternion::rotationMatrixFromQuaternion(_localRotationQuaternion);

			_currentLocalTransform = SMMatrixMultiply(SMMatrixMultiply(S, R), T);
		}

		return _currentLocalTransform;
	}

	const Matrix4x4& Transform::world() const {
		return _currentWorldTransform;
	}

	const Matrix4x4& Transform::updateWorldTransform(const Matrix4x4& parent) {
		_currentWorldTransform = SMMatrixMultiply(parent, _currentLocalTransform);
		return _currentWorldTransform;
	}
}
