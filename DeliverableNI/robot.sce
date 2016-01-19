clc;clear;

velMin = 1;
velMax = 1;
k_est = zeros(1, 9);
o_n_est = zeros(1, 9);
o_n_est2 = zeros(1, 9);
xi_est = zeros(1, 9);
AE = zeros(1,9);
AE2 = zeros(1,9);

for v = velMin : 1 : velMax
fileName = strcat(['C:\Users\Paolo\Desktop\universita\LAR\DATA\p_', string(v + 29), '_st_2_b_1000.csv']);
File = read(fileName,-1,4);
packet = size(File(:,1),1);
tempo = zeros(1, packet);
inputSignal = zeros(1, packet);
contatore = zeros(1, packet);
velocita = zeros(1, packet);
output = zeros(1, packet);
filteredSignal = zeros(1, packet);
tempo = File(:,1)'/1000;
inputSignal = File(:,2)';
contatore = File(:,3)';
velocita = File(:,4)' * (%pi/180) * 1000;

//myfilter = iir(3,'lp','butt',[0.01 0],[0 0]);
//butt3 = flts(velocita, myfilter);
//myfilter = iir(5,'lp','butt',[0.01 0],[0 0]);
//butt5 = flts(velocita, myfilter);
//myfilter = iir(7,'lp','butt',[0.01 0],[0 0]);
//butt7 = flts(velocita, myfilter);
//myfilter = iir(9,'lp','butt',[0.01 0],[0 0]);
//butt9 = flts(velocita, myfilter);
myfilter = iir(3,'lp','cheb2',[0.04 0],[0 0.01]);
output = flts(velocita, myfilter);

scf(1);
clf;
xtitle("RAW DATA vs FILTERED DATA");
xlabel("Time (s)");
ylabel("Speed (rad/s)");
plot(tempo, output,'r');
plot(tempo, velocita, 'g');
h2=legend(['Filtered data'; 'Raw data'], 2);
//
//scf(2);
//clf;
//xtitle("BUTTERWORTH vs CHEBYSHEV");
//plot(tempo, output, 'b');
//plot(tempo, butt3,'r--');
////plot(tempo, butt5,'g--');
////plot(tempo, butt7,'m--');
////plot(tempo, butt9,'c--');
//h2=legend(['Chebyshev filter'; 'Butterworth (3) filter'; 'Butterworth (5) filter'; 'Butterworth (7) filter'; 'Butterworth (9) filter' ], 4);


// Steady state value
q_est = output($);
k_est(v) = q_est/inputSignal($);

// Damping factor
y_0 = output(1);
y_inf = q_est;
y_max = max(output);
Overshoot = abs(y_max - y_inf)/abs(y_0 - y_inf);
xi_est(v) = sqrt(log(Overshoot)^2/(%pi^2 + log(Overshoot)^2));

// Natural frequency
alpha = 3;
Ts = -1;
output_at_Ts = -1;
for i=size(output,2):-1:1
  if abs(q_est - output(i)) < alpha/100*q_est
    Ts = tempo(i);
    output_at_Ts = output(i);
  else
    break;
  end
end
if Ts == -1
  disp('No settling time available');
end
N_bar = 1/sqrt(1 - xi_est(v)^2);
o_n_est(v) = (log(alpha/100) - log(N_bar))/(-xi_est(v)*Ts/1000);

// Natural frequency from output period
[OmegaFirstMax, FirstMax] = max(output);
[OmegaFirstMin, FirstMin] = min(output(FirstMax:$));
FirstMin = FirstMin + FirstMax - 1;
[OmegaSecondMax, SecondMax] = max(output(FirstMin:$));
SecondMax = SecondMax + FirstMin - 1;
T = tempo(SecondMax) - tempo(FirstMax);
o_n_est2(v) = 2*%pi/(T*sqrt(1 - xi_est(v)^2));

// Simulation 1
//A = 60;
s = poly(0, 's');
G_est = k_est(v)/(s^2/o_n_est(v)^2 + 2*xi_est(v)/o_n_est(v)*s + 1);
G_est = syslin('c', G_est);
output_est = csim(inputSignal, tempo, G_est);

// Simulation 2
G_est2 = k_est(v)/(s^2/o_n_est2(v)^2 + 2*xi_est(v)/o_n_est2(v)*s + 1);
G_est2 = syslin('c', G_est2);
output_est2 = csim(inputSignal, tempo, G_est2);

// Plot
//scf(3);
//clf;
//plot(tempo, output);
//plot(tempo, q_est*(100 - alpha)/100, 'r--');
//plot(tempo, q_est*(100 + alpha)/100, 'r--');
//plot([Ts, Ts], [0, output_at_Ts], 'k');
//plot(tempo, output_est, 'g');
//plot(tempo, output_est2, 'm');
//plot([tempo(FirstMax), tempo(FirstMin), tempo(SecondMax)], [output(FirstMax), output(FirstMin), output(SecondMax)], 'bo');

ISE = sum((output - output_est).^2);
IAE = sum(abs(output - output_est));
ITSE = sum(tempo .* (output - output_est).^2);
ITAE = sum(tempo .* abs(output - output_est));

ISE2 = sum((output - output_est2).^2);
IAE2 = sum(abs(output - output_est2));
ITSE2 = sum(tempo .* (output - output_est2).^2);
ITAE2 = sum(tempo .* abs(output - output_est2));

AE(v) = (ISE + IAE + ITSE + ITAE)/4;
AE2(v) = (ISE2 + IAE2 + ITSE2 + ITAE2)/4;
end

misurazioni = (1 + velMax - velMin);

//for i = 1 : 1 : 9
//    if(AE > 1000 & AE2 > 1000)
//        k_est(i) = 0;
//        xi_est(i) = 0;
//        o_n_est(i) = 0;
//        o_n_est2(i) = 0;
//        misurazioni = misurazioni - 1;
//    end
//end

AE
AE2
k_est_mean = sum(k_est)/misurazioni
xi_est_mean = sum(xi_est)/misurazioni
o_n_est_mean = sum(o_n_est)/misurazioni
o_n_est2_mean = sum(o_n_est2)/misurazioni

//minERR = 1234567890;
//minFreq = -1;
//minOrdine = -1;

//for ordine = 5 : 1 : 7
//    for frequenza = 0.002 : 0.001 : 0.05
//        ordine
//        frequenza
//myfilter = iir(ordine,'lp','butt',[frequenza 0],[0 0]);
//if (AE < minERR)
//    minERR=AE;
//    minFreq = frequenza;
//    minOrdine = ordine;
//end
//if (AE2 < minERR)
//    minERR = AE2;
//    minFreq = frequenza;
//    minOrdine = ordine;
//end

//end
//end

//minFreq
//minOrdine




//finestra = 9;
//for i = 1:1:size(velocita,2)
//    somma = 0;
//    for j = i - finestra/2 : 1 : i + finestra/2
//        if j < 1 
//            somma = somma + velocita(1);    
//        else 
//            if j > size(velocita)
//                somma = somma + velocita($);
//            else
//                somma = somma + velocita(j);
//            end
//        end
//    end
//    output(i) = somma/finestra;
//end
