#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_2134637299902994035);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_1594447846279431986);
void car_H_mod_fun(double *state, double *out_7193387902926598405);
void car_f_fun(double *state, double dt, double *out_2639133560059027935);
void car_F_fun(double *state, double dt, double *out_5906632028717766242);
void car_h_25(double *state, double *unused, double *out_90461990304450059);
void car_H_25(double *state, double *unused, double *out_3670407252937106728);
void car_h_24(double *state, double *unused, double *out_7572167216432730553);
void car_H_24(double *state, double *unused, double *out_112481654134783592);
void car_h_30(double *state, double *unused, double *out_7790810660449599126);
void car_H_30(double *state, double *unused, double *out_1152074294429858101);
void car_h_26(double *state, double *unused, double *out_5170849381919120035);
void car_H_26(double *state, double *unused, double *out_7411910571811162952);
void car_h_27(double *state, double *unused, double *out_5052841987985743291);
void car_H_27(double *state, double *unused, double *out_1071519776754085116);
void car_h_29(double *state, double *unused, double *out_8548660805610417936);
void car_H_29(double *state, double *unused, double *out_641842950115465917);
void car_h_28(double *state, double *unused, double *out_3967701424454733151);
void car_H_28(double *state, double *unused, double *out_5724241967184996491);
void car_h_31(double *state, double *unused, double *out_2291249998825916495);
void car_H_31(double *state, double *unused, double *out_3639761291060146300);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}