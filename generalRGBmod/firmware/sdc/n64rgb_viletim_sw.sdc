
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

set_output_delay -clock { VCLK } 0 [get_ports {R_o* G_o* B_o* nHSYNC nVSYNC nCSYNC nCLAMP}] -add



#**************************************************************
# Set Clock Groups
#**************************************************************



#**************************************************************
# Set False Path
#**************************************************************

set_false_path -from [get_ports {nAutoDeBlur nForceDeBlur_i1 nForceDeBlur_i99 n15bit_mode}]



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

