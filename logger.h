#ifndef LOGGER_H
#define LOGGER_H

#include "Aria.h"

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <QThread>
#include <QTimer>
using namespace std;


class Logger: public QThread
{
		Q_OBJECT
	public:
		Logger();

		void log(ArSensorReading * readings);
		void close();

		fstream fout;
	protected:
		void run();
};

#endif // LOGGER_H
