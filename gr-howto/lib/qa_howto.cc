/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * This class gathers together all the test cases for the gr-filter
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include "qa_howto.h"
#include "qa_rxbf_tx.h"
#include "qa_rx_bf_gen_fb.h"
#include "qa_relayBF.h"
#include "qa_randphpert.h"
#include "qa_forward_block.h"
#include "qa_random_ph_pert.h"

CppUnit::TestSuite *
qa_howto::suite()
{
  CppUnit::TestSuite *s = new CppUnit::TestSuite("howto");
  s->addTest(gr::howto::qa_rxbf_tx::suite());
  s->addTest(gr::howto::qa_rx_bf_gen_fb::suite());
  s->addTest(gr::howto::qa_relayBF::suite());
  s->addTest(gr::howto::qa_randphpert::suite());
  s->addTest(gr::howto::qa_forward_block::suite());
  s->addTest(gr::howto::qa_random_ph_pert::suite());

  return s;
}
