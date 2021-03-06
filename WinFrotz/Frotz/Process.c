/*
 * process.c
 *
 * Interpreter loop and program control
 *
 */

#include "frotz.h"
#include "..\include\logfile_c.h"

zword zargs[8];
int zargc;

#ifdef WINFROTZ
int finished = 0;
#ifndef _CONSOLE
extern int 	zcodeops;
#endif
#else
static finished = 0;
#endif

static void __extended__ (void);
static void __illegal__ (void);

void (*op0_opcodes[0x10]) (void) = {
    z_rtrue,
    z_rfalse,
    z_print,
    z_print_ret,
    z_nop,
    z_save,
    z_restore,
    z_restart,
    z_ret_popped,
    z_catch,
    z_quit,
    z_new_line,
    z_show_status,
    z_verify,
    __extended__,
    z_piracy
};

void (*op1_opcodes[0x10]) (void) = {
    z_jz,
    z_get_sibling,
    z_get_child,
    z_get_parent,
    z_get_prop_len,
    z_inc,
    z_dec,
    z_print_addr,
    z_call_s,
    z_remove_obj,
    z_print_obj,
    z_ret,
    z_jump,
    z_print_paddr,
    z_load,
    z_call_n
};

void (*var_opcodes[0x40]) (void) = {
    __illegal__,
    z_je,
    z_jl,
    z_jg,
    z_dec_chk,
    z_inc_chk,
    z_jin,
    z_test,
    z_or,
    z_and,
    z_test_attr,
    z_set_attr,
    z_clear_attr,
    z_store,
    z_insert_obj,
    z_loadw,
    z_loadb,
    z_get_prop,
    z_get_prop_addr,
    z_get_next_prop,
    z_add,
    z_sub,
    z_mul,
    z_div,
    z_mod,
    z_call_s,
    z_call_n,
    z_set_colour,
    z_throw,
    __illegal__,
    __illegal__,
    __illegal__,
    z_call_s,
    z_storew,
    z_storeb,
    z_put_prop,
    z_read,
    z_print_char,
    z_print_num,
    z_random,
    z_push,
    z_pull,
    z_split_window,
    z_set_window,
    z_call_s,
    z_erase_window,
    z_erase_line,
    z_set_cursor,
    z_get_cursor,
    z_set_text_style,
    z_buffer_mode,
    z_output_stream,
    z_input_stream,
    z_sound_effect,
    z_read_char,
    z_scan_table,
    z_not,
    z_call_n,
    z_call_n,
    z_tokenise,
    z_encode_text,
    z_copy_table,
    z_print_table,
    z_check_arg_count
};

void (*ext_opcodes[0x1d]) (void) = {
    z_save,
    z_restore,
    z_log_shift,
    z_art_shift,
    z_set_font,
    z_draw_picture,
    z_picture_data,
    z_erase_picture,
    z_set_margins,
    z_save_undo,
    z_restore_undo,
    z_print_unicode,
    z_check_unicode,
    __illegal__,
    __illegal__,
    __illegal__,
    z_move_window,
    z_window_size,
    z_window_style,
    z_get_wind_prop,
    z_scroll_window,
    z_pop_stack,
    z_read_mouse,
    z_mouse_window,
    z_push_stack,
    z_put_wind_prop,
    z_print_form,
    z_make_menu,
    z_picture_table
};

/*
 * load_operand
 *
 * Load an operand, either a variable or a constant.
 *
 */

static void load_operand (zbyte type)
{
    zword value;

	//LOG_ENTER
    if (type & 2) { 			/* variable */

	zbyte variable;

	CODE_BYTE (variable)

	if (variable == 0)
	    value = *sp++;
	else if (variable < 16)
	    value = *(fp - variable);
	else {
	    zword addr = h_globals + 2 * (variable - 16);
	    LOW_WORD (addr, value)
	}

    } else if (type & 1) { 		/* small constant */

	zbyte bvalue;

	CODE_BYTE (bvalue)
	value = bvalue;

    } else CODE_WORD (value) 		/* large constant */

    zargs[zargc++] = value;

	//LOG_EXIT
}/* load_operand */

/*
 * load_all_operands
 *
 * Given the operand specifier byte, load all (up to four) operands
 * for a VAR or EXT opcode.
 *
 */

static void load_all_operands (zbyte specifier)
{
    int i;

	//LOG_ENTER
    for (i = 6; i >= 0; i -= 2) {

	zbyte type = (specifier >> i) & 0x03;

	if (type == 3)
	    break;

	load_operand (type);

    }

	//LOG_EXIT
}/* load_all_operands */

/*
 * interpret
 *
 * Z-code interpreter main loop
 *
 */

void interpret (void)
{
	//LOG_ENTER

    do {

	zbyte opcode;

#if defined (WINFROTZ) && !defined( _CONSOLE )
	zcodeops++;
#endif

	CODE_BYTE (opcode)

	zargc = 0;

	if (opcode < 0x80) {			/* 2OP opcodes */

	    load_operand ((zbyte) ((opcode & 0x40) ? 2 : 1));
	    load_operand ((zbyte) ((opcode & 0x20) ? 2 : 1));

	    var_opcodes[opcode & 0x1f] ();

	} else if (opcode < 0xb0) {		/* 1OP opcodes */

	    load_operand ((zbyte) (opcode >> 4));

	    op1_opcodes[opcode & 0x0f] ();

	} else if (opcode < 0xc0) {		/* 0OP opcodes */

	    op0_opcodes[opcode - 0xb0] ();

	} else {				/* VAR opcodes */

	    zbyte specifier1;
	    zbyte specifier2;

	    if (opcode == 0xec || opcode == 0xfa) {	/* opcodes 0xec */
		CODE_BYTE (specifier1)                  /* and 0xfa are */
		CODE_BYTE (specifier2)                  /* call opcodes */
		load_all_operands (specifier1);		/* with up to 8 */
		load_all_operands (specifier2);         /* arguments    */
	    } else {
		CODE_BYTE (specifier1)
		load_all_operands (specifier1);
	    }

	    var_opcodes[opcode - 0xc0] ();

	}

    } while (finished == 0);

    finished--;

	//LOG_EXIT
}/* interpret */

/*
 * call
 *
 * Call a subroutine. Save PC and FP then load new PC and initialise
 * new stack frame. Note that the caller may legally provide less or
 * more arguments than the function actually has. The call type "ct"
 * can be 0 (z_call_s), 1 (z_call_n) or 2 (direct call).
 *
 */

void call (zword routine, int argc, zword *args, int ct)
{
    long pc;
    zword value;
    zbyte count;
    int i;

	//LOG_ENTER
    if (sp - stack < 4)
	runtime_error ("Stack overflow");

    GET_PC (pc)

    *--sp = (zword) (pc >> 9);		/* for historical reasons */
    *--sp = (zword) (pc & 0x1ff);	/* Frotz keeps its stack  */
    *--sp = (zword) (fp - stack - 1);	/* format compatible with */
    *--sp = (zword) (argc | (ct << 8));	/* Mark Howell's Zip      */

    fp = sp;

    /* Calculate byte address of routine */

    if (h_version <= V3)
	pc = (long) routine << 1;
    else if (h_version <= V5)
	pc = (long) routine << 2;
    else if (h_version <= V7)
	pc = ((long) routine << 2) + ((long) h_functions_offset << 3);
    else /* h_version == V8 */
	pc = (long) routine << 3;

    if (pc >= story_size)
	runtime_error ("Call to illegal address");

    SET_PC (pc)

    /* Initialise local variables */

    CODE_BYTE (count)

    if (count > 15)
	runtime_error ("Call to non-routine");
    if (sp - stack < count)
	runtime_error ("Stack overflow");

    value = 0;

    for (i = 0; i < count; i++) {

	if (h_version <= V4)		/* V1 to V4 games provide default */
	    CODE_WORD (value)		/* values for all local variables */

	*--sp = (zword) ((argc-- > 0) ? args[i] : value);

    }

    /* Start main loop for direct calls */

    if (ct == 2)
	interpret ();

	//LOG_EXIT
}/* call */

/*
 * ret
 *
 * Return from the current subroutine and restore the previous stack
 * frame. The result may be stored (0), thrown away (1) or pushed on
 * the stack (2). In the latter case a direct call has been finished
 * and we must exit the interpreter loop.
 *
 */

void ret (zword value)
{
    long pc;
    int ct;

	//LOG_ENTER
    if (sp > fp)
	runtime_error ("Stack underflow");

    sp = fp;

    ct = *sp++ >> 8;
    fp = stack + 1 + *sp++;
    pc = *sp++;
    pc = ((long) *sp++ << 9) | pc;

    SET_PC (pc)

    /* Handle resulting value */

    if (ct == 0)
	store (value);
    if (ct == 2)
	*--sp = value;

    /* Stop main loop for direct calls */

    if (ct == 2)
	finished++;

	//LOG_EXIT
}/* ret */

/*
 * branch
 *
 * Take a jump after an instruction based on the flag, either true or
 * false. The branch can be short or long; it is encoded in one or two
 * bytes respectively. When bit 7 of the first byte is set, the jump
 * takes place if the flag is true; otherwise it is taken if the flag
 * is false. When bit 6 of the first byte is set, the branch is short;
 * otherwise it is long. The offset occupies the bottom 6 bits of the
 * first byte plus all the bits in the second byte for long branches.
 * Uniquely, an offset of 0 means return false, and an offset of 1 is
 * return true.
 *
 */

void branch (bool flag)
{
    long pc;
    zword offset;
    zbyte specifier;
    zbyte off1;
    zbyte off2;

	//LOG_ENTER
    CODE_BYTE (specifier)

    off1 = specifier & 0x3f;

    if (!flag)
	specifier ^= 0x80;

    if (!(specifier & 0x40)) {		/* it's a long branch */

	if (off1 & 0x20)		/* propagate sign bit */
	    off1 |= 0xc0;

	CODE_BYTE (off2)

	offset = (off1 << 8) | off2;

    } else offset = off1;		/* it's a short branch */

    if (specifier & 0x80)

	if (offset > 1) {		/* normal branch */

	    GET_PC (pc)
	    pc += (short) offset - 2;
	    SET_PC (pc)

	} else ret (offset);		/* special case, return 0 or 1 */

	//LOG_EXIT
}/* branch */

/*
 * store
 *
 * Store an operand, either as a variable or pushed on the stack.
 *
 */

void store (zword value)
{
    zbyte variable;

	//LOG_ENTER
    CODE_BYTE (variable)

    if (variable == 0)
	*--sp = value;
    else if (variable < 16)
	*(fp - variable) = value;
    else {
	zword addr = h_globals + 2 * (variable - 16);
	SET_WORD (addr, value)
    }

	//LOG_EXIT
}/* store */

/*
 * direct_call
 *
 * Call the interpreter loop directly. This is necessary when
 *
 * - a sound effect has been finished
 * - a read instruction has timed out
 * - a newline countdown has hit zero
 *
 * The interpreter returns the result value on the stack.
 *
 */

int direct_call (zword addr)
{
    zword saved_zargs[8];
    int saved_zargc;
    int i;

	//LOG_ENTER
    /* Calls to address 0 return false */

    if (addr == 0)
	{
	//LOG_EXIT
	return 0;
	}

    /* Save operands and operand count */

    for (i = 0; i < 8; i++)
	saved_zargs[i] = zargs[i];

    saved_zargc = zargc;

    /* Call routine directly */

    call (addr, 0, 0, 2);

    /* Restore operands and operand count */

    for (i = 0; i < 8; i++)
	zargs[i] = saved_zargs[i];

    zargc = saved_zargc;

    /* Resulting value lies on top of the stack */

	//LOG_EXIT
    return (short) *sp++;

}/* direct_call */

/*
 * __extended__
 *
 * Load and execute an extended opcode.
 *
 */

static void __extended__ (void)
{
    zbyte opcode;
    zbyte specifier;

	//LOG_ENTER
    CODE_BYTE (opcode)
    CODE_BYTE (specifier)

    load_all_operands (specifier);

    if (opcode < 0x1d)			/* extended opcodes from 0x1d on */
	ext_opcodes[opcode] ();		/* are reserved for future spec' */

	//LOG_EXIT
}/* __extended__ */

/*
 * __illegal__
 *
 * Exit game because an unknown opcode has been hit.
 *
 */

static void __illegal__ (void)
{
	//LOG_ENTER

    runtime_error ("Illegal opcode");

	//LOG_EXIT
}/* __illegal__ */

/*
 * z_catch, store the current stack frame for later use with z_throw.
 *
 *	no zargs used
 *
 */

void z_catch (void)
{
	//LOG_ENTER

    store ((zword) (fp - stack));

	//LOG_EXIT
}/* z_catch */

/*
 * z_throw, go back to the given stack frame and return the given value.
 *
 *	zargs[0] = value to return
 *	zargs[1] = stack frame
 *
 */

void z_throw (void)
{
	//LOG_ENTER

    if (zargs[1] > STACK_SIZE)
	runtime_error ("Bad stack frame");

    fp = stack + zargs[1];

    ret (zargs[0]);

	//LOG_EXIT
}/* z_throw */

/*
 * z_call_n, call a subroutine and discard its result.
 *
 * 	zargs[0] = packed address of subroutine
 *	zargs[1] = first argument (optional)
 *	...
 *	zargs[7] = seventh argument (optional)
 *
 */

void z_call_n (void)
{

	//LOG_ENTER
    if (zargs[0] != 0)
	call (zargs[0], zargc - 1, zargs + 1, 1);

	//LOG_EXIT
}/* z_call_n */

/*
 * z_call_s, call a subroutine and store its result.
 *
 * 	zargs[0] = packed address of subroutine
 *	zargs[1] = first argument (optional)
 *	...
 *	zargs[7] = seventh argument (optional)
 *
 */

void z_call_s (void)
{
	//LOG_ENTER

    if (zargs[0] != 0)
	call (zargs[0], zargc - 1, zargs + 1, 0);
    else
	store (0);

	//LOG_EXIT
}/* z_call_s */

/*
 * z_check_arg_count, branch if subroutine was called with >= n arg's.
 *
 * 	zargs[0] = number of arguments
 *
 */

void z_check_arg_count (void)
{
	//LOG_ENTER

    if (fp == stack + STACK_SIZE)
	branch (zargs[0] == 0);
    else
	branch (zargs[0] <= (*fp & 0xff));

	//LOG_EXIT
}/* z_check_arg_count */

/*
 * z_jump, jump unconditionally to the given address.
 *
 *	zargs[0] = PC relative address
 *
 */

void z_jump (void)
{
    long pc;

	//LOG_ENTER
    GET_PC (pc)

    pc += (short) zargs[0] - 2;

    if (pc >= story_size)
	runtime_error ("Jump to illegal address");

    SET_PC (pc)

	//LOG_EXIT
}/* z_jump */

/*
 * z_nop, no operation.
 *
 *	no zargs used
 *
 */

void z_nop (void)
{
	//LOG_ENTER

    /* Do nothing */

	//LOG_EXIT
}/* z_nop */

/*
 * z_quit, stop game and exit interpreter.
 *
 *	no zargs used
 *
 */

void z_quit (void)
{
	//LOG_ENTER

    finished = 9999;

	//LOG_EXIT
}/* z_quit */

/*
 * z_ret, return from a subroutine with the given value.
 *
 *	zargs[0] = value to return
 *
 */

void z_ret (void)
{
	//LOG_ENTER

    ret (zargs[0]);

	//LOG_EXIT
}/* z_ret */

/*
 * z_ret_popped, return from a subroutine with a value popped off the stack.
 *
 *	no zargs used
 *
 */

void z_ret_popped (void)
{
	//LOG_ENTER

    ret (*sp++);

	//LOG_EXIT
}/* z_ret_popped */

/*
 * z_rfalse, return from a subroutine with false (0).
 *
 * 	no zargs used
 *
 */

void z_rfalse (void)
{
	//LOG_ENTER

    ret (0);

	//LOG_EXIT
}/* z_rfalse */

/*
 * z_rtrue, return from a subroutine with true (1).
 *
 * 	no zargs used
 *
 */

void z_rtrue (void)
{
	//LOG_ENTER

    ret (1);

	//LOG_EXIT
}/* z_rtrue */
