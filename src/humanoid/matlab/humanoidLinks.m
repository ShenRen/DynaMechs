% Torso Link
clear link;
%link.mass = .578 * Mass;
link.mass = 12.1;
link.length = .41;
link.width  = .18;
link.depth  = .1;
%link.com    = [0 0 link.length*.66];
link.com    = [0 0 .15];
link.I      = 1/12 * link.mass * diag([link.depth^2 + link.width^2, ...
                    link.length^2+link.width^2, link.length^2+link.depth^2]);
link.Ibar   = link.I + link.mass*cross(link.com)*cross(link.com)';
link.hipWidth = .18;
link.type   = 'MobileBaseLink';
link.model  = '"./humanoid_box_model/torso.xan"';
link.contacts = [];
link.quat   = [0 0 0 1];
link.pos    = [2 2 .62];
link.offset = [-link.depth/2 -link.width/2 0];
link.scale  = [link.depth link.width link.length];

link.neckWidth = .04;
link.neckHeight = .02;
link.neckDepth = .04;
link.headHeight = .12;
link.headWidth = .1;
link.headDepth = .1;

link.neckOffset = [-link.neckDepth/2 -link.neckWidth/2 link.length];
link.neckScale  = [link.neckDepth, link.neckWidth, link.neckHeight];
link.headOffset = [-link.headDepth/2 -link.headWidth/2 link.length+link.neckHeight];
link.headScale  = [link.headDepth link.headWidth link.headHeight];

torsoLink = link;
totalMass = totalMass + link.mass;


clear link;
link.type = 'ZScrewTxLink';
link.params = [0 0];
zScrewLink = link;


% Thigh Link
clear link;
%link.mass = .1 * Mass;
link.mass = .81;
totalMass = totalMass + 2*link.mass;
link.length = .25;
link.width  = .04*1.5;
link.depth  = .04*1.5;
%link.com    = [.433*link.length 0 0];
link.com    = [.0784 0 0];
link.I      = 1/12 * link.mass * diag([link.depth^2 + link.width^2, ...
                    link.length^2+link.width^2, link.length^2+link.depth^2]);
link.Ibar   = link.I + link.mass*cross(link.com)*cross(link.com)';
link.contacts = [link.length	0	0];
link.type   = 'QuaternionLink';
link.quat   = [0 0 0 1];
link.model  = '"./humanoid_box_model/thigh.xan"';
link.offset = [0 -link.depth/2 -link.width/2];
link.scale  = [link.length link.depth,link.width];

hipLink = link;


% Shank Link
clear link;
%link.mass = .0465 * Mass;
link.mass = .63;
totalMass = totalMass + 2*link.mass;
link.length = .25;
link.width  = .04*1.5;
link.depth  = .04*1.5;
%link.com    = [.433*link.length 0 0];
link.com    = [0.0964	0 0];
link.I      = 1/12 * link.mass * diag([link.depth^2 + link.width^2, ...
                    link.length^2+link.width^2, link.length^2+link.depth^2]);
link.Ibar   = link.I + link.mass*cross(link.com)*cross(link.com)';
link.contacts = [link.length	0	0];
link.mdh    = [hipLink.length 0 0 0];
link.type   = 'RevoluteLink';
link.model  = '"./humanoid_box_model/shank.xan"';
link.offset = [0 -link.depth/2 -link.width/2];
link.scale  = [link.length link.depth,link.width];
shankLink = link;

% Ankle Link
clear link;
link.mass = 0;
link.com = [0,0,0];
link.I = zeros(3,3);
link.Ibar = link.I;
link.type = 'RevoluteLink';
link.mdh  = [shankLink.length	0	0	0];
link.model  = '"./humanoid_box_model/ankle.xan"';
link.contacts = [];
ankleLink = link;


% Foot Link
clear link;
%link.mass = .0145 * Mass;
link.mass = .63;
totalMass = totalMass + 2*link.mass;
link.length = .17;
link.width  = .1;
link.depth  = .01;
%%%%%%%%%%%%%%
link.ankleLocation = [0 0 .05];
link.contacts = [link.depth	link.width/2	0; ...
                 link.depth -link.width/2   0; ...
                 link.depth link.width/2    link.length; ...
                 link.depth -link.width/2   link.length] - ...
                [link.ankleLocation;link.ankleLocation; ...
                    link.ankleLocation;link.ankleLocation];
link.com    = [.5*link.depth 0 .5*link.length] - link.ankleLocation;
link.I      = 1/12 * link.mass * diag([link.length^2 + link.width^2, ...
                    link.length^2+link.depth^2, link.width^2+link.depth^2]);
link.Ibar   = link.I + link.mass*cross(link.com)*cross(link.com)';
link.type   = 'RevoluteLink';
link.mdh    = [0	pi/2	0	0];
link.model  = '"./humanoid_box_model/foot.xan"';
link.offset = [0 -link.width/2 0] - link.ankleLocation;
link.scale  = [link.depth link.width link.length];
footLink = link;


% Upper Arm
clear link;
link.mass = .028 * Mass;
totalMass = totalMass + 2*link.mass;
link.length = .25;
link.width  = .04;
link.depth  = .04;
link.com    = [.436*link.length 0 0];
link.I      = 1/12 * link.mass * diag([link.depth^2 + link.width^2, ...
                    link.length^2+link.depth^2, link.length^2+link.width^2]);
link.Ibar   = link.I + link.mass*cross(link.com)*cross(link.com)';
link.contacts = [link.length	0	0];
link.type   = 'QuaternionLink';
link.quat   = [0 0 0 1];
link.model  = '"./humanoid_box_model/upperArm.xan"';
link.offset = [0 -link.depth/2 -link.width/2];
link.scale  = [link.length link.depth,link.width];
upperArmLink = link;


% Fore Arm
clear link;
link.mass = .022 * Mass;
totalMass = totalMass + 2*link.mass;
link.length = .25;
link.width  = .04;
link.depth  = .04;
link.com    = [.682*link.length 0 0];
link.I      = 1/12 * link.mass * diag([link.depth^2 + link.width^2, ...
                    link.length^2+link.depth^2, link.length^2+link.width^2]);
link.Ibar   = link.I + link.mass*cross(link.com)*cross(link.com)';
link.contacts = [link.length	0	0];
link.type   = 'RevoluteLink';
link.mdh    = [upperArmLink.length	0	0	0];
link.model  = '"./humanoid_box_model/foreArm.xan"';
link.offset = [0 -link.depth/2 -link.width/2];
link.scale  = [link.length link.depth,link.width];
foreArmLink = link;