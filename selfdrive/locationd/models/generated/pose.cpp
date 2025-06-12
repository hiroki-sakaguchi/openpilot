#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.13.3                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_6399721819463021516) {
   out_6399721819463021516[0] = delta_x[0] + nom_x[0];
   out_6399721819463021516[1] = delta_x[1] + nom_x[1];
   out_6399721819463021516[2] = delta_x[2] + nom_x[2];
   out_6399721819463021516[3] = delta_x[3] + nom_x[3];
   out_6399721819463021516[4] = delta_x[4] + nom_x[4];
   out_6399721819463021516[5] = delta_x[5] + nom_x[5];
   out_6399721819463021516[6] = delta_x[6] + nom_x[6];
   out_6399721819463021516[7] = delta_x[7] + nom_x[7];
   out_6399721819463021516[8] = delta_x[8] + nom_x[8];
   out_6399721819463021516[9] = delta_x[9] + nom_x[9];
   out_6399721819463021516[10] = delta_x[10] + nom_x[10];
   out_6399721819463021516[11] = delta_x[11] + nom_x[11];
   out_6399721819463021516[12] = delta_x[12] + nom_x[12];
   out_6399721819463021516[13] = delta_x[13] + nom_x[13];
   out_6399721819463021516[14] = delta_x[14] + nom_x[14];
   out_6399721819463021516[15] = delta_x[15] + nom_x[15];
   out_6399721819463021516[16] = delta_x[16] + nom_x[16];
   out_6399721819463021516[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_3334656739795904655) {
   out_3334656739795904655[0] = -nom_x[0] + true_x[0];
   out_3334656739795904655[1] = -nom_x[1] + true_x[1];
   out_3334656739795904655[2] = -nom_x[2] + true_x[2];
   out_3334656739795904655[3] = -nom_x[3] + true_x[3];
   out_3334656739795904655[4] = -nom_x[4] + true_x[4];
   out_3334656739795904655[5] = -nom_x[5] + true_x[5];
   out_3334656739795904655[6] = -nom_x[6] + true_x[6];
   out_3334656739795904655[7] = -nom_x[7] + true_x[7];
   out_3334656739795904655[8] = -nom_x[8] + true_x[8];
   out_3334656739795904655[9] = -nom_x[9] + true_x[9];
   out_3334656739795904655[10] = -nom_x[10] + true_x[10];
   out_3334656739795904655[11] = -nom_x[11] + true_x[11];
   out_3334656739795904655[12] = -nom_x[12] + true_x[12];
   out_3334656739795904655[13] = -nom_x[13] + true_x[13];
   out_3334656739795904655[14] = -nom_x[14] + true_x[14];
   out_3334656739795904655[15] = -nom_x[15] + true_x[15];
   out_3334656739795904655[16] = -nom_x[16] + true_x[16];
   out_3334656739795904655[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_4849798422577497893) {
   out_4849798422577497893[0] = 1.0;
   out_4849798422577497893[1] = 0.0;
   out_4849798422577497893[2] = 0.0;
   out_4849798422577497893[3] = 0.0;
   out_4849798422577497893[4] = 0.0;
   out_4849798422577497893[5] = 0.0;
   out_4849798422577497893[6] = 0.0;
   out_4849798422577497893[7] = 0.0;
   out_4849798422577497893[8] = 0.0;
   out_4849798422577497893[9] = 0.0;
   out_4849798422577497893[10] = 0.0;
   out_4849798422577497893[11] = 0.0;
   out_4849798422577497893[12] = 0.0;
   out_4849798422577497893[13] = 0.0;
   out_4849798422577497893[14] = 0.0;
   out_4849798422577497893[15] = 0.0;
   out_4849798422577497893[16] = 0.0;
   out_4849798422577497893[17] = 0.0;
   out_4849798422577497893[18] = 0.0;
   out_4849798422577497893[19] = 1.0;
   out_4849798422577497893[20] = 0.0;
   out_4849798422577497893[21] = 0.0;
   out_4849798422577497893[22] = 0.0;
   out_4849798422577497893[23] = 0.0;
   out_4849798422577497893[24] = 0.0;
   out_4849798422577497893[25] = 0.0;
   out_4849798422577497893[26] = 0.0;
   out_4849798422577497893[27] = 0.0;
   out_4849798422577497893[28] = 0.0;
   out_4849798422577497893[29] = 0.0;
   out_4849798422577497893[30] = 0.0;
   out_4849798422577497893[31] = 0.0;
   out_4849798422577497893[32] = 0.0;
   out_4849798422577497893[33] = 0.0;
   out_4849798422577497893[34] = 0.0;
   out_4849798422577497893[35] = 0.0;
   out_4849798422577497893[36] = 0.0;
   out_4849798422577497893[37] = 0.0;
   out_4849798422577497893[38] = 1.0;
   out_4849798422577497893[39] = 0.0;
   out_4849798422577497893[40] = 0.0;
   out_4849798422577497893[41] = 0.0;
   out_4849798422577497893[42] = 0.0;
   out_4849798422577497893[43] = 0.0;
   out_4849798422577497893[44] = 0.0;
   out_4849798422577497893[45] = 0.0;
   out_4849798422577497893[46] = 0.0;
   out_4849798422577497893[47] = 0.0;
   out_4849798422577497893[48] = 0.0;
   out_4849798422577497893[49] = 0.0;
   out_4849798422577497893[50] = 0.0;
   out_4849798422577497893[51] = 0.0;
   out_4849798422577497893[52] = 0.0;
   out_4849798422577497893[53] = 0.0;
   out_4849798422577497893[54] = 0.0;
   out_4849798422577497893[55] = 0.0;
   out_4849798422577497893[56] = 0.0;
   out_4849798422577497893[57] = 1.0;
   out_4849798422577497893[58] = 0.0;
   out_4849798422577497893[59] = 0.0;
   out_4849798422577497893[60] = 0.0;
   out_4849798422577497893[61] = 0.0;
   out_4849798422577497893[62] = 0.0;
   out_4849798422577497893[63] = 0.0;
   out_4849798422577497893[64] = 0.0;
   out_4849798422577497893[65] = 0.0;
   out_4849798422577497893[66] = 0.0;
   out_4849798422577497893[67] = 0.0;
   out_4849798422577497893[68] = 0.0;
   out_4849798422577497893[69] = 0.0;
   out_4849798422577497893[70] = 0.0;
   out_4849798422577497893[71] = 0.0;
   out_4849798422577497893[72] = 0.0;
   out_4849798422577497893[73] = 0.0;
   out_4849798422577497893[74] = 0.0;
   out_4849798422577497893[75] = 0.0;
   out_4849798422577497893[76] = 1.0;
   out_4849798422577497893[77] = 0.0;
   out_4849798422577497893[78] = 0.0;
   out_4849798422577497893[79] = 0.0;
   out_4849798422577497893[80] = 0.0;
   out_4849798422577497893[81] = 0.0;
   out_4849798422577497893[82] = 0.0;
   out_4849798422577497893[83] = 0.0;
   out_4849798422577497893[84] = 0.0;
   out_4849798422577497893[85] = 0.0;
   out_4849798422577497893[86] = 0.0;
   out_4849798422577497893[87] = 0.0;
   out_4849798422577497893[88] = 0.0;
   out_4849798422577497893[89] = 0.0;
   out_4849798422577497893[90] = 0.0;
   out_4849798422577497893[91] = 0.0;
   out_4849798422577497893[92] = 0.0;
   out_4849798422577497893[93] = 0.0;
   out_4849798422577497893[94] = 0.0;
   out_4849798422577497893[95] = 1.0;
   out_4849798422577497893[96] = 0.0;
   out_4849798422577497893[97] = 0.0;
   out_4849798422577497893[98] = 0.0;
   out_4849798422577497893[99] = 0.0;
   out_4849798422577497893[100] = 0.0;
   out_4849798422577497893[101] = 0.0;
   out_4849798422577497893[102] = 0.0;
   out_4849798422577497893[103] = 0.0;
   out_4849798422577497893[104] = 0.0;
   out_4849798422577497893[105] = 0.0;
   out_4849798422577497893[106] = 0.0;
   out_4849798422577497893[107] = 0.0;
   out_4849798422577497893[108] = 0.0;
   out_4849798422577497893[109] = 0.0;
   out_4849798422577497893[110] = 0.0;
   out_4849798422577497893[111] = 0.0;
   out_4849798422577497893[112] = 0.0;
   out_4849798422577497893[113] = 0.0;
   out_4849798422577497893[114] = 1.0;
   out_4849798422577497893[115] = 0.0;
   out_4849798422577497893[116] = 0.0;
   out_4849798422577497893[117] = 0.0;
   out_4849798422577497893[118] = 0.0;
   out_4849798422577497893[119] = 0.0;
   out_4849798422577497893[120] = 0.0;
   out_4849798422577497893[121] = 0.0;
   out_4849798422577497893[122] = 0.0;
   out_4849798422577497893[123] = 0.0;
   out_4849798422577497893[124] = 0.0;
   out_4849798422577497893[125] = 0.0;
   out_4849798422577497893[126] = 0.0;
   out_4849798422577497893[127] = 0.0;
   out_4849798422577497893[128] = 0.0;
   out_4849798422577497893[129] = 0.0;
   out_4849798422577497893[130] = 0.0;
   out_4849798422577497893[131] = 0.0;
   out_4849798422577497893[132] = 0.0;
   out_4849798422577497893[133] = 1.0;
   out_4849798422577497893[134] = 0.0;
   out_4849798422577497893[135] = 0.0;
   out_4849798422577497893[136] = 0.0;
   out_4849798422577497893[137] = 0.0;
   out_4849798422577497893[138] = 0.0;
   out_4849798422577497893[139] = 0.0;
   out_4849798422577497893[140] = 0.0;
   out_4849798422577497893[141] = 0.0;
   out_4849798422577497893[142] = 0.0;
   out_4849798422577497893[143] = 0.0;
   out_4849798422577497893[144] = 0.0;
   out_4849798422577497893[145] = 0.0;
   out_4849798422577497893[146] = 0.0;
   out_4849798422577497893[147] = 0.0;
   out_4849798422577497893[148] = 0.0;
   out_4849798422577497893[149] = 0.0;
   out_4849798422577497893[150] = 0.0;
   out_4849798422577497893[151] = 0.0;
   out_4849798422577497893[152] = 1.0;
   out_4849798422577497893[153] = 0.0;
   out_4849798422577497893[154] = 0.0;
   out_4849798422577497893[155] = 0.0;
   out_4849798422577497893[156] = 0.0;
   out_4849798422577497893[157] = 0.0;
   out_4849798422577497893[158] = 0.0;
   out_4849798422577497893[159] = 0.0;
   out_4849798422577497893[160] = 0.0;
   out_4849798422577497893[161] = 0.0;
   out_4849798422577497893[162] = 0.0;
   out_4849798422577497893[163] = 0.0;
   out_4849798422577497893[164] = 0.0;
   out_4849798422577497893[165] = 0.0;
   out_4849798422577497893[166] = 0.0;
   out_4849798422577497893[167] = 0.0;
   out_4849798422577497893[168] = 0.0;
   out_4849798422577497893[169] = 0.0;
   out_4849798422577497893[170] = 0.0;
   out_4849798422577497893[171] = 1.0;
   out_4849798422577497893[172] = 0.0;
   out_4849798422577497893[173] = 0.0;
   out_4849798422577497893[174] = 0.0;
   out_4849798422577497893[175] = 0.0;
   out_4849798422577497893[176] = 0.0;
   out_4849798422577497893[177] = 0.0;
   out_4849798422577497893[178] = 0.0;
   out_4849798422577497893[179] = 0.0;
   out_4849798422577497893[180] = 0.0;
   out_4849798422577497893[181] = 0.0;
   out_4849798422577497893[182] = 0.0;
   out_4849798422577497893[183] = 0.0;
   out_4849798422577497893[184] = 0.0;
   out_4849798422577497893[185] = 0.0;
   out_4849798422577497893[186] = 0.0;
   out_4849798422577497893[187] = 0.0;
   out_4849798422577497893[188] = 0.0;
   out_4849798422577497893[189] = 0.0;
   out_4849798422577497893[190] = 1.0;
   out_4849798422577497893[191] = 0.0;
   out_4849798422577497893[192] = 0.0;
   out_4849798422577497893[193] = 0.0;
   out_4849798422577497893[194] = 0.0;
   out_4849798422577497893[195] = 0.0;
   out_4849798422577497893[196] = 0.0;
   out_4849798422577497893[197] = 0.0;
   out_4849798422577497893[198] = 0.0;
   out_4849798422577497893[199] = 0.0;
   out_4849798422577497893[200] = 0.0;
   out_4849798422577497893[201] = 0.0;
   out_4849798422577497893[202] = 0.0;
   out_4849798422577497893[203] = 0.0;
   out_4849798422577497893[204] = 0.0;
   out_4849798422577497893[205] = 0.0;
   out_4849798422577497893[206] = 0.0;
   out_4849798422577497893[207] = 0.0;
   out_4849798422577497893[208] = 0.0;
   out_4849798422577497893[209] = 1.0;
   out_4849798422577497893[210] = 0.0;
   out_4849798422577497893[211] = 0.0;
   out_4849798422577497893[212] = 0.0;
   out_4849798422577497893[213] = 0.0;
   out_4849798422577497893[214] = 0.0;
   out_4849798422577497893[215] = 0.0;
   out_4849798422577497893[216] = 0.0;
   out_4849798422577497893[217] = 0.0;
   out_4849798422577497893[218] = 0.0;
   out_4849798422577497893[219] = 0.0;
   out_4849798422577497893[220] = 0.0;
   out_4849798422577497893[221] = 0.0;
   out_4849798422577497893[222] = 0.0;
   out_4849798422577497893[223] = 0.0;
   out_4849798422577497893[224] = 0.0;
   out_4849798422577497893[225] = 0.0;
   out_4849798422577497893[226] = 0.0;
   out_4849798422577497893[227] = 0.0;
   out_4849798422577497893[228] = 1.0;
   out_4849798422577497893[229] = 0.0;
   out_4849798422577497893[230] = 0.0;
   out_4849798422577497893[231] = 0.0;
   out_4849798422577497893[232] = 0.0;
   out_4849798422577497893[233] = 0.0;
   out_4849798422577497893[234] = 0.0;
   out_4849798422577497893[235] = 0.0;
   out_4849798422577497893[236] = 0.0;
   out_4849798422577497893[237] = 0.0;
   out_4849798422577497893[238] = 0.0;
   out_4849798422577497893[239] = 0.0;
   out_4849798422577497893[240] = 0.0;
   out_4849798422577497893[241] = 0.0;
   out_4849798422577497893[242] = 0.0;
   out_4849798422577497893[243] = 0.0;
   out_4849798422577497893[244] = 0.0;
   out_4849798422577497893[245] = 0.0;
   out_4849798422577497893[246] = 0.0;
   out_4849798422577497893[247] = 1.0;
   out_4849798422577497893[248] = 0.0;
   out_4849798422577497893[249] = 0.0;
   out_4849798422577497893[250] = 0.0;
   out_4849798422577497893[251] = 0.0;
   out_4849798422577497893[252] = 0.0;
   out_4849798422577497893[253] = 0.0;
   out_4849798422577497893[254] = 0.0;
   out_4849798422577497893[255] = 0.0;
   out_4849798422577497893[256] = 0.0;
   out_4849798422577497893[257] = 0.0;
   out_4849798422577497893[258] = 0.0;
   out_4849798422577497893[259] = 0.0;
   out_4849798422577497893[260] = 0.0;
   out_4849798422577497893[261] = 0.0;
   out_4849798422577497893[262] = 0.0;
   out_4849798422577497893[263] = 0.0;
   out_4849798422577497893[264] = 0.0;
   out_4849798422577497893[265] = 0.0;
   out_4849798422577497893[266] = 1.0;
   out_4849798422577497893[267] = 0.0;
   out_4849798422577497893[268] = 0.0;
   out_4849798422577497893[269] = 0.0;
   out_4849798422577497893[270] = 0.0;
   out_4849798422577497893[271] = 0.0;
   out_4849798422577497893[272] = 0.0;
   out_4849798422577497893[273] = 0.0;
   out_4849798422577497893[274] = 0.0;
   out_4849798422577497893[275] = 0.0;
   out_4849798422577497893[276] = 0.0;
   out_4849798422577497893[277] = 0.0;
   out_4849798422577497893[278] = 0.0;
   out_4849798422577497893[279] = 0.0;
   out_4849798422577497893[280] = 0.0;
   out_4849798422577497893[281] = 0.0;
   out_4849798422577497893[282] = 0.0;
   out_4849798422577497893[283] = 0.0;
   out_4849798422577497893[284] = 0.0;
   out_4849798422577497893[285] = 1.0;
   out_4849798422577497893[286] = 0.0;
   out_4849798422577497893[287] = 0.0;
   out_4849798422577497893[288] = 0.0;
   out_4849798422577497893[289] = 0.0;
   out_4849798422577497893[290] = 0.0;
   out_4849798422577497893[291] = 0.0;
   out_4849798422577497893[292] = 0.0;
   out_4849798422577497893[293] = 0.0;
   out_4849798422577497893[294] = 0.0;
   out_4849798422577497893[295] = 0.0;
   out_4849798422577497893[296] = 0.0;
   out_4849798422577497893[297] = 0.0;
   out_4849798422577497893[298] = 0.0;
   out_4849798422577497893[299] = 0.0;
   out_4849798422577497893[300] = 0.0;
   out_4849798422577497893[301] = 0.0;
   out_4849798422577497893[302] = 0.0;
   out_4849798422577497893[303] = 0.0;
   out_4849798422577497893[304] = 1.0;
   out_4849798422577497893[305] = 0.0;
   out_4849798422577497893[306] = 0.0;
   out_4849798422577497893[307] = 0.0;
   out_4849798422577497893[308] = 0.0;
   out_4849798422577497893[309] = 0.0;
   out_4849798422577497893[310] = 0.0;
   out_4849798422577497893[311] = 0.0;
   out_4849798422577497893[312] = 0.0;
   out_4849798422577497893[313] = 0.0;
   out_4849798422577497893[314] = 0.0;
   out_4849798422577497893[315] = 0.0;
   out_4849798422577497893[316] = 0.0;
   out_4849798422577497893[317] = 0.0;
   out_4849798422577497893[318] = 0.0;
   out_4849798422577497893[319] = 0.0;
   out_4849798422577497893[320] = 0.0;
   out_4849798422577497893[321] = 0.0;
   out_4849798422577497893[322] = 0.0;
   out_4849798422577497893[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_7276829301405261923) {
   out_7276829301405261923[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_7276829301405261923[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_7276829301405261923[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_7276829301405261923[3] = dt*state[12] + state[3];
   out_7276829301405261923[4] = dt*state[13] + state[4];
   out_7276829301405261923[5] = dt*state[14] + state[5];
   out_7276829301405261923[6] = state[6];
   out_7276829301405261923[7] = state[7];
   out_7276829301405261923[8] = state[8];
   out_7276829301405261923[9] = state[9];
   out_7276829301405261923[10] = state[10];
   out_7276829301405261923[11] = state[11];
   out_7276829301405261923[12] = state[12];
   out_7276829301405261923[13] = state[13];
   out_7276829301405261923[14] = state[14];
   out_7276829301405261923[15] = state[15];
   out_7276829301405261923[16] = state[16];
   out_7276829301405261923[17] = state[17];
}
void F_fun(double *state, double dt, double *out_2644685186844474170) {
   out_2644685186844474170[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2644685186844474170[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2644685186844474170[2] = 0;
   out_2644685186844474170[3] = 0;
   out_2644685186844474170[4] = 0;
   out_2644685186844474170[5] = 0;
   out_2644685186844474170[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2644685186844474170[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2644685186844474170[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2644685186844474170[9] = 0;
   out_2644685186844474170[10] = 0;
   out_2644685186844474170[11] = 0;
   out_2644685186844474170[12] = 0;
   out_2644685186844474170[13] = 0;
   out_2644685186844474170[14] = 0;
   out_2644685186844474170[15] = 0;
   out_2644685186844474170[16] = 0;
   out_2644685186844474170[17] = 0;
   out_2644685186844474170[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2644685186844474170[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2644685186844474170[20] = 0;
   out_2644685186844474170[21] = 0;
   out_2644685186844474170[22] = 0;
   out_2644685186844474170[23] = 0;
   out_2644685186844474170[24] = 0;
   out_2644685186844474170[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2644685186844474170[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2644685186844474170[27] = 0;
   out_2644685186844474170[28] = 0;
   out_2644685186844474170[29] = 0;
   out_2644685186844474170[30] = 0;
   out_2644685186844474170[31] = 0;
   out_2644685186844474170[32] = 0;
   out_2644685186844474170[33] = 0;
   out_2644685186844474170[34] = 0;
   out_2644685186844474170[35] = 0;
   out_2644685186844474170[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2644685186844474170[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2644685186844474170[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2644685186844474170[39] = 0;
   out_2644685186844474170[40] = 0;
   out_2644685186844474170[41] = 0;
   out_2644685186844474170[42] = 0;
   out_2644685186844474170[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2644685186844474170[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2644685186844474170[45] = 0;
   out_2644685186844474170[46] = 0;
   out_2644685186844474170[47] = 0;
   out_2644685186844474170[48] = 0;
   out_2644685186844474170[49] = 0;
   out_2644685186844474170[50] = 0;
   out_2644685186844474170[51] = 0;
   out_2644685186844474170[52] = 0;
   out_2644685186844474170[53] = 0;
   out_2644685186844474170[54] = 0;
   out_2644685186844474170[55] = 0;
   out_2644685186844474170[56] = 0;
   out_2644685186844474170[57] = 1;
   out_2644685186844474170[58] = 0;
   out_2644685186844474170[59] = 0;
   out_2644685186844474170[60] = 0;
   out_2644685186844474170[61] = 0;
   out_2644685186844474170[62] = 0;
   out_2644685186844474170[63] = 0;
   out_2644685186844474170[64] = 0;
   out_2644685186844474170[65] = 0;
   out_2644685186844474170[66] = dt;
   out_2644685186844474170[67] = 0;
   out_2644685186844474170[68] = 0;
   out_2644685186844474170[69] = 0;
   out_2644685186844474170[70] = 0;
   out_2644685186844474170[71] = 0;
   out_2644685186844474170[72] = 0;
   out_2644685186844474170[73] = 0;
   out_2644685186844474170[74] = 0;
   out_2644685186844474170[75] = 0;
   out_2644685186844474170[76] = 1;
   out_2644685186844474170[77] = 0;
   out_2644685186844474170[78] = 0;
   out_2644685186844474170[79] = 0;
   out_2644685186844474170[80] = 0;
   out_2644685186844474170[81] = 0;
   out_2644685186844474170[82] = 0;
   out_2644685186844474170[83] = 0;
   out_2644685186844474170[84] = 0;
   out_2644685186844474170[85] = dt;
   out_2644685186844474170[86] = 0;
   out_2644685186844474170[87] = 0;
   out_2644685186844474170[88] = 0;
   out_2644685186844474170[89] = 0;
   out_2644685186844474170[90] = 0;
   out_2644685186844474170[91] = 0;
   out_2644685186844474170[92] = 0;
   out_2644685186844474170[93] = 0;
   out_2644685186844474170[94] = 0;
   out_2644685186844474170[95] = 1;
   out_2644685186844474170[96] = 0;
   out_2644685186844474170[97] = 0;
   out_2644685186844474170[98] = 0;
   out_2644685186844474170[99] = 0;
   out_2644685186844474170[100] = 0;
   out_2644685186844474170[101] = 0;
   out_2644685186844474170[102] = 0;
   out_2644685186844474170[103] = 0;
   out_2644685186844474170[104] = dt;
   out_2644685186844474170[105] = 0;
   out_2644685186844474170[106] = 0;
   out_2644685186844474170[107] = 0;
   out_2644685186844474170[108] = 0;
   out_2644685186844474170[109] = 0;
   out_2644685186844474170[110] = 0;
   out_2644685186844474170[111] = 0;
   out_2644685186844474170[112] = 0;
   out_2644685186844474170[113] = 0;
   out_2644685186844474170[114] = 1;
   out_2644685186844474170[115] = 0;
   out_2644685186844474170[116] = 0;
   out_2644685186844474170[117] = 0;
   out_2644685186844474170[118] = 0;
   out_2644685186844474170[119] = 0;
   out_2644685186844474170[120] = 0;
   out_2644685186844474170[121] = 0;
   out_2644685186844474170[122] = 0;
   out_2644685186844474170[123] = 0;
   out_2644685186844474170[124] = 0;
   out_2644685186844474170[125] = 0;
   out_2644685186844474170[126] = 0;
   out_2644685186844474170[127] = 0;
   out_2644685186844474170[128] = 0;
   out_2644685186844474170[129] = 0;
   out_2644685186844474170[130] = 0;
   out_2644685186844474170[131] = 0;
   out_2644685186844474170[132] = 0;
   out_2644685186844474170[133] = 1;
   out_2644685186844474170[134] = 0;
   out_2644685186844474170[135] = 0;
   out_2644685186844474170[136] = 0;
   out_2644685186844474170[137] = 0;
   out_2644685186844474170[138] = 0;
   out_2644685186844474170[139] = 0;
   out_2644685186844474170[140] = 0;
   out_2644685186844474170[141] = 0;
   out_2644685186844474170[142] = 0;
   out_2644685186844474170[143] = 0;
   out_2644685186844474170[144] = 0;
   out_2644685186844474170[145] = 0;
   out_2644685186844474170[146] = 0;
   out_2644685186844474170[147] = 0;
   out_2644685186844474170[148] = 0;
   out_2644685186844474170[149] = 0;
   out_2644685186844474170[150] = 0;
   out_2644685186844474170[151] = 0;
   out_2644685186844474170[152] = 1;
   out_2644685186844474170[153] = 0;
   out_2644685186844474170[154] = 0;
   out_2644685186844474170[155] = 0;
   out_2644685186844474170[156] = 0;
   out_2644685186844474170[157] = 0;
   out_2644685186844474170[158] = 0;
   out_2644685186844474170[159] = 0;
   out_2644685186844474170[160] = 0;
   out_2644685186844474170[161] = 0;
   out_2644685186844474170[162] = 0;
   out_2644685186844474170[163] = 0;
   out_2644685186844474170[164] = 0;
   out_2644685186844474170[165] = 0;
   out_2644685186844474170[166] = 0;
   out_2644685186844474170[167] = 0;
   out_2644685186844474170[168] = 0;
   out_2644685186844474170[169] = 0;
   out_2644685186844474170[170] = 0;
   out_2644685186844474170[171] = 1;
   out_2644685186844474170[172] = 0;
   out_2644685186844474170[173] = 0;
   out_2644685186844474170[174] = 0;
   out_2644685186844474170[175] = 0;
   out_2644685186844474170[176] = 0;
   out_2644685186844474170[177] = 0;
   out_2644685186844474170[178] = 0;
   out_2644685186844474170[179] = 0;
   out_2644685186844474170[180] = 0;
   out_2644685186844474170[181] = 0;
   out_2644685186844474170[182] = 0;
   out_2644685186844474170[183] = 0;
   out_2644685186844474170[184] = 0;
   out_2644685186844474170[185] = 0;
   out_2644685186844474170[186] = 0;
   out_2644685186844474170[187] = 0;
   out_2644685186844474170[188] = 0;
   out_2644685186844474170[189] = 0;
   out_2644685186844474170[190] = 1;
   out_2644685186844474170[191] = 0;
   out_2644685186844474170[192] = 0;
   out_2644685186844474170[193] = 0;
   out_2644685186844474170[194] = 0;
   out_2644685186844474170[195] = 0;
   out_2644685186844474170[196] = 0;
   out_2644685186844474170[197] = 0;
   out_2644685186844474170[198] = 0;
   out_2644685186844474170[199] = 0;
   out_2644685186844474170[200] = 0;
   out_2644685186844474170[201] = 0;
   out_2644685186844474170[202] = 0;
   out_2644685186844474170[203] = 0;
   out_2644685186844474170[204] = 0;
   out_2644685186844474170[205] = 0;
   out_2644685186844474170[206] = 0;
   out_2644685186844474170[207] = 0;
   out_2644685186844474170[208] = 0;
   out_2644685186844474170[209] = 1;
   out_2644685186844474170[210] = 0;
   out_2644685186844474170[211] = 0;
   out_2644685186844474170[212] = 0;
   out_2644685186844474170[213] = 0;
   out_2644685186844474170[214] = 0;
   out_2644685186844474170[215] = 0;
   out_2644685186844474170[216] = 0;
   out_2644685186844474170[217] = 0;
   out_2644685186844474170[218] = 0;
   out_2644685186844474170[219] = 0;
   out_2644685186844474170[220] = 0;
   out_2644685186844474170[221] = 0;
   out_2644685186844474170[222] = 0;
   out_2644685186844474170[223] = 0;
   out_2644685186844474170[224] = 0;
   out_2644685186844474170[225] = 0;
   out_2644685186844474170[226] = 0;
   out_2644685186844474170[227] = 0;
   out_2644685186844474170[228] = 1;
   out_2644685186844474170[229] = 0;
   out_2644685186844474170[230] = 0;
   out_2644685186844474170[231] = 0;
   out_2644685186844474170[232] = 0;
   out_2644685186844474170[233] = 0;
   out_2644685186844474170[234] = 0;
   out_2644685186844474170[235] = 0;
   out_2644685186844474170[236] = 0;
   out_2644685186844474170[237] = 0;
   out_2644685186844474170[238] = 0;
   out_2644685186844474170[239] = 0;
   out_2644685186844474170[240] = 0;
   out_2644685186844474170[241] = 0;
   out_2644685186844474170[242] = 0;
   out_2644685186844474170[243] = 0;
   out_2644685186844474170[244] = 0;
   out_2644685186844474170[245] = 0;
   out_2644685186844474170[246] = 0;
   out_2644685186844474170[247] = 1;
   out_2644685186844474170[248] = 0;
   out_2644685186844474170[249] = 0;
   out_2644685186844474170[250] = 0;
   out_2644685186844474170[251] = 0;
   out_2644685186844474170[252] = 0;
   out_2644685186844474170[253] = 0;
   out_2644685186844474170[254] = 0;
   out_2644685186844474170[255] = 0;
   out_2644685186844474170[256] = 0;
   out_2644685186844474170[257] = 0;
   out_2644685186844474170[258] = 0;
   out_2644685186844474170[259] = 0;
   out_2644685186844474170[260] = 0;
   out_2644685186844474170[261] = 0;
   out_2644685186844474170[262] = 0;
   out_2644685186844474170[263] = 0;
   out_2644685186844474170[264] = 0;
   out_2644685186844474170[265] = 0;
   out_2644685186844474170[266] = 1;
   out_2644685186844474170[267] = 0;
   out_2644685186844474170[268] = 0;
   out_2644685186844474170[269] = 0;
   out_2644685186844474170[270] = 0;
   out_2644685186844474170[271] = 0;
   out_2644685186844474170[272] = 0;
   out_2644685186844474170[273] = 0;
   out_2644685186844474170[274] = 0;
   out_2644685186844474170[275] = 0;
   out_2644685186844474170[276] = 0;
   out_2644685186844474170[277] = 0;
   out_2644685186844474170[278] = 0;
   out_2644685186844474170[279] = 0;
   out_2644685186844474170[280] = 0;
   out_2644685186844474170[281] = 0;
   out_2644685186844474170[282] = 0;
   out_2644685186844474170[283] = 0;
   out_2644685186844474170[284] = 0;
   out_2644685186844474170[285] = 1;
   out_2644685186844474170[286] = 0;
   out_2644685186844474170[287] = 0;
   out_2644685186844474170[288] = 0;
   out_2644685186844474170[289] = 0;
   out_2644685186844474170[290] = 0;
   out_2644685186844474170[291] = 0;
   out_2644685186844474170[292] = 0;
   out_2644685186844474170[293] = 0;
   out_2644685186844474170[294] = 0;
   out_2644685186844474170[295] = 0;
   out_2644685186844474170[296] = 0;
   out_2644685186844474170[297] = 0;
   out_2644685186844474170[298] = 0;
   out_2644685186844474170[299] = 0;
   out_2644685186844474170[300] = 0;
   out_2644685186844474170[301] = 0;
   out_2644685186844474170[302] = 0;
   out_2644685186844474170[303] = 0;
   out_2644685186844474170[304] = 1;
   out_2644685186844474170[305] = 0;
   out_2644685186844474170[306] = 0;
   out_2644685186844474170[307] = 0;
   out_2644685186844474170[308] = 0;
   out_2644685186844474170[309] = 0;
   out_2644685186844474170[310] = 0;
   out_2644685186844474170[311] = 0;
   out_2644685186844474170[312] = 0;
   out_2644685186844474170[313] = 0;
   out_2644685186844474170[314] = 0;
   out_2644685186844474170[315] = 0;
   out_2644685186844474170[316] = 0;
   out_2644685186844474170[317] = 0;
   out_2644685186844474170[318] = 0;
   out_2644685186844474170[319] = 0;
   out_2644685186844474170[320] = 0;
   out_2644685186844474170[321] = 0;
   out_2644685186844474170[322] = 0;
   out_2644685186844474170[323] = 1;
}
void h_4(double *state, double *unused, double *out_8115001930196127561) {
   out_8115001930196127561[0] = state[6] + state[9];
   out_8115001930196127561[1] = state[7] + state[10];
   out_8115001930196127561[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_4095637420522785286) {
   out_4095637420522785286[0] = 0;
   out_4095637420522785286[1] = 0;
   out_4095637420522785286[2] = 0;
   out_4095637420522785286[3] = 0;
   out_4095637420522785286[4] = 0;
   out_4095637420522785286[5] = 0;
   out_4095637420522785286[6] = 1;
   out_4095637420522785286[7] = 0;
   out_4095637420522785286[8] = 0;
   out_4095637420522785286[9] = 1;
   out_4095637420522785286[10] = 0;
   out_4095637420522785286[11] = 0;
   out_4095637420522785286[12] = 0;
   out_4095637420522785286[13] = 0;
   out_4095637420522785286[14] = 0;
   out_4095637420522785286[15] = 0;
   out_4095637420522785286[16] = 0;
   out_4095637420522785286[17] = 0;
   out_4095637420522785286[18] = 0;
   out_4095637420522785286[19] = 0;
   out_4095637420522785286[20] = 0;
   out_4095637420522785286[21] = 0;
   out_4095637420522785286[22] = 0;
   out_4095637420522785286[23] = 0;
   out_4095637420522785286[24] = 0;
   out_4095637420522785286[25] = 1;
   out_4095637420522785286[26] = 0;
   out_4095637420522785286[27] = 0;
   out_4095637420522785286[28] = 1;
   out_4095637420522785286[29] = 0;
   out_4095637420522785286[30] = 0;
   out_4095637420522785286[31] = 0;
   out_4095637420522785286[32] = 0;
   out_4095637420522785286[33] = 0;
   out_4095637420522785286[34] = 0;
   out_4095637420522785286[35] = 0;
   out_4095637420522785286[36] = 0;
   out_4095637420522785286[37] = 0;
   out_4095637420522785286[38] = 0;
   out_4095637420522785286[39] = 0;
   out_4095637420522785286[40] = 0;
   out_4095637420522785286[41] = 0;
   out_4095637420522785286[42] = 0;
   out_4095637420522785286[43] = 0;
   out_4095637420522785286[44] = 1;
   out_4095637420522785286[45] = 0;
   out_4095637420522785286[46] = 0;
   out_4095637420522785286[47] = 1;
   out_4095637420522785286[48] = 0;
   out_4095637420522785286[49] = 0;
   out_4095637420522785286[50] = 0;
   out_4095637420522785286[51] = 0;
   out_4095637420522785286[52] = 0;
   out_4095637420522785286[53] = 0;
}
void h_10(double *state, double *unused, double *out_8853117002444431783) {
   out_8853117002444431783[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_8853117002444431783[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_8853117002444431783[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_2122054065778683402) {
   out_2122054065778683402[0] = 0;
   out_2122054065778683402[1] = 9.8100000000000005*cos(state[1]);
   out_2122054065778683402[2] = 0;
   out_2122054065778683402[3] = 0;
   out_2122054065778683402[4] = -state[8];
   out_2122054065778683402[5] = state[7];
   out_2122054065778683402[6] = 0;
   out_2122054065778683402[7] = state[5];
   out_2122054065778683402[8] = -state[4];
   out_2122054065778683402[9] = 0;
   out_2122054065778683402[10] = 0;
   out_2122054065778683402[11] = 0;
   out_2122054065778683402[12] = 1;
   out_2122054065778683402[13] = 0;
   out_2122054065778683402[14] = 0;
   out_2122054065778683402[15] = 1;
   out_2122054065778683402[16] = 0;
   out_2122054065778683402[17] = 0;
   out_2122054065778683402[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_2122054065778683402[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_2122054065778683402[20] = 0;
   out_2122054065778683402[21] = state[8];
   out_2122054065778683402[22] = 0;
   out_2122054065778683402[23] = -state[6];
   out_2122054065778683402[24] = -state[5];
   out_2122054065778683402[25] = 0;
   out_2122054065778683402[26] = state[3];
   out_2122054065778683402[27] = 0;
   out_2122054065778683402[28] = 0;
   out_2122054065778683402[29] = 0;
   out_2122054065778683402[30] = 0;
   out_2122054065778683402[31] = 1;
   out_2122054065778683402[32] = 0;
   out_2122054065778683402[33] = 0;
   out_2122054065778683402[34] = 1;
   out_2122054065778683402[35] = 0;
   out_2122054065778683402[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_2122054065778683402[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_2122054065778683402[38] = 0;
   out_2122054065778683402[39] = -state[7];
   out_2122054065778683402[40] = state[6];
   out_2122054065778683402[41] = 0;
   out_2122054065778683402[42] = state[4];
   out_2122054065778683402[43] = -state[3];
   out_2122054065778683402[44] = 0;
   out_2122054065778683402[45] = 0;
   out_2122054065778683402[46] = 0;
   out_2122054065778683402[47] = 0;
   out_2122054065778683402[48] = 0;
   out_2122054065778683402[49] = 0;
   out_2122054065778683402[50] = 1;
   out_2122054065778683402[51] = 0;
   out_2122054065778683402[52] = 0;
   out_2122054065778683402[53] = 1;
}
void h_13(double *state, double *unused, double *out_4983375721393997945) {
   out_4983375721393997945[0] = state[3];
   out_4983375721393997945[1] = state[4];
   out_4983375721393997945[2] = state[5];
}
void H_13(double *state, double *unused, double *out_3514993787793915643) {
   out_3514993787793915643[0] = 0;
   out_3514993787793915643[1] = 0;
   out_3514993787793915643[2] = 0;
   out_3514993787793915643[3] = 1;
   out_3514993787793915643[4] = 0;
   out_3514993787793915643[5] = 0;
   out_3514993787793915643[6] = 0;
   out_3514993787793915643[7] = 0;
   out_3514993787793915643[8] = 0;
   out_3514993787793915643[9] = 0;
   out_3514993787793915643[10] = 0;
   out_3514993787793915643[11] = 0;
   out_3514993787793915643[12] = 0;
   out_3514993787793915643[13] = 0;
   out_3514993787793915643[14] = 0;
   out_3514993787793915643[15] = 0;
   out_3514993787793915643[16] = 0;
   out_3514993787793915643[17] = 0;
   out_3514993787793915643[18] = 0;
   out_3514993787793915643[19] = 0;
   out_3514993787793915643[20] = 0;
   out_3514993787793915643[21] = 0;
   out_3514993787793915643[22] = 1;
   out_3514993787793915643[23] = 0;
   out_3514993787793915643[24] = 0;
   out_3514993787793915643[25] = 0;
   out_3514993787793915643[26] = 0;
   out_3514993787793915643[27] = 0;
   out_3514993787793915643[28] = 0;
   out_3514993787793915643[29] = 0;
   out_3514993787793915643[30] = 0;
   out_3514993787793915643[31] = 0;
   out_3514993787793915643[32] = 0;
   out_3514993787793915643[33] = 0;
   out_3514993787793915643[34] = 0;
   out_3514993787793915643[35] = 0;
   out_3514993787793915643[36] = 0;
   out_3514993787793915643[37] = 0;
   out_3514993787793915643[38] = 0;
   out_3514993787793915643[39] = 0;
   out_3514993787793915643[40] = 0;
   out_3514993787793915643[41] = 1;
   out_3514993787793915643[42] = 0;
   out_3514993787793915643[43] = 0;
   out_3514993787793915643[44] = 0;
   out_3514993787793915643[45] = 0;
   out_3514993787793915643[46] = 0;
   out_3514993787793915643[47] = 0;
   out_3514993787793915643[48] = 0;
   out_3514993787793915643[49] = 0;
   out_3514993787793915643[50] = 0;
   out_3514993787793915643[51] = 0;
   out_3514993787793915643[52] = 0;
   out_3514993787793915643[53] = 0;
}
void h_14(double *state, double *unused, double *out_823804089178252810) {
   out_823804089178252810[0] = state[6];
   out_823804089178252810[1] = state[7];
   out_823804089178252810[2] = state[8];
}
void H_14(double *state, double *unused, double *out_132396564183300757) {
   out_132396564183300757[0] = 0;
   out_132396564183300757[1] = 0;
   out_132396564183300757[2] = 0;
   out_132396564183300757[3] = 0;
   out_132396564183300757[4] = 0;
   out_132396564183300757[5] = 0;
   out_132396564183300757[6] = 1;
   out_132396564183300757[7] = 0;
   out_132396564183300757[8] = 0;
   out_132396564183300757[9] = 0;
   out_132396564183300757[10] = 0;
   out_132396564183300757[11] = 0;
   out_132396564183300757[12] = 0;
   out_132396564183300757[13] = 0;
   out_132396564183300757[14] = 0;
   out_132396564183300757[15] = 0;
   out_132396564183300757[16] = 0;
   out_132396564183300757[17] = 0;
   out_132396564183300757[18] = 0;
   out_132396564183300757[19] = 0;
   out_132396564183300757[20] = 0;
   out_132396564183300757[21] = 0;
   out_132396564183300757[22] = 0;
   out_132396564183300757[23] = 0;
   out_132396564183300757[24] = 0;
   out_132396564183300757[25] = 1;
   out_132396564183300757[26] = 0;
   out_132396564183300757[27] = 0;
   out_132396564183300757[28] = 0;
   out_132396564183300757[29] = 0;
   out_132396564183300757[30] = 0;
   out_132396564183300757[31] = 0;
   out_132396564183300757[32] = 0;
   out_132396564183300757[33] = 0;
   out_132396564183300757[34] = 0;
   out_132396564183300757[35] = 0;
   out_132396564183300757[36] = 0;
   out_132396564183300757[37] = 0;
   out_132396564183300757[38] = 0;
   out_132396564183300757[39] = 0;
   out_132396564183300757[40] = 0;
   out_132396564183300757[41] = 0;
   out_132396564183300757[42] = 0;
   out_132396564183300757[43] = 0;
   out_132396564183300757[44] = 1;
   out_132396564183300757[45] = 0;
   out_132396564183300757[46] = 0;
   out_132396564183300757[47] = 0;
   out_132396564183300757[48] = 0;
   out_132396564183300757[49] = 0;
   out_132396564183300757[50] = 0;
   out_132396564183300757[51] = 0;
   out_132396564183300757[52] = 0;
   out_132396564183300757[53] = 0;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_6399721819463021516) {
  err_fun(nom_x, delta_x, out_6399721819463021516);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_3334656739795904655) {
  inv_err_fun(nom_x, true_x, out_3334656739795904655);
}
void pose_H_mod_fun(double *state, double *out_4849798422577497893) {
  H_mod_fun(state, out_4849798422577497893);
}
void pose_f_fun(double *state, double dt, double *out_7276829301405261923) {
  f_fun(state,  dt, out_7276829301405261923);
}
void pose_F_fun(double *state, double dt, double *out_2644685186844474170) {
  F_fun(state,  dt, out_2644685186844474170);
}
void pose_h_4(double *state, double *unused, double *out_8115001930196127561) {
  h_4(state, unused, out_8115001930196127561);
}
void pose_H_4(double *state, double *unused, double *out_4095637420522785286) {
  H_4(state, unused, out_4095637420522785286);
}
void pose_h_10(double *state, double *unused, double *out_8853117002444431783) {
  h_10(state, unused, out_8853117002444431783);
}
void pose_H_10(double *state, double *unused, double *out_2122054065778683402) {
  H_10(state, unused, out_2122054065778683402);
}
void pose_h_13(double *state, double *unused, double *out_4983375721393997945) {
  h_13(state, unused, out_4983375721393997945);
}
void pose_H_13(double *state, double *unused, double *out_3514993787793915643) {
  H_13(state, unused, out_3514993787793915643);
}
void pose_h_14(double *state, double *unused, double *out_823804089178252810) {
  h_14(state, unused, out_823804089178252810);
}
void pose_H_14(double *state, double *unused, double *out_132396564183300757) {
  H_14(state, unused, out_132396564183300757);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
