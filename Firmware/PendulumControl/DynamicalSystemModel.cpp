#include "DynamicalSystemModel.h"

DynamicalSystemModel::DynamicalSystemModel()
  : initialized(false)
{
}

StateSpaceSystemModel::StateSpaceSystemModel(
    const Eigen::MatrixXf& A,
    const Eigen::MatrixXf& B,
    const Eigen::MatrixXf& C,
    const Eigen::MatrixXf& D)
  : A_(A), B_(B), C_(C), D_(D),
    m(C.rows()), q(B.rows()), n(A.rows()),
    I_(n, n), x_(n)
{
   I_.setIdentity();
}

void StateSpaceSystemModel::init()
{
    Eigen::VectorXf x0 = Eigen::VectorXf::Zero(n);
    init(x0);
}
void StateSpaceSystemModel::init(const Eigen::VectorXf& x0)
{
    x_ = x0;
    initialized = true;
}
void StateSpaceSystemModel::update(const Eigen::VectorXf& u) {
    x_ = f(x_, u);
}
Eigen::VectorXf StateSpaceSystemModel::f(const Eigen::VectorXf& x, const Eigen::VectorXf& u)
{
    return A_ * x + B_ * u;
}

KalmanFilter::KalmanFilter(
    const Eigen::MatrixXf& A,
    const Eigen::MatrixXf& B,
    const Eigen::MatrixXf& C,
    const Eigen::MatrixXf& D,
    const Eigen::MatrixXf& Q,
    const Eigen::MatrixXf& R,
    const Eigen::MatrixXf& P)
    : StateSpaceSystemModel(A, B, C, D),
      Q_(Q),
      R_(R),
      P_(P) {}
      
void KalmanFilter::predict(const Eigen::VectorXf& u) {
   // x_ = A_ * x_ + B_ * u;
    x_ = f(x_, u);
    P_ = A_ * P_ * A_.transpose() + Q_;
}
void KalmanFilter::update(const Eigen::VectorXf& y) {
    K_ = P_ * C_.transpose() * (C_ * P_ * C_.transpose() + R_).inverse();
    x_ += K_ * (y - C_ * x_);
    P_ = (I_ - K_ * C_) * P_;
    //x_hat = x_hat_new;
}
