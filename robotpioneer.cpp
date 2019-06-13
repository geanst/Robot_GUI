#include "robotpioneer.h"
#include "Aria.h"
#include "qdebug.h"

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

RobotPioneer::RobotPioneer(int tipoConexao,char* info,int *sucesso)
{     
	this->tipoConexao = tipoConexao;
	this->info = info;
	this->sucesso = sucesso;
    this->th = 0;
}

void RobotPioneer::robotShutdown()
{
    Aria::shutdown();
    robot.stopRunning();
    return;
}



void RobotPioneer::accelerate()
{
	robot.setVel(robot.getVel()+10);
}

void RobotPioneer::deaccelerate()
{
	robot.setVel(robot.getVel()-10);
}

void RobotPioneer::right()
{
	robot.setRotVel(robot.getRotVel()-10);
}

void RobotPioneer::left()
{
	robot.setRotVel(robot.getRotVel()+10);
}

void RobotPioneer::run()
{

	int argc=0;char** argv;
	Aria::init();
	switch (tipoConexao)
	{
	case ConexaoSerial:
		{  char port1[4];
		   strcpy(port1,info);

		   int ret;
		   std::string str;

		   *sucesso=((ret = con1.open(port1))== 0);
		   if (*sucesso)
		   { robot.setDeviceConnection(&con1);}
			 *sucesso=robot.blockingConnect();
		   break;
		}
	case ConexaoRadio:
		{ argc=4;
		  argv =(char **) new char[4];
		  argv[0]=new char[4];
		  argv[1]=new char[20];
		  argv[2]=new char[20];
		  argv[3]=new char[7];

		  strcpy(argv[0],"-rh");
		  strcpy(argv[1],info);
		  strcpy(argv[2],"-remoteLaserTcpPort");
		  strcpy(argv[3],"10002");

		  break;
		}

	case ConexaoSimulacao:

		{ argc=2;
		  argv =(char **) new char[2];

		  argv[0]=new char[4];
		  argv[1]=new char[20];

		  strcpy(argv[0],"-rh");
		  strcpy(argv[1],"localhost");

		}
	}

	parser=new ArArgumentParser(&argc, argv);

	robotConnector=new ArRobotConnector(parser,&robot);

	*sucesso=robotConnector->connectRobot();
	if (!(*sucesso))
	{   Aria::shutdown();
		}
	robot.addRangeDevice(&sick);

	laserConnector=new ArLaserConnector(parser, &robot, robotConnector);
	laserConnector->setupLaser(&sick);


	if (*sucesso)
	{
		robot.addRangeDevice(&sonarDev);
	   if (*sucesso)
	   {
		   sick.runAsync();
		   robot.setHeading(0);
		   robot.runAsync(true);
		   robot.enableMotors();
		   robot.setRotVelMax(10);
           //ArUtil::sleep(1000);
		   //robot.setVel2(30,-30);
/*		   printf("Connecting...\n");
		   if (!laserConnector->connectLaser(&sick))
		   { printf("Could not connect to lasers... exiting\n");
			 Aria::exit(2);
		   }
*/

	   }
	   else Aria::shutdown();
	}else 	Aria::shutdown();
//	robot.setHeading(-90);
//	robot.setVel(20);
	while(robot.isRunning())
    {
	 /*   if(robot.getSonarReading(3)->getRange() < 1000 || robot.getSonarReading(2)->getRange() < 800)
			robot.setVel2(50,-50);
		else if(robot.getSonarReading(4)->getRange() < 1000 || robot.getSonarReading(5)->getRange() < 800)
			robot.setVel2(50,-50);
        else
			robot.setVel2(50,50);*/
		while (!robot.isHeadingDone(0.5))
		{
			//robot.setVel(10);
            robot.setHeading(180+th);
			robot.setVel(20);
            //qDebug() <<  180+th << robot.getTh();
		}
		robot.setVel(50);
        robot.setHeading(180+th);

	}

}


