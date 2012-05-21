% (1) sim_time
% (2) qd[9]
% (3) JointTorque[9]
% (4) CoMx_ICS
% (5) p_Rf_ICS[0]

close all
clear all

data = load('biped_sim_data.txt');

ax=[];

figure()
a = gca;
ax = [ax a];
hold on

plot(data(:,1),data(:,3),'r'); % flywheel torque
hold off
title('flywheel torque');


figure()
a = gca;
ax = [ax a];
hold on
plot(data(:,1),data(:,2)); % flywheel velocity
hold off
title('flywheel angular velocity');
linkaxes(ax,'x');
 
 
figure()
a = gca;
ax = [ax a];
hold on
plot(data(:,1),data(:,4)); % CoMx_ICS
plot(data(:,1),data(:,5),'r'); % p_Rf_ICS_x
hold off
title('CoMx and Rfx in ICS');
linkaxes(ax,'x');

% sim_time