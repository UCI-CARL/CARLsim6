% initialize OAT
initOAT

% open SpikeReader object on spike file
SR = SpikeReader('../results/spk_output.dat');

% read # spikes binned into 1000ms bins
spk = SR.readSpikes(500);

% plot input-output (FF) curve
figure(1)


subplot(2,1,1)
boxplot(transpose(spk))

subplot(2,1,2)
plot(10:10:100, spk, 'b.:', 'LineWidth',1)


xlabel('input rate (Hz)')
ylabel('output rate (Hz)')
