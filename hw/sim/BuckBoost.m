clc
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

fsw = 300e3;

kind = 0.2; %20-40% of the output current
eta = 0.8;

Vout_ripcoef = 0.01
vout_overshoot = 0.01*Vout_volt

%% Buck boost calculations
Dbuck = Vout_curr./(Vin_range.*eta);
Dboost = 1-(Vin_range.*eta)./Vout_volt;


Lbuck_range = Vout_curr.*(Vin_range-Vout_curr)./(kind.*fsw.*Vin_range.*Iout_curr);
Lboost_range = Vin_range.^2 .*(Vout_volt-Vin_range)./(fsw.*kind.*Iout_volt.*Vout_volt.^2);

Lbuck = max(Lbuck_range)
Lboost = max(Lboost_range)

% Todo: Select here correct L
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

disp("Minimum required output capacitance by voltage/current ripple")
fprintf("Current Mode: \t%.2f uF\n", Coutmin_buck*1e6);
fprintf("Voltage Mode: \t%.2f uF\n", Coutmin_boost*1e6);


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

