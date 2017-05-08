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

#ifndef INCLUDED_HOWTO_FORWARD_BLOCK_IMPL_H
#define INCLUDED_HOWTO_FORWARD_BLOCK_IMPL_H

#include <howto/forward_block.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

namespace gr {
  namespace howto {

    class forward_block_impl : public forward_block
    {
     private:
      // Nothing to declare in this block.
     double d_sample_rate;
	double d_rx_min_treshold;
	double d_rx_max_treshold;
 	double d_time_delay; 
	double d_time_cycle; 
	int d_N; 
	int d_max_missed_symbols; 
	int d_nbr_zeros_per_burst;

	double Fs;
	int samples_cycle;
	int nbr_samples_wait_bef_list;
	int state;
	int cpt_samples;
	int cpt_zeros_burst;
	int cpt_missed_symbols;
	int cpt_samples_rx_packet;
	int cpt_samples_tx_packet;
	int cpt_samples_wait_bef_list;
	bool first_burst_of_zeros;
	float out_real;
	float out_imag;
	float power_packet;
	gsl_vector_float* rx_samples_arg;
	gsl_vector_float* rx_samples_real;
	gsl_vector_float* rx_samples_imag;

	double fracs;
	double fracs_rx;
	double fracs_tx;
	double fracs_tx_zeros;
	uint64_t secs;
	uint64_t secs_rx;
	uint64_t secs_tx;
	uint64_t secs_tx_zeros;
	uint64_t offs;
	uint64_t start_time_packet;

     public:
      forward_block_impl(double sample_rate, double rx_min_treshold, double rx_max_treshold, double time_delay, double time_cycle, int   packet_length, int max_missed_symbols, int nbr_zeros_per_burst);
      ~forward_block_impl();
    void make_time_tag(const uint64_t tag_count);
  void make_time_tag_zeros(const uint64_t tag_count);
  void make_sob_tag(const uint64_t tag_count);
  void make_eob_tag(const uint64_t tag_count);

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace howto
} // namespace gr

#endif /* INCLUDED_HOWTO_FORWARD_BLOCK_IMPL_H */

