% Parameters
meanValue = 10; % Mean of the distribution
stdDev = 0.8;   % sd 0.8ms  sync
stdDev2 = 2.4;   % sd 2.4ms refrence
stdDev3 = 3.2;   % sd 3.2ms runs dead ?

% Generate x values (range for the plot)
x = 0:1:20; % Adjust range as needed

% Compute the probability density function (PDF)
pdfValues = (1 / (stdDev * sqrt(2 * pi))) * exp(-((x - meanValue).^2) / (2 * stdDev^2));
pdfValues2 = (1 / (stdDev2 * sqrt(2 * pi))) * exp(-((x - meanValue).^2) / (2 * stdDev2^2));
pdfValues3 = (1 / (stdDev3 * sqrt(2 * pi))) * exp(-((x - meanValue).^2) / (2 * stdDev3^2));


y = round(pdfValues * 400);
sum(y)

y2 = round(pdfValues2 * 400);
sum(y2)

y3 = round(pdfValues3 * 400);
sum(y3)

% Plot the normal distribution
figure;
plot(x, y, 'g-', x, y2, 'b-', x, y3, 'r-', 'LineWidth', 2, 'Marker', '*');
ylim([0,250]);
xlim([0,100]);
title('Normal Distribution (Mean = 0, SD = 2.4ms)');
xlabel('x');
ylabel('Probability Density');
grid on;