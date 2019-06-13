#ifndef GRID_H
#define GRID_H

#include "cell.h"
#include <QColor>
#include <QObject>

class Grid: public QObject
{
			Q_OBJECT
	public:
		Cell *** matriz;
		Grid(int size, int cell_size, int scale, bool mapping);

		void updateCell(int x, int y, QColor new_color);
		int size;
		int cell_size;
		int scale;

};

#endif // GRID_H
