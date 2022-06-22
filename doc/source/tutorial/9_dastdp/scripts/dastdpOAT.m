% Tutorial 9 DA-STDP

% First run initOAT script to set the correct path
initOAT;

% Weights with DA releases
figure(9);

CR = ConnectionReader('../results/conn_pre-ex_post-ex.dat');
[allTimestamps, allWeights] = CR.readWeights();
SR = SpikeReader('../results/spk_DA neurons.dat');
spkData = SR.readSpikes(); % binWindowMs
spkV = spkData(:,1);
idx = (spkV>0);
subplot (4, 1, 1:3)
plot(allTimestamps, allWeights, allTimestamps(idx), allWeights(idx), '+');
title('Synaptic weights with DA releases');
ylabel('Synaptic Weight Strength');

GR = GroupReader('../results/grp_post-ex.dat');
[timestamps,data] = GR.readData();
subplot (4, 1, 4)
plot(timestamps, data, 'color', 'r');  
title('DA level in target group');
ylabel('DA level');
xlabel('Time (ms)');