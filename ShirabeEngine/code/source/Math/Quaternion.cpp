#include "Math/Quaternion.h"

#include "Math/Common.h"

namespace Engine {
	namespace Math {

		Quaternion::Quaternion()
			: Vector<4>({ 0, 0, 0, 0 })
		{
		}

		Quaternion::Quaternion(const Quaternion& other)
			: Vector<4>(other)
		{
		}

		Quaternion::Quaternion(const AxisAngle& other)
			: Quaternion(Quaternion::quaternionFromAxisAngle(other.axis(), other.phi()))
		{}

		Quaternion::Quaternion(value_type w, const Vector3D& vec)
			: Vector<4>({ w, vec.x(), vec.y(), vec.z() })
		{
		}

		Quaternion::Quaternion(value_type w, value_type x, value_type y, value_type z)
			: Vector<4>({ w, x, y, z })
		{
		}

		Quaternion::~Quaternion() { }

		void Quaternion::assign(value_type w, value_type x, value_type y, value_type z) {
			this->w(w);
			this->x(x);
			this->y(y);
			this->z(z);
		}

		void Quaternion::assign(const Quaternion& r) {
			this->w(r.w());
			this->x(r.x());
			this->y(r.y());
			this->z(r.z());
		}

		Quaternion& Quaternion::operator+=(const Quaternion& other) {
			this->w(w() + other.w());
			this->x(x() + other.x());
			this->y(y() + other.y());
			this->z(z() + other.z());

			return *this;
		}

		Quaternion& Quaternion::operator-=(const Quaternion& other) {
			this->w(w() - other.w());
			this->x(x() - other.x());
			this->y(y() - other.y());
			this->z(z() - other.z());

			return *this;
		}

		Quaternion& Quaternion::operator*=(const value_type& factor) {
			this->w(w() * factor);
			this->x(x() * factor);
			this->y(y() * factor);
			this->z(z() * factor);

			return *this;
		}

		Quaternion& Quaternion::operator*=(const Quaternion& other) {
			value_type   scl = (this->w()*other.w() - dot(this->vector(), other.vector()));
			Vector3D vec = (this->w()*other.vector()) + (other.w()*this->vector()) + cross(this->vector(), other.vector());

			assign(scl, vec.x(), vec.y(), vec.z());

			return *this;
		}

		Quaternion& Quaternion::operator/=(const value_type& factor) {
			if (fabs(factor) > 0.00001) {
				this->w(w() / factor);
				this->x(x() / factor);
				this->y(y() / factor);
				this->z(z() / factor);
			}

			return *this;
		}

		Quaternion::value_type Quaternion::magnitude() const {
			return (value_type)sqrt(w()*w() + x()*x() + y()*y() + z()*z());
		}

		Quaternion Quaternion::conjugate() const {
			return Quaternion(w(), -x(), -y(), -z());
		}

		Quaternion Quaternion::inverse() const {
			// Decays to inverse == conjugate if we deal with a unit quaternion.
			value_type mag = magnitude();
			return (conjugate() / (mag * mag));
		}

		Quaternion Quaternion::normalize() const {
			value_type N = magnitude();
			return Quaternion(
				w() / N,
				x() / N,
				y() / N,
				z() / N
			);
		}


		Quaternion::value_type Quaternion::rotationAngle(const Quaternion& q) {
			return (value_type)(2 * acos(q.w()));
		}

		Vector3D Quaternion::rotationAxis(const Quaternion& q) {
			// Threshold to allow for rounding errors.
			static const value_type TOLERANCE = 0.005f;

			Vector3D v = q.vector();
			value_type   m = v.length();

			if (m <= TOLERANCE) {
				return Vector3D();
			} else {
				return (v / m);
			}
		}

		Quaternion Quaternion::quaternionFromEuler(const value_type& x,
												   const value_type& y,
												   const value_type& z) {
			// 
			// q_roll  = [cos(phi/2), (sin(phi/2))i + 0j          + 0k         ]
			// q_pitch = [cos(tau/2), 0i            + sin(tau/2)j + 0k         ]
			// q_yaw   = [cos(psi/2), 0i            + 0j          + sin(psi/2)k]
			// 
			// q = ((q_yaw * q_pitch) * q_roll)
			//
			// q = [ { cos(phi/2)cos(tau/2)cos(psi/2) + sin(phi/2)sin(tau/2)sin(psi/2) },
			//       { sin(phi/2)cos(tau/2)cos(psi/2) - cos(phi/2)sin(tau/2)sin(psi/2) }i,
			//       { cos(phi/2)sin(tau/2)cos(psi/2) + sin(phi/2)cos(tau/2)sin(psi/2) }j,
			//       { cos(phi/2)cos(tau/2)sin(psi/2) - sin(phi/2)sin(tau/2)cos(psi/2) }k ];
			//

			value_type phi = x; // deg_to_rad(x);
			value_type tau = y; // deg_to_rad(y);
			value_type psi = z; // deg_to_rad(z);

			value_type cos_psi = cos(0.5f * psi);
			value_type cos_tau = cos(0.5f * tau);
			value_type cos_phi = cos(0.5f * phi);
			value_type sin_psi = sin(0.5f * psi);
			value_type sin_tau = sin(0.5f * tau);
			value_type sin_phi = sin(0.5f * phi);

			value_type cos_psi_cos_tau = (cos_psi * cos_tau);
			value_type sin_psi_sin_tau = (sin_psi * sin_tau);
			value_type cos_psi_sin_tau = (cos_psi * sin_tau);
			value_type sin_psi_cos_tau = (sin_psi * cos_tau);

			return Quaternion(
				((cos_psi_cos_tau * cos_phi) + (sin_psi_sin_tau * sin_phi)),
				((cos_psi_cos_tau * sin_phi) - (sin_psi_sin_tau * cos_phi)),
				((cos_psi_sin_tau * cos_phi) + (sin_psi_cos_tau * sin_phi)),
				((sin_psi_cos_tau * cos_phi) - (cos_psi_sin_tau * sin_phi))
			);
		}

		Quaternion Quaternion::quaternionFromRotationMatrix(const Matrix4x4& m) {
			value_type r00 = m.r00();
			value_type r11 = m.r11();
			value_type r22 = m.r22();

			value_type q_w = 0.0f;
			value_type q_x = 0.0f;
			value_type q_y = 0.0f;
			value_type q_z = 0.0f;

			value_type trace = r00 + r11 + r22;
			if (trace > 0) {
				value_type S = sqrtf(trace + 1.0f) * 2.0f; // S = 4qw
				q_w = (0.25f * S);
				q_x = (m.r21() - m.r12()) / S;
				q_y = (m.r02() - m.r20()) / S;
				q_z = (m.r10() - m.r01()) / S;
			} else if ((r00 > r11) & (r00 > r22)) {
				value_type S = sqrtf(1.0f + r00 - r11 - r22) * 2.0f; // S=4qx
				q_w = (m.r21() - m.r12()) / S;
				q_x = (0.25f * S);
				q_y = (m.r01() + m.r10()) / S;
				q_z = (m.r02() + m.r20()) / S;
			} else if (r11 > r22) {
				value_type S = sqrtf(1.0f + r11 - r00 - r22) * 2.0f; // S = 4qy
				q_w = (m.r02() - m.r20()) / S;
				q_x = (m.r01() + m.r10()) / S;
				q_y = (0.25f * S);
				q_z = (m.r12() + m.r21()) / S;
			} else {
				value_type S = sqrtf(1.0f + r22 - r00 - r11) * 2.0f; // S = 4qz
				q_w = (m.r10() - m.r01()) / S;
				q_x = (m.r02() + m.r20()) / S;
				q_y = (m.r12() + m.r21()) / S;
				q_z = (0.25f * S);
			}

			return Quaternion(q_w, q_x, q_y, q_z);
		}

		Quaternion Quaternion::quaternionFromAxisAngle(const Vector3D& axis,
													   const value_type&   phi) {
			value_type hsin = sinf(phi / 2.0f);
			value_type hcos = cosf(phi / 2.0f);
			return Quaternion(hcos,
							  axis.x() * hsin,
							  axis.y() * hsin,
							  axis.z() * hsin);
		}

		Vector3D Quaternion::eulerFromQuaternion(const Quaternion& q) {
			Vector3D v;

			value_type q00sq = q.scalar()     * q.scalar();
			value_type q11sq = q.vector().x() * q.vector().x();
			value_type q22sq = q.vector().y() * q.vector().y();
			value_type q33sq = q.vector().z() * q.vector().z();

			value_type r00 = q00sq + q11sq - q22sq - q33sq;
			value_type r10 = 2 * (q.vector().x()*q.vector().y() + q.scalar()*q.vector().z());
			value_type r20 = 2 * (q.vector().x()*q.vector().z() - q.scalar()*q.vector().y());
			value_type r21 = 2 * (q.vector().y()*q.vector().z() + q.scalar()*q.vector().x());
			value_type r22 = q00sq - q11sq - q22sq + q33sq;

			value_type r01 = 0;
			value_type r02 = 0;

			value_type tmp = fabs(r20);
			if (tmp > 0.999999) { // Test for pole singularities
				r01 = 2 * (q.vector().x()*q.vector().y() - q.scalar()*q.vector().z());
				r02 = 2 * (q.vector().x()*q.vector().z() + q.scalar()*q.vector().y());

				v = Vector3D(
					rad_to_deg(0.0f),                    // phi
					rad_to_deg((-((value_type)M_PI / 2.0f)*r20 / tmp)), // tau
					rad_to_deg(atan2(-r01, -r20*r02))    // psi
				);
			} else {
				v = Vector3D(
					rad_to_deg(atan2(r21, r22)), // phi 
					rad_to_deg(asin(-r20)),      // tau 
					rad_to_deg(atan2(r10, r00))  // psi
				);
			}

			return v;
		}

		Matrix4x4 Quaternion::rotationMatrixFromQuaternion(const Quaternion& q) {
			value_type r11 = 1 - (2 * q.y()*q.y()) - (2 * q.z()*q.z());
			value_type r12 = (2 * q.x()*q.y()) - (2 * q.w()*q.z());
			value_type r13 = (2 * q.x()*q.z()) + (2 * q.w()*q.y());
			value_type r21 = (2 * q.x()*q.y()) + (2 * q.w()*q.z());
			value_type r22 = 1 - (2 * q.x()*q.x()) - (2 * q.z()*q.z());
			value_type r23 = (2 * q.y()*q.z()) - (2 * q.w()*q.x());
			value_type r31 = (2 * q.x()*q.z()) - (2 * q.w()*q.y());
			value_type r32 = (2 * q.y()*q.z()) + (2 * q.w()*q.x());
			value_type r33 = 1 - (2 * q.x()*q.x()) - (2 * q.y()*q.y());
			value_type r44 = 1.0;

			return Matrix4x4({ r11, r12, r13, 0.0, r21, r22, r23, 0.0, r31, r32, r33, 0.0, 0.0, 0.0, 0.0, r44 });
		}

		Vector4D Quaternion::axisAngleFromQuaternion(const Quaternion& q) {
			Quaternion p = q;
			if (p.scalar() > 1.0)
				p = p.normalize();

			value_type x = 0.0;
			value_type y = 0.0;
			value_type z = 0.0;

			value_type sqrtOneMinusScalarSq = sqrt(1 - p.scalar() * p.scalar());
			if (sqrtOneMinusScalarSq < 0.001) {
				x = p.x();
				y = p.y();
				z = p.z();
			} else {
				x = p.x() / sqrtOneMinusScalarSq;
				y = p.y() / sqrtOneMinusScalarSq;
				z = p.z() / sqrtOneMinusScalarSq;
			}

			return Vector4D(
			{
				x,
				y,
				z,
				2.0f * acos(p.scalar())
			});
		}

		Quaternion rotate(const Quaternion& q,
						  const Quaternion& p) {
			// This function assumes a unit quaternion be passed!
			return (q*p)*q.conjugate();
		}

		Vector3D rotate(const Quaternion& q,
						const Vector3D&   v) {
			Quaternion p = Quaternion(0.0f, v);
			return rotate(q, p).vector();
		}

		Quaternion operator+(const Quaternion &q,
							 const Quaternion &other) {
			return Quaternion(q.w() + other.w(),
							  q.x() + other.x(),
							  q.y() + other.y(),
							  q.z() + other.z());
		}

		Quaternion operator-(const Quaternion &q,
							 const Quaternion &other) {
			return Quaternion(q.w() - other.w(),
							  q.x() - other.x(),
							  q.y() - other.y(),
							  q.z() - other.z());
		}

		Quaternion operator*(const Quaternion             &q,
							 const Quaternion::value_type &factor) {
			return Quaternion((q.w() * factor),
				(q.x() * factor),
							  (q.y() * factor),
							  (q.z() * factor));
		}

		Quaternion operator*(const Quaternion::value_type &factor,
							 const Quaternion             &q) {
			return Quaternion((factor * q.w()),
				(factor * q.x()),
							  (factor * q.y()),
							  (factor * q.z()));
		}

		Quaternion operator*(const Quaternion &q,
							 const Vector3D   &v) {
			// qp = (nq*0 - dot(vq, vp)) + (nq*vp + 0*vq + cross(vq, vp))ijk

			return Quaternion(
				-(q.x()*v.x() + q.y()*v.y() + q.z()*v.z()),
				q.w()*v.x() + q.y()*v.z() - q.z()*v.y(),
				q.w()*v.y() + q.z()*v.x() - q.x()*v.z(),
				q.w()*v.z() + q.x()*v.y() - q.y()*v.x()
			);
		}

		Quaternion operator*(const Vector3D   &v,
							 const Quaternion &q) {
			// qp = (0*0 - dot(vq, vp)) + (0*vp + 0*vq + cross(vq, vp))ijk

			return Quaternion(
				-(q.x()*v.x() + q.y()*v.y() + q.z()*v.z()),
				q.w()*v.x() + q.z()*v.y() - q.y()*v.z(),
				q.w()*v.y() + q.x()*v.z() - q.z()*v.x(),
				q.w()*v.z() + q.y()*v.x() - q.x()*v.y()
			);
		}

		Quaternion operator*(const Quaternion &q,
							 const Quaternion &p) {
			// qp = (nq*np - dot(vq, vp)) + (nq*vp + np*vq + cross(vq, vp))ijk

			return Quaternion(
				q.w()*p.w() - q.x()*p.x() - q.y()*p.y() - q.z()*p.z(),
				q.w()*p.x() + q.x()*p.w() + q.y()*p.z() - q.z()*p.y(),
				q.w()*p.y() + q.y()*p.w() + q.z()*p.x() - q.x()*p.z(),
				q.w()*p.z() + q.z()*p.w() + q.x()*p.y() - q.y()*p.x()
			);
		}

		Quaternion operator/(const Quaternion             &q,
							 const Quaternion::value_type &factor) {
			return Quaternion((q.w() / factor),
				              (q.x() / factor),
							  (q.y() / factor),
							  (q.z() / factor));
		}
	}
}