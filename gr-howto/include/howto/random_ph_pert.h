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


#ifndef INCLUDED_HOWTO_RANDOM_PH_PERT_H
#define INCLUDED_HOWTO_RANDOM_PH_PERT_H

#include <howto/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace howto {

    /*!
     * \brief <+description of block+>
     * \ingroup howto
     *
     */
    class HOWTO_API random_ph_pert : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<random_ph_pert> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of howto::random_ph_pert.
       *
       * To avoid accidental use of raw pointers, howto::random_ph_pert's
       * constructor is in a private implementation
       * class. howto::random_ph_pert::make is the public interface for
       * creating new instances.
       */
      static sptr make(double random_perturbation_size, int bf_flag);
      virtual gr_complex poll_complex_gain() const = 0;   
      virtual int bf_flag() const = 0;
    };

  } // namespace howto
} // namespace gr

#endif /* INCLUDED_HOWTO_RANDOM_PH_PERT_H */

