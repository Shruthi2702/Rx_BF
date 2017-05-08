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
using namespace std;
#include <stdio.h>
#include <gnuradio/fxpt.h>
#include <gnuradio/math.h>
#include <gnuradio/io_signature.h>
#include "rx_bf_gen_fb_impl.h"
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/format.hpp>


namespace gr {
  namespace howto {

    rx_bf_gen_fb::sptr
    rx_bf_gen_fb::make(double sampling_rate, double treshold, int decim_factor, int nbr_samples_packet, int nbr_samples_cycle, double     delay_security_factor, double time_delay)
    {
      return gnuradio::get_initial_sptr (new rx_bf_gen_fb_impl(sampling_rate, treshold, decim_factor, nbr_samples_packet, nbr_samples_cycle, delay_security_factor, time_delay));
    }

    /*
     * The private constructor
     */
    rx_bf_gen_fb_impl::rx_bf_gen_fb_impl(double sampling_rate, double treshold, int decim_factor, int nbr_samples_packet, int nbr_samples_cycle, double delay_security_factor, double time_delay)
      : gr::block("rx_bf_gen_fb",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(float))),
              d_sampling_rate(sampling_rate),
	      d_treshold(treshold),
              d_decim_factor(decim_factor), 
	      d_nbr_samples_packet(nbr_samples_packet), 
	      d_nbr_samples_cycle(nbr_samples_cycle),
	      d_delay_security_factor(delay_security_factor),
	      d_time_delay(time_delay)
    {

	/*d_treshold 		= 0.00025;
	d_nbr_samples_packet 	= 800;
	d_nbr_samples_cycle 	= 10000;
	d_delay_security_factor = 1.5;
	d_time_delay 		= 0.015;*/

	cpt_samples 		= 0;
	cpt_samples_packet 	= 0;
	packet_number 		= 0;
	feedback_symbol 	= 0;
	avg_rss 		= 0;
	current_rss 		= 0;
	last_rss 		= 0;
	rss_max 		= 0;
	double past_rss[4]      ={0,0,0,0};			

	delay_since_last_rss_meas = 0;
	last_cpt_samples = 0;
	

	}

    /*
     * Our virtual destructor.
     */
    rx_bf_gen_fb_impl::~rx_bf_gen_fb_impl()
    {
    }

    void
    rx_bf_gen_fb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    rx_bf_gen_fb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      float *out = (float *) output_items[0];
      int j=0;
      for (int i = 0; i < noutput_items*d_decim_factor; i++){
      cpt_samples++;

      if ( (cpt_samples)%d_nbr_samples_cycle == 0 ){ 		
	    	out[j] = feedback_symbol; 
	    	printf ("Output = %d\n", (int)out[j]); 
	    	j+=1; 
	} // End of if( (cpt_samples)%d_nbr_samples_cycle == 0 )
	
	// Monitor input	
	// -------------
    	if (std::abs(in[i]) > d_treshold){	    
	    cpt_samples_packet++;
	    avg_rss = avg_rss + std::abs(in[i]);
	    // Packet complete
	    if (cpt_samples_packet == d_nbr_samples_packet){
		current_rss = avg_rss/(d_nbr_samples_packet);
		delay_since_last_rss_meas = cpt_samples - last_cpt_samples;
		last_cpt_samples = cpt_samples;
		//printf(" -- Packet complete - RSS = %f \n", current_rss);
		//printf(" -- delay_since_last_rss_meas = %d  \n", delay_since_last_rss_meas);
		//printf(" -- RSS = %f  -- last RSS  %f\n", current_rss, last_rss);
		if ( (delay_since_last_rss_meas < d_time_delay * d_sampling_rate * d_delay_security_factor) && (current_rss > last_rss) )  {		
		    if (packet_number<1000) { packet_number +=1; }
        	    else 			{ packet_number = 1; } 
		    //printf(" -- RSS = %f \n", current_rss);
		    rss_max= *max_element(past_rss,past_rss+4);
		    if (current_rss > rss_max){ feedback_symbol = packet_number; }   
        	    else			  { feedback_symbol = -packet_number; }
		    avg_rss=0; 
        	    cpt_samples_packet = 0;
      		    past_rss[3]=past_rss[2]; 
        	    past_rss[2]=past_rss[1];
        	    past_rss[1]=past_rss[0];
        	    past_rss[0]=current_rss;
		} // End of if( delay_since_last_rss_meas < d_time_delay * d_sampling_rate *1.5)
		last_rss = current_rss;
	    } // End of if(cpt_samples_packet == d_nbr_samples_packet)
	} // End of if(std::abs(in[i]) > d_treshold)
	else{
	    cpt_samples_packet = 0;
	    avg_rss = 0;
	} // End of else-statement of if(std::abs(in[i]) > d_treshold)


  } //end of for(int i = 0; i < noutput_items*d_decim_factor; i++)

  return noutput_items;
  //printf("End of work()...\n");

	} // end of work()

  } /* namespace howto */
} /* namespace gr */
