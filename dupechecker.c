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
// File : dupechecker.c
// Contains: Main code
//
// Written by: Jean-François DEL NERO
//
// Change History (most recent first):
//-----------------------------------------------------------------------------

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>

#include "types_def.h"

#include "dupechecker.h"
#include "file_utils.h"
#include "file_tree_db.h"
#include "utils.h"
#include "md5.h"

int write_outputfile(char * filename,filelisthead *fl)
{
	FILE * duplog;
	CNT_TYPE numberoffile,i,j;
	filedescription * fileelement;

	duplog = fopen(filename,"w");
	if(duplog)
	{
		numberoffile = fl->numberoffile;
		fileelement = fl->filelist;

		i = 0;
		do
		{
			if(!(fileelement->status&IGNORE_FILE))
			{
				fprintf(duplog,"%s;%s;"PRT_SIZE_TYPE";",fileelement->filename,fileelement->filepath,fileelement->size);
				for(j=0;j<16;j++)
				{
					fprintf(duplog,"%.2X",fileelement->md5[j]);
				}
				fprintf(duplog,"\n");
			}

			fileelement = fileelement->previouselement;
			i++;
		}while ( fileelement && i<numberoffile);

		fclose(duplog);

		return 0;
	}
	else
	{
		printf("Error ! Cannot create the output file %s !\n",filename);

		return -1;
	}
}

int write_deloutputfile(char * filename,filelisthead *fl)
{
	FILE * duplog;
	unsigned char signature[16];
	CNT_TYPE numberoffile,i,j;
	filedescription * fileelement;

	memset(signature,0,16);

	duplog = fopen(filename,"w");
	if(duplog)
	{
		numberoffile = fl->numberoffile;
		fileelement = fl->filelist;

		i = 0;
		while ( fileelement && i<numberoffile)
		{
			if(!(fileelement->status&IGNORE_FILE))
			{
				if(memcmp(signature,fileelement->md5,16))
				{
					fprintf(duplog,"\n");
					fprintf(duplog,"# %s, "PRT_SIZE_TYPE" bytes, ",fileelement->filename,fileelement->size);
					for(j=0;j<16;j++)
					{
						fprintf(duplog,"%.2X",fileelement->md5[j]);
					}
					fprintf(duplog,"\n");

					fprintf(duplog,"##rm -f \"%s\"",fileelement->filepath);
				}
				else
				{
					fprintf(duplog,"#rm -f \"%s\"",fileelement->filepath);
				}

				fprintf(duplog,"\n");

				memcpy(signature,fileelement->md5,16);
			}

			fileelement = fileelement->previouselement;
			i++;
		}

		fclose(duplog);

		return 0;
	}
	else
	{
		printf("Error ! Cannot create the output file %s !\n",filename);
		return -1;
	}
}


int isinteresting(filedescription * fileelement,char * pathofinterest)
{
	unsigned char signature[16];
	unsigned long path_base_len;
	CNT_TYPE cnt_matching_path,cnt_not_matching_path;

	cnt_not_matching_path = 0;
	cnt_matching_path = 0;

	memcpy(signature,fileelement->md5,16);

	path_base_len = strlen(pathofinterest);

	while ( fileelement )
	{
		if(!(fileelement->status&IGNORE_FILE))
		{
			if(memcmp(signature,fileelement->md5,16))
			{
				if(cnt_matching_path && cnt_not_matching_path)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}

			if(!memcmp(fileelement->filepath, pathofinterest,path_base_len))
			{
				cnt_matching_path++;
			}
			else
			{
				cnt_not_matching_path++;
			}

			memcpy(signature,fileelement->md5,16);
		}

		fileelement = fileelement->previouselement;
	}

	if(cnt_matching_path && cnt_not_matching_path)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int filterlist(filelisthead *fl, char * pathofinterest)
{
	unsigned char signature[16];
	CNT_TYPE numberoffile,i;
	int is_interesting;
	filedescription * fileelement;

	if(!strlen(pathofinterest))
	{
		return 0;
	}

	memset(signature,0,16);

	numberoffile = fl->numberoffile;
	fileelement = fl->filelist;

	is_interesting = 0;

	i = 0;
	while ( fileelement && i<numberoffile)
	{
		if(!(fileelement->status&IGNORE_FILE))
		{
			if(memcmp(signature,fileelement->md5,16))
			{
				is_interesting = isinteresting(fileelement,pathofinterest);
			}

			if(!is_interesting)
			{
				fileelement->status |= IGNORE_FILE;
			}

			memcpy(signature,fileelement->md5,16);
		}

		fileelement = fileelement->previouselement;
		i++;
	}

	return 0;
}


int isOption(int argc, char* argv[],char * paramtosearch,char * argtoparam)
{
	int param=1;
	int i,j;

	char option[512];

	memset(option,0,512);
	while(param<=argc)
	{
		if(argv[param])
		{
			if(argv[param][0]=='-')
			{
				memset(option,0,512);

				j=0;
				i=1;
				while( argv[param][i] && argv[param][i]!=':')
				{
					option[j]=argv[param][i];
					i++;
					j++;
				}

				if( !strcmp(option,paramtosearch) )
				{
					if(argtoparam)
					{
						if(argv[param][i]==':')
						{
							i++;
							j=0;
							while( argv[param][i] )
							{
								argtoparam[j]=argv[param][i];
								i++;
								j++;
							}
							argtoparam[j]=0;
							return 1;
						}
						else
						{
							return -1;
						}
					}
					else
					{
						return 1;
					}
				}
			}
		}
		param++;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	char outputfilename[512];
	char pathfilter[512];

	CNT_TYPE numberofoccur;
	CNT_TYPE number_of_file;

	filedescription * filesystemtree;
	filedescription * tempfile;
	filedescription ** tabfilefound;
	filedescription ** tabfilepossibledup;

	filelisthead * possibleduplist;
	filelisthead * possibleduplist2;
	CNT_TYPE i;
	CNT_TYPE numberoffile;

	printf("dupechecker v1.1\n");
	printf("(c) 2008-2022 Jean-François DEL NERO\n");

	outputfilename[0] = 0;
	if( isOption( argc, argv,"o",(char*)&outputfilename) != 1 )
	{
		goto printsyntaxandexit;
	}

	pathfilter[0] = 0;
	isOption( argc, argv,"f",(char*)&pathfilter);

	if(argv[1] && strlen(outputfilename))
	{
		// Pass 1 : scan all file

		printf("PASS 1 : Get file names + size\n");

		filesystemtree=(filedescription*) malloc(sizeof(filedescription));
		if(!filesystemtree)
			goto fatalerror;

		memset(filesystemtree,0,sizeof(filedescription));

		filesystemtree->filename = alloc_and_print("Root");
		filesystemtree->filepath = alloc_and_print("Root");
		filesystemtree->status=FOLDER;

		number_of_file=0;
		i=1;
		while(argv[i])
		{
			if( argv[i][0] != '-' )
			{
				filesystemtree->number_of_entry++;
				filesystemtree->filedescription_array = (filedescription**) realloc(filesystemtree->filedescription_array,filesystemtree->number_of_entry*sizeof(filedescription*));
				if(!filesystemtree->filedescription_array)
					goto fatalerror;

				filesystemtree->filedescription_array[filesystemtree->number_of_entry-1] = (filedescription*) malloc(sizeof(filedescription));
				if(!filesystemtree->filedescription_array[filesystemtree->number_of_entry-1])
					goto fatalerror;

				memset(filesystemtree->filedescription_array[filesystemtree->number_of_entry-1],0,sizeof(filedescription));

				filesystemtree->filedescription_array[filesystemtree->number_of_entry-1]->filename = alloc_and_print(argv[i]);
				filesystemtree->filedescription_array[filesystemtree->number_of_entry-1]->filepath = alloc_and_print(argv[i]);
				filesystemtree->filedescription_array[filesystemtree->number_of_entry-1]->status = FOLDER;

				number_of_file += browse_and_make_fs_tree(filesystemtree->filedescription_array[filesystemtree->number_of_entry-1]);
				filesystemtree->size += filesystemtree->filedescription_array[filesystemtree->number_of_entry-1]->size;
			}
			i++;
		}

		printf("\n\n"PRT_CNT_TYPE" files found (total size: ",number_of_file);
		printsize(filesystemtree->size);
		printf(")\n\n");

////////////////////////////////////////////////////////////////
		printf("PASS 2 : calculate partial md5... :\n");

		numberoffile = 0;
		tabfilefound = 0;

		tabfilefound = getfilelist(filesystemtree,tabfilefound,&numberoffile);

		// Sort the list by file size.
		quicksort_file_size(tabfilefound,0,numberoffile-1);

		// Remove files with unique file size from the list
		tabfilefound = cleanup_filelist(tabfilefound,&numberoffile);

		printf("\n\n"PRT_CNT_TYPE" possibles dup\n\n",numberoffile);

		// Do fast MD5 on the files headers.
		for(i=0;i<numberoffile;i++)
		{
			printf("%.2f%c\r",((float)(i)/(float)(numberoffile-1))*(float)100,'%');
			compute_md5(tabfilefound[i],1);
		}

		quicksort_file_size_and_md5(tabfilefound,0,numberoffile-1);

		printf("\n\n"PRT_CNT_TYPE" partial md5 calculated)\n\n",numberoffile);

//////////////////////////////////////////////////////////////////////////

		printf("PASS 3 : get possible dup... :\n");

		// Remove files with unique MD5 from the list.
		tabfilefound = cleanup_filelist_md5(tabfilefound,&numberoffile);

		printf("\n\n"PRT_CNT_TYPE" possibles dup\n\n",numberoffile);

		printf("PASS 4 : Full MD5... :\n");

		for(i=0;i<numberoffile;i++)
		{
			printf("%.2f%c\r",((float)(i)/(float)(numberoffile-1))*(float)100,'%');
			compute_md5(tabfilefound[i],0);
		}

		quicksort_file_size_and_md5(tabfilefound,0,numberoffile-1);

		// Remove files with unique MD5 from the list.
		tabfilefound = cleanup_filelist_md5(tabfilefound,&numberoffile);

		printf("\n\n"PRT_CNT_TYPE" possible dup (total size: ",numberoffile);

		printsize(get_total_size(tabfilefound,numberoffile));

		printf(")\n\n");

//////////////////////////////////////////////////////////////////////////

		printf("PASS 4 : full md5 filter... :\n");

		possibleduplist=(filelisthead*)malloc(sizeof(filelisthead));
		if(possibleduplist)
		{
			memset(possibleduplist,0,sizeof(filelisthead));

			numberofoccur = 0;

			if(numberoffile && tabfilefound)
			{
				tempfile = tabfilefound[0];

				for(i=1;i<numberoffile;i++)
				{
					printf("%.2f%c\r",((float)(i)/(float)(numberoffile-1))*(float)100,'%');

					if(tempfile->size==tabfilefound[i]->size)
					{
						// add the file
						numberofoccur++;
						add_file_element(tempfile->filepath,tempfile->filename,tempfile->size,0,possibleduplist,tempfile->md5);
					}
					else
					{
						if(numberofoccur)
						{
							add_file_element(tempfile->filepath,tempfile->filename,tempfile->size,0,possibleduplist,tempfile->md5);
							// add the file
							numberofoccur=0;
						}
					}

					tempfile = tabfilefound[i];
				}

				if(numberofoccur)
				{
					add_file_element(tempfile->filepath,tempfile->filename,tempfile->size,0,possibleduplist,tempfile->md5);
					// add the file
					numberofoccur = 0;
				}

				//free_fileelement(tempfile);

				free(tabfilefound);
			}

			printf("\n\n"PRT_CNT_TYPE" full md5 calculated (total size: ",possibleduplist->numberoffile);
			printsize(possibleduplist->totalsize);
			printf(")\n\n");

		}

//////////////////////////////////////////////////////////////////////////

		printf("PASS 5 : get full  dup... :\n");

		tabfilepossibledup = tabification(possibleduplist);

		possibleduplist2 = (filelisthead*)malloc(sizeof(filelisthead));
		if(possibleduplist2 && tabfilepossibledup)
		{
			memset(possibleduplist2,0,sizeof(filelisthead));

			numberofoccur = 0;
			if(possibleduplist->numberoffile)
			{
				tempfile = tabfilepossibledup[0];
				for(i=1;i<possibleduplist->numberoffile;i++)
				{
					printf("%.2f%c\r",((float)(i)/(float)(possibleduplist->numberoffile-1))*(float)100,'%');

					if(!memcmp(tempfile->md5,tabfilepossibledup[i]->md5,16) && (tempfile->size == tabfilepossibledup[i]->size))
					{
						// add the file
						numberofoccur++;
						add_file_element(tempfile->filepath,tempfile->filename,tempfile->size,0,possibleduplist2,tempfile->md5);
					}
					else
					{
						if(numberofoccur)
						{
							add_file_element(tempfile->filepath,tempfile->filename,tempfile->size,0,possibleduplist2,tempfile->md5);
							// add the file
							numberofoccur=0;
						}
					}

				//	free_fileelement(tempfile);
					tempfile=tabfilepossibledup[i];

				}

				if(numberofoccur)
				{
					add_file_element(tempfile->filepath,tempfile->filename,tempfile->size,0,possibleduplist2,tempfile->md5);
					// add the file
					numberofoccur=0;

				}

				dealloc_filelisthead(possibleduplist);
			}


			printf("\n\n"PRT_CNT_TYPE" dup (total size: ",possibleduplist2->numberoffile);
			printsize(possibleduplist2->totalsize);
			printf(")\n\n");
		}

		if(tabfilepossibledup)
			free(tabfilepossibledup);

//////////////////////////////////////////////////////////////////////////

		printf("Writing output file %s\n",outputfilename);

		//write_deloutputfile("before_filter.txt",possibleduplist2);

		filterlist(possibleduplist2, (char*)&pathfilter);

		//write_outputfile(outputfilename,possibleduplist2);
		write_deloutputfile(outputfilename,possibleduplist2);

		dealloc_filelisthead(possibleduplist2);
		freefiletree(filesystemtree);
	}
	else
	{
		printf("Syntax: %s [source dir(s)] [-o:ouputfile]\n",argv[0]);
	}
	return 0;

fatalerror:
	printf("Memory allocation error !\n");
	return -1;

printsyntaxandexit:
	printf("Syntax: %s [source dir(s)] [-o:ouputfile]  [-f:path_filter]\n",argv[0]);
	return -1;
}

