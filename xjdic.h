/**************************************************************************
*                     X J D X G E N
*                                                   Author: Jim Breen
*           Index (.xjdx) generator program fron XJDIC            
*
*		V2.3 - indexes JIS X 0212 (3-byte EUC) kanji
***************************************************************************/
/*  This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.     */

  
#define MAXLINES 24     /* default lines per display */
#define MAXCOLS 80      /* default cols per display */
#define NRKANA 250*2   /*  size of the romaji to kana tables*/
#define NOFILT 10      /* maximum number of general filters */
#define VMAX 350       /* maximum entries in the verb table */
#define DEFAULT_DICDIR ""    /* could be: "/usr/local/lib/xjdic"   */
#define NOHITS 1000       /* size of hittab  */
#define SPTAG '@'         /* character that flags priority entries */
#define KFBUFFSIZE 2000   /* size of the KFlush buffer */
#define KANJARRAYSIZE 600 /* size of the kanji sorting array */
#define SVER "2.3"
#define TINITVAL 2
#define TMAXVAL 30
#define TMAXREP 1
#define VBUFFSIZE 4096  /* size in bytes for each virtual buffer  */
#define NOVB 1000  /* maximum number of virtual buffers   */
#define RADPERLINE 20		/* No of radicals to display per line	*/
#define RADLOOKLIM 2*20		/*threshold for displaying results of radical
				  lookup (default 20)	*/
#define MAXITER 40		/*maximum binary search iterations*/

/* The following is for Solarises   */

#ifndef SEEK_END
#define SEEK_END 2
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef INADDR_NONE
#define INADDR_NONE -1
#endif


/*     type fields for PDUs and server calls               */

#define XJ_FIND         1	/* find entry				*/
#define XJ_ENTRY        2	/* get this entry according to index	*/
#define XJ_OK           3	/* find/entry_get succeeded		*/
#define XJ_NBG          4	/* find/entry_get failed		*/
#define XJ_PROTERR      5	/* protocol error - server only		*/
#define XJ_HULLO        6	/* just send back an XJ_OK and # dics   */
#define XJ_GET          7	/* get this entry, wo checking any match*/

#define XJ_PORTNO       47512	/* server's "well-known" port no	*/

/*	PDU structures		*/

typedef struct {
	int32_t	xjdreq_checksum;
	short		xjdreq_type;
	short		xjdreq_seq;
	short		xjdreq_dicno;
	int32_t	xjdreq_indexpos;
	short		xjdreq_schlen;
	unsigned char	xjdreq_schstr[21];
	} REQ_PDU;

typedef struct {
	int32_t	xjdrsp_checksum;
	short		xjdrsp_type;
	short		xjdrsp_seq;
	int32_t	xjdrsp_resindex;
	short		xjdrsp_hitposn;
	short		xjdrsp_reslen;
	int32_t	xjdrsp_dicloc;
	unsigned char	xjdrsp_resstr[512];
	} RSP_PDU;
