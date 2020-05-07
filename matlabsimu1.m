%define the parameters from bianchi's parameter table
payload = 8184; MACheader = 272; PHYheader = 128; ack = 112;
pkt = payload + MACheader + PHYheader;
ACK = 112 + PHYheader;
SIFS = 28;
SlotTime = 50;
DIFS = 128;

W = [32, 32, 128];
m = [3, 5, 3];
for j = 1 : 3
    simutime = 0;
    throughput = zeros(1,48);
    for n = 3:1:50
        simutime = 0;
        Spkt = 0;
        %for every station, we need to store its stage and waiting time to
        %transimit
        StationStage = zeros(1,n);
        waitingtime = zeros(1,n);

        for i = 1 : n
            StationStage(i) = 0;
            waitingtime(i) = DIFS + floor(W(j)*rand) * SlotTime;
        end

        while(Spkt<10000)
            idle_end = min(waitingtime);
            num_tran = 0;
            %calculate the number of transimission at the same time
            for i = 1 : n
                if waitingtime(i) == idle_end
                    num_tran = num_tran + 1;
                end
            end

            if num_tran == 1 %only one trans: successful
                Spkt = Spkt + 1;
                %if success, simualtion time plus time waiting for idle to end
                %and packet to transmit
                simutime = simutime + pkt+ idle_end + SIFS + ACK + DIFS;
                for i = 1 : n
                    if waitingtime(i) == idle_end
                        StationStage(i) = 0;
                        %success back to stage 0 and randomly goes to a
                        %contention window
                        waitingtime(i) = floor((W(j)-1)*rand)*SlotTime;
                    else 
                        %other stations count down idle_end and freeze the
                        %timer
                        waitingtime(i) = waitingtime(i) -idle_end;
                    end
                end
            else % not only one: collision 
                simutime = simutime + idle_end + pkt + DIFS;
                for i = 1:n
                    if waitingtime(i) == idle_end % uniform backoff at next stage
                        %penalty to next satge
                        if StationStage(i) < m(j)
                            StationStage(i) = StationStage(i) +1;
                        end

                        waitingtime(i) = floor((2^StationStage(i)*W(j)-1)*rand)*SlotTime;
                    else
                        %same like successful situation
                        waitingtime(i) = waitingtime(i) - idle_end;
                    end
                end  
            end
        end
        throughput(n-2) = Spkt*(payload)/simutime;
    end
    xarr = zeros(1,50);
    for i = 1:1:50
        xarr(i)=i;
    end
    plot(3:1:50,throughput,'LineWidth',2);
    legend(['W=' num2str(32) ', m=' num2str(3)],['W=' num2str(32) ', m=' num2str(5)],['W=' num2str(128) ', m=' num2str(3)]);
    hold on;
    title('Saturation throughput')
    xlabel('Number of Stations');
    ylabel('Saturation Throughput');
end

        
        
    