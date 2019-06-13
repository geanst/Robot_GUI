#ifndef THCOMPASS_H
#define THCOMPASS_H

#include <QGraphicsItem>

class ThCompass: public QGraphicsItem
{
	public:
		ThCompass();

		QRectF boundingRect() const override;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
		float robotTh;
		float gradTh;
};

#endif // THCOMPASS_H
