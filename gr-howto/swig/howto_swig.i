/* -*- c++ -*- */

#define HOWTO_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "howto_swig_doc.i"

%{
#include "howto/rxbf_tx.h"
#include "howto/rx_bf_gen_fb.h"
#include "howto/relayBF.h"
#include "howto/randphpert.h"
#include "howto/forward_block.h"
#include "howto/random_ph_pert.h"
%}


%include "howto/rxbf_tx.h"
GR_SWIG_BLOCK_MAGIC2(howto, rxbf_tx);
%include "howto/rx_bf_gen_fb.h"
GR_SWIG_BLOCK_MAGIC2(howto, rx_bf_gen_fb);
%include "howto/relayBF.h"
GR_SWIG_BLOCK_MAGIC2(howto, relayBF);
%include "howto/randphpert.h"
GR_SWIG_BLOCK_MAGIC2(howto, randphpert);
%include "howto/forward_block.h"
GR_SWIG_BLOCK_MAGIC2(howto, forward_block);
%include "howto/random_ph_pert.h"
GR_SWIG_BLOCK_MAGIC2(howto, random_ph_pert);
