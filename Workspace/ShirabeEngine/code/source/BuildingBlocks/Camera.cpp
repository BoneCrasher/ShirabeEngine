#include "BuildingBlocks/Camera.h"

//#ifdef PLATFORM_WINDOWS 
//#include <DirectXMath.h>
//#endif

// #include "Platform/MathAdapter.h"
#include <Math/Geometric/Rect.h>

namespace Engine {

  Camera::Camera()
    : m_viewType(CameraViewType::FreeCamera)
    , m_frustumParameters(FrustumParameters::Default())
    , m_projectionParameters(ProjectionParameters::Default())
    , m_lookAtTarget({ 0, 0, 0 })
  {
  }

  Camera::Camera(Camera const& cam)
    : m_viewType(cam.viewType())
    , m_frustumParameters(cam.frustumParameters())
    , m_projectionParameters(cam.projectionParameters())
    , m_lookAtTarget(cam.lookAtTarget())
  {
  }

  Camera::Camera(
    CameraViewType       const&viewType,
    FrustumParameters    const&frustumParameters,
    ProjectionParameters const&projectionParameters,
    Vector3D             const&lookAt)
    : m_viewType(viewType)
    , m_frustumParameters(frustumParameters)
    , m_projectionParameters(projectionParameters)
    , m_lookAtTarget(lookAt)
  {
  }

  Camera::~Camera()
  {
  }

  static Matrix4x4 lookTo(
    Vector3D         const&eye,
    Vector3D         const&up,
    Vector3D         const&forward,
    CoordinateSystem const&coordinateSystem = CoordinateSystem::LH) 
{
    Vector3D n_up      = Math::normalize(up);
    Vector3D n_forward = Math::normalize(forward);
    Vector3D n_right   = Math::normalize(Math::cross(n_forward, n_up));

    Matrix4x4 orientation ={
      n_right.x(), n_up.x(), n_forward.x(), 0.0f,
      n_right.y(), n_up.y(), n_forward.y(), 0.0f,
      n_right.z(), n_up.z(), n_forward.z(), 0.0f,
      0.0f,        0.0f,     0.0f,          1.0f
    };

    Matrix4x4 translation ={
       1.0f,     0.0f,     0.0f,    0.0f,
       0.0f,     1.0f,     0.0f,    0.0f,
       0.0f,     0.0f,     1.0f,    0.0f,
      -eye.x(), -eye.y(), -eye.z(), 1.0f
    };

    return SMMatrixMultiply(orientation, translation);
  }

  static Matrix4x4 lookAt(
    Vector3D         const&eye,
    Vector3D         const&target,
    Vector3D         const&up,
    CoordinateSystem const&coordinateSystem = CoordinateSystem::RH) {
    Vector3D n_up      = Math::normalize(up);
    Vector3D n_forward = Math::normalize((target - eye));
    Vector3D n_right   = Math::normalize(Math::cross(n_forward, n_up));

    Matrix4x4 orientation ={
      n_right.x(), n_up.x(), n_forward.x(), 0.0f,
      n_right.y(), n_up.y(), n_forward.y(), 0.0f,
      n_right.z(), n_up.z(), n_forward.z(), 0.0f,
      0.0f,        0.0f,     0.0f,          1.0f
    };

    Matrix4x4 translation ={
      1.0f,     0.0f,     0.0f,    0.0f,
      0.0f,     1.0f,     0.0f,    0.0f,
      0.0f,     0.0f,     1.0f,    0.0f,
      -eye.x(), -eye.y(), -eye.z(), 1.0f
    };

    return SMMatrixMultiply(orientation, translation);
  }

  static Matrix4x4 projectionPerspectiveRect(
    Vector4D         const&bounds,
    double           const&near,
    double           const&far,
    CoordinateSystem const&coordinateSystem = CoordinateSystem::LH)
  {
    float coordinateSystemFactor =  ((coordinateSystem == CoordinateSystem::LH) ? 1.0 : -1.0);

    float l      = bounds.x();
    float t      = bounds.y();
    float r      = bounds.z();
    float b      = bounds.w();
    float f      = far;
    float n      = near;

    float width  = (r - l);
    float height = (t - b);
    float depth  = (f - n);
    float n2     = 2.0f * n;
    float fn2    = f * n2;

    return {
        (n2 / width),      0.0f,                0.0f,                                     0.0f,
        0.0f,              (n2 / height),       0.0f,                                     0.0f,
        ((r + l) / width), ((t + b) / height), -((f + n) / depth),                        coordinateSystemFactor,
        0.0f,              0.0f,               -((fn2 / depth) * coordinateSystemFactor), 0.0f
    };
  }
  static Matrix4x4 projectionPerspectiveFOV(
    Rect             const&bounds,
    double           const&near,
    double           const&far,
    double           const&fovY,
    CoordinateSystem const&coordinateSystem = CoordinateSystem::RH)
  {
    float aspect                 =  ((float)bounds.size.x() / (float)bounds.size.y());
    float fovFactorRad           =  ((fovY/2.0f) * (M_PI/180.0f));
    float t                      =  tan(fovFactorRad) * near;
    float b                      = -t;
    float r                      =  t * aspect;
    float l                      = -r;

    return projectionPerspectiveRect({ l, t, r, b }, near, far, coordinateSystem);
  }

  static Matrix4x4 projectionOrtho(
    Vector4D         const&bounds,
    double           const&near,
    double           const&far,
    CoordinateSystem const&coordinateSystem = CoordinateSystem::RH)
  {
    float coordinateSystemFactor =  ((coordinateSystem == CoordinateSystem::LH) ? 1.0 : -1.0);

    float l      = bounds.x();
    float t      = bounds.y();
    float r      = bounds.z();
    float b      = bounds.w();
    float f      = far;
    float n      = near;

    return
    {
      (2.0f / (r - l)), 0.0f,             0.0f,           -((r + l) / (r - l)),
      0.0f,             (2.0f / (t - b)), 0.0f,           -((t + b) / (t - b)),
      0.0f,             0.0f,             -(2 / (f - n)), -((f + n) / (f - n)) * coordinateSystemFactor,
      0.0f,             0.0f,             0.0f,           1.0f
    };
  }

  void Camera::createViewMatrix(CoordinateSystem const&coordinateSystem)
  {
    Matrix4x4 mat{};
    Vector3D  position = m_transform.translation();
    Vector3D  up       = m_transform.up();

    switch(m_viewType) {
    case CameraViewType::TargetCamera:
      mat = lookAt(position, m_lookAtTarget, up, coordinateSystem);
      break;
    case CameraViewType::FreeCamera:
      mat = lookTo(position, up, m_transform.forward());
      break;
    }

    m_viewMatrix = mat;
  }

  void Camera::createProjectionMatrix(CoordinateSystem const&coordinateSystem)
  {
    Matrix4x4 mat{};

    switch(m_projectionParameters.projectionType) {
    case CameraProjectionType::Perspective:
      mat =
        projectionPerspectiveFOV(
          { 0, 0, m_frustumParameters.width, m_frustumParameters.height },
          m_frustumParameters.nearPlaneDistance,
          m_frustumParameters.farPlaneDistance,
          m_frustumParameters.fovY,
          coordinateSystem);
      break;
    case CameraProjectionType::Orthographic:
      mat =
        projectionOrtho(
          {
            (0.5f * m_frustumParameters.width),
            (0.5f * m_frustumParameters.height),
            (0.5f * m_frustumParameters.width),
            (0.5f * m_frustumParameters.height)
          },
          m_frustumParameters.nearPlaneDistance,
          m_frustumParameters.farPlaneDistance,
          coordinateSystem);
      break;
    }

    m_projectionMatrix = mat;
  }
}