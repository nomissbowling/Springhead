/* ------------------------------------------------------------
 * Primitive type fragments and macros 
 * ------------------------------------------------------------ */

/*
  This file provide fragments and macros for the C/C++ primitive types. 

  The file defines default fragments for the  following types:

    bool
    signed char
    unsigned char
    signed wchar_t     // in C++
    unsigned wchar_t   // in C++
    short
    unsigned short
    int
    unsigned int
    float
    size_t
    ptrdiff_t

  which can always be redefined in the swig target languge if needed.
  
  The fragments for the following types, however, need to be defined
  in the target language always:

    long
    unsigned long
    long long
    unsigned long long
    double
 
  If they are not provided, an #error directive will appear in the
  wrapped code.

  --------------------------------------------------------------------
  
  This file provides the macro

    %typemaps_primitive(CheckCode, Type)

  which generate the typemaps for a primitive type with a given
  checkcode. It is assumed the the primitive type is 'normalized' and
  the corresponding SWIG_AsVal(Type) and SWIG_From(Type) methods are
  provided via fragments.
  
   
  The following auxiliar macros (explained with bash pseudo code) are
  also defined:

    %apply_ctypes(Macro)        
      for i in C Type 
      do
        Macro($i)
      done

    %apply_cpptypes(Macro) 
      for i in C++ Type 
      do
        Macro($i)
      done

    %apply_ctypes_2(Macro2)     			     
       for i in C Type 					     
       do							     
         for j in C Type 					     
         do						     
            Macro_2($i, $j)				     
         done						     
       done						     
    							     
    %apply_cpptypes_2(Macro2)     			     
       for i in C++ Type 					     
       do							     
         for j in C++ Type 				     
         do						     
            Macro_2($i, $j)				     
         done						     
       done						     
    							     
    %apply_checkctypes(Macro2)
       for i in Check Type 
       do
         Macro2(%checkcode($i), $i)
       done

*/


/* ------------------------------------------------------------
 * Primitive type fragments 
 * ------------------------------------------------------------ */
/* boolean */

%fragment(SWIG_From_frag(bool),"header",fragment=SWIG_From_frag(long)) {
SWIGINTERN SWIG_Object
SWIG_From_dec(bool)(bool value)
{    
  return SWIG_From(long)(value ? 1 : 0);
}
}

%fragment(SWIG_AsVal_frag(bool),"header",fragment=SWIG_AsVal_frag(long)) {
SWIGINTERN int
SWIG_AsVal_dec(bool)(SWIG_Object obj, bool *val)
{
  long v;
  int res = SWIG_AsVal(long)(obj, val ? &v : 0);
  if (SWIG_IsOK(res)) {    
    if (val) *val = v ? true : false;
    return res;
  }  
  return SWIG_TypeError;
}
}

/* signed/unsigned char */

%numeric_slong(signed char,     "<limits.h>", SCHAR_MIN, SCHAR_MAX)
%numeric_ulong(unsigned char,   "<limits.h>", UCHAR_MAX)

/* short/unsigned short */

%numeric_slong(short,           "<limits.h>", SHRT_MIN, SHRT_MAX)
%numeric_ulong(unsigned short,  "<limits.h>", USHRT_MAX)

/* int/unsigned int */

%numeric_slong(int,             "<limits.h>", INT_MIN, INT_MAX)
%numeric_ulong(unsigned int,    "<limits.h>", UINT_MAX)

/* signed/unsigned wchar_t */

#ifdef __cplusplus
%numeric_slong(signed wchar_t,   "<wchar.h>", WCHAR_MIN, WCHAR_MAX)
%numeric_ulong(unsigned wchar_t, "<wchar.h>", UWCHAR_MAX)
#endif

/* float */

%numeric_double(float,           "<float.h>", -FLT_MAX, FLT_MAX)

/* long/unsgined long */

%ensure_type_fragments(long)
%ensure_type_fragments(unsigned long)

/* long long/unsigned long long */

%ensure_type_fragments(long long)
%ensure_type_fragments(unsigned long long)

/* double */

%ensure_type_fragments(double)

/* size_t */

%fragment(SWIG_From_frag(size_t),"header",fragment=SWIG_From_frag(unsigned long)) {
SWIGINTERNINLINE SWIG_Object
SWIG_From_dec(size_t)(size_t value)
{    
  return SWIG_From(unsigned long)(%numeric_cast(value, unsigned long));
}
}

%fragment(SWIG_AsVal_frag(size_t),"header",fragment=SWIG_AsVal_frag(unsigned long)) {
SWIGINTERNINLINE int
SWIG_AsVal_dec(size_t)(SWIG_Object obj, size_t *val)
{
  unsigned long v;
  int res = SWIG_AsVal(unsigned long)(obj, val ? &v : 0);
  if (SWIG_IsOK(res) && val) *val = %numeric_cast(v, size_t);
  return res;
}
}

/* ptrdiff_t */

%fragment(SWIG_From_frag(ptrdiff_t),"header",fragment=SWIG_From_frag(long)) {
SWIGINTERNINLINE SWIG_Object
SWIG_From_dec(ptrdiff_t)(ptrdiff_t value)
{    
  return SWIG_From(long)(%numeric_cast(value,long));
}
}

%fragment(SWIG_AsVal_frag(ptrdiff_t),"header",fragment=SWIG_AsVal_frag(long)) {
SWIGINTERNINLINE int
SWIG_AsVal_dec(ptrdiff_t)(SWIG_Object obj, ptrdiff_t *val)
{
  long v;
  int res = SWIG_AsVal(long)(obj, val ? &v : 0);
  if (SWIG_IsOK(res) && val) *val = %numeric_cast(v, ptrdiff_t);
  return res;
}
}


%fragment("SWIG_CanCastAsInteger","header",
	  fragment=SWIG_AsVal_frag(double),
	  fragment="<float.h>",
	  fragment="<math.h>") {
SWIGINTERNINLINE int
SWIG_CanCastAsInteger(double *d, double min, double max) {
  double x = *d;
  if ((min <= x && x <= max)) {
   double fx = floor(x);
   double cx = ceil(x);
   double rd =  ((x - fx) < 0.5) ? fx : cx; /* simple rint */
   if ((errno == EDOM) || (errno == ERANGE)) {
     errno = 0;
   } else {
     double summ, reps, diff;
     if (rd < x) {
       diff = x - rd;
     } else if (rd > x) {
       diff = rd - x;
     } else {
       return 1;
     }
     summ = rd + x;
     reps = diff/summ;
     if (reps < 8*DBL_EPSILON) {
       *d = rd;
       return 1;
     }
   }
  }
  return 0;
}
}

/* ------------------------------------------------------------
 * Generate the typemaps for primitive type 
 * ------------------------------------------------------------ */

#define %typemaps_primitive(Code, Type) %typemaps_asvalfromn(%arg(Code), Type)

/* ------------------------------------------------------------
 * Primitive Type Macros
 * ------------------------------------------------------------ */

/* useful macros to derive typemap declarations from primitive types */

%define _apply_macro(macro, arg2, arg1...)
#if #arg1 != ""
macro(%arg(arg1),arg2);
#else
macro(arg2);
#endif
%enddef

/* Apply macro to the C-types */
%define %apply_ctypes(Macro, Arg2...)
_apply_macro(Macro, bool               , Arg2);
_apply_macro(Macro, signed char        , Arg2);
_apply_macro(Macro, unsigned char      , Arg2);
_apply_macro(Macro, short              , Arg2);
_apply_macro(Macro, unsigned short     , Arg2);
_apply_macro(Macro, int                , Arg2);
_apply_macro(Macro, unsigned int       , Arg2);
_apply_macro(Macro, long               , Arg2);
_apply_macro(Macro, unsigned long      , Arg2);
_apply_macro(Macro, long long          , Arg2);
_apply_macro(Macro, unsigned long long , Arg2);
_apply_macro(Macro, float              , Arg2);
_apply_macro(Macro, double             , Arg2);
_apply_macro(Macro, char               , Arg2);
_apply_macro(Macro, wchar_t            , Arg2);
_apply_macro(Macro, size_t             , Arg2);
_apply_macro(Macro, ptrdiff_t          , Arg2);
%enddef

/* apply the Macro2(Type1, Type2) to all  C types  */
#define %apply_ctypes_2(Macro2) %apply_ctypes(%apply_ctypes, Macro2)


/* apply the Macro(Type) to all  C++ types  */
%define %apply_cpptypes(Macro, Arg2...)
%apply_ctypes(Macro, Arg2)
_apply_macro(Macro, std::size_t, Arg2);
_apply_macro(Macro, std::ptrdiff_t, Arg2);
_apply_macro(Macro, std::string, Arg2);
_apply_macro(Macro, std::complex<float>, Arg2);
_apply_macro(Macro, std::complex<double>, Arg2);
%enddef

/* apply the Macro2(Type1, Type2) to all  C++ types  */
#define %apply_cpptypes_2(Macro2) %apply_cpptypes(%apply_cpptypes, Macro2)

/* apply the Macro2(CheckCode,Type) to all  Checked Types */
%define %apply_checkctypes(Macro2)
Macro2(%checkcode(BOOL),    bool);
Macro2(%checkcode(INT8),    signed char);
Macro2(%checkcode(UINT8),   unsigned char);
Macro2(%checkcode(INT16),   short);
Macro2(%checkcode(UINT16),  unsigned short);
Macro2(%checkcode(INT32),   int);
Macro2(%checkcode(UINT32),  unsigned int);
Macro2(%checkcode(INT64),   long);
Macro2(%checkcode(UINT64),  unsigned long);
Macro2(%checkcode(INT128),  long long);
Macro2(%checkcode(UINT128), unsigned long long);
Macro2(%checkcode(FLOAT),   float);
Macro2(%checkcode(DOUBLE),  double);
Macro2(%checkcode(CHAR),    char);
Macro2(%checkcode(UNICHAR), wchar_t);
Macro2(%checkcode(SIZE),    size_t);
Macro2(%checkcode(PTRDIFF), ptrdiff_t);
%enddef


/* ------------------------------------------------------------
 * Generate the typemaps for all the primitive types with checkcode
 * ------------------------------------------------------------ */

%apply_checkctypes(%typemaps_primitive);

