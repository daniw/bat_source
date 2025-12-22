clc
%% Requirements
Vin_min = 10;
Vin_max = 18;
Vin = [Vin_min,Vin_max];

Vout_iso = [250, 500, 1000];
Vout=Vout_iso;
Iout_iso = 0.002;
Iout = [Iout_iso];

fprintf("\n\n========= Operating Points ========\n");
fprintf("Input Voltage range: %.1f V - %.1f V\n", Vin);
fprintf("Output Voltages: %.0f V\t%.0f V\t%.0f V\n", Vout);
fprintf("Output Current: %.1f mA\n", Iout*1000);

Vripout = 0.5e-2 * Vout;
Vripin = 5e-2 * Vin_min;
Iripout = 0.5e-2 * Iout;
fbw = 1e3;

Iin_max = 10;

%% Parameter to influence the design
% Currently only BCM is supported
fsw = 350e3;
Dnom = 0.7;
nuest = 0.9;  % Estimated converter efficiency

N_prim = 8;
N_iso = 3*100;
N=N_prim./[N_iso];

fprintf("Transformer Ratios %i:%i\n", N_prim, N_iso)



% Sensing
Asns = 20; % Current Sense amplifier gain
Rsns_prim = 0.010;
Rsns_sek = 0.010;
Vsns_max = 3.3; % Complete Voltage range of ADC (eg. -1.65..1.65V or 0..3.3V)
ENOB_Adc = 10.4; % Effective number of bits of ADC

% THT GT105N10F TO220
Rds_prim = 0.0105;
Qdrv_prim = 10e-9; % nC
Idrv_prim = 2;
Qtot_prim = 54e-9;

% Only for High voltage diode
%US1NWF-7
Vf_sek = 1.35;

%% Winding properties based on Transformer RM5

% Winding length per turn
l_wind_prim =  8e-3*pi;
l_wind_sec =  8e-3*pi;

% fill factor
k_prim = 0.6;
k_sec = 0.5;

% RM5
h_winding_window_prim = 2.425e-3;
h_winding_window_sec  = 2.425e-3;
w_winding_window_prim = 2e-3;
w_winding_window_sec  = 2e-3;

winding_diameter_prim = 2*sqrt(h_winding_window_prim * w_winding_window_prim * k_prim / N_prim / pi);
winding_diameter_sec  = 2*sqrt(h_winding_window_sec  * w_winding_window_sec  * k_sec / N_iso / pi);

% Winding resistance
rho_wind = 1.72e-8;
R_wind_prim = N_prim .* rho_wind .* l_wind_prim ./ (winding_diameter_prim .^ 2 ./ 4 .* pi);
R_wind_sek  = N_iso  .* rho_wind .* l_wind_sec  ./ (winding_diameter_sec .^ 2 ./ 4 .* pi);


Rprim = Rds_prim + Rsns_prim + R_wind_prim;
Rsek = Rsns_sek + R_wind_sek;


% Gatedriver : 1EDN7512BXTSA1


%% Calculations
% The design supports Boundary conduction mode, at maximum output power and nominal frequency

% Peak output
%Ipk = 2.*(Vout+Vsns+Vf).*Iout./(Dnom.*(Vin_min-Vsns-Vf)*nuest)
Ipk = (Vin_min-sqrt(Vin_min.^2-8*(Vout+Rsek.*Iout).*Iout.* Rprim./(Dnom*nuest)))/(2.*Rprim);


% Maximum Duty cycle for Minimum Input voltage
Dcalc_max = N.*(Vout+Rsek*Iout)./((Vin_min-Rprim.*Ipk)+N.*(Vout+Rsek*Iout));
Ipk = (Vin_min-sqrt(Vin_min.^2-8*(Vout+Rsek.*Iout).*Iout.* Rprim./(Dcalc_max*nuest)))/(2.*Rprim);
%Ipkcalc = 2.*(Vout+Rsek*Iout).*Iout./(Dcalc_max.*(Vin_min-Rprim.*Ipk)*nuest)

fprintf("\nStandard Operating Point:\n")
fprintf("Duty: \t\t%.1f %%\t%.1f %%\t%.1f %%\n", Dcalc_max*100)
fprintf("Primary Current\t%.1f A\t%.1f A\t%.1f A\n",Ipk)

if(max(Ipk > Iin_max))
  fprintf("ERROR: Input current exceeding limit of %0.2f A: %0.2f A\r\n",Iin_max,max(Ipk))
end

% Maximum Primary Inductance
Lprim_max = max((Vin_min -Rprim.*Ipk).^2 .*Dcalc_max.^2 * nuest ./(2*(Vout+Rsek*Iout).*Iout.*fsw));

% Maximum primary Currents for power rating
Ipkmax_prim = sqrt(2*Vout.*Iout./(Lprim_max*fsw*nuest));
Ipkrms_prim = Ipkmax_prim .* sqrt((1-Dcalc_max)/3);
Ipkmax_sek  = Ipkmax_prim .* N;
Ipkrms_sek  = Ipkmax_sek .* sqrt((1-Dcalc_max)/3);
disp("RMS currents")
fprintf("Primary RMS Current:   %.2f A\t%.2f A\t%.2f A\n", Ipkrms_prim);
fprintf("Secondary RMS Current: %.2f A\t%.2f A\t%.2f A\n", Ipkrms_sek);

##%% Current Sensing
##fprintf("\n\n========= Current sensing Selection Information ========\n");
##
##% Calculation of Resistance based on range
##Rsns_prim_calc = Vsns_max/(2*max(Ipk)*Asns);
##Rsns_sek_calc =  Vsns_max/(2*max(Iout)*Asns);
##fprintf("Calculated Sense Resistors: Rsns_prim = %0.3f mOhm, Rsns_sek = %0.3f mOhm\n", Rsns_prim_calc*1e3, Rsns_sek_calc*1e3);
##
##% Check if Resistor is in range
##if(Rsns_prim_calc < 0.9*Rsns_prim ||Rsns_prim_calc > 1.1*Rsns_prim)
##    fprintf("WARNING: Selected Primary current sense resistor is outside of the range\n")
##end
##if(Rsns_sek_calc < 0.9*Rsns_sek ||Rsns_sek_calc > 1.1*Rsns_sek)
##    fprintf("WARNING: Selected Primary current sense resistor is outside of the range\n")
##end
##Vsns_prim_max = max(Rsns_prim * Ipk);
##Vsns_sek_max = max(Rsns_sek * Iout);
##fprintf("Maximum sensing voltage Primary:   %.3f V with %.3f Ohm\n", Vsns_prim_max,Rsns_prim);
##fprintf("Maximum sensing voltage Secondary: %.3f V with %.3f Ohm\n", Vsns_sek_max,Rsns_sek);
##
##Psns_prim = max(Ipkrms_prim.^2*Rsns_prim);
##Psns_sek  = max(Iout.^2*Rsns_sek);
##fprintf("Maximum losses Primary:   %.3f W\n", Psns_prim);
##fprintf("Maximum losses Secondary: %.3f W\n", Psns_sek);
##
##% Calculation of maximum Resolution
##Res_prim = Vsns_max/(Asns*Rsns_prim)/2^ENOB_Adc;
##Res_sek = Vsns_max/(Asns*Rsns_sek)/2^ENOB_Adc;
##fprintf("Resolution of Current Sense ADC: %0.3f mA \t %0.3f mA\n", Res_prim*1e3, Res_sek*1e3);
##fprintf("Resolution in percentage of output current: %.3f %%\n", Res_sek/max(Iout)*100);

%% Power MOSFET Calculations
fprintf("\n\n========= MOSFET/Diode Selection Information ========\n");
Vmax_prim = Vin_max + max(N .* (Vout + Rsek * Iout));
Vmax_sek = (Vout + Vin_max ./ N) ./ 3;

fprintf("Minimal Drain-Source Voltage Primary MOSFET:   %.0f V\n", Vmax_prim)
fprintf("Minimal Drain-Source Voltage Secondary Diode: %.0f V\t%.0f V\t%.0f V\n", Vmax_sek)

Pfet_cond_prim =  Ipkrms_prim.^2*Rds_prim;
Pfet_cond_sek_diode = Ipkmax_sek.*(1-Dcalc_max)./2.*Vf_sek;
disp("MOSFET Conduction Losses")
fprintf("Primary:         \t%.3f W \t%.3f W \t%.3f W\n",Pfet_cond_prim);
fprintf("Secondary Diode: \t%.3f W \t%.3f W \t%.3f W\n", Pfet_cond_sek_diode);

% Switching losses
Pfet_sw_prim = 0.25*Qdrv_prim/Idrv_prim .* fsw .* Ipk .* Vin_max;
Pfet_coss_prim = fsw .* Qtot_prim*Vin_max./2;

% TODO: Secondary Diode switching losses
%Pfet_sw_sek = 0.25*Qdrv_sek/Idrv_sek .* fsw .* Ipk .* N .* Vout;
%Pfet_coss_sek = fsw .* Qtot_sek*Vin_max./2;

Pfet_tot_prim = Pfet_sw_prim + Pfet_coss_prim + Pfet_cond_prim;
Pfet_tot_sek = 0;%Pfet_sw_sek + Pfet_coss_sek + Pfet_cond_sek_diode;


disp("Total MOSFET Losses")
fprintf("Primary: \t%.3f W\t%.3f W\t%.3f W\nSecondary: \t%.3f W\t%.3f W\t%.3f W\n", Pfet_tot_prim,Pfet_tot_sek)

%% Ripple current/voltage calculation
fprintf("\n\n========= Capacitor Selection Information ========\n");
Coutmin_voltrip = Ipk.*N.*(1-Dcalc_max)./((Vripout-Ipk.*N.*0).*fsw);
Coutmin_transient = Iripout./(2*pi.*Vripout.*fbw);
Icrms = sqrt(Ipk.^2.*N.^2.*(1-Dcalc_max)/3 - Iout.^2);
Coutmin = max(Coutmin_transient, Coutmin_voltrip);

disp("Minimum required output capacitance by voltage/current ripple")
fprintf("250V:  \t%.2f nF \t%.3f mA\n", Coutmin_voltrip(1) * 1e9, Icrms(1) * 1e3);
fprintf("500V:  \t%.2f nF \t%.3f mA\n", Coutmin_voltrip(2) * 1e9, Icrms(2) * 1e3);
fprintf("1000V: \t%.2f nF \t%.3f mA\n", Coutmin_voltrip(3) * 1e9, Icrms(3) * 1e3);


Cinmin = Ipk.*Dcalc_max./(2.*fsw.*Vripin);
Icinrms = sqrt(Ipk.^2.*Dcalc_max/3-(max(Iout.*Vout)./Vin_min*nuest).^2);

disp("Minimum required input capacitance by voltage/current ripple")
fprintf("All Modes: \t%.2f uF \t%.3f A\n", max(Cinmin)*1e6, max(Icinrms));


%% Core Information
fprintf("\n\n========= Transformer Selection Information ========\n");
##% Material N87
##% ELP 22/6/16 B66285G0000X187
##Al_core = 2000e-9;
##le_core = 20.8e-3;
##B_max = 0.300;
##mu_r = 1480;
##mu_0 = 4*pi*1e-7;
##fprintf("Required maximum Inductance = %0.4f uH\n",min(Lpri_max*1e6))
##
##
##AL_max = min(Lpri_max./N_prim.^2);
##
##lg=(Al_core./AL_max-1).*le_core/mu_r;
##mu_r_gapped = mu_r/(mu_r*lg/le_core+1);
##
##le_min = max(Ipk.*mu_0*mu_r_gapped.*N./(B_max));
##
##fprintf("Required Airgap with selected core: %f mm\n", lg*1e3);
##fprintf("AL_max = %0.4f nH\n",AL_max*1e9)
##fprintf("le_min = %0.4f mm\n", le_min*1e3)
##if(le_min>le_core)
##    fprintf("ERROR: Core length to low. Required %f mm, Core has: %f mm", le_min*1e3,le_core*1e3);
##end

% Selected Core Parameters
% Material M33
% RM5
Al_core = 100e-9;
le_core = 20.8e-3;
B_max = 0.300;
mu_r = 79.6;
mu_0 = 4*pi*1e-7;
L_prim = N_prim^2*Al_core;

fprintf("Required maximum Inductance: %0.2f uH\n",Lprim_max*1e6)
fprintf("Resulting Inductance:        %0.2f uH\n",L_prim*1e6)
% ToDo Calculate Core losses based on frequency


%disp(['Winding resistance: \nPrimary:   ' num2str(R_wind_prim*1e3) 'mOhm\nSecondary: ' mat2str(R_wind_sek.*1e3) 'mOhm']);
fprintf("Winding Diameter: \t%.3f mm \t%.3f mm \r\n", winding_diameter_prim*1e3, winding_diameter_sec*1e3);
fprintf("Winding Resistance: \nPrimary: \t%.3f mOhm\nSecondary: \t%.3f mOhm\n\n", R_wind_prim.*1e3, R_wind_sek.*1e3);


P_wind_prim = Ipkrms_prim.^2.*R_wind_prim;
P_wind_sek = Ipkrms_sek.^2.*R_wind_sek;
fprintf("Winding Losses: \nPrimary: \t%.3f W\t%.3f W\t%.3f W\nSecondary: \t%.3f W\t%.3f W\t%.3f W\nTotal:    \t%.3f W\t%.3f W\t%.3f W\n\n",P_wind_prim, P_wind_sek, P_wind_prim+P_wind_sek);


plecspars = {};
plecssimstruct = {};
flyback_model = 'flyback_converter_v2';
proxy = jsonrpc('http://localhost:1080','Timeout',70);

proxy.plecs.load(sprintf('E:/Repos/bat_source/hw/sim/PLECS/%s.plecs',flyback_model ));


for cases = 1
      plecspars{cases}.vin = Vin(floor(cases/3)+1);
      plecspars{cases}.fsw = fsw;
      plecspars{cases}.L_prim = L_prim;
      plecspars{cases}.cout = Coutmin;
      plecspars{cases}.cin = Cinmin;
      plecspars{cases}.adc_skip = 35;
      plecspars{cases}.N = [N_prim -N_iso/3 -N_iso/3 -N_iso/3];
      plecspars{cases}.voutref = Vout(cases);
      plecspars{cases}.r_wind_prim = R_wind_prim;
      plecspars{cases}.r_wind_sec  = R_wind_sek/3;
      plecspars{cases}.vf_sek = Vf_sek;

      plecssimstruct{cases}.ModelVars = plecspars{cases};
      plecssolveropts.OutputTimes = 0:1e-4:0.08;
      plecssimstruct{cases}.SolverOpts = plecssolveropts;
end


% Data
% 1 = Output Current
% 2 = MOSFET Current
% 3 = Clamp Diode Current
% 4 = Inductor current
% 5 = Output Voltage
% 6-8 = Rectifier capacitor voltages
% 9 = MOSFET Voltage
% 10 = Rectifier Diode Voltage
% 11 = Internal Output Voltage
% 12 = Resistance

plecsres = proxy.plecs.simulate(flyback_model,plecssimstruct);


%% Analysis
for cases = 1:3
  plecsres(cases).Resistance = plecsres(cases).Values(5,:)./plecsres(cases).Values(1,:);
  plecsres(cases).V_rect_max = -min(plecsres(cases).Values(10,:));
  plecsres(cases).V_MOSFET_max = max(plecsres(cases).Values(9,:));
  plecsres(cases).I_MOSFET_max = max(plecsres(cases).Values(2,:));
  % Resistance, where the voltage goes out of range
end;
save("Flyback_Simulation_results_cont.mat", "plecsres", "plecspars");

%% Plot Plecspars
f=figure(3, 'position',[0,0,1200,800], 'paperunits','centimeters', 'papersize', [48,32], 'paperposition', [0,0,48,32]);
clf;
ax = [];
for i = 1:3
  subplot(3,2,2*i-1);
  ax1 = plotyy(plecsres(i).Time, plecsres(i).Values([1 4],:).*[1000;1],plecsres(i).Time, plecsres(i).Values([5 6 9 10 11],:));
  xlabel("Time [s]")
  ylabel(ax1(1), "Current [A]")
  ylabel(ax1(2), "Voltage [V]")
  legend("I_{out}*1000", "I_{ind}", ...
   "V_{out}" , "V_{C1}", "V_{prim}", "V_{sek}", 'orientation','horizontal', 'location','northwest');
  grid on
  grid minor
  ax = [ax gca];

  subplot(3,2,2*i);
  ax1 = semilogy(plecsres(i).Time, plecsres(i).Values(12,:),plecsres(i).Time, plecsres(i).Resistance);
  xlabel("Time [s]")
  ylabel("Resistance [Ohm]")
  legend("Reference Resistance", "Measured Resistance");
  grid on
  grid minor
  ax = [ax gca];
end



linkaxes(ax, "x");

fprintf("\n\n========= Simulation Results =======================\n");
fprintf("Maximum Rectifier Voltage: \t%0.0f V\t%0.0f V\t%0.0f V\r\n", plecsres(:).V_rect_max);
fprintf("Maximum MOSFET Voltage:    \t%0.0f V\t%0.0f V\t%0.0f V\r\n", plecsres(:).V_MOSFET_max);
fprintf("Maximum MOSFET Current:    \t%0.0f A\t%0.0f VA\t%0.0f A\r\n", plecsres(:).I_MOSFET_max);

print(f, "Flyback.pdf")
