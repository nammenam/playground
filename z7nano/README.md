
openocd -f interface/ftdi/um232h.cfg -f target/zynq_7000.cfg


if you want to use only FPGA you can load fsbl with bitstream right after
if you want only CPU you use the fsbl + application.elf
if you want to use both CPU + FPGA you need to use block diagram and connect the fpga design to the processing system
and make a special xsa + fsbl with bitstream
