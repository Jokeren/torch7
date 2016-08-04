#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/THTensorMathDispatch.c"
#else

#include "simd/simd.h"

// NOTE This file will contain static function pointers that will be initialized by
// the initialization call.  It will also have globally linked dispatch stubs
// which delegate to the function pointers.  The dispatch stubs will be the symbols
// called by clients
//
// Somehow, this guy needs access to all SIMD implementations

// Dispatch pointers.  These guys will be set to point to the most-optimized implementation
// for the host.
static void (*THTensor_(dispatchPtrAdd))(THTensor*, THTensor*, real) = NULL;

// Dispatch stubs that just call the pointers
TH_API void THTensor_(add)(THTensor *r_, THTensor *t, real value)
{
  THTensor_(dispatchPtrAdd)(r_, t, value);
}


// Dispatch tables: each optimized implementation of a function
// is described in a table, and the tables are used to initialize
// the function pointers for dynamic dispatch
FunctionDescription THTensor_(dispatchTblAdd)[] = {
  //FUNCTION_IMPL(THTensor_(add_AVX2), AVX2),
  //FUNCTION_IMPL(THTensor_(add_AVX), AVX),
  //FUNCTION_IMPL(THTensor_(add_SSE), SSE),
  FUNCTION_IMPL((void *)THTensor_(add_Default), DEFAULT)
};


int THTensor_(cpuDispatchInit)()
{
  uint32_t hostSimdExts = detectHostSIMDExtensions();

  // Initialize the dispatch pointers to point to the correct functions
  for (int i = 0; i < sizeof(THTensor_(dispatchTblAdd)) / sizeof(FunctionDescription); ++i) {
    THTensor_(dispatchPtrAdd) = THTensor_(dispatchTblAdd)[i].function;
    if (THTensor_(dispatchTblAdd)[i].supportedSimdExt & hostSimdExts) {
      break;
    }
  }
  return 0;
}

#endif
