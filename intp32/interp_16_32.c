/* NOTE:
 * This file was modified for DOSEMU by the DOSEMU-team.
 * The original is 'Copyright 1997 Willows Software, Inc.' and generously
 * was put under the GNU Library General Public License.
 * ( for more information see http://www.willows.com/ )
 *
 * We make use of section 3 of the GNU Library General Public License
 * ('...opt to apply the terms of the ordinary GNU General Public License...'),
 * because the resulting product is an integrated part of DOSEMU and
 * can not be considered to be a 'library' in the terms of Library License.
 * The (below) original copyright notice from Willows therefore was edited
 * conforming to section 3 of the GNU Library General Public License.
 *
 * Nov. 1 1997, The DOSEMU team.
 */


/*    
	interp_16_32.c	1.13
    	Copyright 1997 Willows Software, Inc. 

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this program; see the file COPYING.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.


For more information about the Willows Twin Libraries.

	http://www.willows.com	

To send email to the maintainer of the Willows Twin Libraries.

	mailto:twin@willows.com 

changes for use with dosemu-0.67 1997/10/20 vignani@tin.it
changes for use with dosemu-0.99 1998/12/13 vignani@tin.it
 */

#include "emu-globv.h"

#include "hsw_interp.h"
#include "mod_rm.h"

extern long instr_count;
extern int emu_under_X;
#ifdef EMU_STAT
extern long InsFreq[];
#endif


BYTE *
hsw_interp_16_32 (ENVPARAMS, BYTE *P0, register BYTE *__PC, int *err)
{
    BYTE *mem_ref;
#if defined(__GNUC__) && defined(__i386__)
    register BYTE *PC asm("%esi");
#else
    register BYTE *PC;
#endif
    *err = 0;
    PC = __PC;

#ifndef NO_TRACE_MSGS
    if (d_emu) e_trace(ENVARGS, P0, PC, 1);
#endif
override:
#ifdef EMU_STAT
    InsFreq[*((WORD *)PC)] += 1;
#endif
    switch (*PC) {
/*00*/	case ADDbfrm: {
	    DWORD src1, src2, res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *HREG1; mem_ref = MEM_REF; src1 = *mem_ref;
	    *mem_ref = res = src1 + src2;
	    SETBFLAGS(0);
	    } return (PC); 
/*01*/	case ADDwfrm: {
	    DWORD src1, src2, res;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = *XREG1; mem_ref = MEM_REF; 
	    src1 = mFETCH_WORD(mem_ref);
		res = src1 + src2;
	    mPUT_WORD(mem_ref, res);
	    SETWFLAGS(0);
	    } return (PC); 
/*02*/	case ADDbtrm: {
	    DWORD src1, src2, res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *MEM_REF; mem_ref = HREG1; src1 = *mem_ref;
	    *mem_ref = res = src1 + src2;
	    SETBFLAGS(0);
	    } return (PC); 
/*03*/	case ADDwtrm: {
	    DWORD src1, src2, res;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = *XREG1; mem_ref = MEM_REF; 
	    src1 = mFETCH_WORD(mem_ref);
	    *XREG1 = res = src1 + src2;
	    SETWFLAGS(0);
	    } return (PC); 
/*04*/	case ADDbia: return (PC);
/*05*/	case ADDwia: {
	    DWORD src1, src2, res;
	    src1 = AX; src2 = FETCH_WORD((PC+1));
	    res = src1 + src2;
		AX = (WORD)res;
	    SETWFLAGS(0);
	    } PC += 3; return (PC);
/*06*/	case PUSHes: {
	    WORD temp = SHORT_ES_16;
	    PUSHWORD(temp);
	    } PC += 1; return (PC);
/*07*/	case POPes: {	/* restartable */
	    DWORD temp;
	    temp = TOS_WORD;
	    if ((*err = SET_SEGREG(LONG_ES,BIG_ES,MK_ES,temp))) {
		iVAR->error_addr=temp&0xfffc; return P0; }
	    POPWORD(temp);
	    SHORT_ES_32 = temp; }
	    PC += 1; return (PC);
/*08*/	case ORbfrm: {
	    DWORD res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    mem_ref = MEM_REF;
	    res = *mem_ref | *HREG1;
	    *mem_ref = (BYTE)res;
	    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
	    } return (PC); 
/*09*/	case ORwfrm: {
	    DWORD res;
	    PC += hsw_modrm_32_word(MODARGS);
	    mem_ref = MEM_REF; 
	    res = mFETCH_WORD(mem_ref) | *XREG1;
	    mPUT_WORD(mem_ref, res);
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } return (PC); 
/*0a*/	case ORbtrm: {
	    DWORD res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    res = *HREG1 | *MEM_REF;
	    *HREG1 = (BYTE)res;
	    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
	    } return (PC); 
/*0b*/	case ORwtrm: {
	    DWORD res;
	    PC += hsw_modrm_32_word(MODARGS);
	    res = mFETCH_WORD(MEM_REF) | *XREG1;
	    *XREG1 = (WORD)res;
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } return (PC); 
/*0c*/	case ORbi: return (PC);
/*0d*/	case ORwi: {
	    DWORD res;
	    res = AX | FETCH_WORD((PC+1));
	    AX = (WORD)res;
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } PC += 3; return (PC);
/*0e*/	case PUSHcs: {
	    WORD temp = SHORT_CS_16;
	    PUSHWORD(temp);
	    } PC += 1; return (PC);
/*0f*/	case TwoByteESC: {
	    BYTE op;
	    switch ((op=*(PC+1))) {
		case 0x00: /* GRP6 */
		    switch ((*(PC+2)>>3)&7) {
			case 0: /* SLDT */ {
			    /* Store Local Descriptor Table Register */
			    int temp;
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    mem_ref = MEM_REF;
			    temp = 0 /* should be LDT selector */;
			    mPUT_WORD(mem_ref,temp);
			    } return (PC);
			case 1: /* STR */ {
			    /* Store Task Register */
			    int temp;
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    mem_ref = MEM_REF;
			    temp = 0 /* should be Task Register */;
			    mPUT_WORD(mem_ref,temp);
			    } return (PC);
			case 2: /* LLDT */ /* Privileged */
			    /* Load Local Descriptor Table Register */
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    return (PC);
			case 3: /* LTR */ {
			    /* Load Task Register */
			    int temp;
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    mem_ref = MEM_REF;
			    temp = mFETCH_WORD(mem_ref);
		goto not_implemented;
			    /* hsw_task_register = temp; */
			    } 
			case 4: /* VERR */ {
			    int temp;
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    temp = mFETCH_WORD(MEM_REF);
			    if (hsw_verr(temp)) SET_ZF;
				else CLEAR_ZF;
			    } return (PC);
			case 5: /* VERW */ {
			    int temp;
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    temp = mFETCH_WORD(MEM_REF);
			    if (hsw_verw(temp)) SET_ZF;
				else CLEAR_ZF;
			    } return (PC);
			case 6: /* Illegal */
			case 7: /* Illegal */
		goto illegal_op;
			default: goto not_implemented;
		    }
		case 0x01: /* GRP7 */
		    switch ((*(PC+2)>>3)&7) {
			case 0: /* SGDT */ {
			    /* Store Global Descriptor Table Register */
			    int temp;
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    mem_ref = MEM_REF;
			    e_printf("16/32 SGDT %#x\n",(int)mem_ref);
			    temp = 0; /* should be LIMIT field */;
			    PUT_WORD(mem_ref,temp);
			    temp = 0; /* should be BASE field (4 bytes) */
			    PUT_QUAD(mem_ref+2,temp);
			    } return (PC);
			case 1: /* SIDT */ {
			    /* Store Interrupt Descriptor Table Register */
			    int temp;
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    mem_ref = MEM_REF;
			    e_printf("16/32 SIDT %#x\n",(int)mem_ref);
			    temp = 0; /* should be LIMIT field */;
			    PUT_WORD(mem_ref,temp);
			    temp = 0; /* should be BASE field (4 bytes) */
			    PUT_QUAD(mem_ref+2,temp);
			    } return (PC);
			case 2: /* LGDT */ /* Privileged */
			    /* Load Global Descriptor Table Register */
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    goto not_implemented;
			case 3: /* LIDT */ /* Privileged */
			    /* Load Interrupt Descriptor Table Register */
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
		goto not_implemented;
			case 4: /* SMSW */ {
			    /* Store Machine Status Word */
			    int temp;
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    mem_ref = MEM_REF;
			    e_printf("16/32 SMSW %#x\n",(int)mem_ref);
			    temp = 0 /* should be LIMIT field */;
			    mPUT_WORD(mem_ref,temp);
			    } return (PC);
			case 5: /* Illegal */
			    goto illegal_op; 
			case 6: /* LMSW */ /* Privileged */
			    /* Load Machine Status Word */
			    PC += 1; PC += hsw_modrm_32_word(MODARGS);
			    goto not_implemented;
			case 7: /* Illegal */
			    goto illegal_op; 
			default: goto not_implemented;
		    }
		case 0x02: /* LAR */ {
		    /* Load Access Rights Byte */
		    int temp;
                        WORD _sel;
		    PC += 1; PC += hsw_modrm_32_word(MODARGS);
                    mem_ref = MEM_REF;
		    _sel = mFETCH_WORD(mem_ref);
		    if (IS_GDT(_sel)||((_sel>>3)>LDT_ENTRIES))
			CLEAR_ZF;
		    else {
			temp = (GetSelectorFlags(_sel) & 0xff) << 8;
			if (temp) SetFlagAccessed(_sel);
                            PUT_XREG(mem_ref,temp);
			SET_ZF;
		    }
		    } return(PC); 
		case 0x03: /* LSL */ {
		    /* Load Segment Limit */
		    int temp;
                        WORD _sel;
		    PC += 1; PC += hsw_modrm_32_word(MODARGS);
                    mem_ref = MEM_REF;
		    _sel = mFETCH_WORD(mem_ref);
		if (IS_GDT(_sel)||((_sel>>3)>LDT_ENTRIES))
			CLEAR_ZF;
                else {
		temp= (WORD)GetSelectorByteLimit(_sel);
                        PUT_XREG(mem_ref,temp);
			SET_ZF;
                }
                    /* what do I do here??? */
		    } return(PC); 
		case 0x06: /* CLTS */ /* Privileged */
		    /* Clear Task State Register */
		    PC += 2; return (PC);
		case 0x08: /* INVD */
		    /* INValiDate cache */
		    PC += 2; return (PC);
		case 0x09: /* WBINVD */
		    /* Write-Back and INValiDate cache */
		    PC += 2; return (PC);
		case 0x20: /* MOVcdrd */ /* Privileged */
		goto not_implemented;

		case 0x21: /* MOVddrd */ /* Privileged */
		goto not_implemented;

		case 0x22: /* MOVrdcd */ /* Privileged */
		goto not_implemented;
		
		case 0x23: /* MOVrddd */ /* Privileged */
		goto not_implemented;
	
		case 0x24: /* MOVtdrd */ /* Privileged */
		goto not_implemented;

		case 0x26: /* MOVrdtd */ /* Privileged */
		goto not_implemented;

		case 0x80: /* JOimmdisp */
		    if (IS_OF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x81: /* JNOimmdisp */
		    if (!IS_OF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x82: /* JBimmdisp */
		    if (IS_CF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x83: /* JNBimmdisp */
		    if (!IS_CF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x84: /* JZimmdisp */
		    if (IS_ZF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x85: /* JNZimmdisp */
		    if (!IS_ZF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x86: /* JBEimmdisp */
		    if (IS_CF_SET || IS_ZF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x87: /* JNBEimmdisp */
		    if (!(IS_CF_SET || IS_ZF_SET)) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x88: /* JSimmdisp */
		    if (IS_SF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x89: /* JNSimmdisp */
		    if (!IS_SF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x8a: /* JPimmdisp */
		    if (IS_PF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x8b: /* JNPimmdisp */
		    if (!IS_PF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x8c: /* JLimmdisp */
		    if (IS_SF_SET ^ IS_OF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x8d: /* JNLimmdisp */
		    if (!(IS_SF_SET ^ IS_OF_SET)) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x8e: /* JLEimmdisp */
		    if ((IS_SF_SET ^ IS_OF_SET) || IS_ZF_SET) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x8f: /* JNLEimmdisp */
		    if (!(IS_SF_SET ^ IS_OF_SET) && !(IS_ZF_SET)) {
			PC += (6 + (int)FETCH_QUAD(PC+2));
			return (PC);
		    } PC += 6; return (PC);
		case 0x90: /* SETObrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (IS_OF_SET) ? 1 : 0;
		    } return (PC);
		case 0x91: /* SETNObrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (!IS_OF_SET) ? 1 : 0;
		    } return (PC);
		case 0x92: /* SETBbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (IS_CF_SET) ? 1 : 0;
		    } return (PC);
		case 0x93: /* SETNBbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (!IS_CF_SET) ? 1 : 0;
		    } return (PC);
		case 0x94: /* SETZbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (IS_ZF_SET) ? 1 : 0;
		    } return (PC);
		case 0x95: /* SETNZbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (!IS_ZF_SET) ? 1 : 0;
		    } return (PC);
		case 0x96: /* SETBEbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (IS_CF_SET || IS_ZF_SET) ? 1 : 0;
		    } return (PC);
		case 0x97: /* SETNBEbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (!(IS_CF_SET || IS_ZF_SET)) ? 1 : 0;
		    } return (PC);
		case 0x98: /* SETSbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (IS_SF_SET) ? 1 : 0;
		    } return (PC);
		case 0x99: /* SETNSbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (!(IS_SF_SET)) ? 1 : 0;
		    } return (PC);
		case 0x9a: /* SETPbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (IS_PF_SET) ? 1 : 0;
		    } return (PC);
		case 0x9b: /* SETNPbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (!IS_PF_SET) ? 1 : 0;
		    } return (PC);
		case 0x9c: /* SETLbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (IS_SF_SET ^ IS_OF_SET) ? 1 : 0;
		    } return (PC);
		case 0x9d: /* SETNLbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (!(IS_SF_SET ^ IS_OF_SET)) ? 1 : 0;
		    } return (PC);
		case 0x9e: /* SETLEbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = ((IS_SF_SET ^ IS_OF_SET) || IS_ZF_SET) ? 1 : 0;
		    } return (PC);
		case 0x9f: /* SETNLEbrm */ {
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    *MEM_REF = (!(IS_SF_SET ^ IS_OF_SET) && !(IS_ZF_SET)) ? 1 : 0;
		    } return (PC);
		case 0xa0: /* PUSHfs */ {
		    WORD temp = SHORT_FS_16;
		    PUSHWORD(temp);
		    } PC += 2; return (PC);
		case 0xa1: /* POPfs */ {
		    DWORD temp;
		    temp = TOS_WORD;
		    if ((*err = SET_SEGREG(LONG_FS,BIG_FS,MK_FS,temp))) {
			iVAR->error_addr=temp&0xfffc; return P0; }
		    POPWORD(temp);
		    SHORT_FS_32 = temp;
		    } PC += 2; return (PC);
                case 0xa3: /* BT */ {
		    DWORD temp, ind1;
		    long ind2;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    mem_ref = MEM_REF; ind2 = *XREG1;
		    ind2 = ((ind2 << 16) >> 16);
                    if (IS_MODE_REG) {
			ind2 = (ind2 & 0xf);
			temp = FETCH_XREG(mem_ref);
			CARRY = ((int)temp >> ind2)&1;
                    } else {
			if(ind2 >= 0) {
			    ind1 = (ind2 >> 4) << 1;
                            mem_ref += ind1;
                            temp = FETCH_WORD(mem_ref);
			    ind2 = ind2 & 0xf;
			    CARRY = (WORD)((temp >> ind2) & 0x1);
                        } else {
			    ind2 = -ind2 - 1;
			    ind1 = ((ind2 >> 4) + 1) << 1;
                            mem_ref -= ind1;
                            temp = FETCH_WORD(mem_ref);
			    ind2 = ind2 & 0xf;
			    CARRY = (((temp << ind2) & 0x8000)? 1:0);
                        }
                    }          
                    } return(PC);
		case 0xa4: /* SHLDimm */
		    /* Double Precision Shift Left by IMMED */
		case 0xa5: /* SHLDcl */ {
		    /* Double Precision Shift Left by CL */
		    unsigned int count;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    if (op==0xa4)
			{ count = *PC & 0x1f; PC++; }
		    else
			count = CL & 0x1f;
		    if (count) {
			register DWORD temp; WORD carry;
			mem_ref = MEM_REF;
			/* CF <- [dest(MEM_REF)][ src(REG1) ]
			 * if C==0 no flag change
			 * if C==1 change only OF
			 * if C>1  OF undef,change CF
			 * C modulo 32 */
			/* combine into 32-bit int, low word=src high=dest */
			temp = mFETCH_WORD(mem_ref);
			if (count==1) SRC1_16=SRC2_16=temp;
			temp = (temp<<16) | *XREG1;
			carry = (temp >> (32-count)) & 1;
			temp = (temp << count) >> 16;
			mPUT_WORD(mem_ref, temp);
			RES_16 = temp;
			CARRY=carry;
		    }
		    } return (PC);
		case 0xa6: /* CMPXCHGb */	/* NOT IMPLEMENTED !!!!!! */
		case 0xa7: /* CMPXCHGw */	/* NOT IMPLEMENTED !!!!!! */
		goto not_implemented;
		case 0xa8: /* PUSHgs */ {
		    WORD temp = SHORT_GS_16;
		    PUSHWORD(temp);
		    } PC += 2; return (PC);
		case 0xa9: /* POPgs */ {
		    DWORD temp;
		    temp = TOS_WORD;
		    if ((*err = SET_SEGREG(LONG_GS,BIG_GS,MK_GS,temp))) {
			iVAR->error_addr=temp&0xfffc; return P0; }
		    POPWORD(temp);
		    SHORT_GS_32 = temp;
		    } PC += 2; return (PC);
                case 0xab: /* BTS */ {
		    DWORD temp, ind1;
		    long ind2;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    mem_ref = MEM_REF; ind2 = *XREG1;
		    ind2 = ((ind2 << 16) >> 16);
                    if (IS_MODE_REG) {
			ind2 = (ind2 & 0xf);
                        temp = *(WORD *)mem_ref;
			CARRY = ((int)temp >> ind2)&1;
			temp |= (0x1 << ind2);
			*(WORD *)mem_ref = (WORD)temp;
                    } else {
			if(ind2 >= 0) {
			    ind1 = ((ind2 >> 4) << 1);
                            mem_ref += ind1;
                            temp = FETCH_WORD(mem_ref);
			    ind2 = ind2 & 0xf;
			    CARRY = (WORD)((temp >> ind2) & 0x1);
			    temp |= (0x1 << ind2);
                            PUT_WORD(mem_ref,temp);
                        } else {
			    ind2 = -ind2 - 1;
			    ind1 = ((ind2 >> 4) + 1) << 1;
                            mem_ref -= ind1;
                            temp = FETCH_WORD(mem_ref);
			    ind2 = ind2 & 0xf;
			    CARRY = (((temp << ind2) & 0x8000)? 1:0);
			    temp |= (0x8000 >> ind2);
                            PUT_WORD(mem_ref,temp);
                        }
                    }
                    } return(PC);
		case 0xac: /* SHRDimm */
		    /* Double Precision Shift Right by IMMED */
		case 0xad: /* SHRDcl */ {
		    /* Double Precision Shift Right by CL */
		    unsigned int count;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    if (op==0xac)
			{ count = *PC & 0x1f; PC++; }
		    else
			count = CL & 0x1f;
		    if (count>0 && count<32) {
			register DWORD temp; WORD carry;
			mem_ref = MEM_REF;
			/* [ src(REG1) ][dest(MEM_REF)] -> CF
			 * if C==0 no flag change
			 * if C==1 change only OF
			 * if C>1  OF undef,change CF
			 * C modulo 32 */
			/* combine into 32-bit int, low word=dest high=src */
			temp = mFETCH_WORD(mem_ref);
			if (count==1) SRC1_16=SRC2_16=temp;
			temp |= (*XREG1 << 16);
			carry = (temp >> (count-1)) & 1;
			temp = (temp >> count);
			mPUT_WORD(mem_ref, temp);
			RES_16 = temp;
			CARRY=carry;
		    }
		    } return (PC);
                case 0xaf: { /* IMULregrm */
			int res, src1, src2;
			PC++; PC += hsw_modrm_32_word(MODARGS);
			src1 = (signed short)*XREG1;
			src2 = (signed short)mFETCH_WORD(MEM_REF);
                        res = src1 * src2;
			*XREG1 = res;
			res >>= 15;	/* not 16! */
			if ((res==0)||(res==0xffffffff)) {  /* CF=OF=0 */
			    CARRY=0; SRC1_16 = SRC2_16 = RES_16; /* 000,111 */
			}
			else {
			    CARRY=1; SRC1_16 = SRC2_16 = ~RES_16;  /* 001,110 */
			}
                        } return(PC);
		case 0xb2: /* LSS */ {
		    int temp;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    if (IS_MODE_REG) {
			/* Illegal */
		goto illegal_op; 
		    }
		    mem_ref = MEM_REF;
		    temp = FETCH_WORD(mem_ref+2);
		    if ((*err = SET_SEGREG(LONG_SS,BIG_SS,MK_SS,temp))) {
			iVAR->error_addr=temp&0xfffc; return P0; }
		    *XREG1 = FETCH_WORD(mem_ref);
		    SHORT_SS_32 = temp;
		    } return (PC);
                case 0xb3: /* BTR */ {
		    DWORD temp, ind1;
		    long ind2;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    mem_ref = MEM_REF; ind2 = *XREG1;
		    ind2 = ((ind2 << 16) >> 16);
                    if (IS_MODE_REG) {
			ind2 = (ind2 & 0xf);
                        temp = *(WORD *)mem_ref;
			CARRY = ((int)temp >> ind2)&1;
			temp &= ~(0x1 << ind2);
			*(WORD *)mem_ref = (WORD)temp;
                    } else {
			if(ind2 >= 0) {
			    ind1 = ((ind2 >> 4) << 1);
                            mem_ref += ind1;
                            temp = FETCH_WORD(mem_ref);
			    ind2 = ind2 & 0xf;
			    CARRY = (WORD)((temp >> ind2) & 0x1);
			    temp &= ~(0x1 << ind2);
                            PUT_WORD(mem_ref,temp);
                        } else {
			    ind2 = -ind2 - 1;
			    ind1 = ((ind2 >> 4) + 1) << 1;
                            mem_ref -= ind1;
                            temp = FETCH_WORD(mem_ref);
			    ind2 = ind2 & 0xf;
			    CARRY = (((temp << ind2) & 0x8000)? 1:0);
			    temp &= ~(0x8000 >> ind2);
                            PUT_WORD(mem_ref,temp);
                        }
                    }
                    } return(PC);
		case 0xb4: /* LFS */ {
		    int temp;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    if (IS_MODE_REG) {
			/* Illegal */
		goto illegal_op; 
		    }
		    mem_ref = MEM_REF;
		    temp = FETCH_WORD(mem_ref+2);
		    if ((*err = SET_SEGREG(LONG_FS,BIG_FS,MK_FS,temp))) {
			iVAR->error_addr=temp&0xfffc; return P0; }
		    *XREG1 = FETCH_WORD(mem_ref);
		    SHORT_FS_32 = temp;
		    } return (PC);
		case 0xb5: /* LGS */ {
		    int temp;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    if (IS_MODE_REG) {
			/* Illegal */
		goto illegal_op; 
		    }
		    mem_ref = MEM_REF;
		    temp = FETCH_WORD(mem_ref+2);
		    if ((*err = SET_SEGREG(LONG_GS,BIG_GS,MK_GS,temp))) {
			iVAR->error_addr=temp&0xfffc; return P0; }
		    *XREG1 = FETCH_WORD(mem_ref);
		    SHORT_GS_32 = temp;
		    } return (PC);
		case 0xb6: /* MOVZXb */ {
		    DWORD temp;
		    int ref = (*(PC+2)>>3)&7;
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    temp = *(BYTE *)MEM_REF;
		    switch (ref) {
		      case 0: AX = (WORD)temp; return (PC);
		      case 1: CX = (WORD)temp; return (PC);
		      case 2: DX = (WORD)temp; return (PC);
		      case 3: BX = (WORD)temp; return (PC);
		      case 4: SP = (WORD)temp; return (PC);
		      case 5: BP = (WORD)temp; return (PC);
		      case 6: SI = (WORD)temp; return (PC);
		      case 7: DI = (WORD)temp; return (PC);
		    }
		    }
		case 0xb7: { /* MOVZXw */
		    DWORD temp;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
                    mem_ref = MEM_REF;
		    temp = mFETCH_WORD(mem_ref);
                    *XREG1 = (WORD)temp;
		    } return(PC);
		case 0xba: /* GRP8 */
		    switch ((*(PC+2)>>3)&7) {
			case 0: /* Illegal */
			case 1: /* Illegal */
			case 2: /* Illegal */
			case 3: /* Illegal */
		goto illegal_op; 
                        case 4: /* BT */ {
			    int temp,temp1;
			    PC++; PC += hsw_modrm_32_word(MODARGS);
                            mem_ref = MEM_REF; temp = *PC;  PC++;
			    temp1 = mFETCH_WORD(mem_ref);
                            CARRY = ((int)temp1>>(int)( temp & 0xf))&1;
                    } return(PC);
                        case 5: /* BTS */ {
			    int temp,temp1;
			    PC++; PC += hsw_modrm_32_word(MODARGS);
                            mem_ref = MEM_REF; temp = (*PC) & 0xf;  PC++;
			    temp1 = mFETCH_WORD(mem_ref);
                                CARRY = (temp1 >> temp)&1;
                                temp1 |= (0x1 << temp);
			    mPUT_WORD(mem_ref,temp1);
                    } return(PC);
                        case 6: /* BTR */ {
			    int temp,temp1;
			    PC++; PC += hsw_modrm_32_word(MODARGS);
                            mem_ref = MEM_REF; temp = (*PC) & 0xf;  PC++;
			    temp1 = mFETCH_WORD(mem_ref);
                                CARRY = (temp1 >> temp)&1;
                                temp1 &= ~(0x1 << temp);
			    mPUT_WORD(mem_ref,temp1);
                    } return(PC);
                        case 7: /* BTC */ {
			    int temp,temp1;
			    PC++; PC += hsw_modrm_32_word(MODARGS);
                            mem_ref = MEM_REF; temp = (*PC) & 0xf;  PC++;
			    temp1 = mFETCH_WORD(mem_ref);
                                CARRY = (temp1 >> temp)&1;
                                temp1 ^= (0x1 << temp);
			    mPUT_WORD(mem_ref,temp1);
                    } return(PC);
                    }
                case 0xbb: /* BTC */ {
		    DWORD temp, ind1;
		    long ind2;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    mem_ref = MEM_REF; ind2 = *XREG1;
		    ind2 = ((ind2 << 16) >> 16);
                    if (IS_MODE_REG) {
			ind2 = (ind2 & 0xf);
                        temp = *(WORD *)mem_ref;
			CARRY = ((int)temp >> ind2)&1;
			temp ^= (0x1 << ind2);
			*(WORD *)mem_ref = (WORD)temp;
                    } else {
			if(ind2 >= 0) {
			    ind1 = ((ind2 >> 4) << 1);
                            mem_ref += ind1;
                            temp = FETCH_WORD(mem_ref);
			    ind2 = ind2 & 0xf;
			    CARRY = (WORD)((temp >> ind2) & 0x1);
			    temp ^= (0x1 << ind2);
                            PUT_WORD(mem_ref,temp);
                        } else {
			    ind2 = -ind2 - 1;
			    ind1 = ((ind2 >> 4) + 1) << 1;
                            mem_ref += ind1;
                            temp = FETCH_WORD(mem_ref);
			    ind2 = ind2 & 0xf;
			    CARRY = (((temp << ind2) & 0x8000)? 1:0);
			    temp ^= (0x8000 >> ind2);
                            PUT_WORD(mem_ref,temp);
                        }
                    }
                    } return(PC);
                case 0xbc: /* BSF */ {
                    DWORD temp, i;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
                    mem_ref = MEM_REF;
		    temp = mFETCH_WORD(mem_ref);
		    if(temp) {
                    for(i=0; i<16; i++)
                        if((temp >> i) & 0x1) break;
		    *XREG1 = (WORD)i; RES_16 = 1;
		    } else RES_16 = 0;
                    } return(PC);
                case 0xbd: /* BSR */ {
		    DWORD temp; int i;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
                    mem_ref = MEM_REF;
		    temp = mFETCH_WORD(mem_ref);
		    if(temp) { 
                    for(i=15; i>=0; i--)
                        if((temp & ( 0x1 << i))) break;
                    *XREG1 = i; RES_16 = 1;
		    } else RES_16 = 0;
                    } return(PC);
		case 0xbe: /* MOVSXb */ {
		    signed long temp; 
		    int ref = (*(PC+2)>>3)&7;
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
		    temp = *(signed char *)MEM_REF;
		    switch (ref) {
		      case 0: AX = (WORD)temp; return (PC); /* check asm! */
		      case 1: CX = (WORD)temp; return (PC);
		      case 2: DX = (WORD)temp; return (PC);
		      case 3: BX = (WORD)temp; return (PC);
		      case 4: SP = (WORD)temp; return (PC);
		      case 5: BP = (WORD)temp; return (PC);
		      case 6: SI = (WORD)temp; return (PC);
		      case 7: DI = (WORD)temp; return (PC);
		    }
		    }
		case 0xbf: { /* MOVSXw */
		    PC++; PC += hsw_modrm_32_word(MODARGS);
		    *XREG1 = mFETCH_WORD(MEM_REF);
		    } return(PC); 
                case 0xc0: { /* XADDb */
		    int res,src1,src2;
		    PC++; PC += hsw_modrm_32_byte(MODARGS);
                    src2 = *HREG1; mem_ref = MEM_REF; src1 = *mem_ref;
                    *HREG1 = src1;
                    *mem_ref = res = src1 + src2;
                    RES_32 = res << 8; BYTE_FLAG = BYTE_OP;
                    SRC1_8 = src1; SRC2_8 = src2;
                    } return(PC);
                case 0xc1: { /* XADDw */
		    int res,src1,src2;
		    PC++; PC += hsw_modrm_32_word(MODARGS);
                    src2 = *XREG1; mem_ref = MEM_REF;
		    src1 = mFETCH_WORD(mem_ref);
                        res = src1 + src2;
		    mPUT_WORD(mem_ref, res);
                    *XREG1 = src1;
                    RES_32 = res; SRC1_16 = src1; SRC2_16 = src2;
                    } return(PC);
		default: goto not_implemented;
		}
	    }

/*10*/	case ADCbfrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *HREG1; mem_ref = MEM_REF; src1 = *mem_ref;
	    *mem_ref = res = src1 + src2 + (CARRY & 1);
	    SETBFLAGS(0);
	    } return (PC); 
/*11*/	case ADCwfrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = *XREG1; mem_ref = MEM_REF; 
	    src1 = mFETCH_WORD(mem_ref);
	    res = src1 + src2 + (CARRY & 1);
	    mPUT_WORD(mem_ref, res);
	    SETWFLAGS(0);
	    } return (PC); 
/*12*/	case ADCbtrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *MEM_REF; mem_ref = HREG1; src1 = *mem_ref;
	    *mem_ref = res = src1 + src2 + (CARRY & 1);
	    SETBFLAGS(0);
	    } return (PC); 
/*13*/	case ADCwtrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = *XREG1; mem_ref = MEM_REF; 
	    src1 = mFETCH_WORD(mem_ref);
	    *XREG1 = res = src1 + src2 + (CARRY & 1);
	    SETWFLAGS(0);
	    } return (PC); 
/*14*/	case ADCbi: return (PC);
/*15*/	case ADCwi: {
	    int res, src1, src2;
	    src1 = AX; 
	    src2 = (FETCH_WORD((PC+1)));
	    res = src1 + src2 + (CARRY & 1);
	    AX = (WORD)res;
	    SETWFLAGS(0);
	    } PC += 3; return (PC);
/*16*/	case PUSHss: {
	    WORD temp = SHORT_SS_16;
	    PUSHWORD(temp);
	    } PC += 1; return (PC);
/*17*/	case POPss: {
	    DWORD temp;
	    POPWORD(temp);
	    if ((*err = SET_SEGREG(LONG_SS,BIG_SS,MK_SS,temp))) {
		PUSHWORD(temp); iVAR->error_addr=temp&0xfffc; return P0; }
	    SHORT_SS_32 = temp;
	    } PC += 1; return (PC);
/*18*/	case SBBbfrm: {
	    int res, src1, src2;
	    int carry = CARRY & 1;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *HREG1; mem_ref = MEM_REF; src1 = *mem_ref;
	    *mem_ref = res = src1 - src2 - carry;
	    SETBFLAGS(1);
	    } return (PC); 
/*19*/	case SBBwfrm: {
	    int res, src1, src2;
	    int carry = CARRY & 1;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = *XREG1; mem_ref = MEM_REF; 
	    src1 = mFETCH_WORD(mem_ref);
	    res = src1 - src2 - carry;
	    mPUT_WORD(mem_ref, res);
	    SETWFLAGS(1);
	    } return (PC); 
/*1a*/	case SBBbtrm: {
	    int res, src1, src2;
	    int carry = CARRY & 1;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *MEM_REF; mem_ref = HREG1; src1 = *mem_ref;
	    *mem_ref = res = src1 - src2 - carry;
	    SETBFLAGS(1);
	    } return (PC); 
/*1b*/	case SBBwtrm: {
	    int res, src1, src2;
	    int carry = CARRY & 1;
	    PC += hsw_modrm_32_word(MODARGS);
	    src1 = *XREG1; mem_ref = MEM_REF; 
	    src2 = mFETCH_WORD(mem_ref);
	    *XREG1 = res = src1 - src2 - carry;
	    SETWFLAGS(1);
	    } return (PC); 
/*1c*/	case SBBbi: return (PC);
/*1d*/	case SBBwi: {
	    int res, src1, src2;
	    int carry = CARRY & 1;
	    src1 = AX; 
	    src2 = (FETCH_WORD((PC+1)));
	    res = src1 - src2 - carry;
	    AX = (WORD)res;
	    SETWFLAGS(1);
	    } PC += 3; return (PC);
/*1e*/	case PUSHds: {
	    WORD temp = SHORT_DS_16;
	    PUSHWORD(temp);
	    } PC += 1; return (PC);
/*1f*/	case POPds: {
	    WORD temp;
	    temp = TOS_WORD;
	    if ((*err = SET_SEGREG(LONG_DS,BIG_DS,MK_DS,temp))) {
		iVAR->error_addr=temp&0xfffc; return P0; }
	    POPWORD(temp);
	    SHORT_DS_32 = temp;
	    } PC += 1; return (PC);

/*20*/	case ANDbfrm: {
	    DWORD res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    mem_ref = MEM_REF;
	    res = *mem_ref & *HREG1;
	    *mem_ref = (BYTE)res;
	    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
	    } return (PC); 
/*21*/	case ANDwfrm: {
	    DWORD res;
	    PC += hsw_modrm_32_word(MODARGS);
	    mem_ref = MEM_REF; 
	    res = mFETCH_WORD(mem_ref) & *XREG1;
	    mPUT_WORD(mem_ref, res);
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } return (PC); 
/*22*/	case ANDbtrm: {
	    DWORD res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    res = *HREG1 & *MEM_REF;
	    *HREG1 = (BYTE)res;
	    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
	    } return (PC); 
/*23*/	case ANDwtrm: {
	    DWORD res;
	    PC += hsw_modrm_32_word(MODARGS);
	    res = mFETCH_WORD(MEM_REF) & *XREG1;
	    *XREG1 = (WORD)res;
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } return (PC); 
/*24*/	case ANDbi: return (PC);
/*25*/	case ANDwi: {
	    DWORD res;
	    res = AX & FETCH_WORD((PC+1));
	    AX = (WORD)res;
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } PC += 3; return (PC);
/*26*/	case SEGes:
	    if (!VM86F && (SHORT_ES_16 < 4 || LONG_ES==INVALID_OVR)) {
		e_printf0("General Protection Fault: zero ES\n");
		goto bad_override;
	    }
	    OVERR_DS = OVERR_SS = LONG_ES;
	    PC += 1; goto override;
/*27*/	case DAA: return (PC);
/*28*/	case SUBbfrm: {
	    DWORD src1, src2; int res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *HREG1; mem_ref = MEM_REF; src1 = *mem_ref;
	    *mem_ref = res = src1 - src2;
	    SETBFLAGS(1);
	    } return (PC); 
/*29*/	case SUBwfrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = *XREG1; mem_ref = MEM_REF; 
	    src1 = mFETCH_WORD(mem_ref);
		res = src1 - src2;
	    mPUT_WORD(mem_ref, res);
	    SETWFLAGS(1);
	    } return (PC); 
/*2a*/	case SUBbtrm: {
	    DWORD src1, src2; int res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *MEM_REF; mem_ref = HREG1; src1 = *mem_ref;
	    *mem_ref = res = src1 - src2;
	    SETBFLAGS(1);
	    } return (PC); 
/*2b*/	case SUBwtrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_word(MODARGS);
	    src1 = *XREG1; mem_ref = MEM_REF; 
	    src2 = mFETCH_WORD(mem_ref);
	    *XREG1 = res = src1 - src2;
	    SETWFLAGS(1);
	    } return (PC); 
/*2c*/	case SUBbi: return (PC);
/*2d*/	case SUBwi: {
	    DWORD res, src2; WORD src1;
	    src1 = AX; 
	    src2 = FETCH_WORD((PC+1));
	    res = src1 - src2;
	    AX = (WORD)res;
	    SETWFLAGS(1);
	    } PC += 3; return (PC);
/*2e*/	case SEGcs:
	    OVERR_DS = OVERR_SS = LONG_CS;
	    PC+=1; goto override;
/*2f*/	case DAS: return (PC);

/*30*/	case XORbfrm: {
	    DWORD res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    mem_ref = MEM_REF;
	    res = *mem_ref ^ *HREG1;
	    *mem_ref = (BYTE)res;
	    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
	    } return (PC);
/*31*/	case XORwfrm: {
	    DWORD res;
	    PC += hsw_modrm_32_word(MODARGS);
	    mem_ref = MEM_REF; 
	    res = mFETCH_WORD(mem_ref) ^ *XREG1;
	    mPUT_WORD(mem_ref, res);
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } return (PC);
/*32*/	case XORbtrm: {
	    DWORD res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    res = *MEM_REF ^ *HREG1;
	    *HREG1 = (BYTE)res;
	    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
	    } return (PC);
/*33*/	case XORwtrm: {
	    DWORD res;
	    PC += hsw_modrm_32_word(MODARGS);
	    res = mFETCH_WORD(MEM_REF) ^ *XREG1;
	    *XREG1 = (WORD)res;
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } return (PC);
/*34*/	case XORbi: return (PC);
/*35*/	case XORwi: {
	    DWORD res;
	    res = AX ^ FETCH_WORD((PC+1));
	    AX = (WORD)res;
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } PC += 3; return (PC);
/*36*/	case SEGss:
	    OVERR_DS = OVERR_SS = LONG_SS;
	    PC+=1; goto override;
/*37*/	case AAA: return (PC);
/*38*/	case CMPbfrm: {
	    DWORD src1, src2; int res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *HREG1; src1 = *MEM_REF;
	    res = src1 - src2;
	    SETBFLAGS(1);
	    } return (PC); 
/*39*/	case CMPwfrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = *XREG1; mem_ref = MEM_REF; 
	    src1 = mFETCH_WORD(mem_ref);
	    res = src1 - src2;
	    SETWFLAGS(1);
	    } return (PC); 
/*3a*/	case CMPbtrm: {
	    DWORD src1, src2; int res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *MEM_REF; src1 = *HREG1;
	    res = src1 - src2;
	    SETBFLAGS(1);
	    } return (PC); 
/*3b*/	case CMPwtrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_word(MODARGS);
	    src1 = *XREG1; mem_ref = MEM_REF; 
	    src2 = mFETCH_WORD(mem_ref);
	    res = src1 - src2;
	    SETWFLAGS(1);
	    } return (PC); 
/*3c*/	case CMPbi: return (PC);
/*3d*/	case CMPwi: {
	    int res, src1;
	    long src2;
	    src1 = AX;
	    src2 = FETCH_WORD((PC+1));
	    res = src1 - src2;
	    SETWFLAGS(1);
	    } PC += 3; return (PC);
/*3e*/	case SEGds:
	    if (!VM86F && (SHORT_DS_16 < 4 || LONG_DS==INVALID_OVR)) {
		e_printf0("General Protection Fault: zero DS\n");
		goto bad_override;
	    }
	    OVERR_DS = OVERR_SS = LONG_DS;
	    PC+=1; goto override;
/*3f*/	case AAS: return (PC);

/*40*/	case INCax: {
	    register int temp = EAX;
	    SRC1_16 = temp; SRC2_16 = 1;
	    RES_16 = AX = temp + 1;
	    } PC += 1; return (PC);
/*41*/	case INCcx: {
	    register int temp = ECX;
	    SRC1_16 = temp; SRC2_16 = 1;
	    RES_16 = CX = temp + 1;
	    } PC += 1; return (PC);
/*42*/	case INCdx: {
	    register int temp = EDX;
	    SRC1_16 = temp; SRC2_16 = 1;
	    RES_16 = DX = temp + 1;
	    } PC += 1; return (PC);
/*43*/	case INCbx: {
	    register int temp = EBX;
	    SRC1_16 = temp; SRC2_16 = 1;
	    RES_16 = BX = temp + 1;
	    } PC += 1; return (PC);
/*44*/	case INCsp: {
	    register int temp = ESP;
	    SRC1_16 = temp; SRC2_16 = 1;
	    RES_16 = SP = temp + 1;
	    } PC += 1; return (PC);
/*45*/	case INCbp: {
	    register int temp = EBP;
	    SRC1_16 = temp; SRC2_16 = 1;
	    RES_16 = BP = temp + 1;
	    } PC += 1; return (PC);
/*46*/	case INCsi: {
	    register int temp = ESI;
	    SRC1_16 = temp; SRC2_16 = 1;
	    RES_16 = SI = temp + 1;
	    } PC += 1; return (PC);
/*47*/	case INCdi: {
	    register int temp = EDI;
	    SRC1_16 = temp; SRC2_16 = 1;
	    RES_16 = DI = temp + 1;
	    } PC += 1; return (PC);
/*48*/	case DECax: {
	    register int temp = EAX;
	    SRC1_16 = temp; SRC2_16 = -1;
	    RES_16 = AX = temp - 1;
	    } PC += 1; return (PC);
/*49*/	case DECcx: {
	    register int temp = ECX;
	    SRC1_16 = temp; SRC2_16 = -1;
	    RES_16 = CX = temp - 1;
	    } PC += 1; return (PC);
/*4a*/	case DECdx: {
	    register int temp = EDX;
	    SRC1_16 = temp; SRC2_16 = -1;
	    RES_16 = DX = temp - 1;
	    } PC += 1; return (PC);
/*4b*/	case DECbx: {
	    register int temp = EBX;
	    SRC1_16 = temp; SRC2_16 = -1;
	    RES_16 = BX = temp - 1;
	    } PC += 1; return (PC);
/*4c*/	case DECsp: {
	    register int temp = ESP;
	    SRC1_16 = temp; SRC2_16 = -1;
	    RES_16 = SP = temp - 1;
	    } PC += 1; return (PC);
/*4d*/	case DECbp: {
	    register int temp = EBP;
	    SRC1_16 = temp; SRC2_16 = -1;
	    RES_16 = BP = temp - 1;
	    } PC += 1; return (PC);
/*4e*/	case DECsi: {
	    register int temp = ESI;
	    SRC1_16 = temp; SRC2_16 = -1;
	    RES_16 = SI = temp - 1;
	    } PC += 1; return (PC);
/*4f*/	case DECdi: {
	    register int temp = EDI;
	    SRC1_16 = temp; SRC2_16 = -1;
	    RES_16 = DI = temp - 1;
	    } PC += 1; return (PC);

/*50*/	case PUSHax: {
	    PUSHWORD(AX);
	    } PC += 1; return (PC);
/*51*/	case PUSHcx: {
	    PUSHWORD(CX);
	    } PC += 1; return (PC);
/*52*/	case PUSHdx: {
	    PUSHWORD(DX);
	    } PC += 1; return (PC);
/*53*/	case PUSHbx: {
	    PUSHWORD(BX);
	    } PC += 1; return (PC);
/*54*/	case PUSHsp: {
	    register WORD wtemp = SP;
	    PUSHWORD(wtemp);
	    } PC += 1; return (PC);
/*55*/	case PUSHbp: {
	    PUSHWORD(BP);
	    } PC += 1; return (PC);
/*56*/	case PUSHsi: {
	    PUSHWORD(SI);
	    } PC += 1; return (PC);
/*57*/	case PUSHdi: {
	    PUSHWORD(DI);
	    } PC += 1; return (PC);
/*58*/	case POPax: POPWORD(AX); PC += 1; return (PC);
/*59*/	case POPcx: POPWORD(CX); PC += 1; return (PC);
/*5a*/	case POPdx: POPWORD(DX); PC += 1; return (PC);
/*5b*/	case POPbx: POPWORD(BX); PC += 1; return (PC);
/*5c*/	case POPsp: {
	    register WORD wtemp;
	    POPWORD(wtemp);
	    SP = wtemp;
	    } PC += 1; return (PC);
/*5d*/	case POPbp: POPWORD(BP); PC += 1; return (PC);
/*5e*/	case POPsi: POPWORD(SI); PC += 1; return (PC);
/*5f*/	case POPdi: POPWORD(DI); PC += 1; return (PC);

/*60*/	case PUSHA: {
	    register BYTE *sp = LONG_SS+(BIG_SS? ESP:SP);
	    sp -= 16;
	    PUT_WORD(sp, DI);
	    PUT_WORD(sp+2, SI);
	    PUT_WORD(sp+4, BP);
	    PUT_WORD(sp+6, SP);
	    PUT_WORD(sp+8, BX);
	    PUT_WORD(sp+10, DX);
	    PUT_WORD(sp+12, CX);
	    PUT_WORD(sp+14, AX);
	    if (BIG_SS) ESP=sp-LONG_SS; else SP=sp-LONG_SS;
	    } PC += 1; return (PC);
/*61*/	case POPA: {
	    register BYTE *sp = LONG_SS+(BIG_SS? ESP:SP);
	    DI = FETCH_WORD(sp);
	    SI = FETCH_WORD(sp+2);
	    BP = FETCH_WORD(sp+4);
	    BX = FETCH_WORD(sp+8);	/* discard SP */
	    DX = FETCH_WORD(sp+10);
	    CX = FETCH_WORD(sp+12);
	    AX = FETCH_WORD(sp+14);
	    sp += 16;
	    if (BIG_SS) ESP=sp-LONG_SS; else SP=sp-LONG_SS;
	    } PC += 1; return (PC);
/*62*/	case BOUND: 
/*63*/	case ARPL:
	    goto not_implemented;
/*64*/	case SEGfs:
	    if (!VM86F && (SHORT_FS_16 < 4 || LONG_FS==INVALID_OVR)) {
		e_printf0("General Protection Fault: zero FS\n");
		goto bad_override;
	    }
	    OVERR_DS = OVERR_SS = LONG_FS;
	    PC+=1; goto override;
/*65*/	case SEGgs:
	    if (!VM86F && (SHORT_GS_16 < 4 || LONG_GS==INVALID_OVR)) {
		e_printf0("General Protection Fault: zero GS\n");
		goto bad_override;
	    }
	    OVERR_DS = OVERR_SS = LONG_GS;
	    PC+=1; goto override;
/*66*/	case OPERoverride:	/* 0x66: 32 bit operand, 16 bit addressing */
	    if (!data32) {
		if (d_emu>4) e_printf0("ENTER interp_32d_32a\n");
		data32 = 1;
		PC = hsw_interp_32_32 (ENVARGS, P0, PC+1, err, 1);
		data32 = 0;
		return (PC);
	    }
	    PC += 1;
	    goto override;
/*67*/	case ADDRoverride:	/* 0x67: 16 bit operand, 32 bit addressing */
	    if (code32) {
		if (d_emu>4) e_printf0("ENTER interp_16d_16a\n");
		code32 = 0;
		PC = hsw_interp_16_16 (ENVARGS, P0, PC+1, err, 1);
		code32 = 1;
		return (PC);
	    }
	    PC += 1;
	    goto override;
/*68*/	case PUSHwi: {
	    WORD temp;
	    temp = FETCH_WORD(PC+1);
	    PUSHWORD(temp);
	    } PC += 3; return (PC);
/*69*/	case IMULwrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = (signed short)FETCH_WORD(PC); PC += 2; /* imm16 */
	    src1 = (signed short)mFETCH_WORD(MEM_REF);    /* r/m16 */
	    res = src1 * src2;
	    *XREG1 = (WORD)res;
	    if ((res >= -32768) && (res <= 32767)) {  /* CF=OF=0 */
		CARRY=0; SRC1_16 = SRC2_16 = RES_16; /* 000,111 */
	    }
	    else {
		CARRY=1; SRC1_16 = SRC2_16 = ~RES_16;  /* 001,110 */
	    }
	    } return (PC); 
/*6a*/	case PUSHbi: return (PC);
/*6b*/	case IMULbrm: {
	    int res, src1, src2;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = *(signed char *)(PC); PC += 1;	  /* imm8 */
	    src1 = (signed short)mFETCH_WORD(MEM_REF);	  /* r/m16 */
	    res = src1 * src2;
	    *XREG1 = (WORD)res;
	    if ((res >= -32768) && (res <= 32767)) {  /* CF=OF=0 */
		CARRY=0; SRC1_16 = SRC2_16 = RES_16; /* 000,111 */
	    }
	    else {
		CARRY=1; SRC1_16 = SRC2_16 = ~RES_16;  /* 001,110 */
	    }
	    } return (PC); 
/*6c*/	case INSb:
/*6d*/	case INSw:
/*6e*/	case OUTSb:
/*6f*/	case OUTSw:
	    goto not_implemented;

/*82*/	case IMMEDbrm2:    /* out of order */
/*80*/	case IMMEDbrm: {
	    int src1, src2, res;
	    res = (*(PC+1)>>3)& 0x7;
	    PC += hsw_modrm_32_byte(MODARGS);
	    src2 = *PC; PC += 1;
	    mem_ref = MEM_REF; src1 = *mem_ref;
	    switch (res) {
		case 0: /* ADD */
		    *mem_ref = res = src1 + src2;
		    SETBFLAGS(0);
		    return (PC);
		case 1: /* OR */
		    *mem_ref = res = src1 | src2;
		    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
		    return (PC);
		case 2: /* ADC */
		    *mem_ref = res = src1 + src2 + (CARRY & 1);
		    SETBFLAGS(0);
		    return (PC);
		case 3: { /* SBB */
		    int carry = CARRY & 1;
		    *mem_ref = res = src1 - src2 - carry;
		    SETBFLAGS(1);
		    } return (PC);
		case 4: /* AND */
		    *mem_ref = res = src1 & src2;
		    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
		    return (PC);
		case 5: /* SUB */
		    *mem_ref = res = src1 - src2;
		    SETBFLAGS(1);
		    return (PC);
		case 6: /* XOR */
		    *mem_ref = res = src1 ^ src2;
		    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
		    return (PC);
		case 7: /* CMP */
		    res = src1 - src2;
		    SETBFLAGS(1);
		    return (PC);
		default: goto not_implemented;
	    }}
/*81*/	case IMMEDwrm: {
	    int src1, src2, res;
	    res = (*(PC+1)>>3)& 0x7;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = FETCH_WORD(PC); PC += 2;
	    mem_ref = MEM_REF; 
	    src1 = mFETCH_WORD(mem_ref);
	      switch (res) {
		case 0: /* ADD */
		    res = src1 + src2;
		    mPUT_WORD(mem_ref, res);
		    SETWFLAGS(0);
		    return (PC);
		case 1: /* OR */
		    res = src1 | src2;
		    mPUT_WORD(mem_ref, res);
		    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
		    return (PC);
		case 2: /* ADC */
		    res = src1 + src2 + (CARRY & 1);
		    mPUT_WORD(mem_ref, res);
		    SETWFLAGS(0);
		    return (PC);
		case 3: { /* SBB */
		    int carry = CARRY & 1;
		    res = src1 - src2 - carry;
		    mPUT_WORD(mem_ref, res);
		    SETWFLAGS(1);
		    } return (PC);
		case 4: /* AND */
		    res = src1 & src2;
		    mPUT_WORD(mem_ref, res);
		    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
		    return (PC);
		case 5: /* SUB */
		    res = src1 - src2;
		    mPUT_WORD(mem_ref, res);
		    SETWFLAGS(1);
		    return (PC);
		case 6: /* XOR */
		    res = src1 ^ src2;
		    mPUT_WORD(mem_ref, res);
		    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
		    return (PC);
		case 7: /* CMP */
		    res = src1 - src2;
		    SETWFLAGS(1);
		    return (PC);
		default: goto not_implemented;
	      }
	    }
/*83*/	case IMMEDisrm: {
	    int src1, src2, res;
	    res = (*(PC+1)>>3)& 0x7;
	    PC += hsw_modrm_32_word(MODARGS);
	    src2 = (signed char)*(PC); PC += 1;
	    src2 = src2 & 0xffff;
	    mem_ref = MEM_REF; 
	    src1 = mFETCH_WORD(mem_ref);
	      switch (res) {
		case 0: /* ADD */
		    res = src1 + src2;
		    mPUT_WORD(mem_ref, res);
		    SETWFLAGS(0);
		    return (PC);
		case 1: /* OR */
		    res = src1 | src2;
		    mPUT_WORD(mem_ref, res);
		    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
		    return (PC);
		case 2: /* ADC */
		    res = src1 + src2 + (CARRY & 1);
		    mPUT_WORD(mem_ref, res);
		    SETWFLAGS(0);
		    return (PC);
		case 3: { /* SBB */
		    int carry = CARRY & 1;
		    res = src1 - src2 - carry;
		    mPUT_WORD(mem_ref, res);
		    SETWFLAGS(1);
		    } return (PC);
		case 4: /* AND */
		    res = src1 & src2;
		    mPUT_WORD(mem_ref, res);
		    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
		    return (PC);
		case 5: /* SUB */
		    res = src1 - src2;
		    mPUT_WORD(mem_ref, res);
		    SETWFLAGS(1);
		    return (PC);
		case 6: /* XOR */
		    res = src1 ^ src2;
		    mPUT_WORD(mem_ref, res);
		    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
		    return (PC);
		case 7: /* CMP */
		    res = src1 - src2;
		    SETWFLAGS(1);
		    return (PC);
		default: goto not_implemented;
	      }
	    }
/*84*/	case TESTbrm: {
	    DWORD res;
	    PC += hsw_modrm_32_byte(MODARGS);
	    res = *MEM_REF & *HREG1;
	    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
	    } return (PC); 
/*85*/	case TESTwrm: {
	    DWORD res;
	    PC += hsw_modrm_32_word(MODARGS);
	    res = mFETCH_WORD(MEM_REF) & *XREG1;
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } return (PC); 
/*86*/	case XCHGbrm: {
	    BYTE temp;
	    PC += hsw_modrm_32_byte(MODARGS);
	    temp = *MEM_REF; *MEM_REF = *HREG1; *HREG1 = temp;
	    } return (PC); 
/*87*/	case XCHGwrm: {
	    WORD temp;
	    PC += hsw_modrm_32_word(MODARGS);
	    temp = *XREG1;
	    *XREG1 = mFETCH_WORD(MEM_REF);
	    mPUT_WORD(MEM_REF, temp);
		return (PC); 
	    }

/*88*/	case MOVbfrm:
	    PC += hsw_modrm_32_byte(MODARGS);
	    mPUT_BYTE(MEM_REF, *HREG1); return (PC);
/*89*/	case MOVwfrm:
	    PC += hsw_modrm_32_word(MODARGS);
	    mPUT_WORD(MEM_REF, *XREG1); return (PC); 
/*8a*/	case MOVbtrm:
	    PC += hsw_modrm_32_byte(MODARGS);
	    *HREG1 = mFETCH_BYTE(MEM_REF); return (PC);
/*8b*/	case MOVwtrm:
	    PC += hsw_modrm_32_word(MODARGS);
	    *XREG1 = mFETCH_WORD(MEM_REF); return (PC);
/*8c*/	case MOVsrtrm: {
	    BYTE seg_reg = (*(PC + 1) >> 3) & 0x7;
	    PC += hsw_modrm_32_word(MODARGS);
		switch (seg_reg) {
		    case 0: /* ES */
			mPUT_WORD(MEM_REF, SHORT_ES_16);
			return (PC);
		    case 1: /* CS */
			mPUT_WORD(MEM_REF, SHORT_CS_16);
			return (PC);
		    case 2: /* SS */
			mPUT_WORD(MEM_REF, SHORT_SS_16);
			return (PC);
		    case 3: /* DS */
			mPUT_WORD(MEM_REF, SHORT_DS_16);
			return (PC);
		    case 4: /* FS */
			mPUT_WORD(MEM_REF, SHORT_FS_16);
			return (PC);
		    case 5: /* GS */
			mPUT_WORD(MEM_REF, SHORT_GS_16);
			return (PC);
		    case 6: /* Illegal */
		    case 7: /* Illegal */
		goto illegal_op; 
			/* trap this */
		    default: goto not_implemented;
		}
		}
/*8d*/	case LEA:
	    OVERR_DS = OVERR_SS = 0;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) {
		goto illegal_op;
	    } else {
		*XREG1 = (DWORD)MEM_REF;
	    }
	    return (PC);
/*8e*/	case MOVsrfrm: {
	    WORD wtemp;
	    BYTE seg_reg = (*(PC + 1) >> 3) & 0x7;
	    PC += hsw_modrm_32_word(MODARGS);
	    wtemp = mFETCH_WORD(MEM_REF);
		switch (seg_reg) {
		    case 0: /* ES */
		    if ((*err = SET_SEGREG(LONG_ES,BIG_ES,MK_ES,wtemp))) {
			iVAR->error_addr=wtemp&0xfffc; return P0; }
		    SHORT_ES_16 = wtemp;
			return (PC);
		    case 2: /* SS */
		    if ((*err = SET_SEGREG(LONG_SS,BIG_SS,MK_SS,wtemp))) {
			iVAR->error_addr=wtemp&0xfffc; return P0; }
		    SHORT_SS_16 = wtemp;
			return (PC);
		    case 3: /* DS */
		    if ((*err = SET_SEGREG(LONG_DS,BIG_DS,MK_DS,wtemp))) {
			iVAR->error_addr=wtemp&0xfffc; return P0; }
		    SHORT_DS_16 = wtemp;
			return (PC);
		    case 4: /* FS */
		    if ((*err = SET_SEGREG(LONG_FS,BIG_FS,MK_FS,wtemp))) {
			iVAR->error_addr=wtemp&0xfffc; return P0; }
		    SHORT_FS_16 = wtemp;
			return (PC);
		    case 5: /* GS */
		    if ((*err = SET_SEGREG(LONG_GS,BIG_GS,MK_GS,wtemp))) {
			iVAR->error_addr=wtemp&0xfffc; return P0; }
		    SHORT_GS_16 = wtemp;
			return (PC);
		case 1: /* CS */
		    case 6: /* Illegal */
		    case 7: /* Illegal */
		goto illegal_op; 
			/* trap this */
		default: goto not_implemented;
	    }}
/*8f*/	case POPrm: {
	    WORD temp;
	    POPWORD(temp);
	    PC += hsw_modrm_32_word(MODARGS);
	    mem_ref = MEM_REF;
	    PUT_WORD(mem_ref, temp);
	    } return (PC);

/*90*/	case NOP:
	    PC += 1; return (PC);
/*91*/	case XCHGcx: {
	    WORD temp = AX;
	    AX = CX;
	    CX = temp;
	    } PC += 1; return (PC);
/*92*/	case XCHGdx: {
	    WORD temp = AX;
	    AX = DX;
	    DX = temp;
	    } PC += 1; return (PC);
/*93*/	case XCHGbx: {
	    WORD temp = AX;
	    AX = BX;
	    BX = temp;
	    } PC += 1; return (PC);
/*94*/	case XCHGsp: {
	    WORD temp = AX;
	    AX = SP;
	    SP = temp;
	    } PC += 1; return (PC);
/*95*/	case XCHGbp: {
	    WORD temp = AX;
	    AX = BP;
	    BP = temp;
	    } PC += 1; return (PC);
/*96*/	case XCHGsi: {
	    WORD temp = AX;
	    AX = SI;
	    SI = temp;
	    } PC += 1; return (PC);
/*97*/	case XCHGdi: {
	    WORD temp = AX;
	    AX = DI;
	    DI = temp;
	    } PC += 1; return (PC);
/*98*/	case CBW:
	    AH = (AL & 0x80) ? 0xff : 0;
	    PC += 1; return (PC);
/*99*/	case CWD:
	    DX = (AX & 0x8000) ? 0xffff : 0;
	    PC += 1; return (PC);
/*9a*/	case CALLl: goto not_implemented;
/*9b*/	case WAIT:
	    PC += 1; return (PC);
/*9c*/	case PUSHF: {
	    DWORD temp;
	    if (VM86F) goto not_permitted;
	    temp = trans_interp_flags(ENVARGS);
	    PUSHWORD(temp);
	    } PC += 1; return (PC);
/*9d*/	case POPF: {
	    DWORD temp=0;
	    if (VM86F) goto not_permitted;
	    POPWORD(temp);
	    temp = (temp & 0x7fd5) | (iENV->flags & 0xffff0000) | 2;
	    BYTE_FLAG=0;
	    trans_flags_to_interp(ENVARGS, temp);
	    } PC += 1; return (PC);
/*9e*/	case SAHF: return (PC);
/*9f*/	case LAHF: return (PC);

/*a0*/	case MOVmal: {
	    mem_ref = OVERR_DS + FETCH_QUAD((PC+1));
	    AL = *mem_ref;
	    } PC += 5; return (PC);
/*a1*/	case MOVmax: {
	    mem_ref = OVERR_DS + FETCH_QUAD((PC+1));
	    AX = FETCH_WORD(mem_ref);
	    } PC += 5; return (PC);
/*a2*/	case MOValm: {
	    mem_ref = OVERR_DS + FETCH_QUAD((PC+1));
	    *mem_ref = AL;
	    } PC += 5; return (PC);
/*a3*/	case MOVaxm: {
	    mem_ref = OVERR_DS + FETCH_QUAD((PC+1));
            *mem_ref = AL;
            *(mem_ref + 1) = AH;
	    } PC += 5; return (PC);
/*a4*/	case MOVSb: {
	    BYTE *src, *dest;
	    src = OVERR_DS + (ESI);
	    dest = LONG_ES + (EDI);
	    *dest = *src;
	    (iENV->flags & DIRECTION_FLAG)?(ESI--,EDI--):(ESI++,EDI++);
	    } PC += 1; return (PC);
/*a5*/	case MOVSw: {
	    int temp;
	    BYTE *src, *dest;
	    src = OVERR_DS + (ESI);
	    dest = LONG_ES + (EDI);
	    temp = FETCH_WORD(src);
	    PUT_WORD(dest, temp);
	    (iENV->flags & DIRECTION_FLAG)?(ESI-=2,EDI-=2):(ESI+=2,EDI+=2);
	    } PC += 1; return (PC);
/*a6*/	case CMPSb: {
	    DWORD res, src1, src2;
	    BYTE *src, *dest;
	    src = OVERR_DS + (ESI);
	    dest = LONG_ES + (EDI);
	    src1 = *src;
	    src2 = *dest;
	    res = src1 - src2;
	    (iENV->flags & DIRECTION_FLAG)?(ESI--,EDI--):(ESI++,EDI++);
	    SETBFLAGS(1);
	    } PC += 1; return (PC);
/*a7*/	case CMPSw: {
	    DWORD res, src1, src2;
	    BYTE *src, *dest;
	    src = OVERR_DS + (ESI);
	    dest = LONG_ES + (EDI);
	    src1 = FETCH_WORD(src);
	    src2 = FETCH_WORD(dest);
	    res = src1 - src2;
	    (iENV->flags & DIRECTION_FLAG)?(ESI-=2,EDI-=2):(ESI+=2,EDI+=2);
	    SETWFLAGS(1);
	    } PC += 1; return (PC);
/*a8*/	case TESTbi: return (PC);
/*a9*/	case TESTwi: {
	    DWORD res;
	    res = AX & FETCH_WORD((PC+1));
	    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
	    } PC += 3; return (PC);
/*aa*/	case STOSb:
	    LONG_ES[EDI] = AL;
	    (iENV->flags & DIRECTION_FLAG)?EDI--:EDI++;
	    PC += 1; return (PC);
/*ab*/	case STOSw:
	    PUT_WORD(LONG_ES+EDI, AX);
	    (iENV->flags & DIRECTION_FLAG)?(EDI-=2):(EDI+=2);
	    PC += 1; return (PC);
/*ac*/	case LODSb: {
	    BYTE *seg;
	    seg = OVERR_DS + (ESI);
	    AL = *seg;
	    (iENV->flags & DIRECTION_FLAG)?ESI--:ESI++;
	    } PC += 1; return (PC);
/*ad*/	case LODSw: {
	    BYTE *seg;
	    seg = OVERR_DS + (ESI);
	    AX = FETCH_WORD(seg);
	    (iENV->flags & DIRECTION_FLAG)?(ESI-=2):(ESI+=2);
	    } PC += 1; return (PC);
/*ae*/	case SCASb: {
	    DWORD res, src1, src2;
	    src1 = AL;
	    src2 = LONG_ES[EDI];
	    res = src1 - src2;
	    (iENV->flags & DIRECTION_FLAG)?EDI--:EDI++;
	    SETBFLAGS(1);
	    } PC += 1; return (PC);
/*af*/	case SCASw: {
	    DWORD res, src1, src2;
	    src1 = AX;
	    mem_ref = LONG_ES + (EDI);
	    src2 = FETCH_WORD(mem_ref);
	    res = src1 - src2;
	    (iENV->flags & DIRECTION_FLAG)?(EDI-=2):(EDI+=2);
	    SETWFLAGS(1);
	    } PC += 1; return (PC);

/*b0*/	case MOVial: return (PC);
/*b1*/	case MOVicl: return (PC);
/*b2*/	case MOVidl: return (PC);
/*b3*/	case MOVibl: return (PC);
/*b4*/	case MOViah: return (PC);
/*b5*/	case MOVich: return (PC);
/*b6*/	case MOVidh: return (PC);
/*b7*/	case MOVibh: return (PC);
/*b8*/	case MOViax:
	    AX = FETCH_WORD((PC+1));
	    PC += 3; return (PC);
/*b9*/	case MOVicx:
	    CX = FETCH_WORD((PC+1));
	    PC += 3; return (PC);
/*ba*/	case MOVidx:
	    DX = FETCH_WORD((PC+1));
	    PC += 3; return (PC);
/*bb*/	case MOVibx:
	    BX = FETCH_WORD((PC+1));
	    PC += 3; return (PC);
/*bc*/	case MOVisp:
	    SP = FETCH_WORD((PC+1)); 
	    PC += 3; return (PC);
/*bd*/	case MOVibp:
	    BP = FETCH_WORD((PC+1)); 
	    PC += 3; return (PC);
/*be*/	case MOVisi:
	    SI = FETCH_WORD((PC+1));
	    PC += 3; return (PC);
/*bf*/	case MOVidi:
	    DI = FETCH_WORD((PC+1));
	    PC += 3; return (PC);

/*d0*/	case SHIFTb:
/*d2*/	case SHIFTbv:
/*c0*/	case SHIFTbi: {
	    int temp, count;
	    DWORD rbef, raft; BYTE opc;
	    opc = *PC;
	    temp = (*(PC+1)>>3)& 0x7;
	    PC += hsw_modrm_32_byte(MODARGS);
	    if (opc==SHIFTb) count = 1;
	      else if (opc==SHIFTbv) count = CX & 0x1f;
		else { count = *PC & 0x1f; PC += 1; }
	    /* are we sure that for count==0 CF is not changed? */
	    if (count) {
		mem_ref = MEM_REF;
		switch (temp) {
		    case 0: /* ROL */
			rbef = *mem_ref;
			SRC1_8 = SRC2_8 = rbef;		/* OF: 00x or 11x */
			count &= 7;	/* rotation modulo 8 */
			raft = (rbef << count) | (rbef >> (8 - count));
			*mem_ref = RES_8 = (BYTE)raft;
			BYTE_FLAG = BYTE_OP;
			CARRYB = (BYTE)(raft & 1);
			return (PC);
		    case 1: /* ROR */
			rbef = *mem_ref; 
			SRC1_8 = SRC2_8 = rbef;
                        count &= 7;
			raft = (rbef >> count) | (rbef << (8 - count));
			*mem_ref = RES_8 = (BYTE)raft;
			BYTE_FLAG = BYTE_OP;
			CARRYB = (BYTE)((raft >> 7) & 1);
			return (PC);
		    case 2: /* RCL */
			rbef = *mem_ref; 
			SRC1_8 = SRC2_8 = rbef;
                        count &= 7;
			raft = (rbef << count) | ((rbef >> (9 - count))
			    | ((CARRY & 1) << (count - 1)));
			*mem_ref = RES_8 = (BYTE)raft;
			BYTE_FLAG = BYTE_OP;
			CARRYB = (BYTE)((rbef >> (8 - count)) & 1);
			return (PC);
		    case 3: /* RCR */
			rbef = *mem_ref; 
			SRC1_8 = SRC2_8 = rbef;
                        count &= 7;
			raft = (rbef >> count) | ((rbef << (9 - count))
			    | ((CARRY & 1) << (8 - count)));
			*mem_ref = RES_8 = (BYTE)raft;
			BYTE_FLAG = BYTE_OP;
			CARRYB = (BYTE)((rbef >> (count - 1)) & 1);
			return (PC);
		    case 6: /* Undocumented */
			if (opc==SHIFTbv) goto illegal_op;
			/* fall thru for SHIFTbi */
		    case 4: /* SHL/SAL */
			temp = *mem_ref; 
			SRC1_8 = SRC2_8 = temp;
			CARRYB = (temp >> (8 - count)) & 1;
			temp = (temp << count);
			*mem_ref = temp;
			/* if cnt==1 OF=1 if CF after shift != MSB 1st op */
			RES_8 = temp; BYTE_FLAG = BYTE_OP;
			return (PC);
		    case 5: /* SHR */
			temp = *mem_ref; 
			SRC1_8 = SRC2_8 = temp;
			CARRYB = (temp >> (count-1)) & 1;
			temp = (temp >> count);
			*mem_ref = temp;
			/* if cnt==1 OF=1 if MSB was changed */
			RES_8 = temp; BYTE_FLAG = BYTE_OP;
			return (PC);
		    case 7: /* SAR */
			temp = *(signed char *)mem_ref; 
			CARRYB = (temp >> (count-1)) & 1;
			temp = (temp >> count);
			*mem_ref = (BYTE)temp;
			/* if cnt==1 OF=0 */
			SRC1_8 = SRC2_8 = RES_8 = (BYTE)temp;
			BYTE_FLAG = BYTE_OP;
			return (PC);
		    default: goto not_implemented;
		} } else  return (PC);
	    }
/*d1*/	case SHIFTw:
/*d3*/	case SHIFTwv:
/*c1*/	case SHIFTwi: {
	    int temp, count;
	    DWORD rbef, raft; BYTE opc;
	    opc = *PC;
	    temp = (*(PC+1)>>3)& 0x7;
	    PC += hsw_modrm_32_word(MODARGS);
	    mem_ref = MEM_REF;
	    if (opc==SHIFTw) count = 1;
	      else if (opc==SHIFTwv) count = CX & 0x1f;
		else { count = *PC & 0x1f; PC += 1; }
	    /* are we sure that for count==0 CF is not changed? */
	    if (count) {
		mem_ref = MEM_REF;
		switch (temp) {
		    case 0: /* ROL */
			rbef = mFETCH_WORD(mem_ref);
			SRC1_16 = SRC2_16 = rbef;
			count &= 0xf;	/* rotation modulo 16 */
			raft = (rbef << count) | (rbef >> (16 - count));
			mPUT_WORD(mem_ref,raft);
			RES_16 = (WORD)raft;
			CARRY = (WORD)(raft & 1);	/* -> BYTE_FLAG=0 */
			return (PC);
		    case 1: /* ROR */
			rbef = mFETCH_WORD(mem_ref);
			SRC1_16 = SRC2_16 = rbef;
                        count &= 0xf;
			raft = (rbef >> count) | (rbef << (16 - count));
			mPUT_WORD(mem_ref,raft);
			RES_16 = (WORD)raft;
			CARRY = (WORD)((raft >> 15) & 1);
			return (PC);
		    case 2: /* RCL */
			rbef = mFETCH_WORD(mem_ref);
			SRC1_16 = SRC2_16 = rbef;
                        count &= 0xf;
			raft = (rbef << count) | ((rbef >> (17 - count))
			    | ((CARRY & 1) << (count - 1)));
			mPUT_WORD(mem_ref,raft);
			RES_16 = (WORD)raft;
			CARRY = (WORD)((rbef >> (16 - count)) & 1);
			return (PC);
		    case 3: /* RCR */
			rbef = mFETCH_WORD(mem_ref);
			SRC1_16 = SRC2_16 = rbef;
                        count &= 0xf;
			raft = (rbef >> count) | ((rbef << (17 - count))
			    | ((CARRY & 1) << (16 - count)));
			mPUT_WORD(mem_ref,raft);
			RES_16 = (WORD)raft;
			CARRY = (WORD)((rbef >> (count - 1)) & 1);
			return (PC);
		    case 6: /* Undocumented */
			if ((opc==SHIFTw)||(opc==SHIFTwv))
			    goto illegal_op;
			/* fall thru for SHIFTwi */
		    case 4: /* SHL/SAL */
			temp = mFETCH_WORD(mem_ref);
			SRC1_16 = SRC2_16 = temp;
			CARRY = (temp >> (16 - count)) & 1;
			temp = (temp << count);
			mPUT_WORD(mem_ref,temp);
			RES_16 = temp;
			return (PC);
		    case 5: /* SHR */
			temp = mFETCH_WORD(mem_ref); 
			SRC1_16 = SRC2_16 = temp;
			CARRY = (temp >> (count-1)) & 1;
			temp = (temp >> count);
			mPUT_WORD(mem_ref,temp);
			RES_16 = temp;
			return (PC);
		    case 7: /* SAR */
			temp = (signed short)mFETCH_WORD(mem_ref);
			CARRY = (temp >> (count-1)) & 1;
			temp = (temp >> count);
			mPUT_WORD(mem_ref,temp);
			/* if cnt==1 OF=0 */
			SRC1_16 = SRC2_16 = RES_16 = (WORD)temp;
			return (PC);
		    default: goto not_implemented;
	      } } else  return (PC);
	    }
/*c2*/	case RETisp:
	    goto not_implemented;
/*c3*/	case RET:
	    goto not_implemented;
/*c4*/	case LES: {
	    int temp;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) {
		/* Illegal */
		goto illegal_op; 
	    }
	    mem_ref = MEM_REF;
	    temp = FETCH_WORD(mem_ref+2);
	    if ((*err = SET_SEGREG(LONG_ES,BIG_ES,MK_ES,temp))) {
		iVAR->error_addr=temp&0xfffc; return P0; }
	    *XREG1 = FETCH_WORD(mem_ref);
	    SHORT_ES_32 = temp;
	    } return (PC);
/*c5*/	case LDS: {
	    int temp;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) {
		/* Illegal */
		goto illegal_op; 
	    }
	    mem_ref = MEM_REF;
	    temp = FETCH_WORD(mem_ref+2);
	    if ((*err = SET_SEGREG(LONG_DS,BIG_DS,MK_DS,temp))) {
		iVAR->error_addr=temp&0xfffc; return P0; }
	    *XREG1 = FETCH_WORD(mem_ref);
	    SHORT_DS_32 = temp;
	    } return (PC);
/*c6*/	case MOVbirm:
	    PC += hsw_modrm_32_byte(MODARGS);
	    *MEM_REF = *PC; PC += 1; return (PC);
/*c7*/	case MOVwirm: {
	    PC += hsw_modrm_32_word(MODARGS);
	    mPUT_WORD(MEM_REF, FETCH_WORD(PC));
		PC += 2; return (PC);
	    } 
/*cb*/	case RETl: /* restartable */
	    if (VM86F) goto not_permitted;
	    else {
		DWORD cs, ip;
		WORD transfer_magic;
		POPWORD(ip);
		cs = TOS_WORD;
		transfer_magic = (WORD)GetSelectorType(cs);
		if ((*err = SET_SEGREG(LONG_CS,BIG_CS,MK_CS,cs))==EXCP0B_NOSEG) {
		    PUSHWORD(ip); iVAR->error_addr=cs&0xfffc; return P0; }
		POPWORD(cs);
		SHORT_CS_16 = (WORD)cs;
		PC = ip + LONG_CS;
		if (transfer_magic == TRANSFER_CODE32) {
#ifdef DOSEMU
			return (PC);
#else
		    goto not_implemented;
#endif
		} else
		if (transfer_magic == TRANSFER_CODE16) {
		    *err = EXCP_GOBACK;
			return (PC);
		} else
		    invoke_data(ENVARGS);    /* TRANSFER_DATA or garbage */
		}
/*c8*/	case ENTER:
/*c9*/	case LEAVE:
/*ca*/	case RETlisp:
/*cc*/	case INT3:
/*cd*/	case INT:
/*ce*/	case INTO:
	    goto not_implemented;
/*cf*/	case IRET: /* restartable */
	    if (VM86F) goto not_permitted;
	    else {
		DWORD cs, ip, flags;
		WORD transfer_magic;
		POPWORD(ip);
		cs = TOS_WORD;
		transfer_magic = (WORD)GetSelectorType(cs);
		if ((*err = SET_SEGREG(LONG_CS,BIG_CS,MK_CS,cs))==EXCP0B_NOSEG) {
		    PUSHWORD(ip); iVAR->error_addr=cs&0xfffc; return P0; }
		POPWORD(cs);
		POPWORD(flags);
		/* skip reserved bits, in case of PUSH XX..POPF */
		flags = (flags & 0x7fd5) | 2;
		trans_flags_to_interp(ENVARGS, flags);
		SHORT_CS_16 = (WORD)cs;
		PC = ip + LONG_CS;
		if (transfer_magic == TRANSFER_CODE32) {
#ifdef DOSEMU
			return (PC);
#else
		    goto not_implemented;
#endif
		} else
		if (transfer_magic == TRANSFER_CODE16) {
		    *err = EXCP_GOBACK;
			return (PC);
		} else
		    invoke_data(ENVARGS);    /* TRANSFER_DATA or garbage */
	    }

/*d0*/	    /* see before */
/*d1*/	    /* see before */
/*d2*/	    /* see before */
/*d3*/	    /* see before */
/*d4*/	case AAM: return (PC);
/*d5*/	case AAD: return (PC);
	case 0xd6:    /* illegal on 8086 and 80x86*/
		goto illegal_op; 
/*d7*/	case XLAT: {
	    mem_ref = OVERR_DS + (EBX) + (AL);
	    AL = *mem_ref; }
	    PC += 1; return (PC);

/*d8*/	case ESC0:
/*d9*/	case ESC1:
/*da*/	case ESC2:
/*db*/	case ESC3:
/*dc*/	case ESC4:
/*dd*/	case ESC5:
/*de*/	case ESC6:
/*df*/	case ESC7:
    {
    switch(*PC){
/*d8*/	case ESC0: {
	    int reg = (*(PC+1) & 7);
	    DWORD funct = (DWORD)(*(PC+1) & 0x38) >> 3;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) hsw_fp87_reg0[funct](reg);
	    else hsw_fp87_mem0[funct](MEM_REF);
	    } break;
/*d9*/	case ESC1: {
	    int reg = (*(PC+1) & 7);
	    DWORD funct = (DWORD)(*(PC+1) & 0x38) >> 3;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) hsw_fp87_reg1[funct](reg);
	    else hsw_fp87_mem1[funct](MEM_REF);
	    } break;
/*da*/	case ESC2: {
	    int reg = (*(PC+1) & 7);
	    DWORD funct = (DWORD)(*(PC+1) & 0x38) >> 3;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) hsw_fp87_reg2[funct](reg);
	    else hsw_fp87_mem2[funct](MEM_REF);
	    } break;
/*db*/	case ESC3: {
	    int reg = (*(PC+1) & 7);
	    DWORD funct = (DWORD)(*(PC+1) & 0x38) >> 3;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) hsw_fp87_reg3[funct](reg);
	    else hsw_fp87_mem3[funct](MEM_REF);
	    } break;
/*dc*/	case ESC4: {
	    int reg = (*(PC+1) & 7);
	    DWORD funct = (DWORD)(*(PC+1) & 0x38) >> 3;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) hsw_fp87_reg4[funct](reg);
	    else hsw_fp87_mem4[funct](MEM_REF);
	    } break;
/*dd*/	case ESC5: {
	    int reg = (*(PC+1) & 7);
	    DWORD funct = (DWORD)(*(PC+1) & 0x38) >> 3;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) hsw_fp87_reg5[funct](reg);
	    else hsw_fp87_mem5[funct](MEM_REF);
	    } break;
/*de*/	case ESC6: {
	    int reg = (*(PC+1) & 7);
	    DWORD funct = (DWORD)(*(PC+1) & 0x38) >> 3;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) hsw_fp87_reg6[funct](reg);
	    else hsw_fp87_mem6[funct](MEM_REF);
	    } break;
/*df*/	case ESC7: {
	    int reg = (*(PC+1) & 7);
	    DWORD funct = (DWORD)(*(PC+1) & 0x38) >> 3;
	    PC += hsw_modrm_32_word(MODARGS);
	    if (IS_MODE_REG) hsw_fp87_reg7[funct](reg);
	    else hsw_fp87_mem7[funct](MEM_REF);
	    } break;
			    }
#ifndef NO_TRACE_MSGS
    if (d_emu>3) e_trace_fp(&hsw_env87);
#endif
    if ((*PC==WAIT) || (*PC==NOP)) PC += 1;
    return (PC); }

/*e0*/	case LOOPNZ_LOOPNE: 
	    if ((--ECX != 0) && (!IS_ZF_SET)) {
		JUMP((PC+1)); return (PC);
	    } PC += 2; return (PC);
/*e1*/	case LOOPZ_LOOPE: 
	    if ((--ECX != 0) && (IS_ZF_SET)) {
		JUMP((PC+1)); return (PC);
	    } PC += 2; return (PC);
/*e2*/	case LOOP: 
	    if (--ECX != 0) {
		JUMP((PC+1)); return (PC);
	    } PC += 2; return (PC);
/*e3*/	case JCXZ: 
	    if (ECX == 0) {
		JUMP((PC+1)); return (PC);
	    } PC += 2; return (PC);
/*e5*/	case INw: {
	      BYTE a = FETCH_BYTE(PC+1);
	      /* We CAN't allow unlimited port access even in RM - all
	       * MUST be filtered through the I/O bitmap */
	      /*if (!PMODEF || (CPL > IOPL)) {*/
	      AX = hsw_port_inw(a);
	      PC += 2; return (PC);
	    }
/*e7*/	case OUTw: {
	      BYTE a = FETCH_BYTE(PC+1);
	      hsw_port_outw(a, AX);
	      PC += 2; return (PC);
	    }
/*ed*/	case INvw: {
	      AX = hsw_port_inw(DX);
	      PC += 1; return (PC);
	    }
/*ef*/	case OUTvw: {
	      hsw_port_outw(DX, AX);
	      PC += 1; return (PC);
	    }

/*e8*/	case CALLd:
/*e9*/	case JMPd:
/*ea*/	case JMPld:
/*eb*/	case JMPsid: 
	    goto not_implemented;

/*f0*/	case LOCK:
	    PC += 1; return (PC);
	case 0xf1:    /* illegal on 8086 and 80x86 */
	    goto illegal_op;

/*f2*/	case REPNE:
/*f3*/	case REP:     /* also is REPE */
	{
	    unsigned int count = ECX;
	    int longd = 2;
	    register unsigned int i;
	    BYTE repop,test;
	    DWORD res, src1=0, oldcnt; int src2=0;

	    repop = (*PC-REPNE);
	    PC += 2;
segrep:
	    switch (*(PC-1)) {
		case INSb:
		case INSw:
		case OUTSb:
		case OUTSw:
		    if (VM86F) goto not_permitted;
		    goto not_implemented;
		case MOVSb: {
		    BYTE *src, *dest;
		    if (count == 0) return (PC);
		    i = count;
		    src  = (BYTE *)(OVERR_DS + ESI);
		    dest = (BYTE *)(LONG_ES + EDI);
		    if (iENV->flags & DIRECTION_FLAG) {
			while (i--) {PUT_BYTE(dest,FETCH_BYTE(src)); src--,dest--;}
			EDI -= count; ESI -= count;
		    } else {
			while (i--) {PUT_BYTE(dest,FETCH_BYTE(src)); src++,dest++;}
			EDI += count; ESI += count;
		    }
		    ECX = 0; instr_count += count;
		    return (PC);
		    } 
		case MOVSw: {
		    int lcount = count * longd;
		    BYTE *src  = (BYTE *)(OVERR_DS + ESI);
		    BYTE *dest = (BYTE *)(LONG_ES + EDI);
		    if (count == 0) return (PC);
		    if (iENV->flags & DIRECTION_FLAG) lcount = -lcount;
		    i = count;
		    if (longd==2) {
		      if (lcount < 0) {
			while (i--) {PUT_WORD(dest,FETCH_WORD(src)); src-=2,dest-=2;}
		      } else {
			while (i--) {PUT_WORD(dest,FETCH_WORD(src)); src+=2,dest+=2;}
		      }
		    } else {
		      if (lcount < 0) {
			while (i--) {PUT_QUAD(dest,FETCH_QUAD(src)); src-=4,dest-=4;}
		      } else {
			while (i--) {PUT_QUAD(dest,FETCH_QUAD(src)); src+=4,dest+=4;}
		      }
		    } 
		    EDI += lcount; ESI += lcount;
		    ECX = 0; instr_count += count;
		    return (PC);
		    }
		case CMPSb: {
		    BYTE *src, *dest, *ovr;
		    if (count == 0) return (PC);
		    oldcnt = count;
		    src = (ovr=OVERR_DS) + (ESI);
		    dest = LONG_ES + EDI;
		    if (iENV->flags & DIRECTION_FLAG) { /* backwards */
			while (count) {
			    src1 = FETCH_BYTE(src); src--;
			    src2 = FETCH_BYTE(dest); dest--;
			    test = ((int)src1 != src2) ^ repop;
			    if (test) count--;
			    else {
			        res = src1 - src2;
				instr_count += (oldcnt-count);
				ECX = count - 1;
				EDI = dest - LONG_ES;
				ESI = src - ovr;
				RES_32 = res << 8; SRC1_8 = (BYTE)src1; 
				SRC2_8 = (((src2 & 0xff)== 0x80)? 0:-src2);
				BYTE_FLAG = BYTE_OP;
				return (PC);
			    }
			}
		    } else {		      /* forwards */
			while (count) {
			    src1 = FETCH_BYTE(src); src++;
			    src2 = FETCH_BYTE(dest); dest++;
			    test = ((int)src1 != src2) ^ repop;
			    if (test) count--;
			    else {
			        res = src1 - src2;
				instr_count += (oldcnt-count);
				ECX = count - 1;
				EDI = dest - LONG_ES;
				ESI = src - ovr;
				RES_32 = res << 8; SRC1_8 = (BYTE)src1; 
				SRC2_8 = (((src2 & 0xff)== 0x80)? 0:-src2); 
				BYTE_FLAG = BYTE_OP;
				return (PC);
			    }
			}
		    }
		    res = src1 - src2;
		    instr_count += count;
		    ECX = 0; EDI = dest - LONG_ES;
		    ESI = src - ovr;
		    RES_32 = res << 8; SRC1_8 = (BYTE)src1; 
		    SRC2_8 = (((src2 & 0xff)== 0x80)? 0:-src2);
		    BYTE_FLAG = BYTE_OP;
		    } return (PC);
		case CMPSw: {
		    BYTE *src, *dest, *ovr;
		    if (count == 0) return (PC);
		    oldcnt = count;
		    src = (ovr=OVERR_DS) + (ESI);
		    dest = LONG_ES + EDI;
		    if (iENV->flags & DIRECTION_FLAG) { /* backwards */
			while (count) {
			    if (longd==4) {
			      src1 = FETCH_QUAD(src);
			      src2 = FETCH_QUAD(dest);
			    }
			    else {
			    src1 = FETCH_WORD(src);
			    src2 = FETCH_WORD(dest);
			    }
			    src -= longd; dest -= longd;
			    test = ((int)src1 != src2) ^ repop;
			    if (test) count--;
			    else {
			        res = src1 - src2;
				instr_count += (oldcnt-count);
				ECX = count - 1;
				EDI = dest - LONG_ES;
				ESI = src - ovr;
				if (longd==2) {
				  SRC1_16 = (WORD)src1; RES_32 = res;
	    			SRC2_16 = (((src2 & 0xffff)== 0x8000)? 0:-src2);
				}
				else {
				  SETDFLAGS(1,1);
				}
				return (PC);
			    }
			}
		    } else {		      /* forwards */
			while (count) {
			    if (longd==4) {
			      src1 = FETCH_QUAD(src);
			      src2 = FETCH_QUAD(dest);
			    }
			    else {
			    src1 = FETCH_WORD(src);
			    src2 = FETCH_WORD(dest);
			    }
			    src += longd; dest += longd;
			    test = ((int)src1 != src2) ^ repop;
			    if (test) count--;
	  		    else {
			        res = src1 - src2;
				instr_count += (oldcnt-count);
				ECX = count - 1;
				EDI = dest - LONG_ES;
				ESI = src - ovr;
				if (longd==2) {
				  SRC1_16 = (WORD)src1; RES_32 = res;
	    			SRC2_16 = (((src2 & 0xffff)== 0x8000)? 0:-src2);
				}
				else {
				  SETDFLAGS(1,1);
				}
				return (PC);
			    }
			}
		    }
		    res = src1 - src2;
		    instr_count += count;
		    ECX = 0; EDI = dest - LONG_ES;
		    ESI = src - ovr;
		    if (longd==2) {
		      RES_32 = res; SRC1_16 = (WORD)src1;
	    	    SRC2_16 = (((src2 & 0xffff)== 0x8000)? 0:-src2);
		    }
		    else {
		      SETDFLAGS(1,1);
		    }
		    } return (PC);
		case STOSb: {
		    register BYTE al;
		    BYTE *dest;
		    if (count == 0) return (PC);
		    i = count;
		    dest = (BYTE *)(LONG_ES + EDI);
		    al = AL;
		    if (iENV->flags & DIRECTION_FLAG) { /* backwards */
			while (i--) {PUT_BYTE(dest,al); dest--;}
			EDI -= count;
		    } else {		      /* forwards */
			while (i--) {PUT_BYTE(dest,al); dest++;}
			EDI += count;
		    }
		    ECX = 0; instr_count += count;
		    return (PC);
		    } 
		case STOSw: {
		    BYTE *dest;
		    register DWORD ddat = EAX;
		    if (count == 0) return (PC);
		    i = count;
		    dest = (BYTE *)(LONG_ES + EDI);
		    if (iENV->flags & DIRECTION_FLAG) { /* backwards */
			if (longd==4)
			    while (i--) {PUT_QUAD(dest,ddat); dest-=4;}
			else
			    while (i--) {PUT_WORD(dest,ddat); dest-=2;}
			EDI -= count * longd;
		    } else {		      /* forwards */
			if (longd==4)
			    while (i--) {PUT_QUAD(dest,ddat); dest+=4;}
			else
			    while (i--) {PUT_WORD(dest,ddat); dest+=2;}
			EDI += count * longd;
		    }
		    ECX = 0; instr_count += count;
		    return (PC);
		    }
		case LODSb:
		    goto not_implemented;
		case LODSw:
		    goto not_implemented;
		case SCASb: {
		    BYTE *dest;
		    if (count == 0) return (PC);
		    oldcnt = count;
		    dest = LONG_ES + EDI;
		    src1 = AL;
		    if (iENV->flags & DIRECTION_FLAG) { /* backwards */
			while (count) {
			    src2 = FETCH_BYTE(dest); dest--;
			    test = ((int)src1 != src2) ^ repop;
			    if (test) count--;
			    else {
			    	res = src1 - src2;
				instr_count += (oldcnt-count);
				ECX = count - 1;
				EDI = dest - LONG_ES;
				RES_32 = res << 8; SRC1_8 = (BYTE)src1;
				SRC2_8 = (((src2 & 0xff)== 0x80)? 0:-src2); 
				BYTE_FLAG = BYTE_OP;
				if(((int)src1 & 0xf) < src2) SET_AF
	                        else CLEAR_AF
				return (PC);
			    }
			}
		    } else {		      /* forwards */
			while (count) {
			    src2 = FETCH_BYTE(dest); dest++;
			    test = ((int)src1 != src2) ^ repop;
			    if (test) count--;
			    else {
			        res = src1 - src2;
				instr_count += (oldcnt-count);
				ECX = count - 1;
				EDI = dest - LONG_ES;
				RES_32 = res << 8; SRC1_8 = (BYTE)src1;
				SRC2_8 = (((src2 & 0xff)== 0x80)? 0:-src2); 
				BYTE_FLAG = BYTE_OP;
				if(((int)src1 & 0xf) < src2) SET_AF
	                        else CLEAR_AF
				return (PC);
			    }
			}
		    }
		    res = src1 - src2;
		    instr_count += count;
		    ECX = 0; EDI = dest - LONG_ES;
		    RES_32 = res << 8; SRC1_8 = (BYTE)src1;
		    SRC2_8 = (((src2 & 0xff)== 0x80)? 0:-src2);
		    BYTE_FLAG = BYTE_OP;
		    if(((int)src1 & 0xf) < src2) SET_AF
                    else CLEAR_AF
		    } return (PC);
		case SCASw: {
		    BYTE *dest;
		    if (count == 0) return (PC);
		    oldcnt = count;
		    dest = LONG_ES + EDI;
		    src1 = AX;
		    if (iENV->flags & DIRECTION_FLAG) { /* backwards */
			while (count) {
			    src2 = (longd==4? FETCH_QUAD(dest) : FETCH_WORD(dest));
			    dest -= longd;
			    test = ((int)src1 != src2) ^ repop;
			    if (test) count--;
			    else {
			        res = src1 - src2;
				instr_count += (oldcnt-count);
				ECX = count - 1;
				EDI = dest - LONG_ES;
				if (longd==2) {
				  RES_32 = res; SRC1_16 = (WORD)src1;
				  SRC2_16 = (((src2 & 0xffff)== 0x8000)? 0:-src2);
				}
				else {
				  SETDFLAGS(1,1);
				}
				if(((int)src1 & 0xf) < src2) SET_AF
	                        else CLEAR_AF
				return (PC);
			    }
			}
		    } else {		      /* forwards */
			while (count) {
			    src2 = (longd==4? FETCH_QUAD(dest) : FETCH_WORD(dest));
			    dest += longd;
			    test = ((int)src1 != src2) ^ repop;
			    if (test) count--;
			    else {
			        res = src1 - src2;
				instr_count += (oldcnt-count);
				ECX = count - 1;
				EDI = dest - LONG_ES;
				if (longd==2) {
				  RES_32 = res; SRC1_16 = (WORD)src1;
	    			SRC2_16 = (((src2 & 0xffff)== 0x8000)? 0:-src2);
				}
				else {
				  SETDFLAGS(1,1);
				}
				if(((int)src1 & 0xf) < src2) SET_AF
	                        else CLEAR_AF
				return (PC);
			    }
			}
		    } 
		    res = src1 - src2;
		    instr_count += count;
		    ECX = 0; EDI = dest - LONG_ES;
		    if (longd==2) {
		      RES_32 = res; SRC1_16 = (WORD)src1;
	    	    SRC2_16 = (((src2 & 0xffff)== 0x8000)? 0:-src2);
		    }
		    else {
		      SETDFLAGS(1,1);
		    }
		    if(((int)src1 & 0xf) < src2) SET_AF
	            else CLEAR_AF
		    } return (PC);
/*----------IA--------------------------------*/
	        case SEGes:
		    OVERR_DS = OVERR_SS = LONG_ES;
            	    PC+=1; goto segrep;
        	case SEGcs:
		    OVERR_DS = OVERR_SS = LONG_CS;
          	    PC+=1; goto segrep;
        	case SEGss:
		    OVERR_DS = OVERR_SS = LONG_SS;
            	    PC+=1; goto segrep;
        	case SEGds:
		    OVERR_DS = OVERR_SS = LONG_DS;
            	    PC+=1; goto segrep;
        	case SEGfs:
		    OVERR_DS = OVERR_SS = LONG_FS;
            	    PC+=1; goto segrep;
        	case SEGgs:
		    OVERR_DS = OVERR_SS = LONG_GS;
		    PC+=1; goto segrep;
		case OPERoverride:	/* 0x66 if we come from 16:16 */
		    longd = 4;
		case ADDRoverride:	/* ignore 0x67 */
            	    PC+=1; goto segrep;
/*----------IA--------------------------------*/
		default: PC--; goto not_implemented;
	    } }
/*f4*/	case HLT:
	    goto not_implemented;
/*f5*/	case CMC: return (PC);

/*f6*/	case GRP1brm: {
	    DWORD src1, src2; int res;
	    res = (*(PC+1)>>3)& 0x7;
	    PC += hsw_modrm_32_byte(MODARGS);
	    mem_ref = MEM_REF; 
	    switch (res) {
		case 0: /* TEST */
		    res = *mem_ref & *PC; PC += 1;
		    SRC1_32 = SRC2_32 = RES_32 = BYTEDUP(res);	/* CF=OF=0 */
		    return (PC);
		case 1: /* TEST (Is this Illegal?) */
		goto illegal_op;
		case 2: /* NOT */
		    src1 = ~(*mem_ref);
		    *mem_ref = src1;
		    return (PC);
		case 3: /* NEG */
		    src1 = 0; src2 = *mem_ref;
		    res = (BYTE)(0x100-src2);
		    *mem_ref = (BYTE)res;
		    SETBFLAGS(1);
		    CARRYB = (src2 != 0);	/* not CARRY! */
		    return (PC);
		case 4: /* MUL AL */
		    src1 = *mem_ref;
		    src2 = AL;
		    res = src1 * src2;
		    AX = res;
		    if (res&0xff00) {  /* CF=OF=1 */
			CARRY=1; SRC1_16 = SRC2_16 = ~RES_16; /* 001,110 */
		    }
		    else {
			CARRY=0; SRC1_16 = SRC2_16 = RES_16;  /* 000,111 */
		    }
		    return (PC);
		case 5: /* IMUL AL */
		    src1 = *(signed char *)mem_ref;
		    src2 = (signed char)AL;
		    res = src1 * src2;
		    AX = (WORD)res;
		    res = res >> 7;
		    if ((res==0)||(res==0xffffffff)) {  /* CF=OF=0 */
			CARRY=0; SRC1_16 = SRC2_16 = RES_16; /* 000,111 */
		    }
		    else {
			CARRY=1; SRC1_16 = SRC2_16 = ~RES_16;  /* 001,110 */
		    }
		    return (PC);
		case 6: /* DIV AL */
		    src1 = *mem_ref;
		    if (src1==0) goto div_by_zero;
		    res = AX;
		    AL = res / src1;
		    AH = res % src1;
		    return (PC);
		case 7: /* IDIV AX */
		    src1 = *(signed char *)mem_ref;
		    if (src1==0) goto div_by_zero;
		    res = AX;
		    AL = res / src1;
		    AH = res % src1;
		    return (PC);
		default: goto not_implemented;
	    } }
/*f7*/	case GRP1wrm: {
	    int src1, src2, res;
	    res = (*(PC+1)>>3)& 0x7;
	    PC += hsw_modrm_32_word(MODARGS);
	    mem_ref = MEM_REF;
	      switch (res) {
		case 0: /* TEST */
		    src1 = mFETCH_WORD(mem_ref);
		    src2 = FETCH_WORD(PC); PC += 2;
		    res = src1 & src2;
		    SRC1_32 = SRC2_32 = RES_32 = res & 0xffff;	/* CF=OF=0 */
		    return(PC);
		case 1: /* TEST (Is this Illegal?) */
		goto illegal_op; 
		case 2: /* NOT */
		    src1 = mFETCH_WORD(mem_ref);
		    src1 = ~(src1);
		    mPUT_WORD(mem_ref, src1);
		    return (PC);
		case 3: /* NEG */
		    src1 = 0; /* equiv to SUB 0,src2 */
		    src2 = mFETCH_WORD(mem_ref);
		    res = (WORD)(0x10000-src2);
		    mPUT_WORD(mem_ref, res);
		    SETWFLAGS(1);
		    CARRY = (src2 != 0);	/* not CARRYB! */
		    return (PC);
		case 4: /* MUL AX */
		    src1 = mFETCH_WORD(mem_ref);
		    src2 = AX;
		    res = src1 * src2;
		    AX = res; DX = res >> 16;
		    if (res&0xffff0000) {  /* CF=OF=1 */
			CARRY=1; SRC1_16 = SRC2_16 = ~RES_16; /* 001,110 */
		    }
		    else {
			CARRY=0; SRC1_16 = SRC2_16 = RES_16;  /* 000,111 */
		    }
		    return (PC);
		case 5: /* IMUL AX */
		    src1 = (signed short)mFETCH_WORD(mem_ref);
		    src2 = (signed short)AX;
		    res = src1 * src2;
		    AX = res; DX = res >> 16;
		    res >>= 15;
		    if ((res==0)||(res==0xffffffff)) {  /* CF=OF=0 */
			CARRY=0; SRC1_16 = SRC2_16 = RES_16; /* 000,111 */
		    }
		    else {
			CARRY=1; SRC1_16 = SRC2_16 = ~RES_16;  /* 001,110 */
		    }
		    return (PC);
		case 6: /* DIV DX+AX */
		    src1 = mFETCH_WORD(mem_ref);
		    if (src1==0) goto div_by_zero;
		    res = AX | (DX<<16);
		    AX = res / src1;
		    DX = res % src1;
		    return (PC);
		case 7: /* IDIV DX+AX */
		    src1 = mFETCH_WORD(mem_ref);
		    src1 = ((src1<<16)>>16);
		    if (src1==0) goto div_by_zero;
		    res = AX | (DX<<16);
		    AX = res / src1;
		    DX = res % src1;
		    return (PC);
		default: goto not_implemented;
	      }
	      }
/*f8*/	case CLC: return (PC);
/*f9*/	case STC: return (PC);
/*fa*/	case CLI:
	    goto not_implemented;
/*fb*/	case STI:
	    goto not_implemented;
/*fc*/	case CLD: return (PC);
/*fd*/	case STD: return (PC);

/*fe*/	case GRP2brm: { /* only INC and DEC are legal on bytes */
	    int temp;
	    temp = (*(PC+1)>>3)& 0x7;
	    PC += hsw_modrm_32_byte(MODARGS);
	    switch (temp) {
		case 0: /* INC */
		    mem_ref = MEM_REF;
		    SRC1_8 = temp = *mem_ref;
		    *mem_ref = temp = temp + 1;
		    SRC2_8 = 1;
		    RES_16 = temp << 8; BYTE_FLAG = BYTE_OP;
		    return (PC);
		case 1: /* DEC */
		    mem_ref = MEM_REF;
		    SRC1_8 = temp = *mem_ref;
		    *mem_ref = temp = temp - 1;
		    SRC2_8 = -1;
		    RES_16 = temp << 8; BYTE_FLAG = BYTE_OP;
		    return (PC);
		case 2: /* CALL indirect */
		case 3: /* CALL long indirect */
		case 4: /* JMP indirect */
		case 5: /* JMP long indirect */
		case 6: /* PUSH */
		case 7: /* Illegal */
		goto illegal_op; 
		default: goto not_implemented;
	    }}
/*ff*/	case GRP2wrm: {
	    int temp;
	    DWORD jcs, jip, ocs=0, oip=0;
	    WORD transfer_magic;
	    temp = (*(PC+1)>>3)& 0x7;
	    PC += hsw_modrm_32_word(MODARGS);
	    mem_ref = MEM_REF;
	      switch (temp) {
		case 0: /* INC */
		    temp = mFETCH_WORD(mem_ref);
		    SRC1_16 = temp; SRC2_16 = 1;
		    RES_16 = temp = temp + 1;
		    mPUT_WORD(mem_ref, temp);
		    return (PC);
		case 1: /* DEC */
		    temp = mFETCH_WORD(mem_ref);
		    SRC1_16 = temp; SRC2_16 = -1;
		    RES_16 = temp = temp - 1;
		    mPUT_WORD(mem_ref, temp);
		    return (PC);
		case 2: /* CALL near indirect */
		    temp = PC - LONG_CS;
		    PUSHWORD(temp);
		    PC = LONG_CS + mFETCH_WORD(mem_ref);
		    return (PC);
		case 4: /* JMP near indirect */
		    PC = LONG_CS + mFETCH_WORD(mem_ref);
		    return(PC);
		case 3: /* CALL long indirect restartable */
		    ocs = SHORT_CS_16;
		    oip = PC - LONG_CS;
#ifndef DOSEMU
		    iENV->return_addr = (ocs << 16) | oip;
#endif
		    /* fall through */
		case 5: { /* JMP long indirect restartable */
		    jip = FETCH_WORD(mem_ref);
		    jcs = FETCH_WORD(mem_ref+2);
		    if (!VM86F) {
			transfer_magic = (WORD)GetSelectorType(jcs);
		    }
		    if (VM86F || (transfer_magic==TRANSFER_CODE16)) {
			if ((*err = SET_SEGREG(LONG_CS,BIG_CS,MK_CS,jcs))==EXCP0B_NOSEG) {
			    iVAR->error_addr=jcs&0xfffc; return P0; }
			if (temp==3) {
			    PUSHWORD(ocs);
			    PUSHWORD(oip);
			}
			SHORT_CS_16 = (WORD)jcs;
			PC = jip + LONG_CS;
#ifdef DOSEMU
			if (CEmuStat&CeS_MODE_PM32) {
			    *err = EXCP_GOBACK;
			}
#endif
			return (PC);
		    }
		    else if (transfer_magic==TRANSFER_CODE32) {
#ifdef DOSEMU
			if ((*err = SET_SEGREG(LONG_CS,BIG_CS,MK_CS,jcs))==EXCP0B_NOSEG) {
			    iVAR->error_addr=jcs&0xfffc; return P0; }
			if (temp==3) {
			    PUSHWORD(ocs);
			    PUSHWORD(oip);
			}
			SHORT_CS_16 = (WORD)jcs;
			PC = jip + LONG_CS;
			if (!(CEmuStat&CeS_MODE_PM32)) {
			    *err = EXCP_GOBACK;
			}
			return (PC);
#else
			goto not_implemented;
#endif
		    }
#ifndef DOSEMU
		    if (temp==3) {
			PUSHWORD(ocs);
			PUSHWORD(oip);
		    }
		    if ((transfer_magic == TRANSFER_CALLBACK) ||
				(transfer_magic == TRANSFER_BINARY))  {
			LONGPROC conv,targ;
			SEGIMAGE *lpSegImage = &((SEGIMAGE *)
				(*(long *)(SELECTOR_PADDRESS(jcs))))[jip>>3];
			/* should we transfer flags here? */
			/* trans_interp_flags(ENVARGS); */
			targ = (LONGPROC)lpSegImage->targ;
			conv = (LONGPROC)lpSegImage->conv;
			/* EBP = EBP + (long)LONG_SS;
			   ESP = ESP + (long)LONG_SS; why not? */
			if (transfer_magic == TRANSFER_CALLBACK) {
#ifndef	NO_TRACE_MSGS
			    LogProcName(jcs,jip,1);
#endif
			}
#ifndef	NO_TRACE_MSGS
			else    /* TRANSFER_BINARY */
			    e_printf("do_ext: calling binary thunk %x:%x\n",
			    	(int)jcs, (int)jip);
#endif
			(conv)(env,targ);
#ifndef NO_DEBUG_MSGS
			LogProcName(AX,DX,0);
#endif
			SET_SEGREG(LONG_CS,BIG_CS,MK_CS,jcs);
			SET_SEGREG(LONG_DS,BIG_DS,MK_DS,SHORT_DS_16);
			SET_SEGREG(LONG_ES,BIG_ES,MK_ES,SHORT_ES_16);
			SET_SEGREG(LONG_SS,BIG_SS,MK_SS,SHORT_SS_16);
			/* trans_flags_to_interp(ENVARGS, iENV->flags); */
			/* EBP = EBP - (long)LONG_SS;
			   ESP = ESP - (long)LONG_SS; why not? */
			/* PC = LONG_CS + jip; why not? */
			PC += 5; return (PC);
		    }
		    if (transfer_magic == TRANSFER_RETURN) {
			SHORT_CS_16 = jcs;
			iENV->return_addr = (jcs << 16) | jip;
			trans_interp_flags(ENVARGS);
			/* EBP = EBP + (long)LONG_SS;
			   ESP = ESP + (long)LONG_SS; why not? */
			*err = EXCP_GOBACK;  /* to exit interp_main loop */
			return (PC);
		    }
#endif
		    else
			invoke_data(ENVARGS);   
		    }
		case 6: /* PUSH */
		    temp = mFETCH_WORD(mem_ref);
		    PUSHWORD(temp);
		    return (PC);
		case 7: /* Illegal */
		goto illegal_op; 
		default: goto not_implemented;
	      }
	    }
	default: goto not_implemented;
    }

not_implemented:
	d_emu=9;
	error(" 16/32 nonimplemented instruction %2x %2x %2x at %4x:%4x long PC %x\n",*P0,*(P0+1),*(P0+2),
		SHORT_CS_16,P0-LONG_CS,(int)P0);
#ifndef NO_TRACE_MSGS
	e_trace(ENVARGS, P0, P0, 1);
#endif
	FatalAppExit(0, "INSTR");
	exit(1);

bad_override:
	d_emu=9;
	error(" 16/32 bad code/data sizes at %4x:%4x long PC %x\n",
		SHORT_CS_16,P0-LONG_CS,(int)P0);
#ifndef NO_TRACE_MSGS
	e_trace(ENVARGS, P0, P0, 0);
#endif
	FatalAppExit(0, "SIZE");
	exit(1);
/*
bad_data:
	d_emu=9;
	e_printf(" 16/16 bad code/data at %4x:%4x long PC %x\n",
		SHORT_CS_16,P0-LONG_CS,(int)P0);
	FatalAppExit(0, "OOPS");
	exit(1);
*/
not_permitted:
#ifdef DOSEMU
	*err = EXCP0D_GPF; return P0;
#else
	FatalAppExit(0, "GPF");
	exit(1);
#endif
 
div_by_zero:
#ifdef DOSEMU
	*err = EXCP00_DIVZ; return P0;
#else
	FatalAppExit(0, "DIVZ");
	exit(1);
#endif

illegal_op:
	error(" 16/32 illegal instruction %2x %2x %2x at %4x:%4x long PC %x\n",*P0,*(P0+1),*(P0+2), 
		SHORT_CS_16,P0-LONG_CS,(int)P0);
#ifdef DOSEMU
	*err = EXCP06_ILLOP; return (P0);
#else
	FatalAppExit(0, "ILLEG");
        exit(1); 
#endif
}
