% Multiply permutations in cycle form
				LOC 	Data_Segmentg
				GREG 	@
MAXP		IS		#2000 							% maximum number of permutations
InArg 	OCTA	Buffer,MAXP					% the arguments for Fread
Buffer 	BYTE	0										% Place for input and output
left		GREG	'('
right		GREG 	')'
				LOC 	#100
base		IS		$0									% base address of permutations
k				IS		$1									% index into input
j				IS 		$2									% index into output
x 			IS		$4									% some permutation
current IS	 	$5						
start 	IS		$6
size		IS		$7
t 			IS		$8
Main		LDA		$255,InArg					% prepare for input
				TRAP	0,Fread,StdIn				% read input
				SET		size,$255
				INCL 	size,MAXP						% size <- $255 + MAXP
				BNP		size,Fail						% chekc if input was OK
				LDA		base,Buffer
				ADDU	base,base,size			% base <- Buffer + size
				NEG		k,size							% A1. First pass
2H			LDBU 	current,k,base			% get next element of input
				CMP		t,current,#20			
				CSNP	current,t,0					% set format characters to zero
				STB 	current,k,base			
				CSZ 	start,start,current % remember first nonformat symbol
				CMP		t,current,'('				% is it '('?
				PBNZ	t,1F							
				ORL 	current,#80					% if so, tag it
				STBU  current,k,base			
				SET 	start,0							% reset first nonformat symbol
				JMP   0F				
1H			CMP		t,current,')'				% is it ')'?
				PBNZ  t,0F
				ORL		start,#80						% tag first nonformat symbol
				STBU	start,k,base				% and replace ')' by it
0H			ADD 	k,k,1
				PBN   k,2B								% have all elements been processed?
				SET   j,0
Open		NEG   k,size							% A2. Open.
1H			LDB 	x,k,base						% look for untagged element
				PBP   x,Go
				ADD		k,k,1
				PBN		k,1B
Done		BNZ 	j,0F								% is answer the identity permutation?
				STB   left,base,0					% if so, change to '()'.
				STB 	right,base,1
				SET   j,2
0H			SET		t,#0a								% add a newline
				STB		t,base,j
				ADD		j,j,1
				SET		t,0									% terminate the string
				STB   t,base,j
				SET 	$255,base
				TRAP  0,Fputs,StdOut			% print the answer
				SET		$255,0
Fail		TRAP	0,Halt,0						% halt program
Go			STB   left,base,j					% output '('.
				ADD		j,j,1
				STBU	x,base,j						% output X
				ADD		j,j,1
				SET		start,x
Succ		ORL		x,#80
				STBU	x,k,base						% tag X
3H			ADD		k,k,1								% A3. Set CURRENT
				LDBU	current,k,base
				ANDNL current,#80					% untag
				PBNZ	current,1F					% skip past blanks
				JMP 	3B
5H			STBU	current,base,j			% output CURRENT
				ADD		j,j,1
				NEG		k,size							% scan formula again
4H			LDBU	x,k,base						% A4. Scan formula.
				ANDNL	x,#80								% untag
				CMP		t,x,current
				BZ		t,Succ
1H			ADD		k,k,1								% move to right
				PBN		k,4B								% end of formula?
				CMP		t,start,current			% A5. CURRENT != START
				PBNZ	t,5B
				STBU	right,base,j				% A6. Close.
				SUB		j,j,2								% suppress singleton cycles
				LDB		t,base,j
				CMP		t,t,'('
				BZ		t,Open
				ADD		j,j,3
				JMP 	Open
