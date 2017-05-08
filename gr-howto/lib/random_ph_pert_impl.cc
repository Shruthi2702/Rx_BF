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
#include <gnuradio/fxpt.h>
#include <gnuradio/io_signature.h>
#include "random_ph_pert_impl.h"

namespace gr {
  namespace howto {

    random_ph_pert::sptr
    random_ph_pert::make(double random_perturbation_size, int bf_flag)
    {
      return gnuradio::get_initial_sptr
        (new random_ph_pert_impl(random_perturbation_size, bf_flag));
    }

    /*
     * The private constructor
     */
    random_ph_pert_impl::random_ph_pert_impl(double random_perturbation_size, int bf_flag)
      : gr::sync_block("random_ph_pert",
              gr::io_signature::make(1, 1, sizeof (float)),
              gr::io_signature::make(0, 0, 0)),
              d_random_perturbation_size(random_perturbation_size), 
              d_bf_flag(bf_flag)

    {cum_ang=0;
   etojphi=(gr_complex)1;
   rand_pert_from_prev_iter=0;
   d_seed=42; // seed value for random number generator
   d_random_perturbation_size = (3.141592654/180)*d_random_perturbation_size; // input angle (i.e., the random phase perturbation to be applied in each time-slot) is converted from degrees to radians. 
   last_in_i = 0;
   d_complex_gain=1;
    }

    /*
     * Our virtual destructor.
     */
    random_ph_pert_impl::~random_ph_pert_impl()
    {
    }

    int
    random_ph_pert_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      double randp; // holds the random phase perturbation to apply in each time-slot 
      double i_out, q_out;
      
      for (int i = 0; i < noutput_items; i++){
	if (d_bf_flag==0)
        {
        }
        else {
      //beamforming
      if ( (in[i] > 0) || (- in[i] > 0) ){ //update the transmitter's phase when actual feedback bit is present at input
	if (in[i] != last_in_i){
	 printf ("Feedback from receiver is: %d\n", (int)in[i]);
	 last_in_i = in[i];
         randp = ((float)(d_seed.ran1()))*2;
         if (randp<1){
             randp = -d_random_perturbation_size; }
         else {
             randp =  d_random_perturbation_size; }
         //printf ("random perturbation = %f\n", randp); // for de-bugging purpose
         cum_ang+=randp; 
         //undo previous phase if feedback bit is 0.
         if (  in[i] < 0 ){
            cum_ang-=rand_pert_from_prev_iter;
         }
         //printf ("Feedback from receiver is: %d\n", (int)in[i]);
         //printf ("cumulative angle = %f\n", cum_ang); // for de-bugging purpose
         cum_ang=fmod(cum_ang,6.283185308);//compute modulo-2*pi phase
         i_out=fxpt::cos (fxpt::float_to_fixed(cum_ang)); 
         q_out=fxpt::sin (fxpt::float_to_fixed(cum_ang));
         etojphi=gr_complex (i_out, q_out); 
         rand_pert_from_prev_iter=randp; 
	 d_complex_gain=etojphi;
	}
      }
      //out[i]=etojphi;
   } 
} 

      return noutput_items;
    }
   void
   random_ph_pert_impl::set_bf_flag (int bf_flag)
   {
     d_bf_flag = bf_flag;
   }

  } /* namespace howto */
} /* namespace gr */

