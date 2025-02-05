j       IS $0	            % j
m       IS $1	            % m
kk      IS $2	            % 8k
xk      IS $3	            % X[k]
t       IS $255	            % Temp storage
        LOC #100
Maximum SL, kk, $0, 3	    %M1. Initialize. k=n, j=n
	    LDO m,x0,kk			% m=X[n]
	    JMP DecrK			% To M2 with k=n-1
Loop    LDO xk, x0, kk		%M3. Compare.
	    CMP t, xk, m		%t=[X[k] > m] -[X[k] < m].
	    PBNP t,DecrK		%To M5 if X[k] <= m		
ChangeM SET m,xk			% N4, Change m. m=X[k]
	    SR j,kk,3			% j=k
DecrK   SUB kk,kk,8			%M5. Decrease k. k=k-1
	    PBP kk,Loop			%M2. All tested? To M3 if k > 0.
	    POP 2,0			    % Return to main program.
