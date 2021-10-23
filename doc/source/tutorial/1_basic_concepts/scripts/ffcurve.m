% initialize OAT
initOAT

% open SpikeReader object on spike file
SR = SpikeReader('../results/spk_output.dat');

% read # spikes binned into 1000ms bins
spk = SR.readSpikes(1000);

% plot input-output (FF) curve
% plot(10:10:100, spk, 'bo-', 'LineWidth',2)

plot(1:10:200, spk(1:20), 'bo-', 'LineWidth',2)   # Fix LN 20201001

xlabel('input rate (Hz)')
ylabel('output rate (Hz)')