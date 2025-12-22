import pyvisa
import time
import statistics

class Fluke_45:
    """ Class for Fluke 45 Dual Display Multimeter """

    def opc_check(self, opccnt_time = 1): # Change OPC time to constant <self.opccnt_time>
        opccnt_limit = int(opccnt_time / self.opccnt_delay)
        opc = 0
        opccnt = 0
        while opc != 1:
            opc = int(self.dmm.query("*OPC?"))
            opccnt += 1
            if opccnt > opccnt_limit:
                raise NameError("Multimeter: Operation complete not achieved within {opccnt_time}s")
            time.sleep(self.opccnt_delay)

    def reset(self):
        self.dmm.write("*RST")
        self.opc_check()

    def meas(self, func):
        self.dmm.write(f"{func}")
        res = float(self.dmm.query(f"MEAS?"))
        return res

    def meas_volt_dc(self):
        return self.meas("VDC")

    def meas_volt_ac(self):
        return self.meas("VAC")

    def meas_volt_ac_dc(self):
        return self.meas("VACDC")

    def meas_amp_dc(self):
        return self.meas("ADC")

    def meas_amp_ac(self):
        return self.meas("AAC")

    def meas_amp_ac_dc(self):
        return self.meas("AACDC")

    def meas_res(self):
        return self.meas("OHMS")

    def meas_frequency(self):
        return self.meas("FREQ")

    def meas_continuity(self):
        return self.meas("CONT")

    def meas_diode(self):
        return self.meas("DIODE")

    def set_range(self, range_value, func, speed):
        if speed == "fast" or speed == "medium":
            if func == "VDC":
                if range_value <= 0.3:
                    range_num = 1
                elif range_value <= 3:
                    range_num = 2
                elif range_value <= 30:
                    range_num = 3
                elif range_value <= 300:
                    range_num = 4
                elif range_value <= 1000:
                    range_num = 5
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            elif func == "VAC" or func == "VACDC":
                if range_value <= 0.3:
                    range_num = 1
                elif range_value <= 3:
                    range_num = 2
                elif range_value <= 30:
                    range_num = 3
                elif range_value <= 300:
                    range_num = 4
                elif range_value <= 750:
                    range_num = 5
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            elif func == "ADC" or func == "AAC" or func == "AACDC":
                if range_value <= 0.03:
                    range_num = 1
                elif range_value <= 0.1:
                    range_num = 2
                elif range_value <= 10:
                    range_num = 3
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            elif func == "OHMS":
                if range_value <= 300:
                    range_num = 1
                elif range_value <= 3000:
                    range_num = 2
                elif range_value <= 30000:
                    range_num = 3
                elif range_value <= 300000:
                    range_num = 4
                elif range_value <= 3000000:
                    range_num = 5
                elif range_value <= 30000000:
                    range_num = 6
                elif range_value <= 300000000:
                    range_num = 7
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            elif func == "FREQ":
                if range_value <= 1000:
                    range_num = 1
                elif range_value <= 10000:
                    range_num = 2
                elif range_value <= 100000:
                    range_num = 3
                elif range_value <= 1000000:
                    range_num = 4
                elif range_value <= 1000000:
                    range_num = 5
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            else:
                raise NameError(f"Unknown function: {func}")
        elif speed == "slow":
            if func == "VDC":
                if range_value <= 0.1:
                    range_num = 1
                elif range_value <= 1:
                    range_num = 2
                elif range_value <= 10:
                    range_num = 3
                elif range_value <= 100:
                    range_num = 4
                elif range_value <= 1000:
                    range_num = 5
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            elif func == "VAC" or func == "VACDC":
                if range_value <= 0.1:
                    range_num = 1
                elif range_value <= 1:
                    range_num = 2
                elif range_value <= 10:
                    range_num = 3
                elif range_value <= 100:
                    range_num = 4
                elif range_value <= 750:
                    range_num = 5
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            elif func == "ADC" or func == "AAC" or func == "AACDC":
                if range_value <= 0.01:
                    range_num = 1
                elif range_value <= 0.1:
                    range_num = 2
                elif range_value <= 10:
                    range_num = 3
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            elif func == "OHMS":
                if range_value <= 100:
                    range_num = 1
                elif range_value <= 1000:
                    range_num = 2
                elif range_value <= 10000:
                    range_num = 3
                elif range_value <= 100000:
                    range_num = 4
                elif range_value <= 1000000:
                    range_num = 5
                elif range_value <= 10000000:
                    range_num = 6
                elif range_value <= 100000000:
                    range_num = 7
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            elif func == "FREQ":
                if range_value <= 1000:
                    range_num = 1
                elif range_value <= 10000:
                    range_num = 2
                elif range_value <= 100000:
                    range_num = 3
                elif range_value <= 1000000:
                    range_num = 4
                elif range_value <= 1000000:
                    range_num = 5
                else:
                    raise NameError(f"Range outside of equipment specification: {range_value}")
            else:
                raise NameError(f"Unknown function: {func}")
        else:
            raise NameError(f"Unknown measurement rate: {speed}")
        self.dmm.write(f"FIXED")
        self.dmm.write(f"RANGE {range_num}")

    def __init__(self, rm, Interface="GPIB0", GPIB_Addr=1, run_selftest = True):
        # Variables
        self.rm = rm
        self.ID = "FLUKE, 45"
        self.opccnt_time = 1
        self.opccnt_delay = 0.1

        # Initiate GPIB communication
        self.dmm = self.rm.open_resource(f"{Interface}::{str(GPIB_Addr)}::INSTR")

        # Selftest
        Resp = self.dmm.query("*IDN?")
        if self.ID not in Resp:
            raise NameError(f"Multimeter: Equipment setup incorrect, Expected: {self.ID}, Detected: {Resp}...")
        else:
            [self.Mfg, self.Type, self.SerialNumber, SoftwareVersion] = Resp.split(', ')
            [self.SoftwareVersionMain, self.SoftwareVersionDisplay] = SoftwareVersion.split(' D')
        if run_selftest:
            timeout = self.dmm.timeout
            self.dmm.timeout = 20000
            Resp = self.dmm.query("*TST?")
            if int(Resp) != 0:
                raise NameError(f"Multimeter: Selftest failed, Error code: {Resp}")
            self.dmm.timeout = timeout

        # Reset to power-on state
        self.reset()
        self.opc_check()

    if __name__ == "__main__":
        self.rm = pyvisa.ResourceManager()
        __init__(self, rm, "GPIB0", 5)

