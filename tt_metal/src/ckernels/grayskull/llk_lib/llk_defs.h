#pragma once

enum ReduceDim {
  REDUCE_ROW,
  REDUCE_COL,
  REDUCE_SCALAR
};

enum Dim {
  None      = 0,
  R         = 1,
  C         = 2,
  Z         = 3,
  RC        = 4,
  ZR        = 5,
  Invalid   = 0xFF,
};

enum PoolType {
  SUM,
  AVG,
  MAX
};

enum DataCopyType
{
  A2D,
  B2D
};

enum EltwiseBinaryType
{
  ELWMUL,
  ELWDIV,
  ELWADD,
  ELWSUB,
  ELWLESS
};

enum DstSync {
  SyncHalf    = 0,
  SyncFull    = 1,
  SyncTile16  = 2,
  SyncTile2   = 3
};

enum BroadcastType
{
  NONE    = 0x0, // A - None || B - None
  COL     = 0x1, // A - None || B - Col Broadcast
  ROW     = 0x2, // A - None || B - Row Broadcast
  SCALAR  = 0x3  // A - None || B - Scalar Broadcast
};

enum src_op_id_e
{
  OP_SRC0 = 0,
  OP_SRC1 = 1,
  OP_SRC2 = 2,
  OP_SRC3 = 3,
  OP_SRC4 = 4
};

enum local_op_id_e
{
  OP_LOCAL0 = 0,
  OP_LOCAL1 = 1,
  OP_LOCAL2 = 2,
  OP_LOCAL3 = 3,
  OP_LOCAL4 = 4
};

enum out_op_id_e
{
  OUT_ID0 = 0,
  OUT_ID1 = 1,
  OUT_ID2 = 2,
  OUT_ID3 = 3,
  OUT_ID4 = 4
};

enum ReluType {
  NO_RELU,
  ZERO_RELU,
  MIN_THRESHOLD_RELU,
  MAX_THRESHOLD_RELU
};

enum SfpuType
{
  tanh,
  hardtanh,
  gelu,
  exponential,
  exp_with_base,
  sigmoid,
  sigmoid_appx,
  reciprocal,
  sqrt,
  rsqrt,
  lrelu,
  power,
  square,
  tanh_derivative,
  log,
  log_with_base,
  equal_zero,
  not_equal_zero,
  less_than_zero,
  greater_than_equal_zero,
  less_than_equal_zero,
  greater_than_zero,
  clamp,
  gelu_derivative,
  dropout,
  abs,
  sign,
  max,
  min,
  sine,
  cosine,
  relu_min,
  relu_max,
  elu,
  exp2,
  heaviside,
  expm1,
  signbit,
  asin,
  acos,
  atan,
  erf,
  erfc,
  unused
};


enum SfpiTestType
{
  logical_not,
  bitwise_complement,
  test1,
  test2,
  test3,
  test4,
  test5,
  test6,
  test7,
  test8,
  test9,
  test10,
  test11,
  test12,
  test13,
  test14,
  unused_test
};
