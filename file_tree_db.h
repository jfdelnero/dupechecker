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
// File : file_tree_db.c
// Contains: Files tree database
//
// Written by:	Jean-François DEL NERO
//
// Change History (most recent first):
//-----------------------------------------------------------------------------

#define PARTIALMD5CHECKBUFFERSIZE 8*1024
#define BUFFERSIZE 64*1024*4

typedef struct filedescription_
{
	char * filepath;
	char * filename;
	unsigned char status;
	SIZE_TYPE size;
	CNT_TYPE number_of_entry;
	struct filedescription_ ** filedescription_array;
	unsigned char md5[16];
	void * previouselement;
}filedescription;

typedef struct filelisthead_
{
	filedescription * filelist;
	CNT_TYPE numberoffile;
	SIZE_TYPE totalsize;
}filelisthead;

#define PARTIAL_MD5_DONE 0x01
#define FULL_MD5_DONE 0x02
#define IGNORE_FILE 0x04
#define FOLDER 0x08

void free_fileelement(filedescription * filetofree);

void compute_md5(filedescription * newfile,int partial);
CNT_TYPE browse_and_make_fs_tree(filedescription * entrypoint);

void quicksort_file_size(filedescription * tabfile[], CNT_TYPE index_gauche, CNT_TYPE index_droit);
void quicksort_file_md5(filedescription * tabfile[], CNT_TYPE index_gauche, CNT_TYPE index_droit);
void quicksort_file_size_and_md5(filedescription * tabfile[], CNT_TYPE index_gauche, CNT_TYPE index_droit);

filedescription ** cleanup_filelist(filedescription * tabfile[], CNT_TYPE * numberoffile);
filedescription ** cleanup_filelist_md5(filedescription * tabfile[], CNT_TYPE * number_of_files);
filedescription * create_filedesctription(char * path, char * filename, SIZE_TYPE filesize, int makemd5);
void add_file_element(char * path, char * filename, SIZE_TYPE filesize, int makemd5, filelisthead * listhead, unsigned char * md5);
filedescription ** tabification(filelisthead * filelist);
filedescription ** getfilelist(filedescription * fs_tree, filedescription ** filelist, CNT_TYPE * numberoffile);
void dealloc_filelisthead(filelisthead *fl);

void freefiletree(filedescription * fs_tree);

void print_file_desc(filedescription * file);

void free_fileelement(filedescription * filetofree);
SIZE_TYPE get_total_size(filedescription ** fileslist, CNT_TYPE numberoffile);

