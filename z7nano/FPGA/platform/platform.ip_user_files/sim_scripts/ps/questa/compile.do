vlib questa_lib/work
vlib questa_lib/msim

vlib questa_lib/msim/xilinx_vip
vlib questa_lib/msim/axi_infrastructure_v1_1_0
vlib questa_lib/msim/axi_vip_v1_1_22
vlib questa_lib/msim/processing_system7_vip_v1_0_24
vlib questa_lib/msim/xil_defaultlib

vmap xilinx_vip questa_lib/msim/xilinx_vip
vmap axi_infrastructure_v1_1_0 questa_lib/msim/axi_infrastructure_v1_1_0
vmap axi_vip_v1_1_22 questa_lib/msim/axi_vip_v1_1_22
vmap processing_system7_vip_v1_0_24 questa_lib/msim/processing_system7_vip_v1_0_24
vmap xil_defaultlib questa_lib/msim/xil_defaultlib

vlog -work xilinx_vip -64 -incr -mfcu  -sv -L axi_vip_v1_1_22 -L processing_system7_vip_v1_0_24 -L xilinx_vip "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi4stream_vip_axi4streampc.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi_vip_axi4pc.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/xil_common_vip_pkg.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi4stream_vip_pkg.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi_vip_pkg.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi4stream_vip_if.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/axi_vip_if.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/clk_vip_if.sv" \
"/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/hdl/rst_vip_if.sv" \

vlog -work axi_infrastructure_v1_1_0 -64 -incr -mfcu  "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl" "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl" "+incdir+../../../../../../../../.local/share/vivado/2025.2/data/rsb/busdef" "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" \
"../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl/axi_infrastructure_v1_1_vl_rfs.v" \

vlog -work axi_vip_v1_1_22 -64 -incr -mfcu  -sv -L axi_vip_v1_1_22 -L processing_system7_vip_v1_0_24 -L xilinx_vip "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl" "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl" "+incdir+../../../../../../../../.local/share/vivado/2025.2/data/rsb/busdef" "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" \
"../../../../platform.gen/sources_1/bd/ps/ipshared/b16a/hdl/axi_vip_v1_1_vl_rfs.sv" \

vlog -work processing_system7_vip_v1_0_24 -64 -incr -mfcu  -sv -L axi_vip_v1_1_22 -L processing_system7_vip_v1_0_24 -L xilinx_vip "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl" "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl" "+incdir+../../../../../../../../.local/share/vivado/2025.2/data/rsb/busdef" "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" \
"../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl/processing_system7_vip_v1_0_vl_rfs.sv" \

vlog -work xil_defaultlib -64 -incr -mfcu  "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/ec67/hdl" "+incdir+../../../../platform.gen/sources_1/bd/ps/ipshared/9a25/hdl" "+incdir+../../../../../../../../.local/share/vivado/2025.2/data/rsb/busdef" "+incdir+/home/brage/.local/share/vivado/2025.2/data/xilinx_vip/include" \
"../../../bd/ps/ip/ps_processing_system7_0_0/sim/ps_processing_system7_0_0.v" \

vcom -work xil_defaultlib -64 -93  \
"../../../bd/ps/sim/ps.vhd" \

vlog -work xil_defaultlib \
"glbl.v"

