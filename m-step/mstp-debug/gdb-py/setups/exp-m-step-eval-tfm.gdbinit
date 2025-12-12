# Include common GDB settings
# -s will attach the relative path to the current working directory
source -s /setups/common/common.gdbinit

# Add symbols
#add-symbol-file /home/cris/Documents/0_Projects/jo-collab/copilot/build/nucleo_l552ze_q_mstp_s/bin/bl2.elf
add-symbol-file /home/cris/Documents/0_Projects/jo-collab/copilot/build/nucleo_l552ze_q_mstp_s/bin/tfm_s.elf
add-symbol-file /home/cris/Documents/0_Projects/jo-collab/exp-m-step-eval-tfm/src/NonSecure/build/tfm_ns.elf

# Reset and halt the MCU
monitor reset halt

source -s /scripts/trace_lookup.py

enable-auto-trace

layout split

# set $stop_addr = END_VICTIM

# b single_step

# commands
#     instcounter start
# end

# c
