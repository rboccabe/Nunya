/*
Copyright (C) 2015 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include "kerneltypes.h"
#include "string.h"

void console_init();
void console_putchar( char c );
int  console_write( int unit, const void *buffer, int nblocks, int offset );
void console_heartbeat();
void console_set_fgcolor(int r, int g, int b);
void console_set_bgcolor(int r, int g, int b);
int console_verify_color_range(int x);

#define console_printf printf

#endif
