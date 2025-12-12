# Connect to the GDB server
target extended-remote localhost:3333
# disable user confirmation inputs
set confirm off
# save log to file (default: gdb.txt)
set logging enabled off
# disable pagination to avoid user inputs
set pagination off
# start gdb silently quietly
#set startup-quietly on