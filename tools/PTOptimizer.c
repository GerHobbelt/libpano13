/*
 *  Clone of PTOptimizer
 *
 *  Based on code found in Helmut Dersch's panorama-tools
 *
 *  Dec 2003
 *
 *  Bruno Postle <bruno at postle.net>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

// gcc -oPTOptimizer -I/usr/include/pano12 -lpano12 PTOptimizer.c


#include "filter.h"
#include <stdio.h>

static  AlignInfo	*g;

int main(int argc,char *argv[])
{
	aPrefs		aP;

	char*		script;
	OptInfo		opt;
	AlignInfo	ainf;

	fullPath	infile;
	fullPath	outfile;

	SetAdjustDefaults(&aP);

	if(argc != 2)
	{
		printf("Usage: %s /path/to/script.txt\n", argv[0]);
		exit(1);
	}

	StringtoFullPath(&infile, argv[1]);

	script = LoadScript( &infile );
	if( script != NULL )
	{
		if (ParseScript( script, &ainf ) == 0)
		{
			if( CheckParams( &ainf ) == 0 )
			{
				ainf.fcn	= fcnPano;
				
				SetGlobalPtr( &ainf ); 
				
				opt.numVars 		= ainf.numParam;
				opt.numData 		= ainf.numPts;
				opt.SetVarsToX		= SetLMParams;
				opt.SetXToVars		= SetAlignParams;
				opt.fcn			= ainf.fcn;
				*opt.message		= 0;

				RunLMOptimizer( &opt );
				ainf.data		= opt.message;
				WriteResults( script, &infile, &ainf, distSquared, 0);
        			exit(0);
			}
			DisposeAlignInfo( &ainf );
		}
		free( script );
	}
	exit(1);
}

