% OAT demo

% init OAT by adding path
initOAT

% then open a GroupMonitor on the group's
% spike file and plot the activity
GM = GroupMonitor('g_exc0','.')
GM.plot

GM = GroupMonitor('g_exc1','.')
GM.plot

GM = GroupMonitor('g_exc2,'.')
GM.plot

GM = GroupMonitor('g_exc3,'.')
GM.plot


GM = GroupMonitor('g_inter0','.')
GM.plot

GM = GroupMonitor('g_inter1','.')
GM.plot

GM = GroupMonitor('g_inter2,'.')
GM.plot

GM = GroupMonitor('g_inter3,'.')
GM.plot



pR = PerformanceReader('p_CARLsim.dat');
pValues = pR.readValues();
delete(pR);  % close the file


%%



%% 1 ms precision

pR = PerformanceReader('p_CARLsim.dat');
pValues = pR.readValues();
delete(pR);  % close the file

%x = 1:50;
%x = 101:200;
x = 1:500;
%x = 1:300;
t = pValues.t(x);

subplot(4,1,1)
coreUtil = pValues.pdh_util([7],x)*100;
plot(t, coreUtil', "-r");
ylim([0,100])
ylabel('Core 7')
title("Core Utilization (%)");

subplot(4,1,2)
coreUtil = pValues.pdh_util([6],x)*100;
plot(t, coreUtil', "-r");
ylim([0,100])
ylabel('Core 6')

subplot(4,1,3)
coreUtil = pValues.pdh_util([5],x)*100;
plot(t, coreUtil', "-r");
ylim([0,100])
ylabel('Core 5')

subplot(4,1,4)
coreUtil = pValues.pdh_util([4],x)*100;
plot(t, coreUtil', "-r");
ylim([0,100])
ylabel('Core 4')

xlabel('time (ms)')

%%

