#include <iostream>
#include "kalman.h"

    KalmanFilter::KalmanFilter(
        const Eigen::MatrixXd& A,
        const Eigen::MatrixXd& B,
        const Eigen::MatrixXd& C,
        const Eigen::MatrixXd& D,
        const Eigen::MatrixXd& Q,
        const Eigen::MatrixXd& R,
        const Eigen::MatrixXd& P) :
        A(A), B(B), C(C), D(D), Q(Q), R(R), P0(P),
        m(C.rows()), n(A.rows()), initialized(false),
         I(n,n),x_hat(n), x_hat_new(n)
        {
            I.setIdentity();
        }
