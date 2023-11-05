#pragma once

#include "../traits.h"

// NOTE: user may have eigen install in different location
// so we do not inlcude eigen header here in this file,
// user should handle the dependencies of eigen headers

namespace msync {

// eigen quaternion linear interpolater traits
template <typename Scalar>
struct LinearInterpolaterTraits<Eigen::Quaternion<Scalar>> {
  using T = Eigen::Quaternion<Scalar>;
  using Tangent = Eigen::Matrix<Scalar, 3, 1>;

  static T plus(const T &a, const Tangent &b) { return a * Exp(b); }

  static Tangent between(const T &from, const T &to) {
    return Log(from.inverse() * to);
  }

  static T Exp(const Tangent &tangent) {
    const Scalar theta = tangent.norm();
    auto normalized_tangent = tangent.normalized();
    Scalar sin_theta = sin(theta / 2.0);
    Scalar cos_theta = cos(theta / 2.0);
    T q;
    q.x() = normalized_tangent(0) * sin_theta;
    q.y() = normalized_tangent(1) * sin_theta;
    q.z() = normalized_tangent(2) * sin_theta;
    q.w() = cos_theta;
    return q;
  }

  static Tangent Log(const T &q) {
    Scalar log_coeff;
    const Scalar sin_angle_squared =
        q.coeffs().template head<3>().squaredNorm();
    if (sin_angle_squared > 1e-14) {
      const Scalar sin_angle = sqrt(sin_angle_squared);
      const Scalar cos_angle = q.w();
      /** @note If (cos_angle < 0) then angle >= pi/2 ,
       *  means : angle for angle_axis vector >= pi (== 2*angle)
       *   |-> results in correct rotation but not a normalized angle_axis
       * vector
       *
       * In that case we observe that 2 * angle ~ 2 * angle - 2 * pi,
       * which is equivalent saying
       *
       * angle - pi = atan(sin(angle - pi), cos(angle - pi))
       *            = atan(-sin(angle), -cos(angle))
       */
      const Scalar two_angle =
          Scalar(2.0) * ((cos_angle < Scalar(0.0))
                             ? Scalar(atan2(-sin_angle, -cos_angle))
                             : Scalar(atan2(sin_angle, cos_angle)));

      log_coeff = two_angle / sin_angle;
    } else {
      log_coeff = Scalar(2.0);
    }

    return q.coeffs().template head<3>() * log_coeff;

    // The method is ok,but the angle_axis's angle is different with the behind
    // method.
    // auto qn = q.normalized(); float qw = qn.w(); float qx = qn.x();
    // float qy = qn.y();
    // float qz = qn.z();
    // float denominator_temp = sqrt(1 - qw * qw);
    // Scalar angle = 2 * std::acos(qw);
    // Scalar x = qx / denominator_temp;
    // Scalar y = qy / denominator_temp;
    // Scalar z = qz / denominator_temp;
    // Tangent tangent(x, y, z);
    // tangent *= angle;
    // return tangent;
  }
};

} // namespace msync
