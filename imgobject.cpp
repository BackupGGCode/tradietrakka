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

#include "imgobject.h"
#include <QFontMetrics>

ImgObject::ImgObject(const QPixmap &pixmap) :
        QGraphicsObject()
{
    image = new QPixmap(pixmap);

    //setAcceptedMouseButtons(Qt::LeftButton);
}

ImgObject::ImgObject(const char *imgFile) :
        QGraphicsObject()
{
    image = new QPixmap(imgFile);
}

void ImgObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(QPointF(), *image);
}

QRectF ImgObject::boundingRect() const
{
    return QRectF( QPointF(0, 0), image->size());
}

void ImgObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked();
    event->accept();
}


ChainPanel::ChainPanel(const char *imgFile, QString label) :
        ImgObject(imgFile)
{
    this->label = new QGraphicsTextItem(this);
    this->setText(label);

}

ChainPanel::ChainPanel(const QPixmap &pixmap, QString label) :
        ImgObject(pixmap)
{
    this->label = new QGraphicsTextItem(this);
    this->setText(label);
}

void ChainPanel::setText(QString label)
{

    this->label->setFont(QFont("Serif", 12, -5, true));
    QFontMetrics labelWidth(this->label->font());
    int x = (image->width() - labelWidth.width(label)) / 2;
    this->label->setPos(x - 11, 37);
    this->label->setHtml("<b>" + label + "</b>");
}

DrawerFront::DrawerFront(const char *imgFile, QString label) :
        ImgObject(imgFile)
{
    this->label = new QGraphicsTextItem(this);
    this->setText(label);

}

DrawerFront::DrawerFront(const QPixmap &pixmap, QString label) :
        ImgObject(pixmap)
{
    this->label = new QGraphicsTextItem(this);
    this->setText(label);
}

void DrawerFront::setText(QString label)
{
    this->label->setFont(QFont("Serif", 15, -5, true));
    QFontMetrics labelWidth(this->label->font());
    int x = (image->width() - labelWidth.width(label)) / 2;
    this->label->setPos(x - 10, 23);
    this->label->setHtml("<b>" + label + "</b>");
}
