% topological sort
:TSort	LDA		$255,InOpen
				TRAP 	0,:Fopen,Fin
				LDA		$255, IOArgs
				TRAP	0,:Fread,Fin
				SET		size,SIZE
				LDA   left, Buffer+SIZE
				ADDU	right,left,4
				NEG		i,size
				LDT		n,right,i
				ADD		i,i,8
				SET		:avail,8
				8ADDU	:avail,n,:avail
				LDA		count,Base+COUNT
				LDA		top,Base+TOP
				SL		k,n,3
1H			STCO	0,k,count
				SUB		k,k,8
				PBNN	k,1B
				JMP		T2
T3			SL		k,k,3
				LDT		t,k,count
				ADD 	t,t,1
				STT		t,k,count
				SET		p,:avail
				ADD		:avail,:avail,8
				STT		k,suc,p
				SL		j,j,3
				LDTU	t,top,j
				STTU	t,next,p
				STTU	p,top,j
T2			LDT		j,left,i
				LDT		k,right,i
				ADD 	i,i,8
				PBNZ  j,T3
1H			BNP		i,T4
				TRAP  0,:Fread,Fin
				NEG		i,size
				JMP		T2
T4			TRAP	0,:Fclose,Fin
				SET		r,0
				SL		k,n,3
1H			LDT		t,k,count
				PBNZ  t,0F
				STT		k,qlink,r
				SET   r,k
0H			SUB   k,k,8
				PBP   k,1B
				LDT   f,qlink,0
				LDA   $255,OutOpen
				TRAP  0,:Fopen,Fout
				NEG   i,size
				JMP   T5
T5B			PBN		i,0F
				LDA		$255,IOArgs
				TRAP  0,:Fwrite,Fout
				NEG   i,size
0H			SUB   n,n,1
				LDTU  p,top,f
				BZ    p,T7
T6			LDT   s,suc,p
				LDT 	t,s,count
				SUB   t,t,1
				STT   t,s,count
				PBNZ  t,0F
				STT   s,qlink,r
				SET   r,s
0H			LDT   p,next,p
				PBNZ  p,T6
T7      LDT   f,qlink,f
T5			SR    t,f,3
				STT   t,left,i
				ADD   i,i,4
				PBNZ  f,T5B
T8      LDA   $255,IOArgs
				TRAP  0,:Fwrite,Fout
				TRAP  0,:Fclose,Fout
				POP		1,0
