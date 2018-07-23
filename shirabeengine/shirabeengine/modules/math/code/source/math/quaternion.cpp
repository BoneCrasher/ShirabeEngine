#include "math/quaternion.h"
#include "math/common.h"

namespace Engine
{
  namespace Math
  {

    CQuaternion::CQuaternion()
        : CVector4D_t({ 0, 0, 0, 0 })
    {
    }

    CQuaternion::CQuaternion(const CQuaternion& aOther)
        : CVector4D_t(aOther)
    {
    }

    CQuaternion::CQuaternion(const CAxisAngle& aAxisAngle)
        : CQuaternion(CQuaternion::CQuaternionFromAxisAngle(aAxisAngle.axis(), aAxisAngle.phi()))
    {}

    CQuaternion::CQuaternion(
            value_type  const &aPhi,
            CVector3D_t const &aAxis)
        : CVector4D_t({ aPhi, aAxis.x(), aAxis.y(), aAxis.z() })
    {
    }

    CQuaternion::CQuaternion(
            value_type const &aPhi,
            value_type const &aAxisX,
            value_type const &aAxisY,
            value_type const &aAxisZ)
        : CVector4D_t({ aPhi, aAxisX, aAxisY, aAxisZ })
    {
    }

    CQuaternion::~CQuaternion() { }

    void CQuaternion::assign(
            value_type const &aPhi,
            value_type const &aAxisX,
            value_type const &aAxisY,
            value_type const &aAxisZ)
    {
        this->w(aPhi);
        this->x(aAxisX);
        this->y(aAxisY);
        this->z(aAxisZ);
    }

    void CQuaternion::assign(const CQuaternion& aOther)
    {
        this->w(aOther.w());
        this->x(aOther.x());
        this->y(aOther.y());
        this->z(aOther.z());
    }

    CQuaternion& CQuaternion::operator+=(CQuaternion const &aOther)
    {
        this->w(w() + aOther.w());
        this->x(x() + aOther.x());
        this->y(y() + aOther.y());
        this->z(z() + aOther.z());

        return *this;
    }

    CQuaternion& CQuaternion::operator-=(CQuaternion const &aOther)
    {
        this->w(w() - aOther.w());
        this->x(x() - aOther.x());
        this->y(y() - aOther.y());
        this->z(z() - aOther.z());

        return *this;
    }

    CQuaternion& CQuaternion::operator*=(value_type const &aFactor)
    {
        this->w(w() * aFactor);
        this->x(x() * aFactor);
        this->y(y() * aFactor);
        this->z(z() * aFactor);

        return *this;
    }

    CQuaternion& CQuaternion::operator*=(CQuaternion const &aOther)
    {
        value_type  const phiProduct        = (this->w() * aOther.w());
        value_type  const axisDotProduct    = dot(this->vector(), aOther.vector());

        CVector3D_t const scaledThisVector  = this->vector() * aOther.w();
        CVector3D_t const scaledOtherVecotr = this->w()      * aOther.vector();
        CVector3D_t const crossVector       = cross(this->vector(), aOther.vector());

        value_type  const scale  = (phiProduct - axisDotProduct);
        CVector3D_t const vector = (scaledThisVector + scaledOtherVecotr) + crossVector;

        assign(scale, vector.x(), vector.y(), vector.z());

      return *this;
    }

    CQuaternion& CQuaternion::operator/=(value_type const &aFactor)
    {
      if(fabs(aFactor) > 0.00001f)
      {
        this->w(w() / aFactor);
        this->x(x() / aFactor);
        this->y(y() / aFactor);
        this->z(z() / aFactor);
      }

      return (*this);
    }

    CQuaternion::value_type CQuaternion::magnitude() const
    {
      return (value_type)sqrt(w()*w() + x()*x() + y()*y() + z()*z());
    }

    CQuaternion CQuaternion::conjugate() const
    {
      return CQuaternion(w(), -x(), -y(), -z());
    }

    CQuaternion CQuaternion::inverse() const
    {
      // Decays to inverse == conjugate if we deal with a unit CQuaternion.
      value_type const mag = magnitude();
      return (conjugate() / (mag * mag));
    }

    CQuaternion CQuaternion::normalize() const
    {
      value_type const N = magnitude();

      return CQuaternion(
        w() / N,
        x() / N,
        y() / N,
        z() / N
      );
    }


    CQuaternion::value_type CQuaternion::rotationAngle(CQuaternion const &aQuaternion)
    {
      return (value_type)(2 * acos(aQuaternion.w()));
    }

    CVector3D_t CQuaternion::rotationAxis(CQuaternion const &aQuaternion)
    {
      // Threshold to allow for rounding errors.
      static constexpr value_type const TOLERANCE = 0.005f;

      CVector3D_t const vector = aQuaternion.vector();
      value_type  const m      = vector.length();

      if(m <= TOLERANCE) {
        return CVector3D_t();
      }
      else {
        return (vector / m);
      }
    }

    CQuaternion CQuaternion::CQuaternionFromEuler(
            value_type const &aX,
            value_type const &aY,
            value_type const &aZ)
    {
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

      value_type phi = aX; // deg_to_rad(x);
      value_type tau = aY; // deg_to_rad(y);
      value_type psi = aZ; // deg_to_rad(z);

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

      return CQuaternion(
        ((cos_psi_cos_tau * cos_phi) + (sin_psi_sin_tau * sin_phi)),
        ((cos_psi_cos_tau * sin_phi) - (sin_psi_sin_tau * cos_phi)),
        ((cos_psi_sin_tau * cos_phi) + (sin_psi_cos_tau * sin_phi)),
        ((sin_psi_cos_tau * cos_phi) - (cos_psi_sin_tau * sin_phi))
      );
    }

    CQuaternion CQuaternion::CQuaternionFromRotationMatrix(CMatrix4x4 const &aMatrix) {
      value_type r00 = aMatrix.r00();
      value_type r11 = aMatrix.r11();
      value_type r22 = aMatrix.r22();

      value_type q_w = 0.0f;
      value_type q_x = 0.0f;
      value_type q_y = 0.0f;
      value_type q_z = 0.0f;

      value_type trace = r00 + r11 + r22;
      if(trace > 0) {
        value_type const S = sqrtf(trace + 1.0f) * 2.0f; // S = 4qw
        q_w = (0.25f * S);
        q_x = (aMatrix.r21() - aMatrix.r12()) / S;
        q_y = (aMatrix.r02() - aMatrix.r20()) / S;
        q_z = (aMatrix.r10() - aMatrix.r01()) / S;
      }
      else if((r00 > r11) & (r00 > r22)) {
        value_type const S = sqrtf(1.0f + r00 - r11 - r22) * 2.0f; // S=4qx
        q_w = (aMatrix.r21() - aMatrix.r12()) / S;
        q_x = (0.25f * S);
        q_y = (aMatrix.r01() + aMatrix.r10()) / S;
        q_z = (aMatrix.r02() + aMatrix.r20()) / S;
      }
      else if(r11 > r22) {
        value_type const S = sqrtf(1.0f + r11 - r00 - r22) * 2.0f; // S = 4qy
        q_w = (aMatrix.r02() - aMatrix.r20()) / S;
        q_x = (aMatrix.r01() + aMatrix.r10()) / S;
        q_y = (0.25f * S);
        q_z = (aMatrix.r12() + aMatrix.r21()) / S;
      }
      else {
        value_type const S = sqrtf(1.0f + r22 - r00 - r11) * 2.0f; // S = 4qz
        q_w = (aMatrix.r10() - aMatrix.r01()) / S;
        q_x = (aMatrix.r02() + aMatrix.r20()) / S;
        q_y = (aMatrix.r12() + aMatrix.r21()) / S;
        q_z = (0.25f * S);
      }

      return CQuaternion(q_w, q_x, q_y, q_z);
    }

    CQuaternion CQuaternion::CQuaternionFromAxisAngle(
            CVector3D_t const &aAxis,
            value_type  const &aPhi)
    {
        value_type const hsin = sinf(aPhi / 2.0f);
        value_type const hcos = cosf(aPhi / 2.0f);
        return CQuaternion(
                    hcos,
                    aAxis.x() * hsin,
                    aAxis.y() * hsin,
                    aAxis.z() * hsin);
    }

    CVector3D_t CQuaternion::eulerFromCQuaternion(CQuaternion const &aQuaternion)
    {
        CVector3D_t v {};

        value_type aQuaternion00saQuaternion = aQuaternion.scalar()     * aQuaternion.scalar();
        value_type aQuaternion11saQuaternion = aQuaternion.vector().x() * aQuaternion.vector().x();
        value_type aQuaternion22saQuaternion = aQuaternion.vector().y() * aQuaternion.vector().y();
        value_type aQuaternion33saQuaternion = aQuaternion.vector().z() * aQuaternion.vector().z();

        value_type r00 = aQuaternion00saQuaternion + aQuaternion11saQuaternion
                         - aQuaternion22saQuaternion - aQuaternion33saQuaternion;
        value_type r10 = 2 * ((aQuaternion.vector().x() * aQuaternion.vector().y()) +
                              (aQuaternion.scalar()     * aQuaternion.vector().z()));
        value_type r20 = 2 * ((aQuaternion.vector().x() * aQuaternion.vector().z()) -
                              (aQuaternion.scalar()     * aQuaternion.vector().y()));
        value_type r21 = 2 * ((aQuaternion.vector().y() * aQuaternion.vector().z()) +
                              (aQuaternion.scalar()     * aQuaternion.vector().x()));
        value_type r22 = aQuaternion00saQuaternion - aQuaternion11saQuaternion
                         - aQuaternion22saQuaternion + aQuaternion33saQuaternion;

        value_type r01 = 0;
        value_type r02 = 0;

        value_type tmp = fabs(r20);
        if(tmp > 0.999999)
        {
            // Test for pole singularities
            r01 = 2 * ((aQuaternion.vector().x() * aQuaternion.vector().y()) -
                       (aQuaternion.scalar()     * aQuaternion.vector().z()));
            r02 = 2 * ((aQuaternion.vector().x() * aQuaternion.vector().z()) +
                       (aQuaternion.scalar()     * aQuaternion.vector().y()));

            v = CVector3D_t(
            {
                rad_to_deg(0.0f),                                   // phi
                rad_to_deg((-((value_type)M_PI / 2.0f)*r20 / tmp)), // tau
                rad_to_deg(atan2(-r01, -r20*r02))                   // psi
            });
        }
        else
        {
            v = CVector3D_t(
            {
                rad_to_deg(atan2(r21, r22)), // phi
                rad_to_deg(asin(-r20)),      // tau
                rad_to_deg(atan2(r10, r00))  // psi
            });
        }

        return v;
    }

    CMatrix4x4 CQuaternion::rotationMatrixFromCQuaternion(CQuaternion const &aQuaternion)
    {
        value_type r11 = 1 - (2 * (aQuaternion.y() * aQuaternion.y())) - (2 * (aQuaternion.z() * aQuaternion.z()));
        value_type r12 =     (2 * (aQuaternion.x() * aQuaternion.y())) - (2 * (aQuaternion.w() * aQuaternion.z()));
        value_type r13 =     (2 * (aQuaternion.x() * aQuaternion.z())) + (2 * (aQuaternion.w() * aQuaternion.y()));
        value_type r21 =     (2 * (aQuaternion.x() * aQuaternion.y())) + (2 * (aQuaternion.w() * aQuaternion.z()));
        value_type r22 = 1 - (2 * (aQuaternion.x() * aQuaternion.x())) - (2 * (aQuaternion.z() * aQuaternion.z()));
        value_type r23 =     (2 * (aQuaternion.y() * aQuaternion.z())) - (2 * (aQuaternion.w() * aQuaternion.x()));
        value_type r31 =     (2 * (aQuaternion.x() * aQuaternion.z())) - (2 * (aQuaternion.w() * aQuaternion.y()));
        value_type r32 =     (2 * (aQuaternion.y() * aQuaternion.z())) + (2 * (aQuaternion.w() * aQuaternion.x()));
        value_type r33 = 1 - (2 * (aQuaternion.x() * aQuaternion.x())) - (2 * (aQuaternion.y() * aQuaternion.y()));
        value_type r44 = 1.0;

        return CMatrix4x4({ r11, r12, r13, 0.0, r21, r22, r23, 0.0, r31, r32, r33, 0.0, 0.0, 0.0, 0.0, r44 });
    }

    CVector4D_t CQuaternion::axisAngleFromCQuaternion(CQuaternion const &aQuaternion)
    {
      CQuaternion p = aQuaternion;

      if(p.scalar() > 1.0)
        p = p.normalize();

      value_type x = 0.0;
      value_type y = 0.0;
      value_type z = 0.0;

      value_type sqrtOneMinusScalarSq = sqrt(1 - p.scalar() * p.scalar());
      if(sqrtOneMinusScalarSq < 0.001)
      {
        x = p.x();
        y = p.y();
        z = p.z();
      }
      else
      {
        x = p.x() / sqrtOneMinusScalarSq;
        y = p.y() / sqrtOneMinusScalarSq;
        z = p.z() / sqrtOneMinusScalarSq;
      }

      return CVector4D_t(
                {
                  x,
                  y,
                  z,
                  2.0f * acos(p.scalar())
                });
    }

    CQuaternion rotate(const CQuaternion& q,
      const CQuaternion& p) {
      // This function assumes a unit CQuaternion be passed!
      return (q*p)*q.conjugate();
    }

    CVector3D_t rotate(
            CQuaternion const &aQuaternion,
            CVector3D_t const &aVector)
    {
      CQuaternion p = CQuaternion(0.0f, aVector);
      return rotate(aQuaternion, p).vector();
    }

    CQuaternion operator+(
            CQuaternion const &aQuaternionLHS,
            CQuaternion const &aQuaternionRHS)
    {
      return CQuaternion(
                  aQuaternionLHS.w() + aQuaternionRHS.w(),
                  aQuaternionLHS.x() + aQuaternionRHS.x(),
                  aQuaternionLHS.y() + aQuaternionRHS.y(),
                  aQuaternionLHS.z() + aQuaternionRHS.z());
    }

    CQuaternion operator-(
            CQuaternion const &aQuaternionLHS,
            CQuaternion const &aQuaternionRHS)
    {
      return CQuaternion(
                  aQuaternionLHS.w() - aQuaternionRHS.w(),
                  aQuaternionLHS.x() - aQuaternionRHS.x(),
                  aQuaternionLHS.y() - aQuaternionRHS.y(),
                  aQuaternionLHS.z() - aQuaternionRHS.z());
    }

    CQuaternion operator*(
            CQuaternion             const &aQuaternion,
            CQuaternion::value_type const &aFactor)
    {
      return CQuaternion(
                  (aQuaternion.w() * aFactor),
                  (aQuaternion.x() * aFactor),
                  (aQuaternion.y() * aFactor),
                  (aQuaternion.z() * aFactor));
    }

    CQuaternion operator*(
            CQuaternion::value_type const &aFactor,
            CQuaternion             const &aQuaternion)
    {
      return CQuaternion(
                  (aFactor * aQuaternion.w()),
                  (aFactor * aQuaternion.x()),
                  (aFactor * aQuaternion.y()),
                  (aFactor * aQuaternion.z()));
    }

    CQuaternion operator*(
            CQuaternion const &aQuaternion,
            CVector3D_t const &aVector)
    {
      // qp = (nq*0 - dot(vq, vp)) + (nq*vp + 0*vq + cross(vq, vp))ijk

      return CQuaternion(
        -((aQuaternion.x() * aVector.x()) + (aQuaternion.y() * aVector.y()) + (aQuaternion.z() * aVector.z())),
        +((aQuaternion.w() * aVector.x()) + (aQuaternion.y() * aVector.z()) - (aQuaternion.z() * aVector.y())),
        +((aQuaternion.w() * aVector.y()) + (aQuaternion.z() * aVector.x()) - (aQuaternion.x() * aVector.z())),
        +((aQuaternion.w() * aVector.z()) + (aQuaternion.x() * aVector.y()) - (aQuaternion.y() * aVector.x()))
      );
    }

    CQuaternion operator*(
            CVector3D_t const &aVector,
            CQuaternion const &aQuaternion)
    {
      // qp = (0*0 - dot(vq, vp)) + (0*vp + 0*vq + cross(vq, vp))ijk

      return CQuaternion(
        -((aQuaternion.x() * aVector.x()) + (aQuaternion.y() * aVector.y()) + (aQuaternion.z() * aVector.z())),
        +((aQuaternion.w() * aVector.x()) + (aQuaternion.z() * aVector.y()) - (aQuaternion.y() * aVector.z())),
        +((aQuaternion.w() * aVector.y()) + (aQuaternion.x() * aVector.z()) - (aQuaternion.z() * aVector.x())),
        +((aQuaternion.w() * aVector.z()) + (aQuaternion.y() * aVector.x()) - (aQuaternion.x() * aVector.y()))
      );
    }

    CQuaternion operator*(
            CQuaternion const &aQuaternionLHS,
            CQuaternion const &aQuaternionRHS)
    {
      // qp = (nq*np - dot(vq, vp)) + (nq*vp + np*vq + cross(vq, vp))ijk

      return CQuaternion(
        (aQuaternionLHS.w() * aQuaternionRHS.w()) - (aQuaternionLHS.x() * aQuaternionRHS.x()) - (aQuaternionLHS.y() * aQuaternionRHS.y()) - (aQuaternionLHS.z() * aQuaternionRHS.z()),
        (aQuaternionLHS.w() * aQuaternionRHS.x()) + (aQuaternionLHS.x() * aQuaternionRHS.w()) + (aQuaternionLHS.y() * aQuaternionRHS.z()) - (aQuaternionLHS.z() * aQuaternionRHS.y()),
        (aQuaternionLHS.w() * aQuaternionRHS.y()) + (aQuaternionLHS.y() * aQuaternionRHS.w()) + (aQuaternionLHS.z() * aQuaternionRHS.x()) - (aQuaternionLHS.x() * aQuaternionRHS.z()),
        (aQuaternionLHS.w() * aQuaternionRHS.z()) + (aQuaternionLHS.z() * aQuaternionRHS.w()) + (aQuaternionLHS.x() * aQuaternionRHS.y()) - (aQuaternionLHS.y() * aQuaternionRHS.x())
      );
    }

    CQuaternion operator/(
            CQuaternion             const &aQuaternion,
            CQuaternion::value_type const &aFactor)
    {
      return CQuaternion(
                  (aQuaternion.w() / aFactor),
                  (aQuaternion.x() / aFactor),
                  (aQuaternion.y() / aFactor),
                  (aQuaternion.z() / aFactor));
    }
  }
}
