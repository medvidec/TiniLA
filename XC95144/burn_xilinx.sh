#!/bin/sh
sed -i 's/FREQUENCY 1E6 HZ/FREQUENCY 5E5 HZ/' project.svf

echo "cable ft2232
detect
svf project.svf progress
" > scr$.scr

/opt/urjtag-0.10/bin/jtag  scr$.scr

rm -rf scr$.scr
