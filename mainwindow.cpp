#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cell.h"
#include "grid.h"
#include "qdebug.h"
#include "robotpioneer.h"
#include <QTimer>
#include <QObject>
#include <QBrush>
#include "thcompass.h"
#include "logger.h"

//Função usada para classificar por ordem crescente de Y e depois X
bool sortbyYandX(const pair<int,int> &a,
				   const pair<int,int> &b)
{
	if(a.second == b.second)
		return a.first<b.first;
	else
		return a.second<b.second;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{	//Scene principal
    ui->setupUi(this);
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);

	//Scene da Pose do Robô e gradiente
	scene2 = new QGraphicsScene();
	ui->graphicsView_2->setScene(scene2);
	thRobo = new ThCompass();
	scene2->addItem(thRobo);

	//Matriz onde serão desenhadas as células
	matrix = new Grid(size,cellSize, scale, mapping);
    addToScene(*scene,matrix);

	//Representação do Robõ
	item = new RobotCursor(matrix,10);
    item->setFlag(QGraphicsItem::ItemIsFocusable);
	item->setFocus();
	scene->addItem(item);

	//Conexão do Sinais com os Slots
	connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(zoomIn()));
	connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(zoomOut()));
	connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(rotateView(int)));

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()),this, SLOT(updateRobotPos()));
	timer->start(updateRate);
//	QTimer::singleShot(200, this, SLOT(updateRobotPos()));

	//Conexão com o Robô simulado ou real
	int sucesso;
	//robo = new RobotPioneer(ConexaoRadio,"192.168.1.11", &sucesso);
	robo = new RobotPioneer(ConexaoSimulacao,"", &sucesso);
	robo->start();


	//Logger
//	logging = new Logger();
//	logging->start();

	//Update dos Campos Potenciais
//	qDebug() << "a";
//	QTimer * timer2 = new QTimer(this);
//	qDebug() << "a";
	connect(timer, SIGNAL(timeout()),this, SLOT(updatePotFie()));
	connect(item,SIGNAL(accelerate()),robo,SLOT(accelerate()));
	connect(item,SIGNAL(deaccelerate()),robo,SLOT(deaccelerate()));
	connect(item,SIGNAL(right()),robo,SLOT(right()));
	connect(item,SIGNAL(left()),robo,SLOT(left()));
//	qDebug() << "a";
//	timer2->start(2000);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addToScene(QGraphicsScene &scene, Grid *matrix)
{
	/* Adiciona as células da Matriz na Scene */
	for(int i = 0; i < size; i++)
    {
		for(int j = 0; j < size; j++)
        {
           scene.addItem(matrix->matriz[i][j]);
        }
    }
}

void MainWindow::zoomIn()
{
	ui->graphicsView->scale(0.8,0.8);
}

void MainWindow::zoomOut()
{
	ui->graphicsView->scale(1.2,1.2);
}

void MainWindow::rotateView(int x)
{
	ui->graphicsView->rotate(x - currentViewAngle);
	currentViewAngle = x;
}

void MainWindow::updateRobotPos()
{
	int xTemp;
	int yTemp;
	int sonarRange;
	this->xRobot = robo->robot.getX()/10;
	this->yRobot = robo->robot.getY()/10;
	this->thRobot = robo->robot.getTh();
	robo->sick.lockDevice();

	if(mapping)
	{
	//HIMM SONAR
		ArSensorReading *readings;
		for (int i = 0; i < 8; i++) {
			readings = robo->robot.getSonarReading(i);
			xTemp = readings->getX();
			yTemp = readings->getY();
			sonarRange = readings->getRange()/10;
			xTemp = (xTemp/10)/scale+size/2;
			yTemp = -(yTemp/10)/scale+size/2;
			if(xTemp >= size || yTemp >= size || xTemp < 0 || yTemp < 0 )
				qDebug() << "Fora da MAtriz";
			else
			{
				if(sonarRange < 300)
				{
					matrix->matriz[xTemp][yTemp]->addValue();
				}
				toLine(xRobot/scale+size/2,-yRobot/scale+size/2,xTemp,yTemp);

			}

		}
	//LASER (HIMM)
		/*
			this->rawReadRobot = robo->sick.getRawReadingsAsVector();
			for(std::vector<ArSensorReading>::iterator i=rawReadRobot->begin(); i!=rawReadRobot->end();i++)
			{
				xTemp = ((*i).getX()/10)/scale+size/2;
				yTemp = -((*i).getY()/10)/scale+size/2;
				if(xTemp >= size || yTemp >= size || xTemp < 0 || yTemp < 0 )
					qDebug() << "Fora da MAtriz";
				else
				{

					matrix->matriz[xTemp][yTemp]->addValue();
					matrix->matriz[xTemp][yTemp]->update();

					toLine(xRobot/scale+size/2,-yRobot/scale+size/2,xTemp,yTemp);
				}

			}
		*/
	}
	else
	{
	//BAYES SONAR
		int angletemp;
		ArSensorReading *readings;

		for (int i = 0; i < 8; i++)
		{
			readings = robo->robot.getSonarReading(i);
			sonarRange = (readings->getRange()/10);
			sonarRange = sqrt(pow(readings->getLocalX(),2)+pow(readings->getLocalY(),2))/10;
			angletemp = readings->getSensorTh();

			arcSonar(angletemp+readings->getThTaken(),sonarRange,xRobot,yRobot,((readings->getX())/10-xRobot),(readings->getY()/10)-yRobot);
			if(sonarRange<300)
			{
				QBrush redbrush(Qt::red);
				QPen nopen(Qt::NoPen);

				scene->addRect((readings->getX()/10),(-readings->getY()/10),1,1,nopen,redbrush);
			}

			//logging->log(readings);


		}
	}

		this->item->updatePos(xRobot,yRobot);
		robo->sick.unlockDevice();

}

void MainWindow::updatePotFie()
{
	potentialField(matrix);
	//int rx = robo->robot.getX()/(10*scale)+size/2;
	//int ry = robo->robot.getY()/(10*scale)+size/2;
	//robo->th = matrix->matriz[rx][ry]->gradTh * 180/M_PI;
	//thRobo->gradTh = matrix->matriz[rx][ry]->gradTh;
	//thRobo->robotTh = robo->robot.getTh() * M_PI/180;
	//qDebug() << matrix->matriz[rx][ry]->gradTh * 180/M_PI << robo->robot.getTh();
	//scene->addRect((rx-size/2)*scale,-(ry-size/2)*scale,10,10,QPen(),QBrush(Qt::yellow));
	//thRobo->update();
}

void MainWindow::toLine(int x1,	int y1,	int const x2,	int const y2)
{
	//Bresenham para HIMM
	int delta_x(x2 - x1);
	// if x1 == x2, then it does not matter what we set here
	signed char const ix((delta_x > 0) - (delta_x < 0));
	delta_x = std::abs(delta_x) << 1;

	int delta_y(y2 - y1);
	// if y1 == y2, then it does not matter what we set here
	signed char const iy((delta_y > 0) - (delta_y < 0));
	delta_y = std::abs(delta_y) << 1;

	matrix->matriz[x1][y1]->subValue();
	//matrix->matriz[x1][y1]->update();

	if (delta_x >= delta_y)
	{
		// error may go below zero
		int error(delta_y - (delta_x >> 1));

		while (x1 != x2)
		{
			// reduce error, while taking into account the corner case of error == 0
			if ((error > 0) || (!error && (ix > 0)))
			{
				error -= delta_x;
				y1 += iy;
			}
			// else do nothing

			error += delta_y;
			x1 += ix;

			matrix->matriz[x1][y1]->subValue();
			//matrix->matriz[x1][y1]->update();
		}
	}
	else
	{
		// error may go below zero
		int error(delta_x - (delta_y >> 1));

		while (y1 != y2)
		{
			// reduce error, while taking into account the corner case of error == 0
			if ((error > 0) || (!error && (iy > 0)))
			{
				error -= delta_y;
				x1 += ix;
			}
			// else do nothing

			error += delta_x;
			y1 += iy;
			matrix->matriz[x1][y1]->subValue();
			//matrix->matriz[x1][y1]->update();
		}
	}
}

void MainWindow::arcSonar(int angle, int range, int robotX, int robotY, int xObject, int yObject)
{
	//Arco do Bayes
	vector < pair <int,int> > vect;
	int betha = 15;
    int err = 5;
	int rangeWithErr = range + err;
	//range += err;
	int x0, xx;
	int y0, yy;


   //qDebug() << range << sqrt(pow(xObject,2)+pow(yObject,2));

	x0 = (cos((angle-betha)*M_PI/180) * (rangeWithErr))/scale;
	y0 = (sin((angle-betha)*M_PI/180) * (rangeWithErr))/scale;
	for(int i = angle-betha+1; i < angle+betha+1; i++)
	{

		xx = (cos(i*M_PI/180) * (rangeWithErr))/scale;
		yy = (sin(i*M_PI/180) * (rangeWithErr))/scale;

        drawline(x0, y0, xx, yy,vect);
		x0 = xx;
		y0 = yy;
	}

	int vecSize = vect.size();
	drawline(0,0,vect[0].first,vect[0].second,vect);
	drawline(0,0,vect[vecSize-1].first,vect[vecSize-1].second,vect);

	sort(vect.begin(),vect.end(), sortbyYandX);
	vect.erase( unique( vect.begin(), vect.end() ), vect.end() );

	int a = 0;
	for (int i = 0 ; i < vect.size() ; i++)
	{
		a = i;
		while(vect[i].second == vect[a].second && a < vect.size())
		{
		   a++;
		}

		if(a-i==1)
		{
            float rPolar = sqrt(pow(vect[i].first,2)+pow(vect[i].second,2));
            float aPolar = atan2(vect[i].second,vect[i].first) * 180 / M_PI;
            angle = atan2(yObject,xObject) * 180 / M_PI;
			if((vect[i].first+size/2+xRobot/scale) < size && (-vect[i].second+(size/2)-(yRobot/scale)) < size)
				matrix->matriz[vect[i].first+size/2+xRobot/scale][-vect[i].second+(size/2)-(yRobot/scale)]->whichRegion(err, range, rPolar, aPolar, angle);
		}
		else
		{
			for(int k = vect[i].first; k <= vect[a-1].first; k++)
			{

				assert(k >= 0 || k < size);
				float rPolar = sqrt(pow(k,2)+pow(vect[i].second,2));
                float aPolar = atan2(vect[i].second,k) * 180 / M_PI;
				angle = atan2(yObject,xObject) * 180 / M_PI;
				if((k+(size/2)+(xRobot/scale)) < size && (-vect[i].second+(size/2)-(yRobot/scale)) < size)
				matrix->matriz[k+(size/2)+(xRobot/scale)][-vect[i].second+(size/2)-(yRobot/scale)]->whichRegion(err, range, rPolar, aPolar, angle);
			}
		}

		i = a-1;
	}

	vect.clear();
}

void MainWindow::drawline(int x1, int y1, int const x2, int const y2, vector < pair <int,int> > &vect)
{
	//Arco do Bayes
	int delta_x(x2 - x1);
	// if x1 == x2, then it does not matter what we set here
	signed char const ix((delta_x > 0) - (delta_x < 0));
	delta_x = std::abs(delta_x) << 1;

	int delta_y(y2 - y1);
	// if y1 == y2, then it does not matter what we set here
	signed char const iy((delta_y > 0) - (delta_y < 0));
	delta_y = std::abs(delta_y) << 1;


	vect.push_back( make_pair(x1,y1) );
	//scene->addRect(x1*5,-y1*5,1,1)->setBrush(QBrush(Qt::blue));


	if (delta_x >= delta_y)
	{
		// error may go below zero
		int error(delta_y - (delta_x >> 1));

		while (x1 != x2)
		{
			// reduce error, while taking into account the corner case of error == 0
			if ((error > 0) || (!error && (ix > 0)))
			{
				error -= delta_x;
				y1 += iy;
			}
			// else do nothing

			error += delta_y;
			x1 += ix;

			if(x1 > 1000 || y1 > 1000)
				qDebug() << "coordenada ENVIADA maior que matriz";

			vect.push_back( make_pair(x1,y1) );
			//scene->addRect(x1*5,-y1*5,1,1)->setBrush(QBrush(Qt::blue));

		}
	}
	else
	{
		// error may go below zero
		int error(delta_x - (delta_y >> 1));

		while (y1 != y2)
		{
			// reduce error, while taking into account the corner case of error == 0
			if ((error > 0) || (!error && (iy > 0)))
			{
				error -= delta_y;
				x1 += ix;
			}
			// else do nothing

			error += delta_x;
			y1 += iy;

			if(x1 > 1000 || y1 > 1000)
				qDebug() << "coordenada ENVIADA maior que matriz";

			vect.push_back( make_pair(x1,y1) );
			//scene->addRect(x1*5,-y1*5,1,1)->setBrush(QBrush(Qt::blue));

		}
	}
}

void MainWindow::saveBMP(Grid *matrix)
{
	//Salva o mapa em BMP
	int w = 1000;
	int h = 1000;
	int r=0,g=0,b=0;
	FILE *f;
	unsigned char *img = NULL;
	int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int

	img = (unsigned char *)malloc(3*w*h);
	memset(img,0,3*w*h);

	for(int i=0; i<w; i++)
	{
		for(int j=0; j<h; j++)
		{
			if(matrix->matriz[i][j]->visited)
			{
				int x=i;
				int y=(h-1)-j;
				r = 0;
				g = 255;
				b = 0;
				if (r > 255) r=255;
				if (g > 255) g=255;
				if (b > 255) b=255;
				img[(x+y*w)*3+2] = (unsigned char)(r);
				img[(x+y*w)*3+1] = (unsigned char)(g);
				img[(x+y*w)*3+0] = (unsigned char)(b);
			}
			else
            {
				int x=i;
				int y=(h-1)-j;
                float val = matrix->matriz[i][j]->region;
				if(mapping)
				{
                    r = 255- 255*(val/5);
                    g = 255- 255*(val/5);
                    b = 255- 255*(val/5);
				}
				else
				{
					r = 255- 255*val;
					g = 255- 255*val;
					b = 255- 255*val;
				}

                //if (r > 255) r=255;
                //if (g > 255) g=255;
                //if (b > 255) b=255;
				img[(x+y*w)*3+2] = (unsigned char)(r);
				img[(x+y*w)*3+1] = (unsigned char)(g);
				img[(x+y*w)*3+0] = (unsigned char)(b);
			}
		}
	}

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(       w    );
	bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(       w>>16);
	bmpinfoheader[ 7] = (unsigned char)(       w>>24);
	bmpinfoheader[ 8] = (unsigned char)(       h    );
	bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
	bmpinfoheader[10] = (unsigned char)(       h>>16);
	bmpinfoheader[11] = (unsigned char)(       h>>24);

	f = fopen("Bayes.bmp","wb");
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	for(int i=0; i<h; i++)
	{
		fwrite(img+(w*(h-i-1)*3),3,w,f);
		fwrite(bmppad,1,(4-(w*3)%4)%4,f);
	}

	free(img);
	fclose(f);
}

void MainWindow::potentialField(Grid *matrix)
{

	int windowSize = 60;
	float previous[2*windowSize][2*windowSize];
	int rx = robo->robot.getX()/(10*scale);
    int ry = -robo->robot.getY()/(10*scale);
	int minX = (size/2 + rx) - windowSize;
	int maxX = (size/2 + rx) + windowSize;
	int minY = (size/2 + ry) - windowSize;
	int maxY = (size/2 + ry) + windowSize;
	float mean;
    double error = 1, errorMax = 0.00001;
	int a=0,b=0;

    matrix->matriz[size/2+rx][size/2+ry]->region = 5;

	//Cria mapa do tipo da célula: Vazia, Obstáculo ou Objetivo
	for(int i = minX-1; i < maxX+1; i++)
	{
		for(int j = minY-1; j < maxY+1; j++)
		{
			if(i==minX-1 || j == minY-1 || i == maxX || j == maxY)
			{
				if(matrix->matriz[i][j]->status < 0)
				{
					if(matrix->matriz[i][j]->value < 0.5)
					{
						//empty
						matrix->matriz[i][j]->status = 0;
						matrix->matriz[i][j]->potential = 0;
					}
					else if(matrix->matriz[i][j]->value > 0.5)
					{
						//obstacle
						matrix->matriz[i][j]->status = 1;
						matrix->matriz[i][j]->potential = 1;
                        matrix->matriz[i][j]->region = 0;
					}
					else
					{
						//goal
						matrix->matriz[i][j]->status = 3;
						matrix->matriz[i][j]->potential = 0;
					}
				}
			}
			else if(matrix->matriz[i][j]->value < 0.5)
			{
				//empty
				if(matrix->matriz[i][j]->status != 0)
				{
					matrix->matriz[i][j]->status = 0;
					matrix->matriz[i][j]->potential = 0;
				}
			}
			else if(matrix->matriz[i][j]->value > 0.5)
			{
				//obstacle
				matrix->matriz[i][j]->status = 1;
				matrix->matriz[i][j]->potential = 1;
                matrix->matriz[i][j]->region = 0;
			}
			else
			{
				//goal
				matrix->matriz[i][j]->status = 3;
				matrix->matriz[i][j]->potential = 0;
			}

		}
	}
    do
	{
		error = 0;
		a=0;
		for(int i = minX; i < maxX; i++)
		{b=0;
			for(int j = minY; j < maxY; j++)
			{
				if(matrix->matriz[i][j]->status == 0)
				{
					previous[a][b] = matrix->matriz[i][j]->potential;
					matrix->matriz[i][j]->potential = 0.25*(
							matrix->matriz[i-1][j]->potential +
							matrix->matriz[i][j-1]->potential +
							matrix->matriz[i+1][j]->potential +
							matrix->matriz[i][j+1]->potential);
                    if(matrix->matriz[i-1][j]->region == 5 || matrix->matriz[i][j-1]->region == 5 || matrix->matriz[i+1][j]->region == 5 || matrix->matriz[i][j+1]->region == 5)
                        matrix->matriz[i][j]->region = 5;
				}
			b++;
			}
		a++;
		}
		for(int i = minX; i < maxX; i++)
		{
			for(int j = maxY-1; j > minY-1; j--)
			{
				if(matrix->matriz[i][j]->status == 0)
				{
					matrix->matriz[i][j]->potential = 0.25*(
							matrix->matriz[i-1][j]->potential +
							matrix->matriz[i][j-1]->potential +
							matrix->matriz[i+1][j]->potential +
							matrix->matriz[i][j+1]->potential);
                    if(matrix->matriz[i-1][j]->region == 5 || matrix->matriz[i][j-1]->region == 5 || matrix->matriz[i+1][j]->region == 5 || matrix->matriz[i][j+1]->region == 5)
                        matrix->matriz[i][j]->region = 5;
				}
			}
		}
		for(int i = maxX-1; i > minX-1; i--)
		{
			for(int j = minY; j < maxY; j++)
			{
				if(matrix->matriz[i][j]->status == 0)
				{
					matrix->matriz[i][j]->potential = 0.25*(
							matrix->matriz[i-1][j]->potential +
							matrix->matriz[i][j-1]->potential +
							matrix->matriz[i+1][j]->potential +
							matrix->matriz[i][j+1]->potential);
                     if(matrix->matriz[i-1][j]->region == 5 || matrix->matriz[i][j-1]->region == 5 || matrix->matriz[i+1][j]->region == 5 || matrix->matriz[i][j+1]->region == 5)
                        matrix->matriz[i][j]->region = 5;

				}
			}
		}
		a = 2*windowSize-1;
		for(int i = maxX-1; i > minX-1; i--)
		{
			b = 2*windowSize-1;
			for(int j = maxY-1; j > minY-1; j--)
			{
				if(matrix->matriz[i][j]->status == 0)
				{
					matrix->matriz[i][j]->potential = 0.25*(
							matrix->matriz[i-1][j]->potential +
							matrix->matriz[i][j-1]->potential +
							matrix->matriz[i+1][j]->potential +
							matrix->matriz[i][j+1]->potential);
					error += pow(previous[a][b] - matrix->matriz[i][j]->potential,2);
                    if(matrix->matriz[i-1][j]->region == 5 || matrix->matriz[i][j-1]->region == 5 || matrix->matriz[i+1][j]->region == 5 || matrix->matriz[i][j+1]->region == 5)
                        matrix->matriz[i][j]->region = 5;

				}
			b--;
			}
		a--;
		}
    }while(error>errorMax);

bool stopCondition = true;
	for(int i = (size/2 + rx - windowSize); i < (size/2 + rx + windowSize); i++)
	{
		for(int j = (size/2 + ry - windowSize); j < (size/2 + ry + windowSize); j++)
		{
            float gradTh =  atan2((matrix->matriz[i][j-1]->potential-matrix->matriz[i][j+1]->potential),(matrix->matriz[i+1][j]->potential-matrix->matriz[i-1][j]->potential));
            matrix->matriz[i][j]->gradTh = gradTh;           
            matrix->matriz[i][j]->update();
            matrix->matriz[i][j]->setToolTip(QString("Potencial: ").append(QString::number(matrix->matriz[i][j]->potential)));

           if(matrix->matriz[i][j]->region<=0)
                matrix->matriz[i][j]->region = 0;
           else
                matrix->matriz[i][j]->region--;

           if(matrix->matriz[i][j]->region > 0 && (matrix->matriz[i-1][j]->status == 3 || matrix->matriz[i][j-1]->status == 3 || matrix->matriz[i+1][j]->status == 3 || matrix->matriz[i][j+1]->status == 3))
               stopCondition = false;

			if(i==(size/2 + rx) && j==(size/2 + ry))
			{
				thRobo->robotTh = robo->robot.getTh() * M_PI/180;
				thRobo->update();
                robo->th = gradTh*180/M_PI;
                matrix->matriz[i][j]->visited=true;
                matrix->matriz[i][j]->update();

			}
		}
	}
if(stopCondition)
{
    qDebug() << "Exploração Completa!!!";
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            if(matrix->matriz[i][j]->status == 0 && matrix->matriz[i][j]->region > 0)
            {
                matrix->matriz[i][j]->value = 0;
                matrix->matriz[i][j]->update();
            }
            else if(matrix->matriz[i][j]->status == 1)
            {
                matrix->matriz[i][j]->value = 1;
                matrix->matriz[i][j]->update();
            }
            else
            {
                matrix->matriz[i][j]->value = 0.5;
                matrix->matriz[i][j]->update();
            }
        }
    }
    timer->stop();
    robo->robotShutdown();
    robo->exit();
}
}

void MainWindow::on_actionSave_BMP_triggered(bool checked)
{
	qDebug() << "Save BMP";
    saveBMP(this->matrix);
    //QWidget *widget= ui->graphicsView;
    //
    // QPixmap pic = QPixmap::grabWidget(widget);
    // //widget->render();
    // pic.save("testScreen.png");
}

void MainWindow::on_actionBVP_triggered()
{
	//Mudar para Logging
	allPotentialField(matrix);
	//logging->close();
}

void MainWindow::allPotentialField(Grid *matrix)
{

	int windowSize = 100;
	float previous[2*windowSize][2*windowSize];
	int minX = (size/2) - windowSize;
	int maxX = (size/2) + windowSize;
	int minY = (size/2) - windowSize;
	int maxY = (size/2) + windowSize;
	float mean;
	double error = 1, errorMax = 0.001;
	int a=0,b=0;

	//Cria mapa do tipo da célula: Vazia, Obstáculo ou Objetivo
	for(int i = minX-1; i < maxX+1; i++)
	{
		for(int j = minY-1; j < maxY+1; j++)
		{
			if(i==minX-1 || j == minY-1 || i == maxX || j == maxY)
			{
				if(matrix->matriz[i][j]->status < 0)
				{
					if(matrix->matriz[i][j]->value < 0.5)
					{
						//empty
						matrix->matriz[i][j]->status = 0;
						matrix->matriz[i][j]->potential = 0;
					}
					else if(matrix->matriz[i][j]->value > 0.5)
					{
						//obstacle
						matrix->matriz[i][j]->status = 1;
						matrix->matriz[i][j]->potential = 1;
					}
					else
					{
						//goal
						matrix->matriz[i][j]->status = 3;
						matrix->matriz[i][j]->potential = 0;
					}
				}
			}
			else if(matrix->matriz[i][j]->value < 0.5)
			{
				//empty
				matrix->matriz[i][j]->status = 0;
				matrix->matriz[i][j]->potential = 0;
			}
			else if(matrix->matriz[i][j]->value > 0.5)
			{
				//obstacle
				matrix->matriz[i][j]->status = 1;
				matrix->matriz[i][j]->potential = 1;
			}
			else
			{
				//goal
				matrix->matriz[i][j]->status = 3;
				matrix->matriz[i][j]->potential = 0;
			}

		}
	}
	do
	{
		error = 0;
		a=0;
		for(int i = minX; i < maxX; i++)
		{b=0;
			for(int j = minY; j < maxY; j++)
			{
				if(matrix->matriz[i][j]->status == 0)
				{
					previous[a][b] = matrix->matriz[i][j]->potential;
				/*	mean = (i==minX || i==(maxX-1) || j==minX || j==(maxY-1)) ? 0.33:0.25;
					matrix->matriz[i][j]->potential = mean*(
							(i==minX ? 0:matrix->matriz[i-1][j]->potential) +
							(i==(maxX-1) ? 0:matrix->matriz[i][j-1]->potential) +
							(j==minY ? 0:matrix->matriz[i+1][j]->potential) +
							(j==(maxY-1) ? 0:matrix->matriz[i][j+1]->potential));*/
					matrix->matriz[i][j]->potential = 0.25*(
							matrix->matriz[i-1][j]->potential +
							matrix->matriz[i][j-1]->potential +
							matrix->matriz[i+1][j]->potential +
							matrix->matriz[i][j+1]->potential);
				}
			b++;
			}
		a++;
		}
		for(int i = minX; i < maxX; i++)
		{
			for(int j = maxY-1; j > minY-1; j--)
			{
				if(matrix->matriz[i][j]->status == 0)
				{
				/*	mean = (i==minX || i==(maxX-1) || j==minX || j==(maxY-1)) ? 0.33:0.25;
					matrix->matriz[i][j]->potential = mean*(
							(i==minX ? 0:matrix->matriz[i-1][j]->potential) +
							(i==(maxX-1) ? 0:matrix->matriz[i][j-1]->potential) +
							(j==minY ? 0:matrix->matriz[i+1][j]->potential) +
							(j==(maxY-1) ? 0:matrix->matriz[i][j+1]->potential));*/
					matrix->matriz[i][j]->potential = 0.25*(
							matrix->matriz[i-1][j]->potential +
							matrix->matriz[i][j-1]->potential +
							matrix->matriz[i+1][j]->potential +
							matrix->matriz[i][j+1]->potential);
				}
			}
		}
		for(int i = maxX-1; i > minX-1; i--)
		{
			for(int j = minY; j < maxY; j++)
			{
				if(matrix->matriz[i][j]->status == 0)
				{
				/*	mean = (i==minX || i==(maxX-1) || j==minX || j==(maxY-1)) ? 0.33:0.25;
					matrix->matriz[i][j]->potential = mean*(
							(i==minX ? 0:matrix->matriz[i-1][j]->potential) +
							(i==(maxX-1) ? 0:matrix->matriz[i][j-1]->potential) +
							(j==minY ? 0:matrix->matriz[i+1][j]->potential) +
							(j==(maxY-1) ? 0:matrix->matriz[i][j+1]->potential));*/
					matrix->matriz[i][j]->potential = 0.25*(
							matrix->matriz[i-1][j]->potential +
							matrix->matriz[i][j-1]->potential +
							matrix->matriz[i+1][j]->potential +
							matrix->matriz[i][j+1]->potential);

				}
			}
		}
		a = 2*windowSize-1;
		for(int i = maxX-1; i > minX-1; i--)
		{
			b = 2*windowSize-1;
			for(int j = maxY-1; j > minY-1; j--)
			{
				if(matrix->matriz[i][j]->status == 0)
				{
				/*	mean = (i==minX || i==(maxX-1) || j==minX || j==(maxY-1)) ? 0.33:0.25;
					matrix->matriz[i][j]->potential = mean*(
							(i==minX ? 0:matrix->matriz[i-1][j]->potential) +
							(i==(maxX-1) ? 0:matrix->matriz[i][j-1]->potential) +
							(j==minY ? 0:matrix->matriz[i+1][j]->potential) +
							(j==(maxY-1) ? 0:matrix->matriz[i][j+1]->potential));*/
					matrix->matriz[i][j]->potential = 0.25*(
							matrix->matriz[i-1][j]->potential +
							matrix->matriz[i][j-1]->potential +
							matrix->matriz[i+1][j]->potential +
							matrix->matriz[i][j+1]->potential);
					error += pow(previous[a][b] - matrix->matriz[i][j]->potential,2);


				}
			b--;
			}
		a--;
		}
	}while(error>errorMax);

	for(int i = (size/2 - windowSize); i < (size/2 + windowSize); i++)
	{
		for(int j = (size/2 - windowSize); j < (size/2 + windowSize); j++)
		{
			float gradTh =  atan2((matrix->matriz[i][j-1]->potential-matrix->matriz[i][j+1]->potential),(matrix->matriz[i+1][j]->potential-matrix->matriz[i-1][j]->potential));
			matrix->matriz[i][j]->gradTh = gradTh;
			matrix->matriz[i][j]->update();
		}
	}

}
