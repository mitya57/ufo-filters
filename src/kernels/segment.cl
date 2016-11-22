/*
 * Copyright (C) 2016 Karlsruhe Institute of Technology
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

typedef struct {
    int x;
    int y;
} Label;

#define C_WEST  0
#define C_NORTH 1
#define C_EAST  2
#define C_SOUTH 3
#define C_UP    4
#define C_DOWN  5

kernel void
segment (global float *slices,
         global float *labeled,
         global Label *prelabeled,
         const int width,
         const int height,
         const int num_slices,
         global float *random)
{
    size_t idx;
    int x, y, z;
    int offset;
    int i, j;
    float current;
    float c[6]; /* west, north, east, south, up, down */
    float d[6];
    float sample, sum, mean;
    float sigma_sq;

    idx = get_global_id (0);
    x = prelabeled[idx].x;
    y = prelabeled[idx].y;
    z = 0;
    offset = width * height;
    current = slices[y * width + x + z * offset];

#ifdef DEVICE_GEFORCE_GTX_TITAN_BLACK
#pragma unroll 2
#endif
    for (int depth = 0; depth < num_slices * 5000; depth++) {
        /* FIXME: race condition */
        labeled[y * width + x + z * offset] += 1.0f;

        c[C_WEST]  = slices[y * width + x - 1 + z * offset];
        c[C_EAST]  = slices[y * width + x + 1 + z * offset];
        c[C_NORTH] = slices[(y - 1) * width + x + z * offset];
        c[C_SOUTH] = slices[(y + 1) * width + x + z * offset];

        if (z > 0)
            c[C_DOWN] = slices[y * width + x + (z - 1) * offset];
        else
            c[C_DOWN] = 0.0f;

        if (z < num_slices - 1)
            c[C_UP] = slices[y * width + x + (z + 1) * offset];
        else
            c[C_UP] = 0.0f;

        /* compute mean of neighbourhood */
        sum = 0.0f;

        for (i = 0; i < 6; i++)
            sum += c[i];

        mean = sum / 6;

        /* compute std deviation */
        sum = 0.0f;

        for (i = 0; i < 6; i++)
            sum += (c[i] - mean) * (c[i] - mean);

        sigma_sq = sqrt (sum / 5.0f);

        /* compute probabilities */
        for (i = 0; i < 6; i++) {
            const float v = (c[i] - current);
            c[i] = exp (- v * v / (2 * sigma_sq)) / (sqrt (2 * M_PI_F * sigma_sq));
        }

        /* normalize and initialize d array */
        sum = 0.0f;

        for (i = 0; i < 6; i++)
            sum += c[i];

        for (i = 0; i < 6; i++) {
            c[i] /= sum;
            d[i] = c[i];
        }

        /* insertion sort to get a cumulative distribution */
        for (i = 1; i < 6; i++) {
            float v = d[i];
            j = i;

            while (j > 0 && d[j - 1] < v) {
                d[j] = d[j - 1];
                j--;
            }

            d[j] = v;
        }

        /* draw sample according to our distribution using the inversion method */
        sample = random[(idx + depth) % 32768];
        sum = 0.0f;
        i = 0;

        for (; i < 6; i++) {
            sum += d[i];

            if (sum >= sample)
                break;
        }

        /* find original position */
        for (j = 0; j < 6 && c[j] != d[i]; j++)
            ;

        switch (j) {
            case C_WEST:
                x = max (x - 1, 0);
                break;
            case C_NORTH:
                y = max (y - 1, 0);
                break;
            case C_EAST:
                x = min (x + 1, width - 1);
                break;
            case C_SOUTH:
                y = min (y + 1, height - 1);
                break;
            case C_UP:
                z = min (z + 1, num_slices - 1);
                break;
            case C_DOWN:
                z = max (z - 1, 0);
                break;
        }
    }
}
