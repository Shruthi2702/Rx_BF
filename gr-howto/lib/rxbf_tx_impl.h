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

#ifndef INCLUDED_HOWTO_RXBF_TX_IMPL_H
#define INCLUDED_HOWTO_RXBF_TX_IMPL_H

#include <howto/rxbf_tx.h>

namespace gr {
  namespace howto {

    class rxbf_tx_impl : public rxbf_tx
    {
     private:
      // Nothing to declare in this block.
        double d_Fs;
	double d_Fp;
	int d_N;

	int cpt_samples;
	int periodLength;
     public:
     
     // accessors
	int get_N() const {return d_N; }
	
	// manipulator
	void set_N(int N);
      rxbf_tx_impl(double Fs, double Fp, int N);
      ~rxbf_tx_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace howto
} // namespace gr

#endif /* INCLUDED_HOWTO_RXBF_TX_IMPL_H */

