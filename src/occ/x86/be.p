/* Protogen Version 2.1.1.17Friday October 28, 2005  17:44:50 */

enum e_lk get_dll_linkage(SYMBOL*sym);                /* get dll linkage corresponding to command line switches */

                              /* Dbgfmt.c */

void dbginit(void);
void debug_outline(char *file, int lineno);           /* output a line number */
void debug_outputtypedef(SYMBOL*sym) ; /* dump the typedef HASHTABLE */

                                /* Gen.c */

AMODE *make_label(int lab);
AMODE *makesegreg(int seg);
AMODE *aimmed(ULLONG_TYPE i);
AMODE *aimmedt(long i, int size);
AMODE *make_offset(EXPRESSION *node);
AMODE *make_stack(int number);
void precolor(QUAD *head);			/* precolor an instruction */
int preRegAlloc(QUAD *ins, BRIGGS_SET *globals, BRIGGS_SET *eobGlobals, int pass);
int examine_icode(QUAD *head);
void cg_internal_conflict(QUAD *head);
void asm_line(QUAD *q)               ;
void asm_blockstart(QUAD *q)         ;
void asm_blockend(QUAD *q)           ;
void asm_varstart(QUAD *q)           ;
void asm_func(QUAD *q)               ;
void asm_passthrough(QUAD *q)        ;
void asm_datapassthrough(QUAD *q)    ;
void asm_label(QUAD *q)              ;
void asm_goto(QUAD *q)               ;
void asm_gosubprelude(QUAD *q)       ;
void asm_gosub(QUAD *q)              ;
void asm_gosubpostlude(QUAD *q)      ;
void asm_fargosub(QUAD *q)           ;
void asm_trap(QUAD *q)               ;
void asm_int(QUAD *q)                ;
void asm_ret(QUAD *q)                ;
void asm_fret(QUAD *q)                ;
void asm_rett(QUAD *q)               ;
void asm_add(QUAD *q)                ;
void asm_sub(QUAD *q)                ;
void asm_udiv(QUAD *q)               ;
void asm_umod(QUAD *q)               ;
void asm_sdiv(QUAD *q)               ;
void asm_smod(QUAD *q)               ;
void asm_muluh(QUAD *q)               ;
void asm_mulsh(QUAD *q)               ;
void asm_mul(QUAD *q)               ;
void asm_lsl(QUAD *q)                ;
void asm_lsr(QUAD *q)                ;
void asm_asr(QUAD *q)                ;
void asm_neg(QUAD *q)                ;
void asm_not(QUAD *q)                ;
void asm_and(QUAD *q)                ;
void asm_or(QUAD *q)                 ;
void asm_eor(QUAD *q)                ;
void asm_setne(QUAD *q)              ;
void asm_sete(QUAD *q)               ;
void asm_setc(QUAD *q)               ;
void asm_seta(QUAD *q)               ;
void asm_setnc(QUAD *q)              ;
void asm_setbe(QUAD *q)              ;
void asm_setl(QUAD *q)               ;
void asm_setg(QUAD *q)               ;
void asm_setle(QUAD *q)              ;
void asm_setge(QUAD *q)              ;
void asm_assn(QUAD *q)               ;
void asm_genword(QUAD *q)            ;
void asm_coswitch(QUAD *q)           ;
void asm_swbranch(QUAD *q)           ;
void asm_dc(QUAD *q)                 ;
void asm_assnblock(QUAD *q)          ;
void asm_clrblock(QUAD *q)           ;
void asm_cmpblock(QUAD *q)           ;
void asm_jc(QUAD *q)                 ;
void asm_ja(QUAD *q)                 ;
void asm_je(QUAD *q)                 ;
void asm_jnc(QUAD *q)                ;
void asm_jbe(QUAD *q)                ;
void asm_jne(QUAD *q)                ;
void asm_jl(QUAD *q)                 ;
void asm_jg(QUAD *q)                 ;
void asm_jle(QUAD *q)                ;
void asm_jge(QUAD *q)                ;
void asm_parm(QUAD *q)               ;
void asm_parmadj(QUAD *q)            ;
void asm_parmblock(QUAD *q)          ;
void asm_cppini(QUAD *q)             ;
void asm_prologue(QUAD *q)           ;
void asm_epilogue(QUAD *q)           ;
void asm_pushcontext(QUAD *q)        ;
void asm_popcontext(QUAD *q)         ;
void asm_loadcontext(QUAD *q)        ;
void asm_unloadcontext(QUAD *q)        ;
void asm_tryblock(QUAD *q)			 ;
void asm_stackalloc(QUAD *q)         ;
void asm_loadstack(QUAD *q)			;
void asm_savestack(QUAD *q)			;
void asm_blockstart(QUAD *q)         ;
void asm_blockend(QUAD *q)           ;
void asm_functail(QUAD *q, int begin, int size);
void asm_atomic(QUAD *q);
                               /* Inasm.c */

void inasmini(void);
int inasm_enterauto(EXPRESSION *node, int *reg1, int *reg2);
int isrm(AMODE *ap, int dreg_allowed);
AMODE *getimmed(void);
enum e_opcode inasm_op(void);
LEXEME *inasm_statement(LEXEME *inlex, BLOCKDATA *parent);
void *inlineAsmStmt(void *);
                              /* Invoke.c */

int InsertExternalFile(char *name, bool primary);
void InsertOutputFileName(const char *name);
int RunExternalFiles(char *);

                              /* Object.c */

void omfInit(void);
int put_ident(char *buf, int ident);
void omf_dump_browsedata(BROWSEINFO *bri);
void omf_dump_browsefile(BROWSEFILE *brf);
void omf_globaldef(SYMBOL*sym);
void omf_put_extern(SYMBOL*sym, int code);
void omf_put_impfunc(SYMBOL*sym, char *file);
void omf_put_expfunc(SYMBOL*sym);
void omf_put_includelib(const char *name);
void omfFileName(void);
void omfTranslatorName(void);
void omfFileTime(char *file);
void omfDebugMarker(void);
void omfLNames(void);
void omfLibMod(void);
void omfSegs(void);
void omfComDefs(void);
void omfGroups(void);
void FlushExtBuf(void);
void omfputext(SYMBOL*sym);
void omfExtDefs(void);
void omfputimport(SYMBOL*sym);
void omfImports(void);
int omfgetseg(SYMBOL*sym);
void omfputpub(SYMBOL*sym);
void omfpubdumphash(HASHTABLE *syms);
void omfPublics(void);
void omfputexport(SYMBOL*sym);
void omfExports(void);
void omfPassSeperator(void);
void omfComDefData(void);
void omfData(void);
void omfSourceFile(char *file, int num);
void omfLineNumbers(char *file);
void omfEmitLineInfo(void);
void omfModEnd(void);
void output_obj_file(void);

                              /* Outasm.c */

void oa_ini(void);
void oa_nl(void);
void outop(char *name);
void putop(enum e_opcode op, AMODE *aps, AMODE *apd, int nooptx);
void oa_putconst(int op, int sz, EXPRESSION *offset, bool doSign);
void oa_putlen(int l);
void putsizedreg(char *string, int reg, int size);
void pointersize(int size);
void putseg(int seg, int usecolon);
int islabeled(EXPRESSION *n);
void oa_putamode(int op, int szalt, AMODE *ap);
void oa_put_code(OCODE *cd);
void oa_gen_strlab(SYMBOL*sym);
void oa_put_label(int lab);
void oa_put_string_label(int lab, int type);
void oa_genfloat(enum e_gt type, FPF *val);
void oa_genstring(LCHAR *str, int len);
void oa_genint(enum e_gt type, LLONG_TYPE val);
void oa_genaddress(ULLONG_TYPE val);
void oa_gensrref(SYMBOL*sym, int val, int type);
void oa_genref(SYMBOL*sym, int offset);
void oa_genlabref(int label, int offset);
void oa_genpcref(SYMBOL*sym, int offset);
void oa_genstorage(int nbytes);
void oa_gen_labref(int n);
void oa_gen_labdifref(int n1, int n2);
void oa_exitseg(enum e_sg seg);
void oa_enterseg(enum e_sg seg);
void oa_gen_virtual(SYMBOL*sym, int data);
void oa_gen_endvirtual(SYMBOL*sym);
void oa_gen_vtt(VTABENTRY *entry, SYMBOL*func);
void oa_gen_vc1(SYMBOL*func);
void oa_gen_importThunk(SYMBOL*func);
void oa_align(int size);
void oa_setalign(int code, int data, int bss, int constant);
long queue_large_const(unsigned constant[], int count);
long queue_muldivval(long number);
long queue_floatval(FPF *number, int size);
void dump_muldivval(void);
void dump_browsedata(BROWSEINFO *bri);
void dump_browsefile(BROWSEFILE *brf);
void oa_header(const char *filename, const char *compiler_version);
void oa_trailer(void);
void oa_adjust_codelab(void *select, int offset);
void oa_globaldef(SYMBOL*sym);
void oa_localdef(SYMBOL*sym);
void oa_localstaticdef(SYMBOL*sym);
void oa_output_alias(char *name, char *alias);
void oa_put_extern(SYMBOL*sym, int code);
void oa_put_impfunc(SYMBOL*sym, char *file);
void oa_put_expfunc(SYMBOL*sym);
void oa_output_includelib(const char *name);
void oa_end_generation(void);

                              /* Outcode.c */

void outcode_file_init(void);
void outcode_func_init(void);
void compile_start(char *name);
void include_start(char *name, int num);
void InsertLabel(int lbl, int address, int seg);
int LabelAddress(int lbl);
int LabelSeg(int lbl);
void outcode_enterseg(int seg);
void InsertLine(int address, LINEDATA *linedata);
void emit(int seg, void *data, int len);
void write_to_seg(int seg, int offset, void *value, int len);
void gen_symbol_fixup(enum mode xmode, int seg, int address, SYMBOL*pub);
void outcode_dump_muldivval(void);
void outcode_gen_strlab(SYMBOL*sym);
void outcode_genref(SYMBOL*sym, int offset);
void outcode_gen_labref(int n);
void outcode_gen_labdifref(int n1, int n2);
void outcode_gensrref(SYMBOL*sym, int val);
void outcode_genstorage(int len);
void outcode_genfloat(FPF *val);
void outcode_gendouble(FPF *val);
void outcode_genlongdouble(FPF *val);
void outcode_genstring(LCHAR *string, int len);
void outcode_genbyte(int val);
void outcode_genword(int val);
void outcode_genlong(int val);
void outcode_genlonglong(LLONG_TYPE val);
void outcode_align(int size);
void outcode_put_label(int lab);
void outcode_start_virtual_seg(SYMBOL*sym, int data);
void outcode_end_virtual_seg(SYMBOL*sym);
EXPRESSION *GetSymRef(EXPRESSION *n);
int resolveoffset(EXPRESSION *n, int *resolved);
int asmrm(int reg, OCODE *ins, AMODE *data, UBYTE **p);
int asmfrm(int reg, OCODE *ins, AMODE *data, UBYTE **p);
int AOP0(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP1(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP2(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP3(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP4(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP5(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP6(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP7(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP8(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP9(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP10(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP11(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP12(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP13(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP14(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP15(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP16(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP17(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP18(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP19(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP20(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP21(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP22(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP23(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP24(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP25(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP26(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP27(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP28(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP29(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP30(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP31(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP32(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP33(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP34(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP35(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP36(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP37(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP38(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP40(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP39(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP41(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP42(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP43(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP44(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP45(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP46(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP47(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP48(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP50(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP49(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP51(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP52(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP53(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP54(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP55(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP56(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP57(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP58(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP59(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP60(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP61(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP62(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP63(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP64(OPCODE *descript, OCODE *data, UBYTE **p);
int AOP65(OPCODE *descript, OCODE *data, UBYTE **p);
int outcode_AssembleIns(OCODE *ins, int address);
void outcode_optimize(OCODE *peeplist);
void outcode_dumpIns(OCODE *peeplist);
void outcode_gen(OCODE *peeplist);

                               /* Peep.c */

void o_peepini(void);
AMODE *makedregSZ(int r, char size);
AMODE *makedreg(int r);
AMODE *makefreg(int r);
AMODE *copy_addr(AMODE *ap);
OCODE *gen_code(int op, AMODE *ap1, AMODE *ap2);
OCODE *gen_code_sse(int ops, int opd, int sz, AMODE *ap1, AMODE *ap2);
OCODE* gen_code_sse_imm(int ops, int opd, int sz, AMODE* ap1, AMODE* ap2, AMODE *ap3);
OCODE *gen_code3(int op, AMODE *ap1, AMODE *ap2, AMODE *ap3);
OCODE *gen_codes(int op, int len, AMODE *ap1, AMODE *ap2);
void gen_coden(int op, int len, AMODE *ap1, AMODE *ap2);
void gen_codefs(int op, int len, AMODE *ap1, AMODE *ap2);
void gen_codef(int op, AMODE *ap1, AMODE *ap2);
void gen_codes2(int op, int len, AMODE *ap1, AMODE *ap2);
void gen_code2(int op, int len1, int len2, AMODE *ap1, AMODE *ap2);
void gen_codelab(SYMBOL*lab);
void gen_branch(int op, int label);
void gen_comment(char *txt);
void add_peep(OCODE *newop);
void oa_gen_label(int labno);
void flush_peep(SYMBOL*funcsp, QUAD *list);
void peep_add(OCODE *ip);
void peep_sub(OCODE *ip);
int makeshl(EXPRESSION *node, int scale, int *rv);
void peep_lea2(OCODE *ip, AMODE *ap);
void peep_lea(OCODE *ip);
void peep_lea3(OCODE *ip);
void peep_signextend(OCODE *ip);
void replacereg(AMODE *ap, int reg, int reg2);
OCODE *peep_test(OCODE *ip);
void peep_cmp(OCODE *ip);
void peep_mov(OCODE *ip);
void peep_and(OCODE *ip);
OCODE *peep_neg(OCODE *ip);
void peep_btr(OCODE *ip);
OCODE *peep_jmp(OCODE *ip);
void peep_uctran(OCODE *ip);
void peep_label(OCODE *ip);
int novalue(OCODE *ip);
int equal_address(AMODE *ap1, AMODE *ap2);
int single_bit(int val);
void peep_prefixes(OCODE *ip);
void peep_call(OCODE *ip);
void peep_pop(OCODE *ip);
void remove_peep_entry(OCODE *ip);
void oa_peep(void);
