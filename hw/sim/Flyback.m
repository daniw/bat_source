Vin_min = 10;
Vin_max = 18;
Vin = [Vin_min,Vin_max];

Vout_volt = 60;
Vout_curr = 5;
Vout = [Vout_volt,Vout_curr];

Iout_volt = 1;
Iout_curr = 10;
Iout = [Iout_volt, Iout_curr];

Vrip = 0.5e-2 * Vout;
Irip = 0.5e-2 * Iout;
fbw = 1e3;

Iin_max = 10;

% Parameter to influence the design
% Mode
% Currently only BCM is supported
N_prim = 2
N_volt = N_prim/11; % Np/Ns
N_curr = N_prim/1;
N=[N_volt, N_curr]

fsw = 350e3;
Dnom = 0.7;
nuest = 0.9;

%Vf = 0.25;
%Vsns = 0.25;
%Rsek = min(Vf./Iout+Vsns./Iout)
%Rprim = Rsek
Rsns = 0.010;
Rds_prim = 0.005;
Rds_sek = 0.015;
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
Ipkmax = sqrt(2*Vout.*Iout./(Lpri*fsw*nuest))
Ipkrms = Ipkmax .* sqrt(Dcalc_max/3)

% Check current sensing
Vsns_prim_max = max(Rsns * Ipk)
Vsns_sek_max = max(Rsns * Iout)

Psns_prim = max(Ipkrms.^2*Rsns)
Psns_sek  = max(Iout.^2*Rsns)

% Power MOSFET Calculations
Pfet_cond_prim = max(Ipkrms.^2*Rds_prim)
Pfet_cond_sek = max(Iout.^2*Rds_sek)

% Switching losses not calculated
% Pfet_sw_prim = 0.25*Qdrv_prim/Idrv_prim * fsw * Ipk* Vds
% Pfet_coss_prim = fsw * Qtot*Vds/2

% Ripple current/voltage calculation
Cmin_voltrip = Ipk.*N.*(1-Dcalc_max)./((Vrip-Ipk.*N.*0).*fsw)
Cmin_currip = Irip./(2*pi.*Vrip.*fbw)


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


%% Possible: Fair-rite  9567250602
% or TDK B65525J0100A087


