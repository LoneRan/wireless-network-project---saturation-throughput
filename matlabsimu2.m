%define the parameters from bianchi's parameter table
payload = 8184; MACheader = 272; PHYheader = 128; ack = 112;
ProDelay = 1;
packet = payload + MACheader + PHYheader;
ACK = 112 + PHYheader;
SIFS = 28;
SlotTime = 50;
DIFS = 128;
%calculate time slot of success transmission
Ts = (PHYheader + MACheader + payload + SIFS +  ProDelay + ACK + DIFS + ProDelay)/SlotTime;
%calculate time slot when collision happens
Tc = (PHYheader + MACheader + payload + DIFS + ProDelay)/SlotTime;


W = [32, 32, 128];

m = [3, 5, 3];

for j = 1:3
    
    S = zeros(1,48);
    for i = 3 : 1 : 50 %here we  use i standing for number of stations
        fun = @(p) (p-1+(1-(2*(1-2*p))/((1-2*p)*(W(j)+1) + p*W(j)*(1-(2*p)^m(j))))^(i-1));
        pj = fzero(fun, [0,1]);
        tae = (2*(1-2*pj))/((1-2*pj)*(W(j)+1) + pj*W(j)*(1-(2*pj)^m(j)));
        Ptr = 1 - (1 - tae)^i;
        Ps = (i*tae*(1-tae)^(i-1))/Ptr
        Efi = 1/Ptr - 1;
        S(i-2) = (Ps*payload/SlotTime)/(Efi+Ps*Ts+(1-Ps)*Tc);
    end

    plot(3:1:50, S, 'LineWidth', 1.5);
    legend(['W=' num2str(32) ', m=' num2str(3)],['W=' num2str(32) ', m=' num2str(5)],['W=' num2str(128) ', m=' num2str(3)]);
    hold on ;
end

   