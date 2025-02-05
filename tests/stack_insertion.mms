LINK	IS		0								% offset of the LINK field
INFO	IS		8								% offset of the INFO field
		SET		p,:avail				% p <- AVAIL
		BZ		p,:Overflow			% Is AVAIL = DELTA?
		LDOU	:avail,p,LINK		% AVAIL <- LINK(P)
		STO		y,p,INFO				% INFO(P) <- Y
		LDOU	t,:T
		STOU	t,p,LINK				% LINK(P) <- T
		STOU	p,:T						% T <- P
