/* -*- c++ -*- */

#define STREAMSINK_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "streamsink_swig_doc.i"

%{
#include "streamsink/streamsink.h"
%}


%include "streamsink/streamsink.h"
GR_SWIG_BLOCK_MAGIC2(streamsink, streamsink);
