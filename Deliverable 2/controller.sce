// Second order system
//K = 0.0087755;
K = 0.1531618;
xi = 0.8209478;
o_n = 16.515118;

// Parametri
maxOvershoot = 0.2;
maxSettlingTime = 0.2;
alpha = 5;

// Transfer function of the motor
s = poly(0, 's');
G = K/(s^2/o_n^2 + 2*xi/o_n*s+1);
G = syslin('c',G);

//step response
DesiredValue = 10; // rad/s
StepTime = 0.006;
FinalTime = 2;
Dt = 0.002;
t = 0:Dt:FinalTime;
y = [zeros(1,ceil(StepTime/Dt)+1), DesiredValue*ones(1,ceil((FinalTime-StepTime)/Dt))];
//y = csim('step', t, G);

// Design controller
Kc = 12;
C = (s + 16) * (s + 16)/(s*(s+31));
//C = (s + 5 + 0.1*%i) * (s + 5 - 0.1*%i)/(s * (s + 10));

// Step response of the closed loop
Gcl = Kc*C*G /(1 + Kc*C*G);
Gcl = syslin('c', Gcl);
ycl = csim(y, t, Gcl);

// Plot
scf(1);
clf;
xtitle("Response to the step function");
xlabel("Time (s)");
ylabel("Speed (rad/s)");
a=gca();
a.data_bounds = [0, 0, 0;2, 14, 2000];
plot(t, y, 'b');
plot(t, ycl, 'g');
plot(t, y($)*(1 + maxOvershoot), 'r--');
plot(t, y($)*(100 - alpha)/100, 'k--');
plot(t, y($)*(100 + alpha)/100, 'k--');
plot([maxSettlingTime, maxSettlingTime], [0, y($)*(100 + alpha)/100], 'r--');
h1=legend(['ref function (10 rad/s)'; 'ycl'; 'requirements' ;'ref +- 5%'] ,4);

scf(2);
clf;
evans(C*G, Kc);

