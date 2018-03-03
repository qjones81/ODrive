#ifndef __DYNAMICAL_SYSTEM_MODEL_H
#define __DYNAMICAL_SYSTEM_MODEL_H

#include <Eigen/Dense>

class DynamicalSystemModel {
   public:
    DynamicalSystemModel();

    virtual void init(const Eigen::VectorXf& x0) = 0;
    virtual Eigen::VectorXf state() = 0;

   protected:
    bool initialized;
};

class StateSpaceSystemModel : public DynamicalSystemModel {
   public:
    StateSpaceSystemModel(
        const Eigen::MatrixXf& A,
        const Eigen::MatrixXf& B,
        const Eigen::MatrixXf& C,
        const Eigen::MatrixXf& D);

    void init();
    void init(const Eigen::VectorXf& x0);
    // State Transition Function For The System
    Eigen::VectorXf f(const Eigen::VectorXf& x, const Eigen::VectorXf& u);
    void update(const Eigen::VectorXf& u);
    virtual Eigen::VectorXf state() { return x_; };

   protected:
    Eigen::MatrixXf A_;  // State Transition Matrix
    Eigen::MatrixXf B_;  // System Input
    Eigen::MatrixXf C_;  // System Measurement
    Eigen::MatrixXf D_;  // Feed Forward

    int m, q, n;            // State/Input Size
    Eigen::MatrixXf I_;  // Cached Identity
    Eigen::VectorXf x_;  // Current State
};

class NonLinearizedSystemModel : public DynamicalSystemModel {
   public:
    NonLinearizedSystemModel();

    void init();
    void init(const Eigen::VectorXf& x0);
    // State Transition Function For The System
    Eigen::VectorXf f(const Eigen::VectorXf& x, const Eigen::VectorXf& u);
    void updateJacobian(const Eigen::VectorXf& x, const Eigen::VectorXf& u);
    void updateJacobian(const Eigen::VectorXf& u);
    void update(const Eigen::VectorXf& u);
    virtual Eigen::VectorXf state() { return x_; };

   protected:
    int m, n, q;            // State Size
    Eigen::MatrixXf I_;  // Identity
    Eigen::MatrixXf F_;  // Jacobian
    Eigen::VectorXf x_;  // Current State
};

// TODO: Maybe this a different class and "Added" to the filter
class KalmanFilter : public StateSpaceSystemModel {
   public:
    KalmanFilter(
        const Eigen::MatrixXf& A,
        const Eigen::MatrixXf& B,
        const Eigen::MatrixXf& C,
        const Eigen::MatrixXf& D,
        const Eigen::MatrixXf& Q,
        const Eigen::MatrixXf& R,
        const Eigen::MatrixXf& P);

    void predict(const Eigen::VectorXf& u);  // State estimate and covariance
    void update(const Eigen::VectorXf& y);   // Measurement Update

   protected:
    Eigen::MatrixXf Q_;  // Process Covariance
    Eigen::MatrixXf R_;  // Measurement Covariance
    Eigen::MatrixXf P_;  // Estimate Covariance
    Eigen::MatrixXf K_;  // Kalman Gain
};

#endif /* __DYNAMICAL_SYSTEM_MODEL_H */
