/*
 * math.c
 *
 * Arithmetic, compare and logical opcodes
 *
 */

#include "frotz.h"
#include "..\include\logfile_c.h"

/*
 * z_add, 16bit addition.
 *
 *	zargs[0] = first value
 *	zargs[1] = second value
 *
 */

void z_add (void)
{
	//LOG_ENTER

    store ((zword) ((short) zargs[0] + (short) zargs[1]));

	//LOG_EXIT
}/* z_add */

/*
 * z_and, bitwise AND operation.
 *
 *	zargs[0] = first value
 *	zargs[1] = second value
 *
 */

void z_and (void)
{
	//LOG_ENTER

    store ((zword) (zargs[0] & zargs[1]));

	//LOG_EXIT
}/* z_and */

/*
 * z_art_shift, arithmetic SHIFT operation.
 *
 *	zargs[0] = value
 *	zargs[1] = #positions to shift left (positive) or right
 *
 */

void z_art_shift (void)
{
	//LOG_ENTER

    if ((short) zargs[1] > 0)
	store ((zword) ((short) zargs[0] << (short) zargs[1]));
    else
	store ((zword) ((short) zargs[0] >> - (short) zargs[1]));

	//LOG_EXIT
}/* z_art_shift */

/*
 * z_div, signed 16bit division.
 *
 *	zargs[0] = first value
 *	zargs[1] = second value
 *
 */

void z_div (void)
{
	//LOG_ENTER

    if (zargs[1] == 0)
	runtime_error ("Division by zero");

    store ((zword) ((short) zargs[0] / (short) zargs[1]));

	//LOG_EXIT
}/* z_div */

/*
 * z_je, branch if the first value equals any of the following.
 *
 *	zargs[0] = first value
 *	zargs[1] = second value (optional)
 *	...
 *	zargs[3] = fourth value (optional)
 *
 */

void z_je (void)
{
	//LOG_ENTER

    branch (
	zargc > 1 && (zargs[0] == zargs[1] || (
	zargc > 2 && (zargs[0] == zargs[2] || (
	zargc > 3 && (zargs[0] == zargs[3]))))));

	//LOG_EXIT
}/* z_je */

/*
 * z_jg, branch if the first value is greater than the second.
 *
 *	zargs[0] = first value
 *	zargs[1] = second value
 *
 */

void z_jg (void)
{
	//LOG_ENTER

    branch ((short) zargs[0] > (short) zargs[1]);

	//LOG_EXIT
}/* z_jg */

/*
 * z_jl, branch if the first value is less than the second.
 *
 *	zargs[0] = first value
 *	zargs[1] = second value
 *
 */

void z_jl (void)
{
	//LOG_ENTER

    branch ((short) zargs[0] < (short) zargs[1]);

	//LOG_EXIT
}/* z_jl */

/*
 * z_jz, branch if value is zero.
 *
 * 	zargs[0] = value
 *
 */

void z_jz (void)
{
	//LOG_ENTER

    branch ((short) zargs[0] == 0);

	//LOG_EXIT
}/* z_jz */

/*
 * z_log_shift, logical SHIFT operation.
 *
 * 	zargs[0] = value
 *	zargs[1] = #positions to shift left (positive) or right (negative)
 *
 */

void z_log_shift (void)
{
	//LOG_ENTER

    if ((short) zargs[1] > 0)
	store ((zword) (zargs[0] << (short) zargs[1]));
    else
	store ((zword) (zargs[0] >> - (short) zargs[1]));

	//LOG_EXIT
}/* z_log_shift */

/*
 * z_mod, remainder after signed 16bit division.
 *
 * 	zargs[0] = first value
 *	zargs[1] = second value
 *
 */

void z_mod (void)
{
	//LOG_ENTER

    if (zargs[1] == 0)
	runtime_error ("Division by zero");

    store ((zword) ((short) zargs[0] % (short) zargs[1]));

	//LOG_EXIT
}/* z_mod */

/*
 * z_mul, 16bit multiplication.
 *
 * 	zargs[0] = first value
 *	zargs[1] = second value
 *
 */

void z_mul (void)
{
	//LOG_ENTER

    store ((zword) ((short) zargs[0] * (short) zargs[1]));

	//LOG_EXIT
}/* z_mul */

/*
 * z_not, bitwise NOT operation.
 *
 * 	zargs[0] = value
 *
 */

void z_not (void)
{
	//LOG_ENTER

    store ((zword) ~zargs[0]);

	//LOG_EXIT
}/* z_not */

/*
 * z_or, bitwise OR operation.
 *
 *	zargs[0] = first value
 *	zargs[1] = second value
 *
 */

void z_or (void)
{
	//LOG_ENTER

    store ((zword) (zargs[0] | zargs[1]));

	//LOG_EXIT
}/* z_or */

/*
 * z_sub, 16bit substraction.
 *
 *	zargs[0] = first value
 *	zargs[1] = second value
 *
 */

void z_sub (void)
{
	//LOG_ENTER

    store ((zword) ((short) zargs[0] - (short) zargs[1]));

	//LOG_EXIT
}/* z_sub */

/*
 * z_test, branch if all the flags of a bit mask are set in a value.
 *
 *	zargs[0] = value to be examined
 *	zargs[1] = bit mask
 *
 */

void z_test (void)
{
	//LOG_ENTER

    branch ((zargs[0] & zargs[1]) == zargs[1]);

	//LOG_EXIT
}/* z_test */
