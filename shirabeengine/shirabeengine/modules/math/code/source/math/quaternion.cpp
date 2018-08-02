#include "math/quaternion.h"
#include "math/common.h"

namespace Engine
{
    namespace Math
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CQuaternion::CQuaternion()
            : CVector4D_t({ 0, 0, 0, 0 })
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion::CQuaternion(CQuaternion const &aOther)
            : CVector4D_t(aOther)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion::CQuaternion(CAxisAngle const &aAxisAngle)
            : CQuaternion(CQuaternion::CQuaternionFromAxisAngle(aAxisAngle.axis(), aAxisAngle.phi()))
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion::CQuaternion(
                ValueType_t const &aPhi,
                CVector3D_t const &aAxis)
            : CVector4D_t({ aPhi, aAxis.x(), aAxis.y(), aAxis.z() })
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion::CQuaternion(
                ValueType_t const &aPhi,
                ValueType_t const &aAxisX,
                ValueType_t const &aAxisY,
                ValueType_t const &aAxisZ)
            : CVector4D_t({ aPhi, aAxisX, aAxisY, aAxisZ })
        {
        }

        CQuaternion::~CQuaternion() { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CQuaternion::assign(
                ValueType_t const &aPhi,
                ValueType_t const &aAxisX,
                ValueType_t const &aAxisY,
                ValueType_t const &aAxisZ)
        {
            this->w(aPhi);
            this->x(aAxisX);
            this->y(aAxisY);
            this->z(aAxisZ);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CQuaternion::assign(const CQuaternion& aOther)
        {
            this->w(aOther.w());
            this->x(aOther.x());
            this->y(aOther.y());
            this->z(aOther.z());
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion& CQuaternion::operator+=(CQuaternion const &aOther)
        {
            this->w(w() + aOther.w());
            this->x(x() + aOther.x());
            this->y(y() + aOther.y());
            this->z(z() + aOther.z());

            return *this;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion& CQuaternion::operator-=(CQuaternion const &aOther)
        {
            this->w(w() - aOther.w());
            this->x(x() - aOther.x());
            this->y(y() - aOther.y());
            this->z(z() - aOther.z());

            return *this;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion& CQuaternion::operator*=(ValueType_t const &aFactor)
        {
            this->w(w() * aFactor);
            this->x(x() * aFactor);
            this->y(y() * aFactor);
            this->z(z() * aFactor);

            return *this;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion& CQuaternion::operator*=(CQuaternion const &aOther)
        {
            ValueType_t const phiProduct        = (this->w() * aOther.w());
            ValueType_t const axisDotProduct    = dot(this->vector(), aOther.vector());

            CVector3D_t const scaledThisVector  = this->vector() * aOther.w();
            CVector3D_t const scaledOtherVecotr = this->w()      * aOther.vector();
            CVector3D_t const crossVector       = cross(this->vector(), aOther.vector());

            ValueType_t const scale  = (phiProduct - axisDotProduct);
            CVector3D_t const vector = (scaledThisVector + scaledOtherVecotr) + crossVector;

            assign(scale, vector.x(), vector.y(), vector.z());

            return *this;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion& CQuaternion::operator/=(ValueType_t const &aFactor)
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
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion::ValueType_t CQuaternion::magnitude() const
        {
            return static_cast<ValueType_t>(sqrt(w()*w() + x()*x() + y()*y() + z()*z()));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion CQuaternion::conjugate() const
        {
            return CQuaternion(w(), -x(), -y(), -z());
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion CQuaternion::inverse() const
        {
            // Decays to inverse == conjugate if we deal with a unit CQuaternion.
            ValueType_t const mag = magnitude();
            CQuaternion const qua = (conjugate() / (mag * mag));

            return qua;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion CQuaternion::normalize() const
        {
            ValueType_t const N = magnitude();

            CQuaternion const q
                    = CQuaternion(
                        w() / N,
                        x() / N,
                        y() / N,
                        z() / N
                        );

            return q;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion::ValueType_t CQuaternion::rotationAngle(CQuaternion const &aQuaternion)
        {
            return static_cast<ValueType_t>(2 * acos(aQuaternion.w()));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CVector3D_t CQuaternion::rotationAxis(CQuaternion const &aQuaternion)
        {
            // Threshold to allow for rounding errors.
            static constexpr ValueType_t const TOLERANCE = 0.005f;

            CVector3D_t const vector = aQuaternion.vector();
            ValueType_t const m      = vector.length();

            if(m <= TOLERANCE)
            {
                return CVector3D_t();
            }
            else
            {
                return (vector / m);
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion CQuaternion::CQuaternionFromEuler(
                ValueType_t const &aX,
                ValueType_t const &aY,
                ValueType_t const &aZ)
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

            ValueType_t const phi = aX; // deg_to_rad(x);
            ValueType_t const tau = aY; // deg_to_rad(y);
            ValueType_t const psi = aZ; // deg_to_rad(z);

            ValueType_t const cos_psi = cos(0.5f * psi);
            ValueType_t const cos_tau = cos(0.5f * tau);
            ValueType_t const cos_phi = cos(0.5f * phi);
            ValueType_t const sin_psi = sin(0.5f * psi);
            ValueType_t const sin_tau = sin(0.5f * tau);
            ValueType_t const sin_phi = sin(0.5f * phi);

            ValueType_t const cos_psi_cos_tau = (cos_psi * cos_tau);
            ValueType_t const sin_psi_sin_tau = (sin_psi * sin_tau);
            ValueType_t const cos_psi_sin_tau = (cos_psi * sin_tau);
            ValueType_t const sin_psi_cos_tau = (sin_psi * cos_tau);

            CQuaternion const q
                    = CQuaternion(
                        ((cos_psi_cos_tau * cos_phi) + (sin_psi_sin_tau * sin_phi)),
                        ((cos_psi_cos_tau * sin_phi) - (sin_psi_sin_tau * cos_phi)),
                        ((cos_psi_sin_tau * cos_phi) + (sin_psi_cos_tau * sin_phi)),
                        ((sin_psi_cos_tau * cos_phi) - (cos_psi_sin_tau * sin_phi))
                        );

            return q;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion CQuaternion::CQuaternionFromRotationMatrix(CMatrix4x4 const &aMatrix) {
            ValueType_t const r00 = aMatrix.r00();
            ValueType_t const r11 = aMatrix.r11();
            ValueType_t const r22 = aMatrix.r22();

            ValueType_t q_w = 0.0f;
            ValueType_t q_x = 0.0f;
            ValueType_t q_y = 0.0f;
            ValueType_t q_z = 0.0f;

            ValueType_t trace = (r00 + r11 + r22);
            if(trace > 0) {

                ValueType_t const S = sqrtf(trace + 1.0f) * 2.0f; // S = 4qw
                q_w = (0.25f * S);
                q_x = (aMatrix.r21() - aMatrix.r12()) / S;
                q_y = (aMatrix.r02() - aMatrix.r20()) / S;
                q_z = (aMatrix.r10() - aMatrix.r01()) / S;
            }
            else if((r00 > r11) & (r00 > r22))
            {
                ValueType_t const S = sqrtf(1.0f + r00 - r11 - r22) * 2.0f; // S=4qx
                q_w = (aMatrix.r21() - aMatrix.r12()) / S;
                q_x = (0.25f * S);
                q_y = (aMatrix.r01() + aMatrix.r10()) / S;
                q_z = (aMatrix.r02() + aMatrix.r20()) / S;
            }
            else if(r11 > r22)
            {
                ValueType_t const S = sqrtf(1.0f + r11 - r00 - r22) * 2.0f; // S = 4qy
                q_w = (aMatrix.r02() - aMatrix.r20()) / S;
                q_x = (aMatrix.r01() + aMatrix.r10()) / S;
                q_y = (0.25f * S);
                q_z = (aMatrix.r12() + aMatrix.r21()) / S;
            }
            else
            {
                ValueType_t const S = sqrtf(1.0f + r22 - r00 - r11) * 2.0f; // S = 4qz
                q_w = (aMatrix.r10() - aMatrix.r01()) / S;
                q_x = (aMatrix.r02() + aMatrix.r20()) / S;
                q_y = (aMatrix.r12() + aMatrix.r21()) / S;
                q_z = (0.25f * S);
            }

            return CQuaternion(q_w, q_x, q_y, q_z);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion CQuaternion::CQuaternionFromAxisAngle(
                CVector3D_t const &aAxis,
                ValueType_t const &aPhi)
        {
            ValueType_t const hsin = sinf(aPhi / 2.0f);
            ValueType_t const hcos = cosf(aPhi / 2.0f);

            CQuaternion q
                    = CQuaternion(
                        hcos,
                        aAxis.x() * hsin,
                        aAxis.y() * hsin,
                        aAxis.z() * hsin);

            return q;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CVector3D_t CQuaternion::eulerFromCQuaternion(CQuaternion const &aQuaternion)
        {
            CVector3D_t v {};

            ValueType_t const aQuaternion00saQuaternion = aQuaternion.scalar()     * aQuaternion.scalar();
            ValueType_t const aQuaternion11saQuaternion = aQuaternion.vector().x() * aQuaternion.vector().x();
            ValueType_t const aQuaternion22saQuaternion = aQuaternion.vector().y() * aQuaternion.vector().y();
            ValueType_t const aQuaternion33saQuaternion = aQuaternion.vector().z() * aQuaternion.vector().z();

            ValueType_t r00 =     aQuaternion00saQuaternion + aQuaternion11saQuaternion
                                  - aQuaternion22saQuaternion - aQuaternion33saQuaternion;
            ValueType_t r10 = 2 * ((aQuaternion.vector().x() * aQuaternion.vector().y())
                                  + (aQuaternion.scalar()      * aQuaternion.vector().z()));
            ValueType_t r20 = 2 * ((aQuaternion.vector().x() * aQuaternion.vector().z())
                                  - (aQuaternion.scalar()      * aQuaternion.vector().y()));
            ValueType_t r21 = 2 * ((aQuaternion.vector().y() * aQuaternion.vector().z())
                                  + (aQuaternion.scalar()      * aQuaternion.vector().x()));

            ValueType_t r22 =     aQuaternion00saQuaternion - aQuaternion11saQuaternion
                                  - aQuaternion22saQuaternion + aQuaternion33saQuaternion;

            ValueType_t r01 = 0;
            ValueType_t r02 = 0;

            ValueType_t const tmp = fabs(r20);
            if(tmp > 0.999999f)
            {
                // Test for pole singularities
                r01 = 2 * ((aQuaternion.vector().x() * aQuaternion.vector().y()) -
                           (aQuaternion.scalar()     * aQuaternion.vector().z()));
                r02 = 2 * ((aQuaternion.vector().x() * aQuaternion.vector().z()) +
                           (aQuaternion.scalar()     * aQuaternion.vector().y()));

                v = CVector3D_t({
                                    rad_to_deg(0.0f),                                    // phi
                                    rad_to_deg((-((ValueType_t)M_PI / 2.0f)*r20 / tmp)), // tau
                                    rad_to_deg(atan2(-r01, -r20*r02))                    // psi
                                });
            }
            else
            {
                v = CVector3D_t({
                                    rad_to_deg(atan2(r21, r22)), // phi
                                    rad_to_deg(asin(-r20)),      // tau
                                    rad_to_deg(atan2(r10, r00))  // psi
                                });
            }

            return v;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CMatrix4x4 CQuaternion::rotationMatrixFromCQuaternion(CQuaternion const &aQuaternion)
        {
            ValueType_t const r11 = 1 - (2 * (aQuaternion.y() * aQuaternion.y())) - (2 * (aQuaternion.z() * aQuaternion.z()));
            ValueType_t const r12 =     (2 * (aQuaternion.x() * aQuaternion.y())) - (2 * (aQuaternion.w() * aQuaternion.z()));
            ValueType_t const r13 =     (2 * (aQuaternion.x() * aQuaternion.z())) + (2 * (aQuaternion.w() * aQuaternion.y()));
            ValueType_t const r21 =     (2 * (aQuaternion.x() * aQuaternion.y())) + (2 * (aQuaternion.w() * aQuaternion.z()));
            ValueType_t const r22 = 1 - (2 * (aQuaternion.x() * aQuaternion.x())) - (2 * (aQuaternion.z() * aQuaternion.z()));
            ValueType_t const r23 =     (2 * (aQuaternion.y() * aQuaternion.z())) - (2 * (aQuaternion.w() * aQuaternion.x()));
            ValueType_t const r31 =     (2 * (aQuaternion.x() * aQuaternion.z())) - (2 * (aQuaternion.w() * aQuaternion.y()));
            ValueType_t const r32 =     (2 * (aQuaternion.y() * aQuaternion.z())) + (2 * (aQuaternion.w() * aQuaternion.x()));
            ValueType_t const r33 = 1 - (2 * (aQuaternion.x() * aQuaternion.x())) - (2 * (aQuaternion.y() * aQuaternion.y()));
            ValueType_t const r44 = 1.0;

            CMatrix4x4 mat = CMatrix4x4({ r11, r12, r13, 0.0, r21, r22, r23, 0.0, r31, r32, r33, 0.0, 0.0, 0.0, 0.0, r44 });

            return mat;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CVector4D_t CQuaternion::axisAngleFromCQuaternion(CQuaternion const &aQuaternion)
        {
            CQuaternion p = aQuaternion;

            if(p.scalar() > 1.0)
                p = p.normalize();

            ValueType_t x = 0.0;
            ValueType_t y = 0.0;
            ValueType_t z = 0.0;

            ValueType_t const sqrtOneMinusScalarSq = sqrt(1 - p.scalar() * p.scalar());
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

            CVector4D_t const v
                    = CVector4D_t({
                                      x,
                                      y,
                                      z,
                                      2.0f * acos(p.scalar())
                                  });

            return v;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion rotate(const CQuaternion& q,
                           const CQuaternion& p)
        {
            // This function assumes a unit CQuaternion be passed!
            return (q*p)*q.conjugate();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CVector3D_t rotate(
                CQuaternion const &aQuaternion,
                CVector3D_t const &aVector)
        {
            CQuaternion const p = CQuaternion(0.0f, aVector);

            return rotate(aQuaternion, p).vector();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion operator+(
                CQuaternion const &aQuaternionLHS,
                CQuaternion const &aQuaternionRHS)
        {
            CQuaternion const q
                    = CQuaternion(
                        aQuaternionLHS.w() + aQuaternionRHS.w(),
                        aQuaternionLHS.x() + aQuaternionRHS.x(),
                        aQuaternionLHS.y() + aQuaternionRHS.y(),
                        aQuaternionLHS.z() + aQuaternionRHS.z());

            return q;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion operator-(
                CQuaternion const &aQuaternionLHS,
                CQuaternion const &aQuaternionRHS)
        {
            CQuaternion const q
                    =CQuaternion(
                        aQuaternionLHS.w() - aQuaternionRHS.w(),
                        aQuaternionLHS.x() - aQuaternionRHS.x(),
                        aQuaternionLHS.y() - aQuaternionRHS.y(),
                        aQuaternionLHS.z() - aQuaternionRHS.z());

            return q;
        }
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------f
        CQuaternion operator*(
                CQuaternion             const &aQuaternion,
                CQuaternion::ValueType_t const &aFactor)
        {
            CQuaternion const q
                    = CQuaternion(
                        (aQuaternion.w() * aFactor),
                        (aQuaternion.x() * aFactor),
                        (aQuaternion.y() * aFactor),
                        (aQuaternion.z() * aFactor));

            return q;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion operator*(
                CQuaternion::ValueType_t const &aFactor,
                CQuaternion             const &aQuaternion)
        {
            CQuaternion const q
                    = CQuaternion(
                        (aFactor * aQuaternion.w()),
                        (aFactor * aQuaternion.x()),
                        (aFactor * aQuaternion.y()),
                        (aFactor * aQuaternion.z()));

            return q;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion operator*(
                CQuaternion const &aQuaternion,
                CVector3D_t const &aVector)
        {
            // qp = (nq*0 - dot(vq, vp)) + (nq*vp + 0*vq + cross(vq, vp))ijk

            CQuaternion q
                    = CQuaternion(
                        -((aQuaternion.x() * aVector.x()) + (aQuaternion.y() * aVector.y()) + (aQuaternion.z() * aVector.z())),
                        +((aQuaternion.w() * aVector.x()) + (aQuaternion.y() * aVector.z()) - (aQuaternion.z() * aVector.y())),
                        +((aQuaternion.w() * aVector.y()) + (aQuaternion.z() * aVector.x()) - (aQuaternion.x() * aVector.z())),
                        +((aQuaternion.w() * aVector.z()) + (aQuaternion.x() * aVector.y()) - (aQuaternion.y() * aVector.x()))
                        );

            return q;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion operator*(
                CVector3D_t const &aVector,
                CQuaternion const &aQuaternion)
        {
            // qp = (0*0 - dot(vq, vp)) + (0*vp + 0*vq + cross(vq, vp))ijk

            CQuaternion q
                    = CQuaternion(
                        -((aQuaternion.x() * aVector.x()) + (aQuaternion.y() * aVector.y()) + (aQuaternion.z() * aVector.z())),
                        +((aQuaternion.w() * aVector.x()) + (aQuaternion.z() * aVector.y()) - (aQuaternion.y() * aVector.z())),
                        +((aQuaternion.w() * aVector.y()) + (aQuaternion.x() * aVector.z()) - (aQuaternion.z() * aVector.x())),
                        +((aQuaternion.w() * aVector.z()) + (aQuaternion.y() * aVector.x()) - (aQuaternion.x() * aVector.y()))
                        );

            return q;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion operator*(
                CQuaternion const &aQuaternionLHS,
                CQuaternion const &aQuaternionRHS)
        {
            // qp = (nq*np - dot(vq, vp)) + (nq*vp + np*vq + cross(vq, vp))ijk

            CQuaternion q
                    = CQuaternion(
                        (aQuaternionLHS.w() * aQuaternionRHS.w()) - (aQuaternionLHS.x() * aQuaternionRHS.x()) - (aQuaternionLHS.y() * aQuaternionRHS.y()) - (aQuaternionLHS.z() * aQuaternionRHS.z()),
                        (aQuaternionLHS.w() * aQuaternionRHS.x()) + (aQuaternionLHS.x() * aQuaternionRHS.w()) + (aQuaternionLHS.y() * aQuaternionRHS.z()) - (aQuaternionLHS.z() * aQuaternionRHS.y()),
                        (aQuaternionLHS.w() * aQuaternionRHS.y()) + (aQuaternionLHS.y() * aQuaternionRHS.w()) + (aQuaternionLHS.z() * aQuaternionRHS.x()) - (aQuaternionLHS.x() * aQuaternionRHS.z()),
                        (aQuaternionLHS.w() * aQuaternionRHS.z()) + (aQuaternionLHS.z() * aQuaternionRHS.w()) + (aQuaternionLHS.x() * aQuaternionRHS.y()) - (aQuaternionLHS.y() * aQuaternionRHS.x())
                        );

            return q;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CQuaternion operator/(
                CQuaternion             const &aQuaternion,
                CQuaternion::ValueType_t const &aFactor)
        {
            CQuaternion q
                    = CQuaternion(
                        (aQuaternion.w() / aFactor),
                        (aQuaternion.x() / aFactor),
                        (aQuaternion.y() / aFactor),
                        (aQuaternion.z() / aFactor));

            return q;
        }
        //<-----------------------------------------------------------------------------
    }
}
