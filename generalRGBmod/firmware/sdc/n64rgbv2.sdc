
#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3



#**************************************************************
# Create Clock
#**************************************************************

set n64_vclk_per 20.000
set n64_vclk_waveform [list 0.000 [expr $n64_vclk_per*3/4]]

create_clock -name {VCLK_N64_VIRT} -period $n64_vclk_per -waveform $n64_vclk_waveform
create_clock -name {VCLK} -period $n64_vclk_per -waveform $n64_vclk_waveform [get_ports {VCLK}]



#**************************************************************
# Create Generated Clock
#**************************************************************

create_generated_clock -name {CLK_4M} -source [get_ports {VCLK}] -divide_by 12 [get_registers {igr|CLK_4M}]
create_generated_clock -name {CLK_ADV712x} -source [get_ports {VCLK}] -divide_by 1 -multiply_by 1 [get_ports {CLK_ADV712x}]


#**************************************************************
# Set Clock Latency
#**************************************************************



#**************************************************************
# Set Clock Uncertainty
#**************************************************************



#**************************************************************
# Set Input Delay
#**************************************************************

set n64_data_delay_min 0.0
set n64_data_delay_max 6.5

set_input_delay -clock {VCLK_N64_VIRT} -min $n64_data_delay_min [get_ports {nDSYNC}]
set_input_delay -clock {VCLK_N64_VIRT} -max $n64_data_delay_max [get_ports {nDSYNC}]
set_input_delay -clock {VCLK_N64_VIRT} -min $n64_data_delay_min [get_ports {D_i[*]}]
set_input_delay -clock {VCLK_N64_VIRT} -max $n64_data_delay_max [get_ports {D_i[*]}]


#**************************************************************
# Set Output Delay
#**************************************************************
set adv_tsu 0.5
set adv_th  1.5
set out_dly_max $adv_tsu
set out_dly_min [expr -$adv_th]

set_output_delay -clock {CLK_ADV712x} -max $out_dly_max [get_ports {R_o* G_o* B_o* nCSYNC_ADV712x nBLANK_ADV712x}]
set_output_delay -clock {CLK_ADV712x} -min $out_dly_min [get_ports {R_o* G_o* B_o* nCSYNC_ADV712x nBLANK_ADV712x}]

#set_output_delay -clock {VCLK} 0 [get_ports {nHSYNC nVSYNC nCSYNC nCLAMP}]


#**************************************************************
# Set Clock Groups
#**************************************************************

set_clock_groups -asynchronous -group \
                            {VCLK CLK_ADV712x} \
                            {CLK_4M}



#**************************************************************
# Set False Path
#**************************************************************

set_false_path -from [get_ports {nRST_nManualDB CTRL_nAutoDB nSYNC_ON_GREEN install_type Default_nForceDeBlur Default_DeBlur Default_n15bit_mode}]
set_false_path -to [get_ports {nRST_nManualDB}]

set_false_path -to [get_ports {nHSYNC nVSYNC nCSYNC nCLAMP}]


#**************************************************************
# Set Multicycle Path
#**************************************************************

set dbl_cycle_paths_from [list [get_ports {D_i[*]}] [get_registers {video_demux|*}] [get_registers {get_vinfo|*}] [get_registers {deblur_management|*}]]
set dbl_cycle_paths_to [list [get_registers {video_demux|*}] [get_registers {get_vinfo|*}] [get_registers {deblur_management|*}]]

foreach from_path $dbl_cycle_paths_from {
  foreach to_path $dbl_cycle_paths_to {
    if {!($from_path == "[get_registers {deblur_management|*}]" && $to_path == "[get_registers {get_vinfo|*}]")} {
      set_multicycle_path -from $from_path -to $to_path -setup 2
      set_multicycle_path -from $from_path -to $to_path -hold 1
    }
  }
}

# revert some multicycle paths to their default values
set_multicycle_path -from [get_registers {deblur_management|gradient_changes[*]}] -setup 1
set_multicycle_path -from [get_registers {deblur_management|gradient_changes[*]}] -hold 0
set_multicycle_path -from [get_registers {video_demux|n15bit_mode}] -setup 1
set_multicycle_path -from [get_registers {video_demux|n15bit_mode}] -hold 0
set_multicycle_path -from [get_registers {video_demux|vdata_r_0[*]}] -to [get_registers {video_demux|vdata_r_1[*]}] -setup 1
set_multicycle_path -from [get_registers {video_demux|vdata_r_0[*]}] -to [get_registers {video_demux|vdata_r_1[*]}] -hold 0


#**************************************************************
# Set Maximum Delay
#**************************************************************



#**************************************************************
# Set Minimum Delay
#**************************************************************



#**************************************************************
# Set Input Transition
#**************************************************************

