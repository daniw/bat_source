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
rel_err_range_amp = 1;

filename = 'dat00001.csv';
inputs = { % File name                          Title                        Plot DMM data rounded
'PS1-1A ADC Test 2025-10-30 23.39.19_volt.csv', 'Initial testing (voltage)', 0;
'PS1-1A ADC Test 2025-10-30 23.39.19_amp.csv' , 'Initial testing (curent)' , 0;
'PS1-1A ADC Test 2025-10-30 23.48.31_volt.csv', '0V .. 45V, interrupted'   , 1;
};

for i = 1:size(inputs)(1)
  filename = inputs{i,1};
  pathname = [data_dir '/' filename];
  titlestring = inputs{i,2};
  plot_dmm_rounded = inputs{i,3};

  data = dlmread(pathname, SEPARATOR, 1, 0);

  fid = fopen(pathname, 'r');
  raw = textscan(fid, '%s,%f,%f,%f,%s', 'delimiter', SEPARATOR, 'HeaderLines', 1);
  %fclose(fid);
  sweep = raw{1};
  sampletime = datenum(raw{1}, 'yyyy-mm-dd HH:MM:SS')*24*3600;

  dmm_value = raw{2};
  dut_value_raw = raw{3};
  dut_value = raw{4} ./ 1e3;

  if cell2mat(raw{5}(1)) == 'mV'
    value = "voltage";
    unit = "V";
    abs_err_range = abs_err_range_volt;
    rel_err_range = rel_err_range_volt;
  elseif cell2mat(raw{5}(1)) == "mA"
    value = "current";
    unit = "A";
    abs_err_range = abs_err_range_amp;
    rel_err_range = rel_err_range_amp;
  elseif cell2mat(raw{5}(1)) == "mA"
  else
    error(["Unknown unit: " raw{5}(1)]);
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

  if plot_dmm_rounded
    figure(i*FIGCOUNT+3);
    plot(round(dmm_value), dut_value);
    grid on;
    grid minor on;
    title([titlestring ", DMM rounded"]);
    xlabel(["DMM " value " [" unit "]"]);
    ylabel(["DUT " value " [" unit "]"]);

    figure(i*FIGCOUNT+4);
    plot(round(dmm_value), (dut_value-round(dmm_value)).*1e3);
    ylim([-abs_err_range abs_err_range]);
    grid on;
    grid minor on;
    title([titlestring " - absolute error" ", DMM rounded"]);
    xlabel(["DMM " value " [" unit "]"]);
    ylabel(["Absolute error [m" unit "]"]);

    figure(i*FIGCOUNT+5);
    plot(round(dmm_value), (dut_value./round(dmm_value) - 1).*1e2);
    ylim([-rel_err_range rel_err_range]);
    grid on;
    grid minor on;
    title([titlestring " - relative error" ", DMM rounded"]);
    xlabel(["DMM " value " [" unit "]"]);
    ylabel(["Relative error [%]"]);
  end;

end;
