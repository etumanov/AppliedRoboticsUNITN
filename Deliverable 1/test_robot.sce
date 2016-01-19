clc;clear;

myfilter = iir(3,'lp','cheb2',[0.04 0],[0 0.01]);

//k_est = 8.7755267;
xi_est = 0.8209478;
o_n_est = 16.5151;
k_est = 0.1531618;
 //xi_est = 0.8209478;
 //o_n_est = 0.0165151; 
alpha = 3;
s = poly(0, 's');

// Dati 1
A1 = 40;
fileName = strcat(['C:\Users\Paolo\Desktop\universita\LAR\DATA\p_', string(A1), '_st_2_b_1000.csv']);
File = read(fileName,-1,4);
packet1 = size(File(:,1),1);
tempo1 = zeros(1, packet1)/1000;
inputSignal1 = zeros(1, packet1);
velocita1 = zeros(1, packet1);
output1 = zeros(1, packet1);
tempo1 = File(:,1)'/1000;
inputSignal1 = File(:,2)';
contatore1 = File(:,3)';
velocita1 = File(:,4)' * (%pi/180) * 1000;
output1 = flts(velocita1, myfilter);
// Simulation 1
G_est1 = k_est/(s^2/o_n_est^2 + 2*xi_est/o_n_est*s + 1);
G_est1 = syslin('c', G_est1);
output_est1 = csim(inputSignal1, tempo1, G_est1);

// Dati 2
A2 = 50;
fileName = strcat(['C:\Users\Paolo\Desktop\universita\LAR\DATA\p_', string(A2), '_st_2_b_1000.csv']);
File = read(fileName,-1,4);
packet2 = size(File(:,1),1);
tempo2 = zeros(1, packet2);
inputSignal2 = zeros(1, packet2);
velocita2 = zeros(1, packet2);
output2 = zeros(1, packet2);
tempo2 = File(:,1)'/1000;
inputSignal2 = File(:,2)';
contatore2 = File(:,3)';
velocita2 = File(:,4)' * (%pi/180) * 1000;
output2 = flts(velocita2, myfilter);
// Simulation 2
G_est2 = k_est/(s^2/o_n_est^2 + 2*xi_est/o_n_est*s + 1);
G_est2 = syslin('c', G_est2);
output_est2 = csim(inputSignal2, tempo2, G_est2);

// Dati 3
A3 = 60;
fileName = strcat(['C:\Users\Paolo\Desktop\universita\LAR\DATA\p_', string(A3), '_st_2_b_1000.csv']);
File = read(fileName,-1,4);
packet3 = size(File(:,1),1);
tempo3 = zeros(1, packet3);
inputSignal3 = zeros(1, packet3);
velocita3 = zeros(1, packet3);
output3 = zeros(1, packet3);
tempo3 = File(:,1)'/1000;
inputSignal3 = File(:,2)';
contatore3 = File(:,3)';
velocita3 = File(:,4)' * (%pi/180) * 1000;
output3 = flts(velocita3, myfilter);
// Simulation 3
G_est3 = k_est/(s^2/o_n_est^2 + 2*xi_est/o_n_est*s + 1);
G_est3 = syslin('c', G_est3);
output_est3 = csim(inputSignal3, tempo3, G_est3);

// Plot
scf(1);
clf;
xtitle("TEST vs SIMULATION");
xlabel("Time (s)");
ylabel("Speed (rad/s)");
plot(tempo1, output1, 'g');
plot(tempo1, output_est1, 'g--');
plot(tempo2, output2, 'r');
plot(tempo2, output_est2, 'r--');
plot(tempo3, output3, 'b');
plot(tempo3, output_est3, 'b--');
h2=legend(['power 40 (test)'; 'power 40 (simulation)';'power 50 (test)'; 'power 50 (simulation)';'power 60 (test)'; 'power 60 (simulation)'] ,4);
//plot(tempo, k_est*A*(100 - alpha)/100, 'r--');
//plot(tempo, k_est*A*(100 + alpha)/100, 'r--');
//plot([Ts, Ts], [0, output_at_Ts], 'k');
//plot(tempo, output_est2, 'm');
//plot([tempo(FirstMax), tempo(FirstMin), tempo(SecondMax)], [output(FirstMax), output(FirstMin), output(SecondMax)], 'bo');

ISE1 = sum((output1 - output_est1).^2);
IAE1 = sum(abs(output1 - output_est1));
ITSE1 = sum(tempo1 .* (output1 - output_est1).^2);
ITAE1 = sum(tempo1 .* abs(output1 - output_est1));

ISE2 = sum((output2 - output_est2).^2);
IAE2 = sum(abs(output2 - output_est2));
ITSE2 = sum(tempo2 .* (output2 - output_est2).^2);
ITAE2 = sum(tempo2 .* abs(output2 - output_est2));

ISE3 = sum((output3 - output_est3).^2);
IAE3 = sum(abs(output3 - output_est3));
ITSE3 = sum(tempo3 .* (output3 - output_est3).^2);
ITAE3 = sum(tempo3 .* abs(output3 - output_est3));

AE1 = (ISE1 + IAE1 + ITSE1 + ITAE1)/4
AE2 = (ISE2 + IAE2 + ITSE2 + ITAE2)/4
AE3 = (ISE3 + IAE3 + ITSE3 + ITAE3)/4

ISE1/packet1
ISE2/packet2
ISE3/packet3

scf(2);
clf;
xtitle("DIFFERENCE BETWEEN OUTPUT_EXP AND OUTPUT_EST");
xlabel("Time (s)");
ylabel("Error (rad/s)");
a=gca();
a.x_location = "middle";
a.y_location = "middle";
a.data_bounds = [0,-3, 0;2, 3, 2000];
//a.box = "on";
plot(tempo1, (output1 - output_est1), 'b');
plot(tempo2, (output2 - output_est2), 'g');
plot(tempo3, (output3 - output_est3), 'r');
//h2.box = "on";
h2=legend(['power 40';'power 50';'power 60']);
