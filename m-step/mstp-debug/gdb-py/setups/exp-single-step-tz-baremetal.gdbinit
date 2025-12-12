# Include common GDB settings
# -s will attach the relative path to the current working directory
source -s /setups/common/common.gdbinit

# Add symbols
add-symbol-file /home/cris/Documents/0_Projects/jo-collab/exp-single-step-tz-baremetal/STM32L5-Single-step_Cache_110MHz_TZ_Baremetal/Secure/build/STM32L5-Single-step_Cache_110MHz_S.elf
add-symbol-file /home/cris/Documents/0_Projects/jo-collab/exp-single-step-tz-baremetal/STM32L5-Single-step_Cache_110MHz_TZ_Baremetal/NonSecure/build/STM32L5-Single-step_Cache_110MHz_NS.elf

# Reset and halt the MCU
monitor reset halt

source -s /scripts/trace_lookup.py

enable-auto-trace

# set $stop_addr = END_VICTIM

# b single_step

# commands
#     instcounter start
# end

# c
