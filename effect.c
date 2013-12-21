#include "effect.H"


void effect_set_color(BITMAP *img, int col, int intensity)
{
    int w, h;
    int pixel;
    int r, g, b;

    for (w = 0; w < img->w; w++)
    {
        for (h = 0; h < img->h; h++)
        {
            pixel = ((long *)img->line[h])[w];

            r = getr32(pixel);
            g = getg32(pixel);
            b = getb32(pixel);

            if (r == 255 && b == 255 && g == 0)
                continue;

            switch (col)
            {
                case RED    : g -= intensity, b -= intensity ; break;
                case GREEN  : r -= intensity, b -= intensity ; break;
                case BLUE   : r -= intensity, g -= intensity ; break;
                case BLACK_AND_WHITE :
                    pixel = (int)(r + g + b) / 3;
                    r = g = b = pixel;
                break;
            }

            r = (r < 0) ? 0 : r;
            g = (g < 0) ? 0 : g;
            b = (b < 0) ? 0 : b;

            ((long *)img->line[h])[w] = makecol32(r, g, b);
        }
    }
}

void draw_laser(BITMAP *double_buffer, Entity *e, int camera_x, int camera_y, int dir)
{
    int col = makecol(0, 150, 0);

    switch (dir)
    {
        case DIRECTION_BAS:
            line (
                double_buffer,
                e->display_x + camera_x + (TILE_SIZE / 2),
                e->display_y + camera_y + TILE_SIZE,
                e->display_x + camera_x + (TILE_SIZE / 2),
                e->display_y + 5 * TILE_SIZE + camera_y,
                col
            );
        break;

        case DIRECTION_HAUT:
            line (
                double_buffer,
                e->display_x + camera_x + (TILE_SIZE / 2),
                e->display_y + camera_y,
                e->display_x + camera_x + (TILE_SIZE / 2),
                e->display_y - (4 * TILE_SIZE) + camera_y,
                col
            );
        break;

        case DIRECTION_GAUCHE:
            line (
                double_buffer,
                e->display_x + camera_x,
                e->display_y + camera_y + (TILE_SIZE / 2),
                e->display_x + camera_x - (4 * TILE_SIZE),
                e->display_y + camera_y + (TILE_SIZE / 2),
                col
            );
        break;

        case DIRECTION_DROITE:
            line (
                double_buffer,
                e->display_x + camera_x + TILE_SIZE,
                e->display_y + camera_y + (TILE_SIZE / 2),
                e->display_x + camera_x + (5 * TILE_SIZE),
                e->display_y + camera_y + (TILE_SIZE / 2),
                col
            );
        break;
    }
}
