#ifndef ROBOTPIONEER_H
#define ROBOTPIONEER_H
#include "Aria.h"

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <QThread>
#include <QTimer>

#define GirarBase          1
#define ConexaoSerial      1
#define ConexaoRadio       2
#define ConexaoSimulacao   3

class RobotPioneer: public QThread
{
		Q_OBJECT
	public:
		RobotPioneer(int tipoConexao,char* info,int *sucesso);

		ArRobot robot;
		ArSonarDevice sonarDev;
		ArSick   sick;
		QTimer *timer;

		ArLaserConnector *laserConnector;
		ArRobotConnector *robotConnector;
		ArArgumentParser *parser;
		ArSimpleConnector *simpleConnector;
		ArAnalogGyro *gyro;
		ArSerialConnection con1;
		int tipoConexao;
		char* info;
		int *sucesso;
		int x;
		int y;
		float th;

        void robotShutdown();

	public slots:
		void accelerate();
		void deaccelerate();
		void right();
		void left();

	protected:
		void run();
};

#endif // ROBOTPIONEER_H

