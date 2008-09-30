/* exif-mnote-data-priv.h
 *
 * Copyright © 2003 Lutz Mьller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __EXIF_MNOTE_DATA_PRIV_H__
#define __EXIF_MNOTE_DATA_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "exif-mnote-data.h"
#include "exif-byte-order.h"
#include "exif-log.h"

typedef struct _ExifMnoteDataMethods ExifMnoteDataMethods;
struct _ExifMnoteDataMethods {

	/* Live cycle */
	void (* free) (ExifMnoteData *);

	/* Modification */
	void (* save) (ExifMnoteData *, unsigned char **, unsigned int *);
	void (* load) (ExifMnoteData *, const unsigned char *, unsigned int);
	void (* set_offset)     (ExifMnoteData *, unsigned int);
	void (* set_byte_order) (ExifMnoteData *, ExifByteOrder);

	/* Query */
	unsigned int (* count)           (ExifMnoteData *);
        unsigned int (* get_id)          (ExifMnoteData *, unsigned int);
	const char * (* get_name)        (ExifMnoteData *, unsigned int);
	const char * (* get_title)       (ExifMnoteData *, unsigned int);
	const char * (* get_description) (ExifMnoteData *, unsigned int);
	char * (* get_value)             (ExifMnoteData *, unsigned int, char *val, unsigned int maxlen);
};

typedef struct _ExifMnoteDataPriv ExifMnoteDataPriv;

struct _ExifMnoteData
{
	ExifMnoteDataPriv *priv;

	ExifMnoteDataMethods methods;

	/* Logging */
	ExifLog *log;

	/* Memory management */
	ExifMem *mem;
};

void exif_mnote_data_construct      (ExifMnoteData *, ExifMem *mem);
void exif_mnote_data_set_byte_order (ExifMnoteData *, ExifByteOrder);
void exif_mnote_data_set_offset     (ExifMnoteData *, unsigned int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_MNOTE_PRIV_H__ */
