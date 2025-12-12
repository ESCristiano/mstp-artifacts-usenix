import gdb
import os

trace_info = {}
script_dir = os.path.dirname(os.path.abspath(__file__))
trace_file = os.path.join(script_dir, "..", "../../traces/tfm-inv-mod-001", "trace.txt")

with open(trace_file) as f:
    for line in f:
        addr, time = line.strip().split()
        trace_info[int(addr, 16)] = int(time)

def show_timing():
    pc = int(gdb.parse_and_eval("$pc"))
    timing = trace_info.get(pc)
    msg = f"[Trace Info] PC=0x{pc:08X} | {timing} clk" if timing else f"[Trace Info] No timing info for PC=0x{pc:08X}"
    print(msg)

class AutoTrace(gdb.Command):
    def __init__(self):
        super().__init__("enable-auto-trace", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        gdb.events.stop.connect(on_stop)
        print("Auto trace enabled.")

def on_stop(event):
    show_timing()

AutoTrace()
