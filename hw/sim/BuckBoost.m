clc
%% Requirements
Vin_min = 10;
Vin_max = 18;
Vin = [Vin_min,Vin_max];

Vout_curr = 5;
Vout_volt = 60;
Vout = [Vout_curr,Vout_volt];

Iout_curr = 10;
Iout_volt = 1;
Iout = [Iout_curr, Iout_volt];

fprintf("\n\n========= Operating Points ========\n");
fprintf("Input Voltage range: %.1f V - %.1f V\n", Vin);
fprintf("Output Voltages: %.1f V\t%.1f VV\n", Vout);
fprintf("Output Currents: %.1f A\t%.1f A\n", Iout);

Iin_max = 10;

fsw = 300e3;

kind = 0.2; %20-40% of the output current
eta = 0.8;

Vout_ripcoef = 0.01
vout_overshoot = 0.01*Vout_volt

%% Buck boost calculations
Dbuck = Vout_curr/(Vin_max*eta)
Dboost = 1-(Vin_min*eta)/Vout_volt


Lbuck = Vout_curr.*(Vin_max-Vout_curr)./(kind.*fsw.*Vin_max.*Iout_curr)
Lboost = Vin_min.^2 .*(Vout_volt-Vin_min)./(fsw.*kind.*Iout_volt.*Vout_volt.^2)

% Todo: Select here correct L
L = max(Lboost, Lbuck);


Iswmax_buck = (Vin_max-Vout_curr).*Dbuck./(2*fsw*L)+Iout_curr;
Iswmax_boost = (Vin_min.*Dboost)./(fsw*L*2)+Iout_volt./(1-Dboost);

Iswmax = max(Iswmax_boost, Iswmax_buck);

fprintf("Minimum Inductor Values for Fsw = %.2f kHz\n", fsw/1e3);
fprintf("Current Mode: \t%.2f uF \t %.2f A\n", Lbuck*1e6 , Iswmax_buck);
fprintf("Voltage Mode: \t%.2f uF \t %.2f A\n", Lboost*1e6, Iswmax_boost);
fprintf("Both Modes: \t%.2f uF \t %.2f A\n", L*1e6, Iswmax);




% Output capcitor calculation
Coutmin_buck1 = kind.*Iout_curr./(8.*fsw.*Vout_ripcoef*Vout_curr);
Coutmin_buck2 = (kind.*Iout_curr).^2.*L./(2.*Vout_curr.*vout_overshoot);
Coutmin_buck = max(Coutmin_buck1, Coutmin_buck2);


Coutmin_boost= Iout_volt.*Dboost./(fsw.*Vout_ripcoef.*Vout_volt);

disp("Minimum required output capacitance by voltage/current ripple")
fprintf("Current Mode: \t%.2f uF\n", Coutmin_buck*1e6);
fprintf("Voltage Mode: \t%.2f uF\n", Coutmin_boost*1e6);



