import datetime
import time
import pyvisa
import os
from TTi_CPX200DP import TTi_CPX200DP
from Fluke_45 import Fluke_45
from DUT import DUT

###########################
# Test procedure switches #
###########################

SIMULATION  = False
DEBUG       = True
DEBUG_HALT  = 1 # 0: ignore all halts, 1: only halt when parameter "halt" is True, 2: halt on every debug() statement
DEBUG_DELAY = 0
#MEAS_STAT   = True

TEST_ALL        = False
TEST_VOLTAGE    = False
TEST_CURRENT    = False
TEST_SENS       = True

###################
# Test parameters #
###################

# Voltage
VOLT_MIN = 0
VOLT_MAX = 60
VOLT_STEP = 1
VOLT_CURRENT = 0.1
VOLT_REPEAT = 100

# Current
AMP_MIN = 0
AMP_MAX = 10
AMP_STEP = 0.5
AMP_VOLTAGE = 1.5
AMP_REPEAT = 100

# Voltage
SENS_MIN = 0
SENS_MAX = 6
SENS_STEP = 1
SENS_CURRENT = 0.1
SENS_REPEAT = 100

##################################################
# Measurement equipment connection configuration #
##################################################

# Power supply channels
PS_CH_VOLT = 1
PS_CH_AMP  = 2

######################################
# Measurement equipment connectivity #
######################################

#Power Supply
PS_INTERFACE = "GPIB0"
PS_ADDR = 11

# Multimeter
DMM_INTERFACE = "GPIB0"
DMM_ADDR = 1

# DUT
DUT_SERIALPORT = "COM4"

def debug(msg = "", halt = False):
    if DEBUG:
        print(f"DEBUG: {msg}")
        if DEBUG_HALT >= 2 or (DEBUG_HALT >= 1 and halt):
            print(f"DEBUG: Halted...")
            input()
        else:
            time.sleep(DEBUG_DELAY)

def voltage_on(equipment, volt = 0):
    equipment["ps"].set_volt(volt = volt, channel = PS_CH_VOLT)
    equipment["ps"].set_amp(amp = VOLT_CURRENT, channel = PS_CH_VOLT)
    equipment["ps"].on(channel = PS_CH_VOLT)

def voltage_off(equipment):
    equipment["ps"].set_volt(volt = 0, channel = PS_CH_VOLT)
    equipment["ps"].set_amp(amp = 0, channel = PS_CH_VOLT)
    equipment["ps"].off(channel = PS_CH_VOLT)

def voltage_set(equipment, volt):
    equipment["ps"].set_volt(volt = volt, channel = PS_CH_VOLT)

def amp_on(equipment, amp = 0):
    equipment["ps"].set_volt(volt = AMP_VOLTAGE, channel = PS_CH_AMP)
    equipment["ps"].set_amp(amp = amp, channel = PS_CH_AMP)
    equipment["ps"].on(channel = PS_CH_AMP)

def amp_off(equipment):
    equipment["ps"].set_volt(volt = 0, channel = PS_CH_AMP)
    equipment["ps"].set_amp(amp = 0, channel = PS_CH_AMP)
    equipment["ps"].off(channel = PS_CH_AMP)

def amp_set(equipment, amp):
    equipment["ps"].set_amp(amp = amp, channel = PS_CH_AMP)

def main():

    rm = pyvisa.ResourceManager()
    if not SIMULATION:
        debug(f"Measurement equipment: {rm.list_resources()}")

    # Power supply demonstration
    if not SIMULATION:
        ps = TTi_CPX200DP(rm, PS_INTERFACE, PS_ADDR, [80, 5])
        ps.reset()

    #Multimeter
    if not SIMULATION:
        dmm = Fluke_45(rm, DMM_INTERFACE, DMM_ADDR, False)
        #print(f"DC Voltage: {dmm.meas_volt_dc(1)}")
        #print(f"DC Voltage: {dmm.meas_volt_dc(10)}")
        #print(f"Number of measurements: {len(dmm.meas_volt_dc(1024))}")

    if not SIMULATION:
        dut = DUT(DUT_SERIALPORT)

    equipment = {"ps": ps, "dmm": dmm}

    rep_name = f"PS1-1A ADC Test {datetime.datetime.now().strftime('%Y-%m-%d %H.%M.%S')}"
    report_path = "PS1-1_ADC"
    dut_name = "PS1-1"
    dut_ver = "AC"

    if not SIMULATION:
        ############################
        # Test voltage measurement #
        ############################
        if TEST_ALL or TEST_VOLTAGE:
            if report_path != "":
                path_split = report_path.split("/")
                path_separator = ""
                path_check = ""
                for path_idx in range(len(path_split)):
                    path_check = f"{path_check}{path_separator}{path_split[path_idx]}"
                    path_separator = "/"
                    if not os.path.isdir(path_check):
                        os.mkdir(path_check)
                report_path = f"{report_path}/"
            # File export
            f = open(f"{report_path}{rep_name}_volt.csv", "w")
            print(f"Writing to: {report_path}{rep_name}_volt.csv")
            f.write(f"Time, Voltage Setpoint,Voltage DMM,Voltage DUT raw,Voltage DUT,Unit DUT\n")
            voltage_on(equipment = equipment, volt = 0)
            volt_setpoint = VOLT_MIN
            while volt_setpoint <= VOLT_MAX:
                print(f"Voltage: {volt_setpoint}V")
                dmm.set_range(volt_setpoint, "VDC", "medium")
                for loopcount in range(0, VOLT_REPEAT):
                    voltage_set(equipment = equipment, volt = volt_setpoint)
                    volt_dmm = dmm.meas_volt_dc()
                    [volt_dut_raw, volt_dut, volt_dut_unit] = dut.get_Ext_V_Term_raw_unit()
                    f.write(f"{datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')},{volt_setpoint},{volt_dmm},{volt_dut_raw},{volt_dut},{volt_dut_unit}\n")
                    #debug(f"Measurement: VOLTAGE: {volt_setpoint}, {volt_dmm}, {volt_dut_raw}, {volt_dut}, {volt_dut_unit}")
                volt_setpoint += VOLT_STEP
            voltage_off(equipment = equipment)
            f.close()

        ############################
        # Test current measurement #
        ############################
        if TEST_ALL or TEST_CURRENT:
            if report_path != "":
                path_split = report_path.split("/")
                path_separator = ""
                path_check = ""
                for path_idx in range(len(path_split)):
                    path_check = f"{path_check}{path_separator}{path_split[path_idx]}"
                    path_separator = "/"
                    if not os.path.isdir(path_check):
                        os.mkdir(path_check)
                report_path = f"{report_path}/"
            # File export
            f = open(f"{report_path}{rep_name}_amp.csv", "w")
            f.write(f"Time,Current Setpoint,Current DMM,Current DUT raw,Current DUT,Unit DUT\n")
            amp_on(equipment = equipment, amp = 0)
            amp_setpoint = AMP_MIN
            while amp_setpoint <= AMP_MAX:
                print(f"current: {amp_setpoint}A")
                dmm.set_range(amp_setpoint, "ADC", "medium")
                for loopcount in range(0, VOLT_REPEAT):
                    amp_set(equipment = equipment, amp = amp_setpoint)
                    amp_dmm = dmm.meas_amp_dc()
                    [amp_dut_raw, amp_dut, amp_dut_unit] = dut.get_Ext_I_Out_raw_unit()
                    f.write(f"{datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')},{amp_setpoint},{amp_dmm},{amp_dut_raw},{amp_dut},{amp_dut_unit}\n")
                    #debug(f"Measurement: CURRENT: {amp_setpoint}, {amp_dmm}, {amp_dut_raw}, {amp_dut}, {amp_dut_unit}")
                amp_setpoint += AMP_STEP
            amp_off(equipment = equipment)
            f.close()

        #################################
        # Test voltage sens measurement #
        #################################
        if TEST_ALL == False and TEST_SENS:
            if report_path != "":
                path_split = report_path.split("/")
                path_separator = ""
                path_check = ""
                for path_idx in range(len(path_split)):
                    path_check = f"{path_check}{path_separator}{path_split[path_idx]}"
                    path_separator = "/"
                    if not os.path.isdir(path_check):
                        os.mkdir(path_check)
                report_path = f"{report_path}/"
            # File export
            f = open(f"{report_path}{rep_name}_sens.csv", "w")
            print(f"Writing to: {report_path}{rep_name}_sens.csv")
            f.write(f"Time,Sens Setpoint,Voltage DMM,Sens DUT raw,Sens DUT,Sens Unit DUT\n")
            voltage_on(equipment = equipment, volt = 0)
            sens_setpoint = SENS_MIN
            while sens_setpoint <= SENS_MAX:
                print(f"Sens: {sens_setpoint}V")
                dmm.set_range(sens_setpoint, "VDC", "medium")
                for loopcount in range(0, SENS_REPEAT):
                    voltage_set(equipment = equipment, volt = sens_setpoint)
                    sens_dmm = dmm.meas_volt_dc()
                    [sens_dut_raw, sens_dut, sens_dut_unit] = dut.get_Ext_V_Sns_raw_unit()
                    f.write(f"{datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')},{sens_setpoint},{sens_dmm},{sens_dut_raw},{sens_dut},{sens_dut_unit}\n")
                    #debug(f"Measurement: SENS: {sens_setpoint}, {sens_dmm}, {sens_dut_raw}, {sens_dut}, {sens_dut_unit}")
                sens_setpoint += SENS_STEP
            voltage_off(equipment = equipment)
            f.close()


        ###################
        # Stop everything #
        ###################
        voltage_off(equipment = equipment)
        amp_off(equipment = equipment)

if __name__ == "__main__":
    main()
