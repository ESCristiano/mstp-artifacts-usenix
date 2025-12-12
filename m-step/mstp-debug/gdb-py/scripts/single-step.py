import gdb
import re
# import numpy as np

def run(command):
    return gdb.execute(command, to_string=True)

def get_next_PC():
    sp = re.findall("(?:\$\d+ = 0x)([0-9a-fA-F]+)(?:\\n)",run("p /x $sp"))
    sp_offset = 12
    next_pc = re.findall("(?:\$\d+ = 0x)([0-9a-fA-F]+)(?:\\n)",run("p /x *" + hex(int(sp[0], 16)+sp_offset*4)))
    print("next_pc " + hex(int(str(next_pc[0]),16)))
    return str(next_pc[0])

class InstCounter(gdb.Command):

    def __init__(self):
        super(InstCounter, self).__init__("instcounter", gdb.COMMAND_USER)
        self.count = 0

    def startCounting(self):
        stop_addr = run("p /x $stop_addr")
        stop_addr = re.findall("(?:\$\d+ = 0x)([0-9a-fA-F]+)(?:\\n)",stop_addr)
        print(hex(int(str(stop_addr[0]),16)))
        last_addr = 0

        while(get_next_PC() != stop_addr[0]):
            # print("pc " + str(getPC()))
            # print("stop_addr " + str(stop_addr[0]))
            run("c")
            self.count = self.count + 1
        
        # run("x/16xw $sp")
        # sp = re.findall("(?:\$\d+ = 0x)([0-9a-fA-F]+)(?:\\n)",run("p /x $sp"))
        # pc = re.findall("(?:\$\d+ = 0x)([0-9a-fA-F]+)(?:\\n)",run("p /x *" + hex(int(sp[0], 16)+12*4)))
        # print("pc " + str(pc[0]))
        # run("c")

        print("n of interrupts: " + str(self.count))
        run("quit")

    def invoke(self, arg, from_tty):
        if arg == "start":
            self.startCounting()
        else:
            print("not a valid argument")

InstCounter()
