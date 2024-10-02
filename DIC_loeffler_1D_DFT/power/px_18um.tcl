#####################################################################
#   PrimeTime PX power analysis for
#	Simple Power Measure with VCD dump in the UMC 18um Tech
#####################################################################

#   primetime -f px_18um.tcl > run &

#set the gobal parameters
set design "loeffler_1d"
set report_path  "report_18"
set mapped_path  "../syn/mapped_18"
set source_path_sim  "../presim"
set tech_lib_path "/home/cad/tech/CBDK018_UMC_Faraday_v1.1/CIC/SynopsysDC/db"
set vcd_file $design.vcd
#set saif_file $design.saif

set tp_module stimulus_1ddct
set tp_instance dct_1d


#set prime time mode
set power_enable_analysis TRUE
set power_analysis_mode time_based

#####################################################################
#       link design
#####################################################################
set search_path         "$mapped_path $tech_lib_path . "
set target_library    "fsa0m_a_generic_core_ss1p62v125c.db fsa0m_a_generic_core_ff1p98vm40c.db fsa0m_a_generic_core_tt1p8v25c.db fsa0m_a_t33_generic_io_ss1p62v125c.db fsa0m_a_t33_generic_io_ff1p98vm40c.db fsa0m_a_t33_generic_io_tt1p8v25c.db"
set link_library      "* $target_library"

read_verilog		$mapped_path/CHIP_syn.v
current_design		$design
link


#####################################################################
#       set transition time / annotate parasitics
#####################################################################
read_sdc $mapped_path/CHIP_syn.sdc
#set_disable_timing [get_lib_pins ssc_core_typ/*/G]
read_parasitics	$mapped_path/CHIP_syn.spef


#####################################################################
#       check/update/report timing 
#####################################################################
check_timing
update_timing
report_timing > $report_path/$design.time

#####################################################################
#       read switching activity file
#####################################################################
read_vcd $source_path_sim/$vcd_file -rtl -strip_path $tp_module/$tp_instance
#read_saif $source_path_sim/$saif_file -strip_path $tp_module/$tp_instance

#####################################################################
#       check/update/report power 
#####################################################################
check_power
set_power_analysis_options -waveform_format out -waveform_output vcd
update_power
report_power > $report_path/$design.power



exit

