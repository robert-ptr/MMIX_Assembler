; MMIX Simulator (Simplified)
t           IS      $255                ; volatile register for temporary info
lring_size  IS      256                 ; size of the local register ring
            LOC     Data_Segment        ; start at location #2000 0000 0000 0000
Global      LOC     @+8*256             ; 256 octabytes for global registers
g           GREG    Global              ; base address for globals
Local       LOC     @+8*lring_size      ; lring_size octabytes for local registers
l           GREG    Local               ; base address for locals
            GREG    @                   ; base address for IOArgs and Chunk0
IOArgs      OCTA    0,BinaryRead        
Chunk0      IS      @                   ; beginning of simulated memory area
            LOC     #100                ; put everything else in the text segment 
            PREFIX  :Mem:               ; begin private symbols for MemFind
head        GREG    0                   ; address of first chunk
curkey      GREG    0                   ; KEY(head)
alloc       GREG    0                   ; address of next chunk to allocate
Chunk       IS      #1000               ; bytes per chunk, must be a power of 2 
addr        IS      $0                  ; the given address A 
key         IS      $1                  ; its chunk address
test        IS      $2                  ; temporary register for key search
newlink     IS      $3                  ; the second most recently used node
p           IS      $4                  ; temporary pointer register
t           IS      :t                  ; external temporary register
KEY         IS      0               
LINK        IS      8
DATA        IS      16
nodesize    GREG    Chunk+3*8
mask        GREG    Chunk-1
:MemFind    ANDN    key,addr,mask
            CMPU    t,key,curkey
            PBZ     t,4F                ; branch if head is the right chunk 
            BN      addr,:Error         ; disallow negative addresses A 
            SET     newlink,head        ; prepare for the search loop
1H          SET     p,head              ; p <- head
            LDOU    head,p,LINK         ; head <- LINK(p)
            PBNZ    head,2F             ; branch if head != 0 
            SET     head,alloc          ; otherwise allocate a new node
            STOU    key,head,KEY
            ADDU    alloc,alloc,nodesize
            JMP     3F 
2H          LDOU    test,head,KEY
            CMPU    t,test,key
            BNZ     t,1B                ; loop back if KEY(head) != key
3H          LDOU    t,head,LINK         ; adjust pointers:  t <- LINK(head)
            STOU    newlink,head,LINK   ;                   LINK(head) <- newlink
            SET     curkey,key          ;                   curkey <- key
            STOU    t,p,LINK            ;                   LINK(p) <- t 
4H          SUBU    t,addr,key          ; t <- chunk offset
            LDA     $0,head,DATA        ; $0 <- address of DATA(head)
            ADDU    $0,t,$0 
            POP     1,0                 ; return R
            PREFIX  :                   ; end of the ':Mem:' prefix
res         IS      $2                  ; result register for PUSHJ
arg         IS      res+1               ; argument register for PUSHJ
ss          GREG    0                   ; the simulated stack pointer, rS
oo          GREG    0                   ; the simulated stack offset, rO
ll          GREG    0                   ; the simulated local threshold register, rL, times 8
gg          GREG    0                   ; the simulated global threshold register, rG, times 8
aa          GREG    0                   ; the simualted arithmetic status register, rA
ii          GREG    0                   ; the simulated interval counter, rI
uu          GREG    0                   ; the simulated usage counter, rU
cc          GREG    0                   ; the simulated cycle counter, rC
lring_mask  GREG    8*lring_size-1
:GetReg     CMPU    t,$0,gg             ; subroutine to get $k 
            BN      t,1F                ; branc if k < G 
            LDOU    $0,g,$0             ; otherwise $k is global; load g[k]
            POP     1,0                 ; return the result
1H          CMPU    t,$0,ll             ; t <- [$k is local]
            ADDU    $0,$0,oo            
            AND     $0,$0,lring_mask 
            LDOU    $0,l,$0             ; load l[(a + k) mod p]
            CSNN    $0,t,0              ; zero it if $k is marginal
            POP     1,0                 ; return the result
:StackStore GET     $0,rJ               ; save the return address 
            AND     t,ss,lring_mask
            LDOU    $1,l,t              ; $1 <- l[t]
            SET     arg,ss
            PUSHJ   res,MemFind
            STOU    $1,res,0            ; M_8[rS] <- $1
            ADDU    ss,ss,8             ; increase rS by 8 
            PUT     rJ,$0               ; restore the retrun address 
            POP     0                   ; return to caller
:StackLoad  GET     $0,rJ               ; save the return address 
            SUBU    ss,ss,8             ; decrease rS by 8
            SET     arg,ss
            PUSHJ   res,MemFind
            LDOU    $1,res,0            ; $1 <- M_8[rS]
            AND     t,ss,lring_mask 
            STOU    $1,l,t              ; l[t] <- $1 
            PUT     rJ,$0               ; restore the return address
            POP     0                   ; return to caller
:StackRoom  SUBU    t,ss,oo
            SUBU    t,t,ll 
            AND     t,t,lring_mask 
            PBNZ    t,1F                ; branch if(rS-rO)/8 != rL (modulo p)
            GET     $0,rJ               ; oops, we're not a leaf subroutine
            PUSHJ   res,StackStore      ; advance rS
            PUT     rJ,$0               ; restore the return address
1H          POP     0                   ; return to caller
; The main loop
loc         GREG    0                   ; where the simulator is at 
inst_ptr    GREG    0                   ; where the simulator will be next
inst        GREG    0                   ; the current instruction being simulated
resuming    GREG    0                   ; are we resuming an instruction in rX?
Fetch       PBZ     resuming,1F         ; branch if not resuming
            SUBU    loc,inst_ptr,4      ; loc <- inst_ptr - 4
            LDTU    inst,g,8*rX+4       ; inst <- right half of rX
            JMP     2F                  
1H          SET     loc,inst_ptr        ; loc <- inst_ptr
            SET     arg,loc 
            PUSHJ   res,MemFind
            LDTU    inst,res,0          ; inst <- M_4[loc] 
            ADDU    inst_ptr,loc,4      ; inst_ptr <- loc + 4 
2H          CMPU    t,loc,g 
            BNN     t,Error             ; branch if loc >= Data_Segment
