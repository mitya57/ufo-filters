/*
 * Copyright (C) 2011-2015 Karlsruhe Institute of Technology
 *
 * This file is part of Ufo.
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ufo-reader.h"

typedef UfoReaderIface UfoReaderInterface;

G_DEFINE_INTERFACE (UfoReader, ufo_reader, 0)


void
ufo_reader_open (UfoReader *reader,
                 const gchar *filename)
{
    UFO_READER_GET_IFACE (reader)->open (reader, filename);
}

void
ufo_reader_close (UfoReader *reader)
{
    UFO_READER_GET_IFACE (reader)->close (reader);
}

gboolean
ufo_reader_data_available (UfoReader *reader)
{
    return UFO_READER_GET_IFACE (reader)->data_available (reader);
}

void
ufo_reader_get_meta (UfoReader *reader,
                     gsize *width,
                     gsize *height,
                     UfoBufferDepth *bitdepth)
{
    UFO_READER_GET_IFACE (reader)->get_meta (reader, width, height, bitdepth);
}

void
ufo_reader_read (UfoReader *reader,
                 UfoBuffer *buffer,
                 UfoRequisition *requisition,
                 guint roi_y,
                 guint roi_height,
                 guint roi_step)
{
    UFO_READER_GET_IFACE (reader)->read (reader, buffer, requisition, roi_y, roi_height, roi_step);
}

static void
ufo_reader_default_init (UfoReaderInterface *iface)
{
}