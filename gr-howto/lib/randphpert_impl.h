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

#ifndef INCLUDED_HOWTO_RANDPHPERT_IMPL_H
#define INCLUDED_HOWTO_RANDPHPERT_IMPL_H

#include <gnuradio/analog/noise_type.h>
#include <gnuradio/random.h>
#include <gnuradio/sync_block.h>
#include <howto/randphpert.h>

namespace gr {
  namespace howto {

    class randphpert_impl : public randphpert
    {
     private:
      // Nothing to declare in this block.
      double d_random_perturbation_size; 
      int d_bf_flag; // a flag to turn beamforming on or off

      double cum_ang;
      gr_complex etojphi;
      double rand_pert_from_prev_iter;
      gr::random d_seed;
      gr_complex d_complex_gain;
      float last_in_i;

     public:
      int bf_flag () const{ return d_bf_flag; }
      gr_complex poll_complex_gain() const { return d_complex_gain; }

      void set_bf_flag (int bf_flag);
      randphpert_impl(double random_perturbation_size, int bf_flag);
      ~randphpert_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace howto
} // namespace gr

#endif /* INCLUDED_HOWTO_RANDPHPERT_IMPL_H */

