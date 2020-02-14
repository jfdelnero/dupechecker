/*
//
// Copyright (C) 2006-2019 Jean-François DEL NERO
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
// (c) 2008-2019 Jean-François DEL NERO
//
// File : file_tree_db.c
// Contains: Files tree database
//
// Written by:	Jean-François DEL NERO
//
// Change History (most recent first):
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "md5.h"
#include "file_tree_db.h"
#include "file_utils.h"
#include "utils.h"

void compute_md5(filedescription * newfile,int partial)
{
	struct MD5Context md5c;
	unsigned char signature[16],i;
	unsigned char * checkbuffer;
	FILE * filetocheck;
	unsigned int sizetocheck;
	uint64_t byteremaing;
	int status;

	status=0;

	if(partial==1)
	{
		if( newfile->status & PARTIAL_MD5_DONE )
		{
			// Partial check already done.
			return;
		}

		if(newfile->size > PARTIALMD5CHECKBUFFERSIZE)
		{
			byteremaing = PARTIALMD5CHECKBUFFERSIZE;
			status = PARTIAL_MD5_DONE;
		}
		else
		{
			byteremaing = newfile->size;
			status = PARTIAL_MD5_DONE|FULL_MD5_DONE;
		}
	}
	else
	{
		if( newfile->status & FULL_MD5_DONE )
		{
			// Full check already done.
			return;
		}

		status = FULL_MD5_DONE;
		byteremaing = newfile->size;
	}

	memset( signature,0,sizeof(signature));

	filetocheck = (FILE*)fopen(newfile->filepath,"rb");
	if(filetocheck)
	{
		checkbuffer = (unsigned char *) malloc(BUFFERSIZE);
		if(checkbuffer)
		{
			memset( &md5c,0,sizeof(md5c));

			MD5Init(&md5c);

			do
			{
				if( byteremaing > BUFFERSIZE )
					sizetocheck = BUFFERSIZE;
				else
					sizetocheck = (unsigned int)byteremaing;

				if(fread(checkbuffer,sizetocheck,1,filetocheck))
				{
					MD5Update(&md5c, (unsigned char*)checkbuffer, sizetocheck);

					byteremaing -= sizetocheck;
				}
				else
				{
					printf("MD5 : Read error - %s\n",newfile->filepath);
					free(checkbuffer);
					fclose(filetocheck);

					for(i=0;i<16;i++)
					{
						newfile->md5[i] = 0x00;
					}
					newfile->status = 0x00;
					return;
				}
			}while(byteremaing);

			MD5Final(signature, &md5c);

			free(checkbuffer);


			for(i=0;i<16;i++)
			{
				newfile->md5[i] = signature[i];
			}

			newfile->status |= status;
		}
		else
		{
			for(i=0;i<16;i++)
			{
				newfile->md5[i] = 0x00;
			}
			newfile->status = 0x00;
		}

		fclose(filetocheck);
	}
	else
	{
		printf("MD5 : Access error - %s\n",newfile->filepath);

		for(i=0;i<16;i++)
		{
			newfile->md5[i] = 0x00;
		}
		newfile->status = 0x00;
	}
}

int browse_and_make_fs_tree(filedescription * entrypoint)
{
	void * hfindfile;
	filefoundinfo FindFileData;
	int bbool;
	char * destinationfolder;
	char * fullpath;
	filedescription * new_entrypoint;

	int number_of_file;

	number_of_file=0;
	hfindfile = find_first_file(entrypoint->filepath,"*.*", &FindFileData);
	if( hfindfile )
	{
		bbool=1;
		while( hfindfile && bbool )
		{
			if(FindFileData.isdirectory)
			{
				if(strcmp(".",FindFileData.filename)!=0 && strcmp("..",FindFileData.filename)!=0)
				{
					destinationfolder = malloc(strlen(FindFileData.filename)+2);
					if(!destinationfolder)
						goto alloc_error;

					sprintf(destinationfolder,FILEPATHSEPARATOR"%s",FindFileData.filename);

					fullpath = malloc(strlen(FindFileData.filename)+strlen(entrypoint->filepath)+2);
					if(!fullpath)
						goto alloc_error;

					sprintf(fullpath,"%s"FILEPATHSEPARATOR"%s",entrypoint->filepath,FindFileData.filename);

					printf("Entering directory %s\n",FindFileData.filename);

					new_entrypoint = (filedescription*) malloc(sizeof(filedescription));
					if(!new_entrypoint)
						goto alloc_error;

					memset(new_entrypoint,0,sizeof(filedescription));

					new_entrypoint->filename = alloc_and_print(FindFileData.filename);
					new_entrypoint->filepath = alloc_and_print(fullpath);

					new_entrypoint->status = new_entrypoint->status|FOLDER;

					number_of_file = browse_and_make_fs_tree(new_entrypoint)+number_of_file;

					//browse_and_convert_directory(fullpath,destinationfolder,file,filelist);
					entrypoint->number_of_entry++;
					entrypoint->filedescription_array = (filedescription**) realloc(entrypoint->filedescription_array,entrypoint->number_of_entry*sizeof(filedescription*));
					if(!entrypoint->filedescription_array)
						goto alloc_error;

					entrypoint->filedescription_array[entrypoint->number_of_entry-1]=new_entrypoint;

					entrypoint->size=entrypoint->size+new_entrypoint->size;

					free(destinationfolder);
					free(fullpath);
					printf("Leaving directory %s\n",FindFileData.filename);
				}
			}
			else
			{
				//printf("checking file %s, %dB\n",FindFileData.filename,FindFileData.size)

				if(FindFileData.size)
				{

					fullpath = malloc(strlen(FindFileData.filename)+strlen(entrypoint->filepath)+2);
					if(!fullpath)
						goto alloc_error;

					sprintf(fullpath,"%s"FILEPATHSEPARATOR"%s",entrypoint->filepath,FindFileData.filename);

					new_entrypoint=(filedescription*) malloc(sizeof(filedescription));
					if(!new_entrypoint)
						goto alloc_error;

					memset(new_entrypoint,0,sizeof(filedescription));

					new_entrypoint->filename=alloc_and_print(FindFileData.filename);
					new_entrypoint->filepath=alloc_and_print(fullpath);
					new_entrypoint->size=FindFileData.size;

					number_of_file++;

					entrypoint->number_of_entry++;
					entrypoint->filedescription_array=(filedescription**) realloc(entrypoint->filedescription_array,entrypoint->number_of_entry*sizeof(filedescription*));
					if(!entrypoint->filedescription_array)
						goto alloc_error;

					entrypoint->filedescription_array[entrypoint->number_of_entry-1]=new_entrypoint;

					entrypoint->size=entrypoint->size+new_entrypoint->size;

					//add_file_element(fullpath,FindFileData.filename,FindFileData.size,0,filelist,0);

					free(fullpath);
				}
			}

			bbool = find_next_file(hfindfile,entrypoint->filepath,"*.*",&FindFileData);
		}

	}
	else printf("Error FindFirstFile\n");

	find_close(hfindfile);

	return number_of_file;

alloc_error:
	printf("browse_and_make_fs_tree : Memory allocation error !\n");
	return -1;
}

void quicksort_file_size(filedescription * tabfile[], int index_gauche,int index_droit)
{
	filedescription * tampon;
	uint64_t valeur_pivot;//, tampon;
	int i, j;


	if(index_gauche < index_droit)
	{
		valeur_pivot = tabfile[index_droit]->size;
		i = index_gauche - 1;
		j = index_droit;

		for(;;)
		{
			do
			{
				i++;
			}while(tabfile[i]->size < valeur_pivot);

			do
			{
				j--;
			}while(tabfile[j]->size > valeur_pivot && j );

			if( i>=j )
			{
				break;
			}

			tampon  = tabfile[i];
			tabfile[i] = tabfile [j];
			tabfile[j] = tampon;
		}

		tampon = tabfile[i];
		tabfile[i] = tabfile[index_droit];
		tabfile[index_droit] = tampon;

		quicksort_file_size(tabfile, index_gauche, i-1);
		quicksort_file_size(tabfile, i+1, index_droit);
	}
}

void quicksort_file_md5(filedescription * tabfile[], int index_gauche,int index_droit)
{
	filedescription * tampon;
	unsigned char * md5buf;
	int i, j;

	if(index_gauche < index_droit)
	{
		md5buf = tabfile[index_droit]->md5;

		i = index_gauche - 1;
		j = index_droit;

		for(;;)
		{
			do
			{
				i++;
			}while( memcmp(tabfile[i]->md5,md5buf,16) < 0 );

			do
			{
				j--;
			}while( ( memcmp(tabfile[j]->md5,md5buf,16) > 0 ) && j );

			if( i>=j )
			{
				break;
			}

			tampon  = tabfile[i];
			tabfile[i] = tabfile [j];
			tabfile[j] = tampon;
		}

		tampon = tabfile[i];
		tabfile[i] = tabfile[index_droit];
		tabfile[index_droit] = tampon;

		quicksort_file_md5(tabfile, index_gauche, i-1);
		quicksort_file_md5(tabfile, i+1, index_droit);
	}
}

void quicksort_file_size_and_md5(filedescription * tabfile[], int index_gauche,int index_droit)
{
	int cur_size,index,start_index;

	//quicksort_file_size(tabfile,index_gauche,index_droit);

	cur_size = tabfile[index_gauche]->size;

	start_index = index_gauche;
	index = index_gauche;

	while( index < index_droit )
	{
		start_index = index;
		cur_size = tabfile[index]->size;

		while( index < index_droit && (cur_size == tabfile[index]->size) )
		{
			index++;
		}

		if((index - start_index)>1)
		{
			quicksort_file_md5(&tabfile[start_index], 0, (index - start_index) - 1 );
		}
	}
}

void print_file_desc(filedescription * file)
{
	int i;

	printf("------------------------------\n");
	printf("%s\n",file->filename);
	printf("%s\n",file->filepath);
	printf("Status : 0x%.2x\n",file->status);
	printf("Size : %ld\n",file->size);
	printf("Number of Entry : %d\n",file->number_of_entry);

	printf("MD5 : ");

	for(i=0;i<16;i++)
	{
		printf("%.2X",file->md5[i]);
	}
	printf("\n");
	printf("------------------------------\n");

	return;
}

filedescription ** duplicate_file_array(filedescription * file_array[], int * number_of_files)
{
	unsigned int i,j,count;
	filedescription ** new_array;

	new_array = 0;
	count = 0;

	if( *number_of_files > 0 )
	{
		for(i = 0 ; i < *number_of_files ; i++)
		{
			if(!(file_array[i]->status & IGNORE_FILE))
			{
				count++;
			}
		}

		new_array = malloc(sizeof(filedescription *) * count);
		if(new_array)
		{
			memset(new_array,0,sizeof(filedescription *) * count);

			j = 0;
			for(i = 0 ; i < *number_of_files ; i++)
			{
				if(!(file_array[i]->status & IGNORE_FILE))
				{
					new_array[j] = file_array[i];
					j++;
				}
			}

			*number_of_files = count;
		}
		else
			*number_of_files = 0;
	}

	return new_array;
}

filedescription ** cleanup_filelist(filedescription * tabfile[], int * number_of_files)
{
	int i;
	filedescription ** tab;

	tab = 0;

	if(*number_of_files > 1 && tabfile)
	{
		if(tabfile[0]->size != tabfile[1]->size)
		{
			tabfile[0]->status |= IGNORE_FILE;
		}

		i = 1;
		while(i < ( *number_of_files - 1 ))
		{
			if((tabfile[i]->size != tabfile[i-1]->size)  && (tabfile[i]->size != tabfile[i+1]->size))
			{
				tabfile[i]->status |= IGNORE_FILE;
			}
			i++;
		}

		if(tabfile[i]->size != tabfile[i-1]->size)
		{
			tabfile[i]->status |= IGNORE_FILE;
		}

		tab = duplicate_file_array(tabfile,number_of_files);
	}
	else
		*number_of_files = 0;

	if(tabfile)
		free(tabfile);

	return tab;
}

filedescription ** cleanup_filelist_md5(filedescription * tabfile[], int * number_of_files)
{
	int i;
	filedescription ** tab;

	tab = 0;

	if(*number_of_files > 1 && tabfile)
	{
		if( memcmp(tabfile[0]->md5,tabfile[1]->md5,16) ||
			!( tabfile[0]->status & (PARTIAL_MD5_DONE|FULL_MD5_DONE) )
		)
		{
			tabfile[0]->status |= IGNORE_FILE;
		}

		i = 1;

		while(i<(*number_of_files-1))
		{

			if(
					( memcmp(tabfile[i]->md5,tabfile[i-1]->md5,16) && memcmp(tabfile[i]->md5,tabfile[i+1]->md5,16) ) ||
					!( tabfile[i]->status & (PARTIAL_MD5_DONE|FULL_MD5_DONE) )
				)
			{
				tabfile[i]->status |= IGNORE_FILE;
			}
			i++;
		};

		if( i < *number_of_files )
		{
			if(
				memcmp(tabfile[i]->md5,tabfile[i-1]->md5,16)  ||
				!( tabfile[i]->status & (PARTIAL_MD5_DONE|FULL_MD5_DONE) )
			)
			{
				tabfile[i]->status |= IGNORE_FILE;
			}
		}

		tab = duplicate_file_array(tabfile,number_of_files);
	}
	else
		*number_of_files = 0;


	if(tabfile)
		free(tabfile);

	return tab;
}


filedescription * create_filedesctription(char * path,char * filename,uint64_t filesize, int makemd5)
{
	unsigned char signature[16],i;
	filedescription * newfile;

	newfile=0;

	newfile=(filedescription *)malloc(sizeof(filedescription));
	if(newfile)
	{
		memset( newfile,0,sizeof(filedescription));
		memset( signature,0,sizeof(signature));

		newfile->size = filesize;

		newfile->filename=(char*)malloc(strlen((char*)filename)+1);
		if(newfile->filename)
			sprintf(newfile->filename,"%s",filename);

		newfile->filepath=(char*)malloc(strlen((char*)path)+1);
		if(newfile->filepath)
			sprintf(newfile->filepath,"%s",path);

		newfile->status=0;
		newfile->previouselement=0;

		if(makemd5)
		{
			compute_md5(newfile,0);
		}

		for(i=0;i<16;i++)
		{
			newfile->md5[i] = signature[i];
		}
	}

	return newfile;
}

void add_file_element(char * path,char * filename,uint64_t filesize, int makemd5,filelisthead * listhead,unsigned char * md5)
{
	filedescription * thefile;

	thefile = create_filedesctription(path,filename,filesize,makemd5);
	if(thefile)
	{
		if(md5 && makemd5 == 0)
		{
			memcpy(thefile->md5,md5,16);
		}

		thefile->previouselement = listhead->filelist;
		listhead->filelist = thefile;

		listhead->totalsize = listhead->totalsize+thefile->size;
		listhead->numberoffile++;
	}
}

void dealloc_filelisthead(filelisthead *fl)
{
	unsigned long numberoffile,i;
	filedescription * fileelement;
	filedescription * tmp_fileelement;

	if(fl)
	{
		numberoffile = fl->numberoffile;
		fileelement = fl->filelist;

		if(fileelement)
		{
			i = 0;
			do
			{
				tmp_fileelement = fileelement->previouselement;
				free_fileelement(fileelement);
				fileelement = tmp_fileelement;
				i++;
			}while ( fileelement && i<numberoffile);
		}

		free(fl);
	}
}

filedescription ** tabification(filelisthead * filelist)
{
	unsigned long i;
	filedescription ** tabelement;
	filedescription * fileelement;

	tabelement = 0;

	if( filelist)
	{
		tabelement=(filedescription **)malloc(sizeof(filedescription *)*filelist->numberoffile);
		if(tabelement)
		{
			memset(tabelement,0,sizeof(filedescription *)*filelist->numberoffile);

			fileelement = filelist->filelist;

			for(i=0;i<filelist->numberoffile;i++)
			{
				tabelement[i] = fileelement;
				fileelement = fileelement->previouselement;
			}
		}
	}

	return tabelement;
}

filedescription ** getfilelist(filedescription * fs_tree,filedescription ** filelist,int * numberoffile)
{
	unsigned long i;

	i=0;
	while( i < fs_tree->number_of_entry )
	{
		if( fs_tree->filedescription_array[i]->status & FOLDER )
		{
			filelist = getfilelist(fs_tree->filedescription_array[i],filelist,numberoffile);
		}
		else
		{
			*numberoffile = *numberoffile+1;
			filelist = (filedescription **)realloc( filelist , sizeof(filedescription *)* *numberoffile);
			filelist[*numberoffile-1] = fs_tree->filedescription_array[i];
		}
		i++;
	}

	return filelist;
}

void free_fileelement(filedescription * filetofree)
{
	if(filetofree->filename)
		free(filetofree->filename);

	if(filetofree->filepath)
		free(filetofree->filepath);

	if(filetofree->filedescription_array)
		free(filetofree->filedescription_array);

	free(filetofree);
}

void freefiletree(filedescription * fs_tree)
{
	unsigned long i;

	i=0;
	while( i < fs_tree->number_of_entry )
	{
		if( fs_tree->filedescription_array[i]->status & FOLDER )
		{
			freefiletree(fs_tree->filedescription_array[i]);
		}
		else
		{
			free_fileelement(fs_tree->filedescription_array[i]);
		}
		i++;
	}

	free_fileelement(fs_tree);

	return;
}

uint64_t get_total_size(filedescription ** fileslist, int numberoffile)
{
	unsigned long i;
	uint64_t totalsize;

	totalsize = 0;

	if( fileslist )
	{
		for(i=0;i<numberoffile;i++)
		{
			totalsize = totalsize + fileslist[i]->size;
		}
	}

	return totalsize;
}
