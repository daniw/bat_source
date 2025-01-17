clc
%% Requirements
Vin_min = 10;
Vin_max = 18;
Vin = [Vin_min,Vin_max];

Vout_curr = 5;
Vout_volt = 60;
Vout_iso = 500;
Vout = [Vout_curr,Vout_volt,Vout_iso];

Iout_curr = 10;
Iout_volt = 1;
Iout_iso = 0.01;
Iout = [Iout_curr, Iout_volt, Iout_iso];

fprintf("\n\n========= Operating Points ========\n");
fprintf("Input Voltage range: %.1f V - %.1f V\n", Vin);
fprintf("Output Voltages: %.1f V\t%.1f V\t%.1f V\n", Vout);
fprintf("Output Currents: %.1f A\t%.1f A\t%.3f A\n", Iout);

Vripout = 0.5e-2 * Vout;
Vripin = 5e-2 * Vin_min;
Iripout = 0.5e-2 * Iout;
fbw = 1e3;

Iin_max = 10;

%% Parameter to influence the design
% Currently only BCM is supported
N_prim = 2;
N_curr = 1;
N_volt = 8; % Np/Ns
N_iso = 24;
N=N_prim./[N_curr, N_volt+N_curr, N_iso+N_volt+N_curr];

fprintf("Transformer Ratios %i:%i\t%i:%i\t%i:%i\n", N_prim, N_curr, N_prim, N_volt, N_prim, N_iso)

BASE_PCB_70U = false;
WIND_PCB_70U = false;

% Winding length per turn
l_wind_prim =  71.3e-3 / 2;
l_wind_curr =  35.7e-3 / 1;
l_wind_volt = 305.8e-3 / 8;
l_wind_iso  = 820.6e-3 / 24;

% Winding width
w_wind_prim = 4.5e-3;
w_wind_curr = 4.5e-3;
w_wind_volt = 2.2e-3;
w_wind_iso  = 0.2e-3;

% Winding thickness
if BASE_PCB_70U
    t_wind_volt = 70e-6;
else
    t_wind_volt = 35e-6;
end
if WIND_PCB_70U
    t_wind_prim = 70e-6;
    t_wind_curr = 70e-6;
else
    t_wind_prim = 35e-6;
    t_wind_curr = 35e-6;
end
t_wind_iso  = 35e-6;

% Winding thickness
if WIND_PCB_70U
    p_wind_prim = 2;
else
    p_wind_prim = 3;
end
p_wind_curr = 4;
p_wind_volt = 1;
p_wind_iso  = 1;

% Winding resistance
rho_wind = 1.72e-8;
R_wind_prim = N_prim .* rho_wind .* l_wind_prim ./ (w_wind_prim .* t_wind_prim) ./ p_wind_prim;
R_wind_curr = N_curr .* rho_wind .* l_wind_curr ./ (w_wind_curr .* t_wind_curr) ./ p_wind_curr;
R_wind_volt = N_volt .* rho_wind .* l_wind_volt ./ (w_wind_volt .* t_wind_volt) ./ p_wind_volt;
R_wind_iso  = N_iso  .* rho_wind .* l_wind_iso  ./ (w_wind_iso  .* t_wind_iso ) ./ p_wind_iso ;
R_wind_sek = [R_wind_curr, R_wind_volt+R_wind_curr, R_wind_iso+R_wind_volt+R_wind_curr];
disp(['Winding resistance: \nPrimary:   ' num2str(R_wind_prim*1e3) 'mOhm\nSecondary: ' mat2str(R_wind_sek.*1e3) 'mOhm']);
fprintf("Winding Resistance: \nPrimary: \t%.3f mOhm\nSecondary: \t%.3f mOhm\t%.3f mOhm\t%.3f mOhm\n\n", R_wind_prim.*1e3, R_wind_sek.*1e3);

fsw = 150e3;
Dnom = 0.7;
nuest = 0.9;

% Sensing
Asns = 20; % Current Sense amplifier gain
Rsns_prim = 0.010;
Rsns_sek = 0.010;
Vsns_max = 3.3; % Complete Voltage range of ADC (eg. -1.65..1.65V or 0..3.3V)
ENOB_Adc = 10.4; % Effective number of bits of ADC

% PXN028-100QL
Rds_prim = 0.028;
Qdrv_prim = 3e-9; % nC
Idrv_prim = 4;
Qtot_prim = 7e-9;

% SUM90220E-GE3 oder IRFP4127PBF (THT TO247, dafür teurer)
Rds_sek = 0.0216;
Qdrv_sek = 8.6e-9; % nC
Idrv_sek = 4;
Qtot_sek = 31.6e-9;

% Only for High voltage diode
%ADS12B3-ASARH
Vf_sek = 1.35;

Rprim = Rds_prim + Rsns_prim;
Rsek = Rds_sek + Rsns_sek;

%% Calculations
% The design supports Boundary conduction mode, at maximum output power and nominal frequency

% Peak output
%Ipk = 2.*(Vout+Vsns+Vf).*Iout./(Dnom.*(Vin_min-Vsns-Vf)*nuest)
Ipk = (Vin_min-sqrt(Vin_min.^2-8*(Vout+Rsek.*Iout).*Iout.* Rprim./(Dnom*nuest)))/(2.*Rprim);


% Maximum Duty cycle for Minimum Input voltage
Dcalc_max = N.*(Vout+Rsek*Iout)./((Vin_min-Rprim.*Ipk)+N.*(Vout+Rsek*Iout));
Ipk = (Vin_min-sqrt(Vin_min.^2-8*(Vout+Rsek.*Iout).*Iout.* Rprim./(Dcalc_max*nuest)))/(2.*Rprim);
%Ipkcalc = 2.*(Vout+Rsek*Iout).*Iout./(Dcalc_max.*(Vin_min-Rprim.*Ipk)*nuest)

fprintf("\nStandard Operating Point:\nDuty: \t\t%.1f %%\t%.1f %%\t%.1f %%\n", Dcalc_max*100)
fprintf("Primary Current\t%.1f A\t%.1f A\t%.1f A\n",Ipk)

if(max(Ipk > Iin_max))
  fprintf("ERROR: Input current exceeding limit of %0.2f A: %0.2f A\r\n",Iin_max,max(Ipk))
end



% Maximum Primary Inductance
Lpri_max = (Vin_min -Rprim.*Ipk).^2 .*Dcalc_max.^2 * nuest ./(2*(Vout+Rsek*Iout).*Iout.*fsw);
Lpri = Lpri_max;
% Maximum primary Currents for power rating
Ipkmax_prim = sqrt(2*Vout.*Iout./(Lpri*fsw*nuest));
Ipkrms_prim = Ipkmax_prim .* sqrt((1-Dcalc_max)/3);
Ipkmax_sek  = Ipkmax_prim .* N;
Ipkrms_sek  = Ipkmax_sek .* sqrt((1-Dcalc_max)/3);
disp("RMS currents")
fprintf("Primary RMS Current:   %.2f A\t%.2f A\t%.2f A\n", Ipkrms_prim);
fprintf("Secondary RMS Current: %.2f A\t%.2f A\t%.2f A\n", Ipkrms_sek);

%% Current Sensing
fprintf("\n\n========= Current sensing Selection Information ========\n");

% Calculation of Resistance based on range
Rsns_prim_calc = Vsns_max/(2*max(Ipk)*Asns);
Rsns_sek_calc =  Vsns_max/(2*max(Iout)*Asns);
fprintf("Calculated Sense Resistors: Rsns_prim = %0.3f mOhm, Rsns_sek = %0.3f mOhm\n", Rsns_prim_calc*1e3, Rsns_sek_calc*1e3);

% Check if Resistor is in range
if(Rsns_prim_calc < 0.9*Rsns_prim ||Rsns_prim_calc > 1.1*Rsns_prim)
    fprintf("WARNING: Selected Primary current sense resistor is outside of the range\n")
end
if(Rsns_sek_calc < 0.9*Rsns_sek ||Rsns_sek_calc > 1.1*Rsns_sek)
    fprintf("WARNING: Selected Primary current sense resistor is outside of the range\n")
end
Vsns_prim_max = max(Rsns_prim * Ipk);
Vsns_sek_max = max(Rsns_sek * Iout);
fprintf("Maximum sensing voltage Primary:   %.3f V with %.3f Ohm\n", Vsns_prim_max,Rsns_prim);
fprintf("Maximum sensing voltage Secondary: %.3f V with %.3f Ohm\n", Vsns_sek_max,Rsns_sek);

Psns_prim = max(Ipkrms_prim.^2*Rsns_prim);
Psns_sek  = max(Iout.^2*Rsns_sek);
fprintf("Maximum losses Primary:   %.3f W\n", Psns_prim);
fprintf("Maximum losses Secondary: %.3f W\n", Psns_sek);

% Calculation of maximum Resolution
Res_prim = Vsns_max/(Asns*Rsns_prim)/2^ENOB_Adc;
Res_sek = Vsns_max/(Asns*Rsns_sek)/2^ENOB_Adc;
fprintf("Resolution of Current Sense ADC: %0.3f mA \t %0.3f mA\n", Res_prim*1e3, Res_sek*1e3);
fprintf("Resolution in percentage of output current: %.3f %%\n", Res_sek/max(Iout)*100);

%% Power MOSFET Calculations
fprintf("\n\n========= MOSFET Selection Information ========\n");
Vmax_prim = Vin_max + max(N.*(Vout+Rsek*Iout));
Vmax_sek = Vout+Vin_max./N;

fprintf("Minimal Drain-Source Voltage Primary MOSFET:   %.0f V\n", Vmax_prim)
fprintf("Minimal Drain-Source Voltage Secondary MOSFET: %.0f V\t%.0f V\t%.0f V\n", Vmax_sek)

Pfet_cond_prim =  Ipkrms_prim.^2*Rds_prim;
Pfet_cond_sek = Ipkrms_sek.^2*Rds_sek;
Pfet_cond_sek_diode = Ipkmax_sek.*(1-Dcalc_max)./2.*Vf_sek;
disp("MOSFET Conduction Losses")
fprintf("Primary:         \t%.3f W \t%.3f W \t%.3f W\n",Pfet_cond_prim);
fprintf("Secondary:       \t%.3f W \t%.3f W \t%.3f W\n", Pfet_cond_sek)
fprintf("Secondary Diode: \t%.3f W \t%.3f W \t%.3f W\n", Pfet_cond_sek_diode)

% Switching losses
Pfet_sw_prim = 0.25*Qdrv_prim/Idrv_prim .* fsw .* Ipk .* Vin_max;
Pfet_coss_prim = fsw .* Qtot_prim*Vin_max./2;

Pfet_sw_sek = 0.25*Qdrv_sek/Idrv_sek .* fsw .* Ipk .* N .* Vout;
Pfet_coss_sek = fsw .* Qtot_sek*Vin_max./2;

Pfet_tot_prim = Pfet_sw_prim + Pfet_coss_prim + Pfet_cond_prim;
Pfet_tot_sek = Pfet_sw_sek + Pfet_coss_sek + Pfet_cond_sek;


disp("Total MOSFET Losses")
fprintf("Primary: \t%.3f W\t%.3f W\t%.3f W\nSecondary: \t%.3f W\t%.3f W\t%.3f W\n", Pfet_tot_prim,Pfet_tot_sek)

%% Ripple current/voltage calculation
fprintf("\n\n========= Capacitor Selection Information ========\n");
Coutmin_voltrip = Ipk.*N.*(1-Dcalc_max)./((Vripout-Ipk.*N.*0).*fsw);
Coutmin_transient = Iripout./(2*pi.*Vripout.*fbw);
Icrms = sqrt(Ipk.^2.*N.^2.*(1-Dcalc_max)/3 - Iout.^2);

disp("Minimum required output capacitance by voltage/current ripple")
fprintf("Current Mode: \t%.2f uF \t%.3f A\n", Coutmin_voltrip(1)*1e6, Icrms(1));
fprintf("Voltage Mode: \t%.2f uF \t%.3f A\n", Coutmin_voltrip(2)*1e6, Icrms(2));
fprintf("Iso Mode:     \t%.2f uF \t%.3f A\n", Coutmin_voltrip(3)*1e6, Icrms(3));


Cinmin = Ipk.*Dcalc_max./(2.*fsw.*Vripin);
Icinrms = sqrt(Ipk.^2.*Dcalc_max/3-(max(Iout.*Vout)./Vin_min*nuest).^2);

disp("Minimum required input capacitance by voltage/current ripple")
fprintf("\t%.2f uF \t%.3f A\n", max(Cinmin)*1e6, max(Icinrms));


%% Core Information
fprintf("\n\n========= Transformer Selection Information ========\n");
% Material N87
% ELP 22/6/16 B66285G0000X187
Al_core = 4500e-9;
le_core = 32.5e-3;
B_max = 0.300;
mu_r = 1470;
mu_0 = 4*pi*1e-7;
fprintf("Required maximum Inductance = %0.4f uH\n",min(Lpri_max*1e6))


AL_max = min(Lpri_max./N_prim.^2);

lg=(Al_core./AL_max-1).*le_core/mu_r;
mu_r_gapped = mu_r/(mu_r*lg/le_core+1);

le_min = max(Ipk.*mu_0*mu_r_gapped.*N./(B_max));

fprintf("Required Airgap with selected core: %f mm\n", lg*1e3);
fprintf("AL_max = %0.4f nH\n",AL_max*1e9)
fprintf("le_min = %0.4f mm\n", le_min*1e3)
if(le_min>le_core)
    fprintf("ERROR: Core length to low. Required %f mm, Core has: %f mm", le_min*1e3,le_core*1e3);
end
% ToDo Calculate Core losses based on frequency

%% Winding Power losses
P_wind_prim = Ipkrms_prim.^2.*R_wind_prim;
P_wind_sek = Ipkrms_sek.^2.*R_wind_sek;
fprintf("Winding Losses: \nPrimary: \t%.3f W\t%.3f W\t%.3f W\nSecondary: \t%.3f W\t%.3f W\t%.3f W\nTotal:    \t%.3f W\t%.3f W\t%.3f W\n\n",P_wind_prim, P_wind_sek, P_wind_prim+P_wind_sek);

%% Possible: 
% Fair-rite  9567250602 --> 2.80 per pair
% 9567250802
% or TDK B65525J0100A087 --> 

%% Possible Power Semiconductors: 
%Secondary: NTBG070N120M3S
% Secondary: IPB95R130PFD7

%Primary: PXN028-100QLJ --> 0.3440
