/*
//
// Copyright (C) 2006-2022 Jean-François DEL NERO
//
// This file is part of the DupeChecker software
//
// DupeChecker may be used and distributed without restriction provided
// that this copyright statement is not removed from the file and that any
// derivative work contains the original copyright notice and the associated
// disclaimer.
//
// DupeChecker is free software; you can redistribute it
// and/or modify  it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// DupeChecker is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DupeChecker; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
*/
//﻿-----------------------------------------------------------------------------
//----------H----H--X----X-----CCCCC-----22222----0000-----0000-----11---------
//---------H----H----X-X-----C--------------2---0----0---0----0---1-1----------
//--------HHHHHH-----X------C----------22222---0----0---0----0-----1-----------
//-------H----H----X--X----C----------2-------0----0---0----0-----1------------
//------H----H---X-----X---CCCCC-----22222----0000-----0000----11111-----------
//-----------------------------------------------------------------------------
//-- Contact: hxc2001<at>hxc2001.com ------------------- https://hxc2001.com --
//-----------------------------------------------------------------------------
// DupeChecker
// (c) 2008-2022 Jean-François DEL NERO
//
// File : utils.c
// Contains: utils functions
//
// Written by:	Jean-François DEL NERO
//
// Change History (most recent first):
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "types_def.h"

void printsize(SIZE_TYPE size)
{
	SIZE_TYPE t;

	if(size<1024) // octets
	{
		printf(PRT_SIZE_TYPE" B ",size);
	}
	else
	{
		if(size<(1024*1024)) // koctets
		{
			printf("%.2f KB ",(float)size/(float)1024);
		}
		else
		{
			if(size<(1024*1024*1024))// Moctets
			{
				printf("%.2f MB ",(float)size/(float)(1024*1024));

			}
			else
			{
				t=1024*1024*1024;
				t=t*1024;
				if(size<t)// Goctets
				{
					printf("%.2f GB ",(float)size/(float)(1024*1024*1024));

				}
				else
				{   // tera
					t=size/(1024*1024*1024);
					printf("%.2f TB ",(float)t/(float)(1024));
				}
			}
		}
	}
}

char * alloc_and_print(char * string)
{
	char * stringtoreturn;

	stringtoreturn = (char *)malloc(strlen(string)+1);
	sprintf(stringtoreturn,"%s",string);

	return stringtoreturn;
}
