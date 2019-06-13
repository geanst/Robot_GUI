#include "cell.h"
#include <QPainter>
#include "qdebug.h"
#include "math.h"
#include <sstream>
#include<bits/stdc++.h>
using namespace std;


Cell::Cell(int x, int y, int cellSize, int scale, bool mapping)
{
	this->visited = false;
	this->updated= false;
	this->mapping = mapping;
	if(mapping)
		this->value = 1;
	else {
		this->value = 0.5;
		this->value2 = 0.5;
	}
    this->x = x;
    this->y = y;
	this->scale = scale;
	this->cellSize = cellSize;
	this->status = -1;
	this->potential = 0;
	this->gradTh = 0;
	this->color = Qt::gray;
	setZValue(0);
}

QRectF Cell::boundingRect() const
{
	return QRectF(x-cellSize/2, y-cellSize/2, cellSize, cellSize);
}

void Cell::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(item);

    QRectF rect = boundingRect();

	if(mapping)
	{
		if(updated)
		{
			color = visited==true ? Qt::green:QColor::fromHsv(0,0,255-255*(value/15));
		}
		else
			color = Qt::gray;
	}
	else
	{
		if(updated)
		{
			color = visited==true ? Qt::green:QColor::fromHsv(180,255*potential,255-255*value);
		}
		else
			color = Qt::gray;
	}

    painter->fillRect(rect, color);
    if(updated && this->x%2==0 && this->y%2==0 && this->status == 0)
	{
        float gradXPoint = (((gradTh*180)/M_PI)-8)*16;
		float gradYPoint = 16*16;

		painter->drawPie(rect,gradXPoint,gradYPoint);
	}
}

void Cell::cellUpdate()
{
	update();
}

void Cell::whichRegion(int err, int range, float rPolar, float aPolar, float thRobot)
{
	range = range == 0 ? 0.0001:range;
	if(rPolar > (range-err)/scale && range < 300)
	{
		this->updated = true;
		bayes1(rPolar, aPolar, thRobot, range/scale);
	}
	else if(rPolar < 300/scale)
	{
		this->updated = true;
		rPolar = rPolar > (range/scale) ? range:rPolar;
		bayes2(rPolar, aPolar, thRobot, range/scale);
	}


}

void Cell::addValue()
{
	this->updated = true;
	if(value<12)
		value += 3;
	else
		value = 15;
	update();
}

void Cell::subValue()
{
	this->updated = true;
	if(value>0)
		value -= 1;
	else
		value = 0;
	update();
}


void Cell::bayes1(float rPolar, float aPolar, float thRobot, int range)
{
	int R = 110;
	float betha = 15;

    float alpha = fabs(aPolar - thRobot);
    alpha = alpha > 180 ? (360-alpha):alpha;
    //float alpha = acos(cos(aPolar)*cos(thRobot) + sin(aPolar)*sin(thRobot)) *180/M_PI;
	alpha = alpha>betha ? betha:alpha;

	//float value_temp = (1+(((R-rPolar)/R + (betha-alpha)/betha)/2)*0.95)/2;

	//float value_temp = (1+(1-pow((rPolar-range)/1.4,2))*(1-(pow(alpha/betha,2))))/2;
	float value_temp = (1+(1-pow((rPolar/range)/1.4,2))*(exp(-pow(alpha/betha,2)/pow((2*sqrt(0.01)),2))))/2;
    float value2_temp = 1 - value_temp;

	float prevvalue =  value;

	this->value = (value_temp*this->value)/((value_temp*this->value) + (value2_temp*this->value2));
    this->value = this->value > 0.98 ? 0.98:this->value;
	this->value2 = 1 - this->value;

    if(value > 1 || value < 0 || isnan(value))
        qDebug() << value << value_temp << prevvalue << rPolar<< range;
	update();
}

void Cell::bayes2(float rPolar, float aPolar, float thRobot, int range)
{
	int R = 110;
	float betha = 15;

    float alpha = fabs(aPolar - thRobot);
    alpha = alpha > 180 ? (360-alpha):alpha;
    //float alpha = 180 - fabs(fabs(aPolar - thRobot) - 180);
    //float alpha = acos(cos(aPolar)*cos(thRobot) + sin(aPolar)*sin(thRobot)) *180/M_PI;
	alpha = alpha>betha ? betha:alpha;

	//float value2_temp = (1+(((R-rPolar)/R + (betha-alpha/2)/betha)/2)*0.95)/2;
	//float value2_temp = (1+(1-pow(rPolar/range,2))*(1-pow(alpha/betha,2)))/2;
	float value2_temp = (1+(1-pow((rPolar/range)/2,2))*(0.5*exp(-pow(alpha/betha,2)/pow((2*sqrt(0.01)),2))))/2;
	float value_temp = 1 - value2_temp;

	this->value2 = (value2_temp*this->value2)/((value2_temp*this->value2) + (value_temp*this->value));
    this->value2 = this->value2 > 0.98 ? 0.98:this->value2;
	this->value = 1 - this->value2;

	if(value > 1 || value < 0 || isnan(value))
		qDebug() << value << value_temp  << rPolar<< range;

	update();

}

