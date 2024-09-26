% inverse in place
:Invert	SUBU	x,x,1
				SET		k,n
0H			LDB		i,x,k
				NEG		i,i
				STB		i,x,k
				SUB		k,k,1
				PBP		k,OB
				SET		m,n
2H			SET		i,m
0H			SET		j,i
				LDB		i,x,j
				PBP		i,0B
				NEG		i,i
				LDB		k,x,i
				STB		k,x,j
				STB		m,x,i
				SUB		m,m,i
				BP		m,2B
