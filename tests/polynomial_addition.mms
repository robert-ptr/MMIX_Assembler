% addition of polynomials
:Add 	SET		q1,q
		LDOU    q,q,LINK
0H		LDOU	p,p,LINK
		LDO     coefp,p,COEF
		LDO     abcp,p,ABC
2H		LDO     t,q,ABC
		CMP		t,abcp,t
		BZ		t,A3
		BP		t,A5
		SET		q1,q
		LDOU	q,q,LINK
		JMP		2B
A3		BN		abcp,6F
		LDO     coefq,q,COEF
		ADD     coefq,coefq,coefq
		STO     coefq,q,COEF
		PBNZ    coefq,:Add
		SET		q2,q
		LDOU    q,q,LINK
		STOU    q,q1,LINK
		STOU    :avail,q2,LINK
		SET		:avail,q2
		JMP     0B
A5		SET		q2,:avail
		LDOU	:avail,:avail,LINK
		STO     coefp,q2,COEF
		STOU    abcp,q2,ABC
		STOU    q,q2,LINK
		STOU    q2,q1,LINK
		SET     q1,q2
		JMP     0B
6H		POP     0,0
