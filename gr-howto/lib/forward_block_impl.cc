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

#include <gnuradio/io_signature.h>
#include "forward_block_impl.h"
#include <gnuradio/math.h>
#include <stdio.h>
#include <gnuradio/fxpt.h>
#include <gnuradio/tags.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

namespace gr {
  namespace howto {

    forward_block::sptr
    forward_block::make(double sample_rate, double rx_min_treshold, double rx_max_treshold, double time_delay, double time_cycle, int packet_length, int max_missed_symbols, int nbr_zeros_per_burst)
    {
      return gnuradio::get_initial_sptr
        (new forward_block_impl(sample_rate, rx_min_treshold, rx_max_treshold, time_delay, time_cycle, packet_length, max_missed_symbols, nbr_zeros_per_burst));
    }

    /*
     * The private constructor
     */
    forward_block_impl::forward_block_impl(double sample_rate, double rx_min_treshold, double rx_max_treshold, double time_delay, double time_cycle, int packet_length, int max_missed_symbols, int nbr_zeros_per_burst)
      : gr::block("forward_block",
              gr::io_signature::make(1, 1, sizeof (gr_complex)),
              gr::io_signature::make(1, 1, sizeof (gr_complex))),
                d_sample_rate(sample_rate), 
	      	d_rx_min_treshold(rx_min_treshold),
		d_rx_max_treshold(rx_max_treshold), 
		d_time_delay(time_delay),
		d_time_cycle(time_cycle),
		d_N(packet_length),
		d_max_missed_symbols(max_missed_symbols), 
		d_nbr_zeros_per_burst(nbr_zeros_per_burst)
    {
     Fs = d_sample_rate;
     samples_cycle = (int)(d_time_cycle*Fs);
     nbr_samples_wait_bef_list 	= (int)(0.75*(samples_cycle-(d_time_delay*Fs))); 
     std::cout <<  boost::format("nbr_samples_wait_bef_list = %f")% nbr_samples_wait_bef_list << std::endl;	

        state 				= 1;
	first_burst_of_zeros 		= false;
	rx_samples_arg 			= gsl_vector_float_alloc(d_N);
	rx_samples_real			= gsl_vector_float_alloc(d_N);
	rx_samples_imag			= gsl_vector_float_alloc(d_N);
	cpt_samples 			= 0;
	cpt_zeros_burst 		= 0;
	cpt_missed_symbols 		= 0;

	set_output_multiple (d_N);

	std::cout <<  boost::format("End constructor") << std::endl;

    }

    /*
     * Our virtual destructor.
     */
    forward_block_impl::~forward_block_impl()
    {
    }

    void
    forward_block_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    forward_block_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *)input_items[0];  	
      gr_complex *out = (gr_complex *)output_items[0];
      // Grab all "rx_time" tags in this work call
	// -----------------------------------------
  	const uint64_t samp0_count = this->nitems_read(0);
          
  	  std::vector<tag_t> rx_time_tags;
  	gr::block::get_tags_in_range(rx_time_tags, 0, samp0_count, samp0_count + noutput_items, pmt::string_to_symbol("rx_time"));
  	// Print all tags
  	BOOST_FOREACH(const tag_t &rx_time_tag, rx_time_tags){
	    const uint64_t offset = rx_time_tag.offset;
  	    const pmt::pmt_t &value = rx_time_tag.value;
  	    secs = pmt::to_uint64(pmt::tuple_ref(value, 0));
  	    fracs = pmt::to_double(pmt::tuple_ref(value, 1));
	    offs  = offset;
  	    std::cout << std::endl
			<< boost::format("Time Stamp -- Full seconds %u, Frac seconds %f, Abs sample offset %u")
      			% pmt::to_uint64(pmt::tuple_ref(value, 0))
      			% pmt::to_double(pmt::tuple_ref(value, 1))
      			% offset
      			<< std::endl;
		
	    secs_tx = secs;
	    fracs_tx = fracs;
	    secs_tx_zeros = secs;
	    fracs_tx_zeros = fracs;
  	} // End BOOST_FOREACH(const gr_tag_t &rx_time_tag, rx_time_tags)
        int noutput_items_produced_in_this_work_call = 0;
	// Main loop
  	for (int i = 0; i < noutput_items; i++) {

		// USRP initialization and stabilization
		if (cpt_samples < 10000){
			cpt_samples++;
		}

		// STATE 1 - Listen for packet to record
		// -------------------------------------
		if (state == 1){
			if ((std::abs(in[i]) > d_rx_min_treshold) && (std::abs(in[i]) < d_rx_max_treshold)){
				// Start time of rx packet
				start_time_packet = this->nitems_read(0)+i;
				fracs_rx = fracs + start_time_packet/Fs - (offs)/Fs;
    	    			double intpart; //normalize
    	    			fracs_rx = std::modf(fracs_rx, &intpart);
    	    			secs_rx  = secs + uint64_t(intpart);
	    			//std::cout <<  boost::format("[Rx packet] Full seconds %u, Frac seconds %f")% secs_rx% fracs_rx << std::endl;
				// Start time of tx packet
				fracs_tx = fracs_rx + d_time_delay;
    	    			double intpart2; //normalize
    	    			fracs_tx = std::modf(fracs_tx, &intpart2);
    	    			secs_tx  = secs_rx + uint64_t(intpart2);
				//std::cout <<  boost::format("[Tx packet] Full seconds %u, Frac seconds %f")% secs_tx% fracs_tx << std::endl;
				// Record first sample of rx message
				state = 2;
				cpt_missed_symbols = 0;
				//std::cout <<  boost::format("Going to state 2") << std::endl;
				cpt_samples_rx_packet = 1;
				//gsl_vector_float_set(rx_samples_arg, cpt_samples_rx_packet-1, std::arg(in[i]));
				gsl_vector_float_set(rx_samples_real, cpt_samples_rx_packet-1, std::real(in[i]));
				gsl_vector_float_set(rx_samples_imag, cpt_samples_rx_packet-1, std::imag(in[i]));
				power_packet = std::abs(in[i])*std::abs(in[i]);
			} // End of if (std::abs(in[i]) > d_rxThreshold) {
		} // End of if (state == 1){


		// STATE 2 - Recording packet
		// --------------------------
		else if (state == 2){
			if ((std::abs(in[i]) > d_rx_min_treshold) && (std::abs(in[i]) < d_rx_max_treshold)){
				cpt_samples_rx_packet = cpt_samples_rx_packet+1;
				gsl_vector_float_set(rx_samples_arg, cpt_samples_rx_packet-1, std::arg(in[i]));
				gsl_vector_float_set(rx_samples_real, cpt_samples_rx_packet-1, std::real(in[i]));
				gsl_vector_float_set(rx_samples_imag, cpt_samples_rx_packet-1, std::imag(in[i]));
				power_packet = power_packet + (std::abs(in[i])*std::abs(in[i]));
				if (cpt_samples_rx_packet == d_N) { 
					state = 3; 
					cpt_samples_tx_packet = 0;
					power_packet = power_packet/d_N;
					cpt_zeros_burst = 0;
					consume_each (noutput_items);
                			return noutput_items_produced_in_this_work_call;
				}
			} // End if (std::abs(in[i]) > d_rxThreshold) {
			else{ 
				cpt_missed_symbols++;
				if (cpt_missed_symbols > d_max_missed_symbols){
					state = 1; 
					//std::cout <<  boost::format("Going back to state 1") << std::endl; 
				}
			}
		} // End of else if (state == 2){

		
		// STATE 3 - Forwarding packet
		// ---------------------------
		else if (state == 3){
			cpt_samples_tx_packet++;
			out_real = std::cos(gsl_vector_float_get(rx_samples_arg,cpt_samples_tx_packet-1));
			out_imag = std::sin(gsl_vector_float_get(rx_samples_arg,cpt_samples_tx_packet-1));
			//out_real = gsl_vector_float_get(rx_samples_real,cpt_samples_tx_packet-1)/std::sqrt(power_packet);
			//out_imag = gsl_vector_float_get(rx_samples_imag,cpt_samples_tx_packet-1)/std::sqrt(power_packet);
			out[i] = gr_complex(out_real,out_imag); 
			if (cpt_samples_tx_packet == 1){
	    		    	// Make SOB-tag and time-tag for data packet
	    		    	this->make_sob_tag(this->nitems_written(0)+i);
	    		    	this->make_time_tag(this->nitems_written(0)+i); 
			}
			if (cpt_samples_tx_packet == d_N) {
			    	// Make EOB-tag for data packet
				std::cout <<  boost::format("Sending packet") << std::endl;
			    	this->make_eob_tag(this->nitems_written(0) + i); 
				noutput_items_produced_in_this_work_call = d_N;
			    	state = 4; // wait to listen state
				//std::cout <<  boost::format("Going to state 4") << std::endl;
			    	cpt_samples_wait_bef_list = 0; 
				consume_each (noutput_items);
                		return noutput_items_produced_in_this_work_call;}

		} // End of else if (state == 3){


		// STATE 4 - Wait before starting to listen for Rx packet
		// ------------------------------------------------------
		else if (state == 4){
			cpt_samples_wait_bef_list++;
			if (cpt_samples_wait_bef_list == nbr_samples_wait_bef_list){
				state = 1;
				//std::cout <<  boost::format("Going to state 1") << std::endl;
			}
		} // End of else if (state == 4){
		

	} // End of for (int i = 0; i < noutput_items; i++) {

  // Tell runtime system how many input items we consumed on
  // each input stream.

      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
     return noutput_items_produced_in_this_work_call;
    }
    void forward_block_impl::make_time_tag(const uint64_t tag_count){
        const pmt::pmt_t key = pmt::string_to_symbol("tx_time");
        const pmt::pmt_t value = pmt::make_tuple(
            pmt::from_uint64(secs_tx),
            pmt::from_double(fracs_tx)
        );
        const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
	//std::cout <<  boost::format("[packet]-Writing tx time-tag on sample %u - Full seconds %u, Frac seconds %f") % tag_count % secs_tx % fracs_tx << std::endl;
      }

     void forward_block_impl::make_sob_tag(const uint64_t tag_count){
        const pmt::pmt_t key = pmt::string_to_symbol("tx_sob");
        const pmt::pmt_t value = pmt::PMT_T;
        const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
	//std::cout <<  boost::format("Writing SOB-tag on sample %u ") % tag_count << std::endl;
    }
   void forward_block_impl::make_eob_tag(const uint64_t tag_count){
        const pmt::pmt_t key = pmt::string_to_symbol("tx_eob");
        const pmt::pmt_t value = pmt::PMT_T;
        const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
	//std::cout <<  boost::format("Writing EOB-tag on sample %u ") % tag_count << std::endl << std::endl;
    }



  } /* namespace howto */
} /* namespace gr */

