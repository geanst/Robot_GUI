#include <QKeyEvent>
//#include <QtWidgets>
#include <QPainter>
#include "robotcursor.h"
#include "grid.h"
#include "qdebug.h"

RobotCursor::RobotCursor(Grid *grid, int robotSize)
{
    this->matriz = grid;
	this->robotSize = robotSize;
	this->gridSize = grid->size;
	this->cellSize = grid->cell_size;
	this->scale = grid->scale;
    setZValue(10);
}

QRectF RobotCursor::boundingRect() const
{
	return QRectF(-robotSize/2,-robotSize/2, robotSize, robotSize);
}

void RobotCursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(item);

    QRectF rect = boundingRect();

	//painter->drawRect(rect);
	painter->fillRect(rect, Qt::blue);
}

void RobotCursor::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Up)
		emit accelerate();
	else if(event->key() == Qt::Key_Down)
		emit deaccelerate();
	else if(event->key() == Qt::Key_Left)
		emit left();
	else if(event->key() == Qt::Key_Right)
		emit right();

}

void RobotCursor::updatePos(int x, int y)
{
	setPos(x,-y);
	matriz->matriz[x/scale+gridSize/2][-y/scale+gridSize/2]->visited = true;
	matriz->matriz[x/scale+gridSize/2][-y/scale+gridSize/2]->cellUpdate();
}


