#ifndef CELL_H
#define CELL_H

#include <QColor>
#include <QGraphicsItem>
#include <math.h>

class Cell: public QGraphicsItem
{
public:
	Cell(int x, int y, int cellSiz, int scale, bool mapping);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
    void cellUpdate();

    int x;
    int y;
	int scale;
	float value;
	float value2;
	int status; // 0 = empty, 1 = obstacle, 3 = goal
    float potential;
	float gradTh = 45*180/M_PI;
	int cellSize;
    bool visited;
	bool updated;
	bool mapping; // 0 = Bayes, 1 = HIMM
    int region;

	void whichRegion(int err, int range, float rPolar, float aPolar, float thRobot);
	void bayes1(float rPolar, float aPolar, float thRobot, int range);
	void bayes2(float rPolar, float aPolar, float thRobot, int range);
	void addValue();
	void subValue();

    QColor color;
};

#endif // CELL_H
