
#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3



#**************************************************************
# Create Clock
#**************************************************************

create_clock -name {VCLK} -period 20.000 -waveform { 0.000 15.000 } [get_ports { VCLK }]



#**************************************************************
# Create Generated Clock
#**************************************************************

create_generated_clock -name CLK_4M -source [get_ports {VCLK}] -divide_by 12 [get_registers {n64_igr:igr|CLK_4M}]

create_generated_clock -name CLK_ADV712x -source [get_ports {VCLK}] -divide_by 1 -multiply_by 1 [get_ports {CLK_ADV712x}]



#**************************************************************
# Set Clock Latency
#**************************************************************



#**************************************************************
# Set Clock Uncertainty
#**************************************************************



#**************************************************************
# Set Input Delay
#**************************************************************

set_input_delay -clock { VCLK } -min 0.0 [get_ports {nDSYNC}]
set_input_delay -clock { VCLK } -max 6.5 [get_ports {nDSYNC}]
set_input_delay -clock { VCLK } -min 0.0 [get_ports {D_i[*]}]
set_input_delay -clock { VCLK } -max 6.5 [get_ports {D_i[*]}]



#**************************************************************
# Set Output Delay
#**************************************************************

set_output_delay -clock { CLK_ADV712x } -max 0.5 [get_ports {R_o* G_o* B_o* nCSYNC_ADV712x nBLANK_ADV712x}] -add
set_output_delay -clock { CLK_ADV712x } -min -1.5 [get_ports {R_o* G_o* B_o* nCSYNC_ADV712x nBLANK_ADV712x}] -add

set_output_delay -clock { VCLK } 0 [get_ports {nHSYNC nVSYNC nCSYNC nCLAMP}] -add



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



#**************************************************************
# Set Multicycle Path
#**************************************************************



#**************************************************************
# Set Maximum Delay
#**************************************************************



#**************************************************************
# Set Minimum Delay
#**************************************************************



#**************************************************************
# Set Input Transition
#**************************************************************

