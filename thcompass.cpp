#include "thcompass.h"
#include <QPainter>
#include <qdebug.h>
#include <math.h>

ThCompass::ThCompass()
{
	this->gradTh = 0;
	this->robotTh = 0;
}

QRectF ThCompass::boundingRect() const
{
	return QRectF(0,0,50,50);
}

void ThCompass::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
	Q_UNUSED(widget);
	Q_UNUSED(item);

	QRectF rect = boundingRect();

	painter->drawRect(0,0,5,5);
	painter->drawRect(45,45,5,5);
	painter->drawRect(0,45,5,5);
	painter->drawRect(45,0,5,5);
	painter->drawRect(25,25,5,5);
	//Robô
	painter->drawLine(25,25,25+25*cos(this->robotTh),50-(25+25*sin(this->robotTh)));
	//Gradiente
	painter->drawLine(25,25,25+25*cos(M_PI+this->gradTh),50-(25+25*sin(M_PI+this->gradTh)));

}
