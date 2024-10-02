#pre sim with umc 0.18um simple
#  vsim -c -do post_18um.tcl > run &

set design loeffler_1d
set source_path_pr  "source"
set mapped_path_pr  "../pr/mapped_18_pr"
set tech_lib /home/cad/tech/CBDK018_UMC_Faraday_v1.1/CIC/Verilog/fsa0m_a_generic_core_21.lib.src

set work_path work
set tp_module loeffler_1d_test
set tp_instance stimulus_1ddct

#set num_inputs 512

vlib $work_path
vlog $tech_lib -incr
vlog $mapped_path_pr/CHIP_route.v
#vcom $source_path_pr/$tp_module.vhd
vlog $source_path_pr/$tp_module.v

#vsim -c -l vsim.log -multisource_delay max -sdftyp /$tp_instance/=$mapped_path_pr/$design.sdf -sdfnoerror -sdfnowarn -t ns -novopt  $work_path.$tp_module

#vsim -voptargs=+acc -t ns -sdfnowarn -vopt -sdftyp /uut=/home/ccsun/Project/Lab3/syn/mapped_18/alu.sdf work.alu_simulate -multisource_delay max

#vsim -voptargs=+acc -t ns -sdfnowarn -vopt -sdftyp /$tp_instance=$mapped_path_pr/$design.sdf $work_path.$tp_module -multisource_delay max
vsim -64 -t ns -sdfnowarn -vopt -sdftyp /dct_1d=/home2/VLSI003/yourHomeWork/HW4/pr/mapped_18_pr/CHIP_route.sdf -multisource_delay max -sdfnoerror -voptargs=+acc work.$tp_instance

# vcd is value change dump
vcd file $design.vcd
vcd add -r $tp_instance/dct_1d/*


run -all
#run 1000 ns



exit
