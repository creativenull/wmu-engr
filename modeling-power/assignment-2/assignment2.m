% Arnold Jason Chand
%
% Assignment 2 - Compute the Initial potential distribution
% of a given capacitance matrix of certain spacings
%
% plot for all three spacings
spacings = {'5mm', '6mm', '8mm'};
[~, space_size] = size(spacings);
freq = 1e6; % Hz
Vs = 1;
Ys = 1e6; % Mega Semans
Is = Vs * Ys;
N = 31; % number of turns

% preallocate result
percent_volt = zeros([space_size, N]);

% Calculate and plot each spacings
[~, L] = size(spacings);
for i = 1:L
    % Spacing
    current_spacing = spacings{i};
    
    % Frequency in rads
    current_omega = 2 * pi * freq;
    fprintf('Performing computations for spacing: %s at %.2e Hz\n',...
        current_spacing,...
        freq);
    
    % Read data from csv
    filepath = sprintf('./data/capacitance_%s.csv', current_spacing);
    cap_matrix = csvread(filepath, 5); % offset at 5, start of matrix csv
    
    % Current
    I = zeros([N, 1]);
    I(1) = Is; % inject source Is at first matrix
    
    % Admittance
    Y = 1j * current_omega .* cap_matrix;
    Y(1,1) = Y(1,1) + Ys; % add source admittance to the first matrix
    
    % Voltage
    V = Y\I; % calculate the voltage
    percent_volt(i,:) = abs(V) * 100; % convert to percents
    
    % Plot
    figure(i);
    plot_title = sprintf('%s spacing', current_spacing);
    plot(1:N, percent_volt(i,:), 'b-o');
    xlabel('Number of Turns, N');
    ylabel('Voltage, %V');
    title(plot_title);
    axis([1, N, 0, max(percent_volt(i,:))]);
    grid on;
    legend('initial distribution');
end

% Plotting all 3 
figure(4);
all_plots = plot(1:N, percent_volt, '-o');
title('Comparison of spacings');
xlabel('Number of Turns, N');
ylabel('Voltage, %V');
axis([1, 31, 0, 100]);
grid on;
legend(all_plots,...
    '5mm initial distributions',...
    '6mm initial distributions',...
    '8mm initial distributions');
