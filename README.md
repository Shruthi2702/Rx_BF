# Distributed Receiver Beamforming

#### First step is to build the custom blocks in gnuradio-companion
1. mkdir build
2. cd build
3. cmake ..
4. make
5. sudo make install
6. sudo ldconfig

#### NOTE: Follow the above steps on all four computers that is being used

#### Open the grc files available in the grc folder in each computer. One for transmitter, two for relay and one for receiver flow graph
#### i.e., tx_bf.grc, relay.grc, rx_node.grc
#### Now connect the USRP devices accordingly for each flow graph and run the top_block.py python file.
