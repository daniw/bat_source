%% Script for analyzing the performance of the ADC

% Workspace cleanup
clc;
clear;
%close all;

data_dir = 'PS1-1_ADC';
SEPARATOR = ',';
LW = 2;
plotting = true;
FIGCOUNT = 10;
abs_err_range_volt = 50;
rel_err_range_volt = 1;
abs_err_range_amp = 300;
rel_err_range_amp = 2;
abs_err_range_sens = 200;
rel_err_range_sens = 5;
SENS_CORR_FACTOR = 6

filename = 'dat00001.csv';
inputs = { % File name                           Title                            Measuremnet Plot DMM data rounded
'PS1-1A ADC Test 2025-10-30 23.39.19_volt.csv' , 'Initial testing (voltage)'    , 'Volt'    , 0;
'PS1-1A ADC Test 2025-10-30 23.39.19_amp.csv'  , 'Initial testing (curent)'     , 'Amp'     , 0;
'PS1-1A ADC Test 2025-10-30 23.48.31_volt.csv' , 'Term: 0V .. 45V, interrupted' , 'Volt'    , 1;
'PS1-1A ADC Test 2025-10-31 18.40.21_volt.csv' , 'Term: 0V .. 60V'              , 'Volt'    , 1;
'PS1-1A ADC Test 2025-10-31 18.40.21_amp.csv'  , 'I_Out: 0A .. 10A'             , 'Amp'     , 1;
'PS1-1A ADC Test 2025-11-01 09.48.31_sens.csv' , 'Sens: 0V .. 5V, quick test'   , 'Sens'    , 1;
'PS1-1A ADC Test 2025-11-01 09.57.34_sens.csv' , 'Sens: 0V .. 5V'               , 'Sens'    , 1;
};

for i = 1:size(inputs)(1)
  filename = inputs{i,1};
  pathname = [data_dir '/' filename];
  titlestring = inputs{i,2};
  meas_type = inputs{i, 3};
  plot_setpoint = inputs{i,4};

  data = dlmread(pathname, SEPARATOR, 1, 0);

  fid = fopen(pathname, 'r');
  raw = textscan(fid, '%s,%f,%f,%f,%f,%s', 'delimiter', SEPARATOR, 'HeaderLines', 1);
  %fclose(fid);
  sweep = raw{1};
  sampletime = datenum(raw{1}, 'yyyy-mm-dd HH:MM:SS')*24*3600;

  setpoint = raw{2};
  dmm_value = raw{3};
  dut_value_raw = raw{4};

  if strcmp(meas_type, 'Volt')
    dut_value = raw{5} ./ 1e3;
    value = "voltage";
    unit = "V";
    abs_err_range = abs_err_range_volt;
    rel_err_range = rel_err_range_volt;
  elseif strcmp(meas_type, 'Amp')
    dut_value = raw{5} ./ 1e3;
    value = "current";
    unit = "A";
    abs_err_range = abs_err_range_amp;
    rel_err_range = rel_err_range_amp;
  elseif strcmp(meas_type, 'Sens')
    dut_value = raw{5} ./ 1e6 .* SENS_CORR_FACTOR;
    value = "Sens";
    unit = "V";
    abs_err_range = abs_err_range_sens;
    rel_err_range = rel_err_range_sens;
  else
    error(["Unknown measurement type: " meas_type]);
  end;

  figure(i*FIGCOUNT+0);
  plot(dmm_value, dut_value);
  grid on;
  grid minor on;
  title([titlestring]);
  xlabel(["DMM " value " [" unit "]"]);
  ylabel(["DUT " value " [" unit "]"]);

  figure(i*FIGCOUNT+1);
  plot(dmm_value, (dut_value-dmm_value).*1e3);
  ylim([-abs_err_range abs_err_range]);
  grid on;
  grid minor on;
  title([titlestring " - absolute error"]);
  xlabel(["DMM " value " [" unit "]"]);
  ylabel(["Absolute error [m" unit "]"]);

  figure(i*FIGCOUNT+2);
  plot(dmm_value, (dut_value./dmm_value - 1).*1e2);
  ylim([-rel_err_range rel_err_range]);
  grid on;
  grid minor on;
  title([titlestring " - relative error"]);
  xlabel(["DMM " value " [" unit "]"]);
  ylabel(["Relative error [%]"]);

  if plot_setpoint
    figure(i*FIGCOUNT+3);
    plot(setpoint, dut_value);
    grid on;
    grid minor on;
    title([titlestring ", DMM rounded"]);
    xlabel(["Setpoint " value " [" unit "]"]);
    ylabel(["DUT " value " [" unit "]"]);

    figure(i*FIGCOUNT+4);
    plot(setpoint, (dut_value-setpoint).*1e3);
    ylim([-abs_err_range abs_err_range]);
    grid on;
    grid minor on;
    title([titlestring " - absolute error" ", DMM rounded"]);
    xlabel(["Setpoint " value " [" unit "]"]);
    ylabel(["Absolute error [m" unit "]"]);

    figure(i*FIGCOUNT+5);
    plot(setpoint, (dut_value./setpoint - 1).*1e2);
    ylim([-rel_err_range rel_err_range]);
    grid on;
    grid minor on;
    title([titlestring " - relative error" ", DMM rounded"]);
    xlabel(["Setpoint " value " [" unit "]"]);
    ylabel(["Relative error [%]"]);
  end;

end;
