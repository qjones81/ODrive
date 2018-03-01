#ifndef __PENDULUM_CONTROLLER_H
#define __PENDULUM_CONTROLLER_H

#include <Eigen/Dense>

class KalmanFilter {
   public:
    /**
  * Create a Kalman filter with the specified matrices.
  *   A - System dynamics matrix
  *   B - System input matrix
  *   C - Measurement matrix
  *   D - Feed-forward martrix
  *   Q - Process noise covariance
  *   R - Measurement noise covariance
  *   P - Estimate error covariance
  */
    KalmanFilter(
        const Eigen::MatrixXd& A,
        const Eigen::MatrixXd& B,
        const Eigen::MatrixXd& C,
        const Eigen::MatrixXd& D,
        const Eigen::MatrixXd& Q,
        const Eigen::MatrixXd& R,
        const Eigen::MatrixXd& P);

    void init();
    void init(double t0, const Eigen::VectorXd& x0);
    void update(const Eigen::VectorXd& u, const Eigen::VectorXd& y);
    Eigen::VectorXd state() { return x_hat; }; 
    double time() { return t; };
   
   private:

   Eigen::MatrixXd A,B,C,D,Q,R,P,K,P0;

   int m,n;

   double t0, t;

   bool initialized;

   Eigen::MatrixXd I;

   Eigen::VectorXd x_hat, x_hat_new;


};

#endif /* __PENDULUM_CONTROLLER_H */
