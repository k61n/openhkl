%module "pynsx"

 //%feature("notabstract") Animal;
 //%feature("notabstract") Gender;

%{
#define SWIG_FILE_WITH_INIT
  #include "data/IData.h"
%}

%include "data/IData.h"

