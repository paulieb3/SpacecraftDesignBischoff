% Name: Paul Bischoff
% Vibe test matlab file for phone experiment.

clear all
clc

% load data from vibration test.
data = matfile('serialdata.mat', 'Writable', true);
test1 = data.measurments1;
test2 = data.measurments2;
test3 = data.measurments3;
test4 = data.measurments4;
test5 = data.measurments5;

plot1 = parse(test1);
plot2 = parse(test2);
plot3 = parse(test3);
plot4 = parse(test4);
plot5 = parse(test5);

figure(1)
plot(plot1,'-.')
hold on
plot(plot2, 'g:')
hold on
plot(plot3, 'g:*')
hold on
plot(plot4, '-')
hold on
plot(plot5, '--')
xlim([0 100])
title('Phone Vibrations vs. Time')
xlabel('Time')
ylabel('Amplitude')
legend('Hearbeat1', 'Breeze1', 'Impulse', 'Heartbeat2', 'Breeze2')


function f = parse(test)
    f = nonzeros(test');
    rowsToDelete = f > 3;
    f(rowsToDelete) = [];
end
