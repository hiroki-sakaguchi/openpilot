#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_6399721819463021516);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_3334656739795904655);
void pose_H_mod_fun(double *state, double *out_4849798422577497893);
void pose_f_fun(double *state, double dt, double *out_7276829301405261923);
void pose_F_fun(double *state, double dt, double *out_2644685186844474170);
void pose_h_4(double *state, double *unused, double *out_8115001930196127561);
void pose_H_4(double *state, double *unused, double *out_4095637420522785286);
void pose_h_10(double *state, double *unused, double *out_8853117002444431783);
void pose_H_10(double *state, double *unused, double *out_2122054065778683402);
void pose_h_13(double *state, double *unused, double *out_4983375721393997945);
void pose_H_13(double *state, double *unused, double *out_3514993787793915643);
void pose_h_14(double *state, double *unused, double *out_823804089178252810);
void pose_H_14(double *state, double *unused, double *out_132396564183300757);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}