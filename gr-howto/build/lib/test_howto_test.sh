#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/shruthi/Rx_BF/gr-howto/lib
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/shruthi/Rx_BF/gr-howto/build/lib:$PATH
export LD_LIBRARY_PATH=/home/shruthi/Rx_BF/gr-howto/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-howto 
