#include "grid.h"
#include "cell.h"
#include <QObject>
#include "qdebug.h"

Grid::Grid(int size, int cell_size, int scale, bool mapping)
{
	this->size = size;
    this->cell_size = cell_size;
	this->scale = scale;

	int halfSize = int(size/2);

	matriz = new Cell **[size];

	int xx = 0;
	for(int i = -halfSize*scale; i < (size - halfSize)*scale; i += scale)
    {
		matriz[xx] = new Cell *[size];
		int yy = 0;
		for(int j = -halfSize*scale; j < (size - halfSize)*scale; j += scale)
        {
			matriz[xx][yy] = new Cell(i, j, cell_size, scale, mapping);
			yy++;
		}
		xx++;
    }


}
