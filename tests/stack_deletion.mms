		LDOU	p,:T						% p <- T
		BZ		p,:Underflow		% is T = Delta?
		LDOU	t,p,LINK
		STOU	t,:T						% T <- LINK(P)
		LDO		y,p,INFO				% Y <- INFO(P)
		STOU	:avail,p,LINK		% LINK(P) <- AVAIL
		SET		:avail,p				% AVAIL <- P
