/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gnuradio/math.h>
#include <gnuradio/fxpt.h>
#include <gnuradio/io_signature.h>
#include "rxbf_tx_impl.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

namespace gr {
  namespace howto {

    rxbf_tx::sptr
    rxbf_tx::make(double Fs, double Fp, int N)
    {
      return gnuradio::get_initial_sptr
        (new rxbf_tx_impl(Fs, Fp, N));
    }

    /*
     * The private constructor
     */
    rxbf_tx_impl::rxbf_tx_impl(double Fs, double Fp, int N)
      : gr::block("rxbf_tx",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
              d_Fs(Fs),
	      d_Fp(Fp),
	      d_N(N)
    {
    
    /* initialize instance variables */
	cpt_samples = 0;
	periodLength = int(std::ceil(Fs/Fp));
	printf("Fs = %f\n", d_Fs);
	printf("Fp = %f\n", d_Fp);
	printf("N = %d\n", d_N);
	printf("periodLength = %d\n", periodLength);


    }

    /*
     * Our virtual destructor.
     */
    rxbf_tx_impl::~rxbf_tx_impl()
    {
    }

    void
    rxbf_tx_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    rxbf_tx_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      gr_complex *out = (gr_complex *)output_items[0];
      for (int i = 0; i < noutput_items; i++)
		 {  
    			if (cpt_samples < d_N) {
			// message
			out[i] = gr_complex(1,0);
		}
		else {
			// TX nothing (silent period)
			out[i] = gr_complex(0,0);
		}
		cpt_samples = int(fmod(++cpt_samples, periodLength));

		}

      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace howto */
} /* namespace gr */

