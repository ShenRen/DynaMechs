function [Js Ju] =  evalJacobian(stateInit,params, dynParams)
    

        robotLegLength = dynParams.robotLegLength;
        robotHipDisp = dynParams.robotHipDisp;

       
    
    
    [T_out STATE_out FOOT_out tf statef0 EN_out stanceTime0 tdPos] = simulatePeriod(0,stateInit,dynParams);
    
    
    ep = 1e-5;
    delStateMat = [0 0 1 0 0 0;0 0 0 1 0 0; 0 0 0 0 1 0]';
    Js = zeros(4,3);
    Ju = zeros(4,4);
    
    for i=1:3
        newStateInit = stateInit+ep*delStateMat(:,i);
        [T_out STATE_out FOOT_out tf statef EN_out stanceTime tdPos] = simulatePeriod(0,newStateInit,dynParams);
        
        Js(1:3,i) = (statef(3:5)-statef0(3:5))/ep;
        Js(4,i) = (stanceTime-stanceTime0)/ep;
    end
    
    
    delParamMat = eye(4);
    for i=1:4
        newParams = params+delParamMat(i,:)*ep;
        
        dynParams.tdParams = [newParams(1) newParams(2)];
        dynParams.heightThreshold = robotLegLength*cos(dynParams.tdParams(1));
        dynParams.L0 = sqrt(robotLegLength^2+robotHipDisp^2 +...
                            2*robotLegLength*robotHipDisp*sin(dynParams.tdParams(1))*sin(dynParams.tdParams(2)));
        dynParams.k1 = newParams(3)*1e4;
        dynParams.k2 = newParams(4)*1e4;
    
        [T_out STATE_out FOOT_out tf statef EN_out stanceTime tdPos] = simulatePeriod(0,stateInit,dynParams);
        
        Ju(1:3,i) = (statef(3:5)-statef0(3:5))/ep;
        Ju(4,i) = (stanceTime-stanceTime0)/ep;
    end
    
    

end

