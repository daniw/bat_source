clc
clearvars
clear all
%% Requirements
Vin_min = 8;
Vin_max = 18;
Vin = [Vin_min,Vin_max];
Vin_range = (Vin_min:0.5:Vin_max);

Vout_curr = 5;
Vout_volt = 60;
Vout = [Vout_curr,Vout_volt];

Iout_curr = 10;
Iout_volt = 1;
Iout = [Iout_curr, Iout_volt];

fprintf("\n\n========= Operating Points ========\n");
fprintf("Input Voltage range: %.1f V - %.1f V\n", Vin);
fprintf("Output Voltages: %.1f V\t%.1f V\n", Vout);
fprintf("Output Currents: %.1f A\t%.1f A\n", Iout);

Iin_max = 9;

fsw = 1000e3;
fctrl = 10e3;

kind = 0.4; %20-40% of the output current
eta = 0.8;

Vout_ripcoef = 0.01
Vin_ripcoef = 0.01
vout_overshoot = 0.01*Vout_volt

%% Buck boost calculations
Dbuck = Vout_curr./(Vin_range.*eta);
Dboost = 1-(Vin_range.*eta)./Vout_volt;


Lbuck_range = Vout_curr.*(Vin_range-Vout_curr)./(kind.*fsw.*Vin_range.*Iout_curr);
Lboost_range = Vin_range.^2 .*(Vout_volt-Vin_range)./(fsw.*kind.*Iout_volt.*Vout_volt.^2);

Lbuck = max(Lbuck_range)
Lboost = max(Lboost_range)


L_ideal = max(Lboost, Lbuck)
L = e_series(L_ideal, 'e6', 'up')

Isw_buck = (Vin_range-Vout_curr).*Dbuck./(2*fsw*L)+Iout_curr;
Isw_boost = (Vin_range.*Dboost)./(fsw*L*2)+Iout_volt./(1-Dboost);
Iswmax_buck = max(Isw_buck);
Iswmax_boost = max(Isw_boost);

Iswmax = max(Iswmax_boost, Iswmax_buck);

fprintf("Minimum Inductor Values for Fsw = %.2f kHz\n", fsw/1e3);
fprintf("Current Mode: \t%.2f uH \t %.2f A\n", Lbuck*1e6 , Iswmax_buck);
fprintf("Voltage Mode: \t%.2f uH \t %.2f A\n", Lboost*1e6, Iswmax_boost);
fprintf("Both Modes: \t%.2f uH \t %.2f A\n", L*1e6, Iswmax);


% Output capcitor calculation
Coutmin_buck1 = kind.*Iout_curr./(8.*fsw.*Vout_ripcoef*Vout_curr);
Coutmin_buck2 = (kind.*Iout_curr).^2.*L./(2.*Vout_curr.*vout_overshoot);
Coutmin_buck = max(Coutmin_buck1, Coutmin_buck2);

Coutmin_boost= max(Iout_volt.*Dboost./(fsw.*Vout_ripcoef.*Vout_volt));
Cout = e_series(max(Coutmin_buck, Coutmin_boost), 'e6', 'up');

disp("Minimum required output capacitance by voltage/current ripple")
fprintf("Current Mode: \t%.2f uF\n", Coutmin_buck*1e6);
fprintf("Voltage Mode: \t%.2f uF\n", Coutmin_boost*1e6);


% Input capcitor calculation
Cinmin_buck = max(Dbuck .* (1-Dbuck) * Iout_curr./ (Vin_ripcoef .*Vin_range*fsw));

Cinmin_boost =0;

Cin = e_series(max(Cinmin_buck, Cinmin_boost), 'e6', 'up');

disp("Minimum required input capacitance by voltage/current ripple")
fprintf("Current Mode: \t%.2f uF\n", Cinmin_buck*1e6);
fprintf("Voltage Mode: \t%.2f uF\n", Cinmin_boost*1e6);

% Switching Losses Calculation



% Plots
figure(1);
plot(Vin_range, Dboost, Vin_range, Dbuck);
ylim([0 1]);
grid on;
grid minor on;
title('Duty cycle');
xlabel('Input voltage [V]');
ylabel('Duty [1]');
legend(['Boost (60V, 1A)'; 'Buck (10A, 5V)']);

figure(2);
plot(Vin_range, Isw_boost, Vin_range, Isw_buck);
%ylim([0 1]);
grid on;
grid minor on;
title('Switch current');
xlabel('Input voltage [V]');
ylabel('Isw [A]');
legend(['Boost (60V, 1A)'; 'Buck (10A, 5V)']);


%% Plecs Simulation

% Resistances for Load Modulation
R_low  = [1e6, 0.1, 1e6];
R_high = [ 60, 0.5, 1e6];

plecspars = {};

buckboost_model = 'buck_boost_converters_stm';
proxy = jsonrpc('http://localhost:1080','Timeout',700);

proxy.plecs.load(sprintf('E:/Repos/bat_source/hw/sim/PLECS/%s.plecs',buckboost_model ));

% 1 = Voltage Mode
% 2 = Current Mode
% 3 = Charge
for mode = 1:3
   for cases = 1:2
      plecspars{cases}.vin = Vin(cases);
      plecspars{cases}.R_low  = R_low(mode);
      plecspars{cases}.R_high = R_high(mode);
      plecspars{cases}.fsw = fsw;
      plecspars{cases}.L = L;
      plecspars{cases}.cout = Cout;
      plecspars{cases}.cin = Cin;
      plecspars{cases}.adc_skip = fsw/fctrl;
      plecspars{cases}.mode = mode;
   end

% Load is not needed, of Model is already opened.
pause(5);
temp = proxy.plecs.simulate(buckboost_model,struct('ModelVars',plecspars));
plecsres{2*mode -1 } = temp(1);
plecsres{2*mode}     = temp(2);
end
%% Plot Plecspars
figure(3)
clf;
ax = [];
for i = 1:6
  subplot(2,3,i);
  ax1 = plotyy(plecsres(i).Time, plecsres(i).Values(1:3,:),plecsres(i).Time, plecsres(i).Values(4:5,:));
  xlabel("Time [s]")
  ylabel(ax1(1), "Current [A]")
  ylabel(ax1(2), "Voltage [V]")
  legend("I_L", "I_{bat}", "I_{out}", "V_{in}" , "V_{out}");
  ax = [ax gca];
end

linkaxes(ax, "x");

