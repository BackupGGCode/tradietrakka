/*******************************************************************************

  TradieTrakka - Easy to use small business management software
  Copyright (C) 2010 Steven Tucker

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

#ifndef IMGOBJECT_H
#define IMGOBJECT_H

#include <QtGui>

class ImgObject : public QGraphicsObject
{
    Q_OBJECT

protected:
    QPixmap *image;

public:
    ImgObject(const char *imgFile);
    ImgObject(const QPixmap &pixmap);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
    void clicked();
};


class ChainPanel : public ImgObject
{
    QGraphicsTextItem *label;

    void setText(QString label);

public:
    ChainPanel(const char *imgFile, QString label);
    ChainPanel(const QPixmap &pixmap, QString label);
};


class DrawerFront : public ImgObject
{
    QGraphicsTextItem *label;

    void setText(QString label);

public:
    DrawerFront(const char *imgFile, QString label);
    DrawerFront(const QPixmap &pixmap, QString label);
};

#endif // IMGOBJECT_H
