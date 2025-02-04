/* THIS CODE CARRIES NO GUARANTEE OF USABILITY OR FITNESS FOR ANY PURPOSE.
 * WHILE THE AUTHORS HAVE TRIED TO ENSURE THE PROGRAM WORKS CORRECTLY,
 * IT IS STRICTLY USE AT YOUR OWN RISK.  */

/* Copyright (c) 1995 Bruce Walter <bjw@graphics.cornell.edu>
   The license of these files has been clarified by Bruce Walter during
   a private email conversation.  These files are public domain. */

/* utility for reading and writing Ward's rgbe image format.
   See rgbe.txt file for more details.
*/

#ifndef _H_RGBE
#define _H_RGBE

#include <stdio.h>
#include "panotypes.h"


typedef struct {
  int valid;            /* indicate which fields are valid */
  char programtype[16]; /* listed at beginning of file to identify it 
                         * after "#?".  defaults to "RGBE" */ 
  float gamma;          /* image has already been gamma corrected with 
                         * given gamma.  defaults to 1.0 (no correction) */
  float exposure;       /* a value of 1.0 in an image corresponds to
			 * <exposure> watts/steradian/m^2. 
			 * defaults to 1.0 */
} rgbe_header_info;

/* flags indicating which fields in an rgbe_header_info are valid */
#define RGBE_VALID_PROGRAMTYPE 0x01
#define RGBE_VALID_GAMMA       0x02
#define RGBE_VALID_EXPOSURE    0x04

/* return codes for rgbe routines */
#define RGBE_RETURN_SUCCESS 0
#define RGBE_RETURN_FAILURE -1

#ifdef __cplusplus
extern "C" {
#endif
/* read or write headers */
/* you may set rgbe_header_info to null if you want to */

int RGBE_WriteHeader(FILE *fp, uint32_t width, uint32_t height, rgbe_header_info *info);
int RGBE_ReadHeader(FILE *fp, uint32_t* width, uint32_t* height, rgbe_header_info *info);

/* read or write pixels */
/* can read or write pixels in chunks of any size including single pixels*/
int RGBE_WritePixels(FILE *fp, float *data, int numpixels);
int RGBE_ReadPixels(FILE *fp, float *data, int numpixels);

/* read or write run length encoded files */
/* must be called to read or write whole scanlines */
int RGBE_WritePixels_RLE(FILE *fp, float *data, int scanline_width,
			 int num_scanlines);
int RGBE_ReadPixels_RLE(FILE *fp, float *data, int scanline_width,
			int num_scanlines);

int RGBE_ReadPixels_Raw_RLE(FILE *fp, unsigned char *data, int scanline_width,
            size_t num_scanlines);

#ifdef __cplusplus
}
#endif

#endif /* _H_RGBE */



