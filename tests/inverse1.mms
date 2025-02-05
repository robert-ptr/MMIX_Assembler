% inverse in place
:Invert	    SUBU 	x,x,1			% x <- LOC(X[0])
			SET 	m,n				% I1. Initialize
			NEG		j,1				% j <- -1
2H			LDB		i,x,m			% I2. Next element. i <- X[m]
			BN		i,5F			% To I5 if i < 0
3H			STB		j,x,m			% I3. Invert one. X[m] <- j
			NEG		j,m				% j <- -m
			SET		m,i				% m <- i
			LDB		i,x,m			% i <- X[m]
4H			PBP		i,3B			% I4. End of cycle? To I3 if i > 0
			SET		i,j				% otherwise set i <- j
5H			NEG		i,i				% I5. Store final value. i <- -i
			STB		i,x,m			% X[m] <- i
6H			SUB		m,m,1			% I6. Loop on m
			BP		m,2B			% To I2 if m > 0
