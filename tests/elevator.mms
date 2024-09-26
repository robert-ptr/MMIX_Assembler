%
LLINK1		IS 			0
RLINK1		IS 			8
NEXTINST  IS 			16
NEXTTIME  IS 			24
IN				IS 			30
OUT				IS 			31
LLINK2  	IS 			32
RLINK2 		IS 			40
					LOC			Data_Segment
WAIT			OCTA 		USER1,USER1,0,0
USER1			OCTA		WAIT,WAIT,U1,0
wait 			GREG  	WAIT
ELEV1 		OCTA  	0,0,E1,0
ELEV2 		OCTA  	0,0,E5,0
ELEV3  		OCTA  	0,0,E9,0
time			GREG  	0
c					GREG  	0
c0   			GREG  	0
queue 		GREG  	@-4*8
					OCTA  	@-4*8,@-4*8
					OCTA  	@-4*8,@-4*8
					OCTA  	@-4*8,@-4*8
					OCTA  	@-4*8,@-4*8
					OCTA  	@-4*8,@-4*8
elevator	GREG  	@-4*8
					OCTA  	@-4*8,@-4*8
callup		GREG  	0
calldown  GREG  	0
callcar   GREG  	0
off				IS			0
on				GREG  	1
floor			GREG  	0
d1				GREG  	0
d2 				GREG  	0
d3				GREG  	0
state			GREG  	0
dt				GREG  	0
fi				GREG  	0
fo				GREG  	0
tg 				GREG  	0
					LOC			0
					GET			$0,rX
					GET   	$1,rW
					SR			$2,$0,16
					AND			$2,$2,#FF
					GO			$2,$2,0
Cycle1		STOU		$1,c,NEXTINST
					JMP			Cycle
HoldCI		AND			dt,$0,#FF
HoldC     STOU		$1,c,NEXTINST
					PUSHJ		$0,Hold
Cycle 		LDOU  	c,wait,RLINK1
					LDTU  	time,c,NEXTTIME
					PUSHJ 	$0,DeleteW
					LDOU  	$0,c,NEXTINST
					PUT   	rW,$0
					RESUME 	0
					LOC			#100
					PREFIX	:queue:
p 				IS  		$0
q 				IS 			$1 
:Insert		LDOU 		q,p,:LLINK2
					STOU		q,:c,:LLINK2
					STOU    :c,p,:LLINK2
					STOU		:c,q,:RLINK2
					STOU    p,:c,:RLINK2
					POP     0,0
:Delete 	LDOU    p,:c,:LLINK2
					LDOU    q,:c,:RLINK2
					STOU    p,q,:LLINK2
					STOU    q,p,:RLINK2
					POP			0,0
					PREFIX  :wait:
tc				IS			$0
q 				IS      $1
p 				IS			$2
tp 				IS      $3 
t 				IS      $4
:Immed 		SET 		tc,:time
					STTU  	tc,:c,:NEXTTIME
					SET 		p,:wait
					JMP			2F
:Hold     ADDU    tc,:time,:dt
:SortIn		STTU    tc,:c,:NEXTTIME
					SET			p,:wait
1H				LDOU    p,p,:LLINK1
					LDTU 		tp,p,:NEXTTIME
					CMP			t,tp,tc
					BP			t,1B
2H				LDOU    q,p,:RLINK1
					STOU    q,:c,:RLINK1
					STOU 		p,:c,:LLINK1
					STOU    :c,p,:RLINK1
					STOU    :c,q,:LLINK1
					POP 		0,0
:DeleteW  LDOU 		p,:c,:LLINK1
					LDOU 		q,:c,:RLINK1
					STOU 		p,q,:LLINK1
					STOU 		q,p,:RLINK1
					POP 		0,0
					PREFIX :
U1				PUSHJ 	$0,Values
					PUSHJ		$0,Hold
					PUSHJ   $0,Allocate
					STB			fi,c,IN
					STB 		fo,c,OUT
U2				SET 		c0,c
					CMP 		$0,fi,floor
					BNZ 		$0,2F
					LDA			c,ELEV1
					LDOU		$0,c,NEXTINST
					GETA		$1,$6
					CMPU		$0,$0,$1
					BNZ			$0,3F
					GETA 		$0,E3
					STOU 		$0,c,NEXTINST
					PUSHJ		$0,DeleteW
					JMP 		4F
3H				BZ			d3,2F
					SET			d3,off
					SET			d1,on
4H				PUSHJ		$0,Immed
					JMP 		U3
2H				SL			$1,on,fi
					CMP			$0,fo,ri
					ZSP			$2,$0,$1
					OR			callup,callup,$2
					ZSN			$2,$0,$1
					OR			calldown,calldown,$2
					BZ			d2,0F
					LDOU 		$0,ELEV1+NEXTINST
					GETA		$1,E1
					CMP			$0,$0,$1
					BNZ 		$0,U3
0H				PUSHJ		$0,Decision
U3				SET			c,c0
					16ADDU	$1,fi,queue
					PUSHJ 	$0,Insert
U4A				SET			dt,tg
					TRIP		HoldC,0
U4				LDB			fi,c,IN
					CMP			$0,fi,floor
					BNZ			$0,U6
					BNZ			d1,U4A
U6				PUSHJ		$0,Delete
					PUSHJ		$0,Free
					TRIP 		Cycle,0
U5				PUSHJ		$0,Delete
					SET			$1,elevator
					PUSHJ		$0,Insert
					LDB			fo,c,OUT
					SL 			$0,on,fo
					OR			callcar,callcar,$0
					BZ			state,1F
					TRIP 		Cycle,0
1H				CMP			state,fo,floor
					LDA 		c,ELEV2
					PUSHJ 	$0,DeleteW
					TRIP 		HoldCI,25
					JMP E5
avail 		GREG 		0
poolmax		GREG		0
Allocate  PBNZ 		avail,1F
					SET 		c,poolmax
					ADDU 		poolmax,c,6*8
					POP     1,0
1H				SET 		c,avail
					LDOU 		avail,c,LLINK1
					POP 		1,0
Free			STOU 		avail,c,LLINK1
					SET 		avail,c
					POP 		0,0
E1A 			TRIP 		Cycle1,0
E1				IS 			@
E2A 			TRIP    HoldC,0
E2 				OR 			$0,callup,calldown
					OR 			$0,$0,callcar
					BN 			state,1F
					ADD 		$1,floor,1
					SR 			$2,$0,$1
					BNZ 		$2,E3
					NEG 		$1,64,floor
					SL 			$2,callcar,$1
					JMP 		2F
1H				NEG 		$1,64,floor
					SL 			$2,$0,$1
					BNZ 		$2,E3
					ADD 		$1,floor,1
					SR 			$2,callcar,$1
2H				NEG  		state,state
					CSZ 		state,$2,0
					SL 			$0,on,floor
					ANDN 		callup,callup,$0
					ANDN 		calldown,calldown,$0
					ANDN		callcar,callcar,$0
E3				LDA 		c,ELEV3
					LDO 		$0,c,LLINK1
					BZ 			$0,1F
					PUSHJ		$0,DeleteW
1H				SET 		dt,300
					PUSHJ		$0,Hold
					LDA 		c,ELEV2
					SET 		dt,76
					PUSHJ		$0,Hold
					SET 		d2,on
					SET			d1,on
					SET 		dt,20
E4A				LDA 		c,ELEV1
					TRIP 		HoldC,0
E4				LDA			$0,elevator
					LDA 		c,elevator
1H				LDOU 		c,c,LLINK2
					CMP 		$1,c,$0
					BZ 			$1,1F
					LDB 		$1,c,OUT
					CMP 		$1,$1,floor
					BNZ 		$1,1B
					GETA 		$0,U6
					JMP 		2F
1H 				16ADDU 	$0,floor,queue
					LDOU 		c,$0,RLINK2
					LDOU 		$1,c,RLINK2
					CMP 		$1,$1,c
					BZ 			$1,1F
					PUSHJ 	$0,DeleteW
					GETA 		$0,U5
2H 				STOU 		$0,c,NEXTINST
					PUSHJ 	$0,Immed
					SET  		dt,25
					JMP 		E4A
1H 			  SET 		d1,off
					SET 		d3,on
					TRIP 		Cycle,0
E5				BZ 			d1,0F
					TRIP 	  HoldCI,40
					JMP 		E5
0H				SET 		d3,off
					LDA 		c,ELEV1
					TRIP 		HoldCI,20
E6				SL			$0,on,floor
					ANDN		callcar,callcar,$0
					ZSNN		$1,state,$0
					ANDN 		callup,callup,$1
					ZSNP 		$1,state,$0
					ANDN 		calldown,calldown,$1
					PUSHJ 	$0,Decision
E6B 			BZ 			state,E1A
					BZ 			d2,0F
					LDA 		c,ELEV3
					PUSHJ 	$0,DeleteW
					STCO 	  0,c,LLINK1
0H				LDA 		c,ELEV1
					TRIP 		HoldCI,15
					BN 			state,E8
E7				ADD 		floor,floor,1
					TRIP 		HoldCI,51
					SL			$0,on,floor
					OR 			$1,callcar,callup
					AND 		$2,$1,$0
					BNZ			$2,1F
					CMP 		$2,floor,2
					BZ			$2,2F
					AND 		$2,calldown,$0
					BZ			$2,2F
					AND 		$2,calldown,$0
					BZ			$2,E7
2H				OR 			$1,$1,calldown
					ADD			$2,floor,1
					SR			$1,$1,$2
					BNZ 		$1,E7
1H				SET 		dt,14
					JMP 		E2A
E9				STCO 		0,c,LLINK1
					SET 		d2,off
					PUSHJ 	$0,Decision
					TRIP 		Cycle,0
Main			SET 		floor,2
					SET 		state,0
					SETH 		poolmax,Pool_Segment>>48
					TRIP 		Cycle,0,0
