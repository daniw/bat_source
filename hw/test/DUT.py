import serial
import time

class DUT:
    """ Class to communicate with the device under test """

    def write_read(self, command, delay = -1):
        if self.simulation == False:
            for ch in command:
                self.ser.write(ch.encode())
                time.sleep(self.DELAY_CHAR)
            self.ser.write(b'\n')
            if delay >= 0:
                time.sleep(delay)
            else:
                time.sleep(self.DELAY_WRITE_READ)
            response = self.ser.read_all().decode()
        return response

    # Read data from ADC
    def read_ADC(self):
        if self.simulation == False:
            # TODO: Read actual serial data
            self.ser.write(b'p')
            time.sleep(self.DELAY_CHAR)
            self.ser.write(b'A')
            time.sleep(self.DELAY_CHAR)
            self.ser.write(b'D')
            time.sleep(self.DELAY_CHAR)
            self.ser.write(b'C')
            time.sleep(self.DELAY_CHAR)
            self.ser.write(b'\n')
            time.sleep(self.DELAY_WRITE_READ)
            self.adc_data = self.ser.read_all().decode()
        else:
            self.adc_data = ""
            self.adc_data = self.adc_data + "pADC\n"
            self.adc_data = self.adc_data + "ADC Measurements:\n"
            self.adc_data = self.adc_data + "  v_in         (ADC1_IN1)  : 0 \t: 0 mV\n"
            self.adc_data = self.adc_data + "  v_hv         (ADC1_IN2)  : 39 \t: 0 mV\n"
            self.adc_data = self.adc_data + "  i_bat        (ADC1_IN3)  : 1996 \t: 30920 mA\n"
            self.adc_data = self.adc_data + "  v_out        (ADC1_IN4)  : 391 \t: 2096 mV\n"
            self.adc_data = self.adc_data + "  v_term       (ADC1_IN13) : 2 \t: 0 mV\n"
            self.adc_data = self.adc_data + "  temp_int     (ADC1_INT)  : 640 \t: 0 °C\n"
            self.adc_data = self.adc_data + "  i_out        (ADC2_IN5)  : 1743 \t: 0 mA\n"
            self.adc_data = self.adc_data + "  temp_prim    (ADC2_IN8)  : 10 \t: 0 °C\n"
            self.adc_data = self.adc_data + "  temp_current (ADC2_IN11) : 5 \t: 0 °C\n"
            self.adc_data = self.adc_data + "  temp_sec     (ADC2_IN12) : 10 \t: 0 °C\n"
            self.adc_data = self.adc_data + "  i_iso        (ADC2_IN13) : 1251 \t: 0 mA\n"
            self.adc_data = self.adc_data + "  temp_trafo   (ADC2_IN15) : 1596 \t: 0 °C\n"
            self.adc_data = self.adc_data + "  Ext V_Term : 24 : 3 mV\n"
            self.adc_data = self.adc_data + "  Ext V_Sns  : 2641001 : 1888909 mV\n"
            self.adc_data = self.adc_data + "  Ext I_Iso  : -20 : 0 mA\n"
            self.adc_data = self.adc_data + "  Ext I_Out  : 10493 : -19 mA\n"
        #print(self.adc_data)
        if self.parse_v_in == True:
            [self.v_in_raw, self.v_in, self.v_in_unit] = self.extract_measurement("v_in")
        if self.parse_v_hv == True:
            [self.v_hv_raw, self.v_hv, self.v_hv_unit] = self.extract_measurement("v_hv")
        if self.parse_i_bat == True:
            [self.i_bat_raw, self.i_bat, self.i_bat_unit] = self.extract_measurement("i_bat")
        if self.parse_v_out == True:
            [self.v_out_raw, self.v_out, self.v_out_unit] = self.extract_measurement("v_out")
        if self.parse_v_term == True:
            [self.v_term_raw, self.v_term, self.v_term_unit] = self.extract_measurement("v_term")
        if self.parse_temp_int == True:
            [self.temp_int_raw, self.temp_int, self.temp_int_unit] = self.extract_measurement("temp_int")
        if self.parse_i_out == True:
            [self.i_out_raw, self.i_out, self.i_out_unit] = self.extract_measurement("i_out")
        if self.parse_temp_prim == True:
            [self.temp_prim_raw, self.temp_prim, self.temp_prim_unit] = self.extract_measurement("temp_prim")
        if self.parse_temp_current == True:
            [self.temp_current_raw, self.temp_current, self.temp_current_unit] = self.extract_measurement("temp_current")
        if self.parse_temp_sec == True:
            [self.temp_sec_raw, self.temp_sec, self.temp_sec_unit] = self.extract_measurement("temp_sec")
        if self.parse_i_iso == True:
            [self.i_iso_raw, self.i_iso, self.i_iso_unit] = self.extract_measurement("i_iso")
        if self.parse_temp_trafo == True:
            [self.temp_trafo_raw, self.temp_trafo, self.temp_trafo_unit] = self.extract_measurement("temp_trafo")
        if self.parse_Ext_V_Term == True:
            [self.Ext_V_Term_raw, self.Ext_V_Term, self.Ext_V_Term_unit] = self.extract_measurement("Ext V_Term")
        if self.parse_Ext_V_Sns == True:
            [self.Ext_V_Sns_raw, self.Ext_V_Sns, self.Ext_V_Sns_unit] = self.extract_measurement("Ext V_Sns")
        if self.parse_Ext_I_Iso == True:
            [self.Ext_I_Iso_raw, self.Ext_I_Iso, self.Ext_I_Iso_unit] = self.extract_measurement("Ext I_Iso")
        if self.parse_Ext_I_Out == True:
            [self.Ext_I_Out_raw, self.Ext_I_Out, self.Ext_I_Out_unit] = self.extract_measurement("Ext I_Out")

    #Function to extract measurement values from the string sent by the DUT
    def extract_measurement(self, value):
        adc_lines = self.adc_data.split("\n")
        found = -1
        index = 1
        while (found < 0) and (index < len(adc_lines)-2):
            index = index + 1
            found = adc_lines[index].find(value)
        if found < 0:
            print(f"Error: Value {value} not found in ADC data")
            return [0, 0, "none"]
        else:
            adc_line = adc_lines[index]
            adc_values = adc_line.split(": ")
            adc_raw = int(adc_values[1])
            adc_value_unit = adc_values[2].split(" ")
            adc_value = int(adc_value_unit[0])
            adc_unit = adc_value_unit[1]
            #print(f"{value}: {adc_raw}, {adc_value}{adc_unit}")
            return [adc_raw, adc_value, adc_unit]

    # Get v_in_raw
    # param adc_read Read data from ADC before returning value
    def get_v_in_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.v_in_raw

    # Get v_in
    # param adc_read Read data from ADC before returning value
    def get_v_in(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.v_in

    # Get v_in_unit
    def get_v_in_unit(self):
        return self.v_in_unit

    # Get v_in_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_v_in_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.v_in_raw, self.v_in, self.v_in_unit]

    # Get v_hv_raw
    # param adc_read Read data from ADC before returning value
    def get_v_hv_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.v_hv_raw

    # Get v_hv
    # param adc_read Read data from ADC before returning value
    def get_v_hv(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.v_hv

    # Get v_hv_unit
    def get_v_hv_unit(self):
        return self.v_hv_unit

    # Get v_hv_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_v_hv_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.v_hv_raw, self.v_hv, self.v_hv_unit]

    # Get i_bat_raw
    # param adc_read Read data from ADC before returning value
    def get_i_bat_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.i_bat_raw

    # Get i_bat
    # param adc_read Read data from ADC before returning value
    def get_i_bat(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.i_bat

    # Get i_bat_unit
    def get_i_bat_unit(self):
        return self.i_bat_unit

    # Get i_bat_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_i_bat_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.i_bat_raw, self.i_bat, self.i_bat_unit]

    # Get v_out_raw
    # param adc_read Read data from ADC before returning value
    def get_v_out_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.v_out_raw

    # Get v_out
    # param adc_read Read data from ADC before returning value
    def get_v_out(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.v_out

    # Get v_out_unit
    def get_v_out_unit(self):
        return self.v_out_unit

    # Get v_out_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_v_out_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.v_out_raw, self.v_out, self.v_out_unit]

    # Get v_term_raw
    # param adc_read Read data from ADC before returning value
    def get_v_term_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.v_term_raw

    # Get v_term
    # param adc_read Read data from ADC before returning value
    def get_v_term(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.v_term

    # Get v_term_unit
    def get_v_term_unit(self):
        return self.v_term_unit

    # Get v_term_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_v_term_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.v_term_raw, self.v_term, self.v_term_unit]

    # Get temp_int_raw
    # param adc_read Read data from ADC before returning value
    def get_temp_int_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_int_raw

    # Get temp_int
    # param adc_read Read data from ADC before returning value
    def get_temp_int(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_int

    # Get temp_int_unit
    def get_temp_int_unit(self):
        return self.temp_int_unit

    # Get temp_int_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_temp_int_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.temp_int_raw, self.temp_int, self.temp_int_unit]

    # Get i_out_raw
    # param adc_read Read data from ADC before returning value
    def get_i_out_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.i_out_raw

    # Get i_out
    # param adc_read Read data from ADC before returning value
    def get_i_out(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.i_out

    # Get i_out_unit
    def get_i_out_unit(self):
        return self.i_out_unit

    # Get i_out_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_i_out_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.i_out_raw, self.i_out, self.i_out_unit]

    # Get temp_prim_raw
    # param adc_read Read data from ADC before returning value
    def get_temp_prim_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_prim_raw

    # Get temp_prim
    # param adc_read Read data from ADC before returning value
    def get_temp_prim(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_prim

    # Get temp_prim_unit
    def get_temp_prim_unit(self):
        return self.temp_prim_unit

    # Get temp_prim_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_temp_prim_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.temp_prim_raw, self.temp_prim, self.temp_prim_unit]

    # Get temp_current_raw
    # param adc_read Read data from ADC before returning value
    def get_temp_current_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_current_raw

    # Get temp_current
    # param adc_read Read data from ADC before returning value
    def get_temp_current(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_current

    # Get temp_current_unit
    def get_temp_current_unit(self):
        return self.temp_current_unit

    # Get temp_current_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_temp_current_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.temp_current_raw, self.temp_current, self.temp_current_unit]

    # Get temp_sec_raw
    # param adc_read Read data from ADC before returning value
    def get_temp_sec_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_sec_raw

    # Get temp_sec
    # param adc_read Read data from ADC before returning value
    def get_temp_sec(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_sec

    # Get temp_sec_unit
    def get_temp_sec_unit(self):
        return self.temp_sec_unit

    # Get temp_sec_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_temp_sec_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.temp_sec_raw, self.temp_sec, self.temp_sec_unit]

    # Get i_iso_raw
    # param adc_read Read data from ADC before returning value
    def get_i_iso_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.i_iso_raw

    # Get i_iso
    # param adc_read Read data from ADC before returning value
    def get_i_iso(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.i_iso

    # Get i_iso_unit
    def get_i_iso_unit(self):
        return self.i_iso_unit

    # Get i_iso_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_i_iso_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.i_iso_raw, self.i_iso, self.i_iso_unit]

    # Get temp_trafo_raw
    # param adc_read Read data from ADC before returning value
    def get_temp_trafo_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_trafo_raw

    # Get temp_trafo
    # param adc_read Read data from ADC before returning value
    def get_temp_trafo(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.temp_trafo

    # Get temp_trafo_unit
    def get_temp_trafo_unit(self):
        return self.temp_trafo_unit

    # Get temp_trafo_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_temp_trafo_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.temp_trafo_raw, self.temp_trafo, self.temp_trafo_unit]

    # Get Ext_V_Term_raw
    # param adc_read Read data from ADC before returning value
    def get_Ext_V_Term_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.Ext_V_Term_raw

    # Get Ext_V_Term
    # param adc_read Read data from ADC before returning value
    def get_Ext_V_Term(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.Ext_V_Term

    # Get Ext_V_Term_unit
    def get_Ext_V_Term_unit(self):
        return self.Ext_V_Term_unit

    # Get Ext_V_Term_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_Ext_V_Term_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.Ext_V_Term_raw, self.Ext_V_Term, self.Ext_V_Term_unit]

    # Get Ext_V_Sns_raw
    # param adc_read Read data from ADC before returning value
    def get_Ext_V_Sns_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.Ext_V_Sns_raw

    # Get Ext_V_Sns
    # param adc_read Read data from ADC before returning value
    def get_Ext_V_Sns(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.Ext_V_Sns

    # Get Ext_V_Sns_unit
    def get_Ext_V_Sns_unit(self):
        return self.Ext_V_Sns_unit

    # Get Ext_V_Sns_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_Ext_V_Sns_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.Ext_V_Sns_raw, self.Ext_V_Sns, self.Ext_V_Sns_unit]

    # Get Ext_I_Iso_raw
    # param adc_read Read data from ADC before returning value
    def get_Ext_I_Iso_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.Ext_I_Iso_raw

    # Get Ext_I_Iso
    # param adc_read Read data from ADC before returning value
    def get_Ext_I_Iso(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.Ext_I_Iso

    # Get Ext_I_Iso_unit
    def get_Ext_I_Iso_unit(self):
        return self.Ext_I_Iso_unit

    # Get Ext_I_Iso_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_Ext_I_Iso_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.Ext_I_Iso_raw, self.Ext_I_Iso, self.Ext_I_Iso_unit]

    # Get Ext_I_Out_raw
    # param adc_read Read data from ADC before returning value
    def get_Ext_I_Out_raw(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.Ext_I_Out_raw

    # Get Ext_I_Out
    # param adc_read Read data from ADC before returning value
    def get_Ext_I_Out(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return self.Ext_I_Out

    # Get Ext_I_Out_unit
    def get_Ext_I_Out_unit(self):
        return self.Ext_I_Out_unit

    # Get Ext_I_Out_raw_unit
    # param adc_read Read data from ADC before returning value
    def get_Ext_I_Out_raw_unit(self, adc_read = True):
        if adc_read == True:
            self.read_ADC()
        return [self.Ext_I_Out_raw, self.Ext_I_Out, self.Ext_I_Out_unit]

    # Initialize current controller
    def init_current_controller(self, current = 0):
        self.write_read(f"setDAC 1 0")   # Set current => 0A
        self.write_read(f"setGPIO 3 0")  # Shunt Iso
        self.write_read(f"setGPIO 5 0")  # 1A disable
        if current > 0:
            self.set_current_controller(current)

    # Set current controller set value
    def set_current_controller(self, current = 0):
        if current >= 0 and current <= 1:
            dac_val = int(current * 1241)
        elif current > 1:
            dac_val = int(1 * 1241)
        else:
            dac_val = 0
            print(f"Warining, invalid current selected")
        self.write_read(f"setDAC 1 {dac_val}")       # Set current => 0A

    # Deinitialize current controller
    def deinit_current_controller(self):
        self.write_read(f"setDAC 1 0")       # Set current => 0A
        self.write_read(f"setGPIO 3 0")      # Shunt Iso
        self.write_read(f"setGPIO 5 1")      # 1A disable
        self.write_read(f"setDAC 1 1024")    # Set current => 1A

    # Get serialport of the DUT
    def get_serialport(self):
        return self.serialport

    # Set serial port and connect to DUT
    def set_serialport(self, serialport):
        if self.simulation == False:
            self.ser = serial.Serial(serialport, 921600)
            self.ser.reset_input_buffer()

    def __init__(self, serialport = "", simulation = False, parse_v_in = True, parse_v_hv = True, parse_i_bat = True, parse_v_out = True, parse_v_term = True, parse_temp_int = True, parse_i_out = True, parse_temp_prim = True, parse_temp_current = True, parse_temp_sec = True, parse_i_iso = True, parse_temp_trafo = True, parse_Ext_V_Term = True, parse_Ext_V_Sns = True, parse_Ext_I_Iso = True, parse_Ext_I_Out = True):
        # Variables
        self.adc_data = ""
        self.simulation = simulation
        self.parse_v_in = parse_v_in 
        self.parse_v_hv = parse_v_hv 
        self.parse_i_bat = parse_i_bat 
        self.parse_v_out = parse_v_out 
        self.parse_v_term = parse_v_term 
        self.parse_temp_int = parse_temp_int 
        self.parse_i_out = parse_i_out 
        self.parse_temp_prim = parse_temp_prim 
        self.parse_temp_current = parse_temp_current 
        self.parse_temp_sec = parse_temp_sec 
        self.parse_i_iso = parse_i_iso 
        self.parse_temp_trafo = parse_temp_trafo 
        self.parse_Ext_V_Term = parse_Ext_V_Term
        self.parse_Ext_V_Sns = parse_Ext_V_Sns 
        self.parse_Ext_I_Iso = parse_Ext_I_Iso 
        self.parse_Ext_I_Out = parse_Ext_I_Out 
        self.DELAY_CHAR = 0.1
        self.DELAY_WRITE_READ = 0.3
        self.set_serialport(serialport)
        self.read_ADC()

    if __name__ == "__main__":
        __init__(self)

