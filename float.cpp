// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "float.h"

#include "rectangle.h"

// applied patch by Dr. J. Pfefferl
void Float::resize(Rectangle *rect, Direction dir, int stepW, int stepH)
{
    int newW, newH;

    switch (dir) {
    case NORTH_WEST:
    case SOUTH_WEST:
    case LEFT:
        newW = rect->width() - stepW;
        if (0 < newW) {
          rect->setX(rect->x() + stepW);
          rect->setWidth(newW);
        }
        break;
    case NORTH_EAST:
    case SOUTH_EAST:
    case RIGHT:
        newW = rect->width() + stepW;
        if (0 < newW) {
          rect->setWidth(newW);
        }
        break;
    default:
        break;
    }

    switch (dir) {
    case NORTH_WEST:
    case NORTH_EAST:
    case UP:
        newH = rect->height() - stepH;
        if (0 < newH) {
          rect->setY(rect->y() + stepH);
          rect->setHeight(newH);
        }
        break;
    case SOUTH_WEST:
    case SOUTH_EAST:
    case DOWN:
        newH = rect->height() + stepH;
        if (0 < newH) {
          rect->setHeight(newH);
        }
        break;
    default:
        break;
    }
}

void Float::move(Rectangle *rect, int stepW, int stepH) {
    rect->setX(rect->x() + stepW);
    rect->setY(rect->y() + stepH);
}

void Float::center(Rectangle *rect, Rectangle *parentRect) {

    unsigned int clientCenterX = (rect->x() + rect->width()) / 2;
    unsigned int clientCenterY = (rect->y() + rect->height()) / 2;
    unsigned int parentCenterX = (parentRect->x() + parentRect->width()) / 2;
    unsigned int parentCenterY = (parentRect->y() + parentRect->height()) / 2;
    int dx = parentCenterX - clientCenterX;
    int dy = parentCenterY - clientCenterY;
    rect->setX(rect->x() + dx);
    rect->setY(rect->y() + dy);
}
