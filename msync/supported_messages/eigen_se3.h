#pragma once

#include "eigen_quaternion.h"

// NOTE: user may have eigen install in different location
// so we do not inlcude eigen header here in this file,
// user should handle the dependencies of eigen headers

// TODO: use Matrix<7,1> for SE3 may shade Matrix<7,1> for other usage

namespace msync {

// the 7 data is [x,y,z,qw,qx,qy,qz]
template <typename Scalar>
struct LinearInterpolaterTraits<Eigen::Matrix<Scalar, 7, 1>> {
  using T = Eigen::Matrix<Scalar, 7, 1>;
  using Tangent = Eigen::Matrix<Scalar, 6, 1>;
  using Translation = Eigen::Matrix<Scalar, 3, 1>;
  using Rotation = Eigen::Quaternion<Scalar>;

  static T plus(const T &a, const Tangent &b) { return multiply(a, Exp(b)); }

  static Tangent between(const T &from, const T &to) {
    return Log(multiply(inverse(from), to));
  }

  static T multiply(const T &a, const T &b) {
    Translation a_translation = a.template head<3>();
    Rotation a_quaternion(a(3), a(4), a(5), a(6));
    Translation b_translation = b.template head<3>();
    Rotation b_quaternion(b(3), b(4), b(5), b(6));
    Rotation new_q = a_quaternion * b_quaternion;
    Translation new_t =
        a_quaternion._transformVector(b_translation) + a_translation;
    T result;
    result.template head<3>() = new_t;
    result.template tail<4>() << new_q.w(), new_q.x(), new_q.y(), new_q.z();
    return result;
  }

  static T inverse(const T &a) {
    Rotation a_quaternion(a(3), a(4), a(5), a(6));
    Translation a_translation = a.template head<3>();
    Rotation a_inverse_quaternion = a_quaternion.inverse();
    Translation a_inverse_translation =
        -a_inverse_quaternion._transformVector(a_translation);
    T result;
    result.template head<3>() = a_inverse_translation;
    result.template tail<4>() << a_inverse_quaternion.w(),
        a_inverse_quaternion.x(), a_inverse_quaternion.y(),
        a_inverse_quaternion.z();
    return result;
  }

  static T Exp(const Tangent &tangent) {
    T se3;
    auto quat =
        LinearInterpolaterTraits<Rotation>::Exp(tangent.template tail<3>());
    se3.template head<3>() = tangent.template head<3>();
    se3.template tail<4>() << quat.w(), quat.x(), quat.y(), quat.z();
    return se3;
  }

  static Tangent Log(const T &se3) {
    Tangent tangent;
    Rotation quat(se3(3), se3(4), se3(5), se3(6));
    tangent.template head<3>() = se3.template head<3>();
    tangent.template tail<3>() = LinearInterpolaterTraits<Rotation>::Log(quat);
    return tangent;
  }
};

} // namespace msync
