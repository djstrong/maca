#ifndef SWIG_MACA_I
#define SWIG_MACA_I

%module maca
%{
  #include <libxml++/libxml++.h>
%}

%include <libcorpus2/unicodestring.i>
%include <libcorpus2/unicodestring.i>
//%include <libcorpus2/corpus2exception.i>
%include "plaintextreader.i"
%include "premorphtextreader.i"

#endif /* SWIG_MACA_I */
