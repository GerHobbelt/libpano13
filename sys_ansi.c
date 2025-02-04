/* Panorama_Tools	-	Generate, Edit and Convert Panoramic Images
   Copyright (C) 1998,1999 - Helmut Dersch  der@fh-furtwangen.de
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this software; see the file COPYING.  If not, a copy
   can be downloaded from http://www.gnu.org/licenses/gpl.html, or
   obtained by writing to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/*------------------------------------------------------------*/

#include "sys_ansi.h"
#include "panotypes.h"
#include <signal.h>

//------------------ Public functions required by filter.h -------------------------------


// Required by filter.h but not used in the sys_ansi build of panotools.
// just return and do nothing
void SetWindowOwner(void * Owner) {return;}
void CenterDialog(void * hDlg) {return;}


void filter_main( TrformStr *TrPtr, struct size_Prefs *spref)
{
	dispatch	( TrPtr, spref);
	
}

	

// Error reporting

void  PrintErrorIntern(char* fmt, va_list ap)
{
	char message[512];
	char *toPrint;

	if (strlen(fmt) < 512) {
 	  vsnprintf(message, sizeof(message)-1, fmt, ap);	  
	  toPrint = message;
	}   else {
	  // we don't have enough space, so just 
	  // print original string
	  toPrint = fmt;
	}

#ifdef HasJava
if( JavaUI ){
	  JPrintError( toPrint );
	}else
#endif
  {
	  printf("%s", toPrint);

	  // Add an end of line if none is provide
	  if (strlen(toPrint) > 0 &&
	      toPrint[strlen(toPrint)-1] != '\n') {
	      putchar('\n');
	  }
	  fflush(stdout);
	}
}


// Progress report; return false if canceled


int ProgressIntern( int command, char* argument )
{
	long percent;	

	switch( command ){
		case _initProgress:
			printf( "\n%s          ", argument );
			return TRUE;
		
		case _setProgress:
			sscanf(argument,"%ld", &percent);
			printf("\b\b\b\b%3ld%%", (long) percent);
			fflush (stdout);
			return TRUE;
			
		case _disposeProgress:
			printf("\n");
			return TRUE;
		case _idleProgress:
			return TRUE;
	}
	return TRUE;
}

volatile sig_atomic_t sigFlag;

void sigHandler(int sig PT_UNUSED){
	signal( SIGINT, sigHandler );
	sigFlag = 1;
}

int infoDlgIntern ( int command, char* argument ){
	static char	mainMessage[256];	
	// int reply;
	
	*mainMessage = 0;

	switch( command ){
		case _initProgress:
			signal( SIGINT, sigHandler );
			sigFlag = 0;
			printf( "%s\n", argument );
			return TRUE;
		case _setProgress:
			if( *argument != 0 ){
				if( *argument != '+' ){
					strcpy( mainMessage, argument );
					printf( "%s\n", argument );
				}else{
					printf( "%s%s", mainMessage, &(argument[1]) );
				}
				fflush (stdout);
			}
			//printf("\nContinue (c) or stop (s) ?\n");
			//reply = getchar();
			//if( reply == 's' )
			//	return FALSE;
			//return TRUE;
			if( sigFlag )
				return FALSE;
			return TRUE;
		case _disposeProgress:
			printf("\n");
			return TRUE;
		case _idleProgress:
			return TRUE;
	}
	return TRUE;
}


int readPrefs( char* pref, int selector )
{

	struct {
		char						v[sizeof(PREF_VERSION)];
		struct correct_Prefs		c;
		struct remap_Prefs			r;
		struct perspective_Prefs	p;
		struct adjust_Prefs			a;
		struct size_Prefs			s;
		panControls					pc;
	} prf;
	char* prefname = "pano13.prf";
	long size;

	FILE 	*prfile;
	int result = 0;


	if( (prfile = fopen( prefname, "rb" )) != NULL ){
		size = fread( &prf, 1, sizeof(prf),  prfile);
		fclose( prfile );
		
		if( size != sizeof(prf) ){
			result = -1;
		}else{
			switch( selector){
				case _version:
					memcpy( pref, &prf.v, sizeof( PREF_VERSION ) );
					break;
				case _correct:
					if( prf.c.magic != 20 ) 
						result = -1;
					else
						memcpy( pref, &prf.c, sizeof(struct correct_Prefs)); 
					break;
				case _remap:
					if( prf.r.magic != 30 ) 
						result = -1;
					else
						memcpy( pref, &prf.r , sizeof(struct remap_Prefs)); 
					break;
				case _perspective:
					if( prf.p.magic != 40 ) 
						result = -1;
					else
						memcpy( pref, &prf.p , sizeof(struct perspective_Prefs)); 
					break;
				case _adjust:
					if( prf.a.magic != 50 ) 
						result = -1;
					else
						memcpy( pref, &prf.a , sizeof(struct adjust_Prefs)); 
					break;
				case _sizep:
					if( prf.s.magic != 70 ) 
						result = -1;
					else
						memcpy( pref, &prf.s , sizeof(struct size_Prefs)); 
					break;
				case _panright:
				case _panleft:
				case _panup:
				case _pandown:
				case _zoomin:
				case _zoomout:
				case _apply:
				case _getPano:
				case _increment:
					memcpy( pref, &prf.pc , sizeof(panControls)); 
					break;
			}// switch
		} // sizes match
	}
	else
		result = -1;

	return result;
}





void writePrefs( char* prefs, int selector ){

	struct {
		char						v[sizeof(PREF_VERSION)];
		struct correct_Prefs		c;
		struct remap_Prefs			r;
		struct perspective_Prefs	p;
		struct adjust_Prefs			a;
		struct size_Prefs			s;
		panControls					pc;
	} prf;

	FILE 	*prfile;
	char* prefname = "pano13.prf";



	if( (prfile = fopen( prefname, "rb" )) != NULL ){
            int i;
            i = fread( &prf, sizeof(prf), 1 , prfile);
            if (i != sizeof(prf)) {
                PrintError("Unable to write to preference file [%s]\n", prefname);
            }
            fclose( prfile );
	}

	switch( selector){
		case _version:
			memcpy( &prf.v,  prefs, sizeof( PREF_VERSION ) );
			break;
		case _correct:
			memcpy( &prf.c , prefs, sizeof(struct correct_Prefs)); 
			break;
		case _remap:
			memcpy( &prf.r , prefs, sizeof(struct remap_Prefs)); 
			break;
		case _perspective:
			memcpy( &prf.p , prefs, sizeof(struct perspective_Prefs)); 
			break;
		case _adjust:
			memcpy( &prf.a , prefs, sizeof(struct adjust_Prefs)); 
			break;
		case _sizep:
			memcpy( &prf.s , prefs, sizeof(struct size_Prefs)); 
			break;
		case _panright:
		case _panleft:
		case _panup:
		case _pandown:
		case _zoomin:
		case _zoomout:
		case _apply:
		case _getPano:
		case _increment:
			memcpy( &prf.pc , prefs, sizeof(panControls)); 
			break;
	}
	
	if( (prfile = fopen( prefname, "wb" )) != NULL ){
		fwrite( &prf, sizeof(prf), 1 , prfile);
		fclose(prfile);
	}
}


#define signatureSize	4

void**  mymalloc( size_t numBytes )					// Memory allocation, use Handles
{
	char **mem;
	
	mem = (char**)malloc( sizeof(char*) );			// Allocate memory for pointer
	if(mem == NULL)
		return (void**)NULL;
	else
	{
		(*mem) = (char*) malloc( numBytes );		// Allocate numBytes
		if( *mem == NULL )
		{
			free( mem );
			return (void**)NULL;
		}
		else
			return (void**)mem;
	}
}

void 	myfree( void** Hdl )						// free Memory, use Handles
{
	free( (char*) *Hdl );
	free( (char**) Hdl );
}		

int 	FindFile( fullPath *fname ){
	printf("\n");
	printf("Load File:\n");
	if (scanf("%s", fname->name) != 1) {
            return -1;
        }
	
	if(strlen(fname->name) > 0)
		return 0;
	else
		return -1;
}

int 	SaveFileAs			( fullPath *fname, char *prompt PT_UNUSED, char *name PT_UNUSED){
	printf("\n");
	printf("Save File As:\n");
	if (scanf("%s", fname->name) != 1) {
            return -1;
        }
	
	if(strlen(fname->name) > 0)
		return 0;
	else
		return -1;
}


void makePathForResult	( fullPath *path ){
	strcpy( path->name, "//ptool_result" );
}

int makePathToHost ( fullPath *path ){
	strcpy(path->name, "./");
	return 0;
}




// Fname is appended to host-directory path

void MakeTempName( fullPath *destPath, char *fname ){
	snprintf(destPath->name, sizeof(destPath->name)-1, "pano13.%s", fname);
}

void ConvFileName( fullPath *fspec,char *string){
	strcpy( string, fspec->name );
}

int FullPathtoString (fullPath *path, char *filename){
	if( strlen( path->name ) < 256 )
	{
		strcpy( filename, path->name );
		return 0;
	}
	else
	{
		return -1;
	}
}

int IsTextFile( char* fname )
{

	if( strrchr( fname, '.' ) != NULL && 
			(strcmp( strrchr( fname, '.' ), ".txt") == 0 ||
			 strcmp( strrchr( fname, '.' ), ".TXT") == 0)	)
	return TRUE;
	

	return FALSE;
}

int StringtoFullPath(fullPath *path, char *filename){
	if(strlen( filename ) < 256 ){
		strcpy( path->name, filename);
		return 0;
	}else{
		return -1;
	}
}
