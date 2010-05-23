# 011016 watanabe
# 020612 fcmpd, fcmps‚ÍŽg—p‚µ‚È‚¢‚±‚Æ‚É‚È‚Á‚½B

FP_LIB = adddf3 \
	 addsf3 \
	 divdf3 \
	 divsf3 \
	 extsfdf \
	 fcmpd \
	 fcmps \
	 fixdfi \
	 fixdfui \
	 fixsfi \
	 fixsfui \
	 flosidf \
	 flosisf \
	 muldf3 \
	 mulsf3 \
 	 negdf2 \
	 negsf2 \
	 scan64 \
	 trncdfsf

EMU_LIB = divsi3 \
	  modsi3 \
	  divhi3 \
	  modhi3

LIB1ASMFUNCS = $(FP_LIB) $(EMU_LIB)
#LIB1ASMFUNCS = $(FP_LIB)

GAS_FOR_TARGET = /gnu33/as
AR_FOR_TARGET = ar
AR_FLAGS_FOR_TARGET = rc

libgcc.a: 
	for name in $(LIB1ASMFUNCS); \
	do \
	  echo $${name}; \
	  $(GAS_FOR_TARGET) -alh $${name}.s -o $${name}.o >$${name}.lst; \
	  $(AR_FOR_TARGET) $(AR_FLAGS_FOR_TARGET) tmplibgcc1.a $${name}.o; \
	  rm -f $${name}.o; \
	done
	mv tmplibgcc1.a libgcc.a

