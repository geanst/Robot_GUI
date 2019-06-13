#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include "robotcursor.h"
#include "robotpioneer.h"
#include "grid.h"
#include<bits/stdc++.h>
#include "logger.h"
#include "thcompass.h"
using namespace std;

#define Bayes 0
#define HIMM 1

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0); //= nullptr OR = 0
    ~MainWindow();

    void addToScene(QGraphicsScene &scene, Grid *matrix);

    QGraphicsScene * scene;
	QGraphicsScene * scene2;
    RobotCursor * item;
	QTimer *timer;
	RobotPioneer * robo;
	Grid * matrix;
	Logger * logging;
	ThCompass * thRobo;

	int size = 1000;
	int cellSize = 10;
	int scale = 10;
	bool mapping = Bayes;
	int xRobot;
	int yRobot;
	int thRobot;
	int currentViewAngle = 0;
	int updateRate = 500;
	vector<ArSensorReading> *rawReadRobot;


    Ui::MainWindow *ui;
	void toLine(int x1, int y1, int const x2, int const y2);
	void arcSonar(int angle, int range, int const robotX , int const robotY, int xObject, int yObject);
	void drawline(int x1, int y1, int const x2, int const y2, vector < pair <int,int> > &vect);
	void saveBMP(Grid *matrix);
	void potentialField(Grid *matrix);
	void allPotentialField(Grid *matrix);
public slots:
	void zoomIn();
	void zoomOut();
	void rotateView(int x);
	void updateRobotPos();

	private slots:
	void updatePotFie();
	void on_actionSave_BMP_triggered(bool checked);
	void on_actionBVP_triggered();
};

#endif // MAINWINDOW_H
