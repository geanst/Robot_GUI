#ifndef ROBOTCURSOR_H
#define ROBOTCURSOR_H
#include <QObject>
#include <QGraphicsItem>
//#include <QtWidgets>
#include "grid.h"

class RobotCursor: public QObject, public QGraphicsItem
{
		Q_OBJECT
public:
	RobotCursor(Grid *grid, int robotSize);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    Grid * matriz;

	int robotSize;
	int gridSize;
	int cellSize;
	int scale;
	int xCursor;
	int yCursor;
    void keyPressEvent(QKeyEvent *event) override;
	void updatePos(int x, int y);

	signals:
	void accelerate();
	void deaccelerate();
	void left();
	void right();



};


#endif // ROBOTCURSOR_H
