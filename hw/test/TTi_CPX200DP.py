import pyvisa

class TTi_CPX200DP:
    """ Class for TTi CPX200DP power supply """

    def set_volt(self, volt, channel=1):
        self.ps.write(f"V{channel} {volt}")

    def get_volt(self, channel=1):
        return self.ps.write(f"V{channel}?")

    def meas_volt(self, channel=1):
        return float(self.ps.write(f"V{channel}O?").strip("V\r\n"))

    def set_amp(self, amp, channel=1):
        self.ps.write(f"I{channel} {amp}")

    def get_amp(self, channel=1):
        return self.ps.query(f"I{channel}?")

    def meas_amp(self, channel=1):
        return float(self.ps.query(f"I{channel}O?").strip("A\r\n"))

    def set_ovp(self, volt, channel=1):
        self.ps.write(f"OVP{channel} {volt}")

    def set_ocp(self, amp, channel=1):
        self.ps.write(f"OCP{channel} {amp}")

    def get_ovp(self, channel=1):
        return self.ps.query(f"OVP{channel}?")

    def get_ocp(self, channel=1):
        return self.ps.query(f"OCP{channel}?")

    def all_off(self):
        self.ps.write(f"OPALL 0")

    def off(self, channel=1):
        self.ps.write(f"OP{channel} 0")

    def on(self, channel=1):
        self.ps.write(f"OP{channel} 1")

    def reset(self):
        self.set_volt(0, 1)
        self.set_amp(0, 1)
        self.set_volt(0, 2)
        self.set_amp(0, 2)
        self.all_off()

    def __init__(self, rm, GPIB_Interface="GPIB0", GPIB_Addr=16, ovp = ""):
        # Variables
        self.rm = rm
        self.ID = "THURLBY THANDAR, CPX200DP"

        # Initiate GPIB communication
        self.ps = self.rm.open_resource(f"{GPIB_Interface}::{str(GPIB_Addr)}::INSTR")

        # Selftest
        Resp = self.ps.query("*IDN?")
        if self.ID not in Resp:
            raise NameError(f"Power supply: Equipment setup incorrect, Expected: {self.ID}, Detected: {Resp}...")

        # Reset to power-on state
        self.ps.write("*RST")

        # Set overvoltage protection
        if ovp != "":
            if isinstance(ovp, list):
                print(ovp)
                print(ovp[0])
                print(ovp[1])
                self.set_ovp(ovp[0], 1)
                self.set_ovp(ovp[1], 2)
                print(f"Power supply: OVP Channel 1: {self.get_ovp(1)}")
                print(f"Power supply: OVP Channel 2: {self.get_ovp(2)}")

        # Lock instrument
        Resp = self.ps.query("IFLOCK?")
        if int(Resp) == 0:
            Resp = self.ps.query("IFLOCK")
            if int(Resp) != 1:
                raise NameError("Power supply: Instrument lock not possible")

        # Set outputs to zero and off
        self.reset()

    if __name__ == "__main__":
        self.rm = pyvisa.ResourceManager()
        __init__(self, rm, "GPIB0", 16)

