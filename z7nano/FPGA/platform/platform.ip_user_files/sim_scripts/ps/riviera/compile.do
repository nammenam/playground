transcript off
onbreak {quit -force}
onerror {quit -force}
transcript on

vlib work
vlib riviera/xilinx_vip
vlib riviera/axi_infrastructure_v1_1_0
vlib riviera/axi_vip_v1_1_22
vlib riviera/processing_system7_vip_v1_0_24
vlib riviera/xil_defaultlib

vmap xilinx_vip riviera/xilinx_vip
vmap axi_infrastructure_v1_1_0 riviera/axi_infrastructure_v1_1_0
vmap axi_vip_v1_1_22 riviera/axi_vip_v1_1_22
vmap processing_system7_vip_v1_0_24 riviera/processing_system7_vip_v1_0_24
vmap xil_defaultlib riviera/xil_defaultlib

vlog -work xilinx_vip  -incr "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" -l xilinx_vip -l axi_infrastructure_v1_1_0 -l axi_vip_v1_1_22 -l processing_system7_vip_v1_0_24 -l xil_defaultlib \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi4stream_vip_axi4streampc.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi_vip_axi4pc.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/xil_common_vip_pkg.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi4stream_vip_pkg.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi_vip_pkg.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi4stream_vip_if.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi_vip_if.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/clk_vip_if.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/rst_vip_if.sv" \

vlog -work axi_infrastructure_v1_1_0  -incr -v2k5 "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl" "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl" "+incdir+../../../../../../../../.local/share/vivado/2025.2/data/rsb/busdef" "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" -l xilinx_vip -l axi_infrastructure_v1_1_0 -l axi_vip_v1_1_22 -l processing_system7_vip_v1_0_24 -l xil_defaultlib \
"../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl/axi_infrastructure_v1_1_vl_rfs.v" \

vlog -work axi_vip_v1_1_22  -incr "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl" "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl" "+incdir+../../../../../../../../.local/share/vivado/2025.2/data/rsb/busdef" "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" -l xilinx_vip -l axi_infrastructure_v1_1_0 -l axi_vip_v1_1_22 -l processing_system7_vip_v1_0_24 -l xil_defaultlib \
"../../../../platform.gen/sources_1/bd/ps/ipshared/b16a/hdl/axi_vip_v1_1_vl_rfs.sv" \

vlog -work processing_system7_vip_v1_0_24  -incr "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl" "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl" "+incdir+../../../../../../../../.local/share/vivado/2025.2/data/rsb/busdef" "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" -l xilinx_vip -l axi_infrastructure_v1_1_0 -l axi_vip_v1_1_22 -l processing_system7_vip_v1_0_24 -l xil_defaultlib \
"../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl/processing_system7_vip_v1_0_vl_rfs.sv" \

vlog -work xil_defaultlib  -incr -v2k5 "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl" "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl" "+incdir+../../../../../../../../.local/share/vivado/2025.2/data/rsb/busdef" "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" -l xilinx_vip -l axi_infrastructure_v1_1_0 -l axi_vip_v1_1_22 -l processing_system7_vip_v1_0_24 -l xil_defaultlib \
"../../../bd/ps/ip/ps_processing_system7_0_0/sim/ps_processing_system7_0_0.v" \

vcom -work xil_defaultlib -93  -incr \
"../../../bd/ps/sim/ps.vhd" \

vlog -work xil_defaultlib \
"glbl.v"

