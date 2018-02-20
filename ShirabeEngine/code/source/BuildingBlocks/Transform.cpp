#include "BuildingBlocks/Transform.h"

namespace Engine {
	Transform::Transform()
		: m_localScale({})
		, m_localRotationQuaternion(0, 0, 0, 0)
		, m_localTranslation({})
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
		m_localRotationQuaternion *= q;

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
		m_localRotationQuaternion = quaternionRotation;

		setDirty();
		return *this;
	}

	Transform& Transform::translate(const Vector3D& translation) {
		m_localTranslation += translation;

		setDirty();
		return *this;
	}

	Transform& Transform::resetTranslation(const Vector3D& translation) {
		m_localTranslation = translation;

		setDirty();
		return *this;
	}

	Transform& Transform::scale(const float& factor) {
		return this->scale({ factor, factor, factor });
	}

	Transform& Transform::scale(const Vector3D& factors) {
		for (int k=0; k < 3; ++k) {
			m_localScale[k] *= factors[k];
		}

		setDirty();
		return *this;
	}

	Transform& Transform::resetScale(const float& factor) {
		return this->resetScale({ factor, factor, factor });
	}

	Transform& Transform::resetScale(const Vector3D& factors) {
		m_localScale = factors;

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
			S.r00(m_localScale.x());
			S.r11(m_localScale.y());
			S.r22(m_localScale.z());
			S.r33(1.0);

			Matrix4x4 T = Matrix4x4::identity();
			T.r03(m_localTranslation.x());
			T.r13(m_localTranslation.y());
			T.r23(m_localTranslation.z());

			Matrix4x4 R = Matrix4x4::identity();
			R = Quaternion::rotationMatrixFromQuaternion(m_localRotationQuaternion);

			m_currentLocalTransform = SMMatrixMultiply(SMMatrixMultiply(S, R), T);
		}

		return m_currentLocalTransform;
	}

	const Matrix4x4& Transform::world() const {
		return m_currentWorldTransform;
	}

	const Matrix4x4& Transform::updateWorldTransform(const Matrix4x4& parent) {
		m_currentWorldTransform = SMMatrixMultiply(parent, m_currentLocalTransform);
		return m_currentWorldTransform;
	}
}
