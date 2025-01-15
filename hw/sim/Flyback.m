Vin_min = 10;
Vin_max = 18;
Vin = [Vin_min,Vin_max];

Vout_volt = 60;
Vout_curr = 5;
Vout = [Vout_volt,Vout_curr];

Iout_volt = 1;
Iout_curr = 10;
Iout = [Iout_volt, Iout_curr];

Vripout = 0.5e-2 * Vout;
Vripin = 0.5e-2 * Vin_min;
Iripout = 0.5e-2 * Iout;
fbw = 1e3;

Iin_max = 10;

% Parameter to influence the design
% Mode
% Currently only BCM is supported
N_prim = 2
N_volt = N_prim/11; % Np/Ns
N_curr = N_prim/1;
N=[N_volt, N_curr]

fsw = 100e3;
Dnom = 0.7;
nuest = 0.9;

%Vf = 0.25;
%Vsns = 0.25;
%Rsek = min(Vf./Iout+Vsns./Iout)
%Rprim = Rsek
Rsns = 0.010;
Rds_prim = 0.028;
Qdrv_prim = 3e-9; % nC
Idrv_prim = 4;
Qtot_prim = 7e-9;

% IPB60R099P7ATMA1
Rds_sek = 0.099;
Qdrv_sek = 10e-9; % nC
Idrv_sek = 4;
Qtot_sek = 45e-9;
Vf_sek = 0.9;

Rsek = Rds_sek + Rsns;
Rprim = Rds_prim + Rsns;

% The design supports Boundary conduction mode, at maximum output power and nominal frequency

% Peak output
%Ipk = 2.*(Vout+Vsns+Vf).*Iout./(Dnom.*(Vin_min-Vsns-Vf)*nuest)
Ipk = (Vin_min-sqrt(Vin_min.^2-8*(Vout+Rsek.*Iout).*Iout.* Rprim./(Dnom*nuest)))/(2.*Rprim)


% Maximum Duty cycle for Minimum Input voltage
Dcalc_max = N.*(Vout+Rsek*Iout)./((Vin_min-Rprim.*Ipk)+N.*(Vout+Rsek*Iout))
Ipk = (Vin_min-sqrt(Vin_min.^2-8*(Vout+Rsek.*Iout).*Iout.* Rprim./(Dcalc_max*nuest)))/(2.*Rprim)
%Ipkcalc = 2.*(Vout+Rsek*Iout).*Iout./(Dcalc_max.*(Vin_min-Rprim.*Ipk)*nuest)

if(max(Ipk > Iin_max))
  fprintf("ERROR: Input current exceeding limit of %0.2f A: %0.2f A\r\n",Iin_max,max(Ipk))
end

Vmax_prim = Vin_max + max(N.*(Vout+Rsek*Iout))
Vmax_sek = max(Vout+Vin_max./N)

% Maximum Primary Inductance
Lpri_max = (Vin_min -Rprim.*Ipk).^2 .*Dcalc_max.^2 * nuest ./(2*(Vout+Rsek*Iout).*Iout.*fsw);
fprintf("Lpri_max = %0.4f \t %0.4f uH\r\n",Lpri_max*1e6)
Lpri = Lpri_max;

% Maximum primary Currents for power rating
Ipkmax_prim = sqrt(2*Vout.*Iout./(Lpri*fsw*nuest))
Ipkrms_prim = Ipkmax_prim .* sqrt((1-Dcalc_max)/3)
Ipkmax_sek  = Ipkmax_prim .* N
Ipkrms_sek  = Ipkmax_sek .* sqrt((1-Dcalc_max)/3)

% Check current sensing
Vsns_prim_max = max(Rsns * Ipk)
Vsns_sek_max = max(Rsns * Iout)

Psns_prim = max(Ipkrms_prim.^2*Rsns)
Psns_sek  = max(Iout.^2*Rsns)

% Power MOSFET Calculations
Pfet_cond_prim =  Ipkrms_prim.^2*Rds_prim
Pfet_cond_sek = Ipkrms_sek.^2*Rds_sek
Pfet_cond_sek_diode = Ipkmax_sek.*(1-Dcalc_max)./2.*Vf_sek

% Switching losses not calculated
Pfet_sw_prim = 0.25*Qdrv_prim/Idrv_prim .* fsw .* Ipk .* Vin_max
Pfet_coss_prim = fsw .* Qtot_prim*Vin_max./2

Pfet_sw_sek = 0.25*Qdrv_sek/Idrv_sek .* fsw .* Ipk .* N .* Vout
Pfet_coss_sek = fsw .* Qtot_sek*Vin_max./2

Pfet_tot_prim = Pfet_sw_prim + Pfet_coss_prim + Pfet_cond_prim
Pfet_tot_sek = Pfet_sw_sek + Pfet_coss_sek + Pfet_cond_sek

% Ripple current/voltage calculation
Coutmin_voltrip = Ipk.*N.*(1-Dcalc_max)./((Vripout-Ipk.*N.*0).*fsw)
Coutmin_currip = Iripout./(2*pi.*Vripout.*fbw)

Icrms = sqrt(Ipk.^2.*N.^2.*(1-Dcalc_max)/3 - Iout)

Cinmin = Ipk.*Dcalc_max./(2.*fsw.*Vripin)
Icinrms = sqrt(Ipk.^2.*Dcalc_max/3-(max(Iout.*Vout)./Vin_min*nuest).^2)


%% Core Information
fprintf("========= Core Selection Information ========\r\n");
% Material N87
B_max = 0.39;
mu = 4*pi*1e-7 * 140;

% Material 67
B_max = 0.23;
mu = 4*pi*1e-7 * 40;

AL_max = min(Lpri_max./N_prim.^2);
le_min = max(Ipk.*mu.*N./(B_max));
fprintf("AL_max = %0.4f nH\r\n",AL_max*1e9)
fprintf("le_min = %0.4f cm\r\n", le_min*1e2)


%% Possible: 
% Fair-rite  9567250602 --> 2.80 per pair
% 9567250802
% or TDK B65525J0100A087 --> 

%% Possible Power Semiconductors: 
%Secondary: IPAN60R125PFD7SXKSA1 --> Better for higher frequency 2.87 Fr
%Secondary: IXFP34N65X2M --> Lower conduction losses, better for low 3.41
%Secondary: IPB60R099P7ATMA1 --> SMD 2.6160

%Primary: PXN028-100QLJ --> 0.3440
