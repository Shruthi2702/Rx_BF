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

#ifndef INCLUDED_HOWTO_RX_BF_GEN_FB_IMPL_H
#define INCLUDED_HOWTO_RX_BF_GEN_FB_IMPL_H

#include <howto/rx_bf_gen_fb.h>
#include <gnuradio/sync_decimator.h>
#include <gnuradio/analog/noise_type.h>
#include <gnuradio/random.h>

namespace gr {
  namespace howto {

    class rx_bf_gen_fb_impl : public rx_bf_gen_fb
    {
     private:
      // Nothing to declare in this block.
      double d_sampling_rate; 
  	int d_decim_factor;
	double d_treshold;
	int d_nbr_samples_packet;
	int d_nbr_samples_cycle;
	double d_delay_security_factor;
	double d_time_delay;

	int cpt_samples;
	int cpt_samples_packet;
	int packet_number;
	float feedback_symbol;
	double avg_rss;
	double current_rss;
	double last_rss;
	double rss_max;
	double past_rss [4];
	int delay_since_last_rss_meas;
	int last_cpt_samples;
     public:
      rx_bf_gen_fb_impl(double sampling_rate, double treshold, int decim_factor, int nbr_samples_packet, int nbr_samples_cycle, double   delay_security_factor, double time_delay);
      ~rx_bf_gen_fb_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace howto
} // namespace gr

#endif /* INCLUDED_HOWTO_RX_BF_GEN_FB_IMPL_H */

