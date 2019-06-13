#include "logger.h"
#include <qdebug.h>

Logger::Logger()
{
	fout.open("pioneer_log.csv", ios::out | ios::app);
	fout << "getPoseTaken.getX" << ","
		 << "getPoseTaken.getY" << ","
		 << "getPoseTaken.getTh" << ","
		 << "getX" << ","
		 << "getY" << ","
//		 << "getPose"  << ","
		 << "getRange"  << ","
		 << "getLocalX"  << ","
		 << "getLocalY"  << ","
		 << "getXTaken"  << ","
		 << "getYTaken"  << ","
		 << "getSensorX"  << ","
		 << "getSensorY"  << ","
		 << "getThTaken"  << ","
		 << "getSensorDX"  << ","
		 << "getSensorDY"  << ","
		 << "getSensorTh"  << ","
//		 << "getLocalPose"  << ","
//		 << "getPoseTaken"  << ","
//		 << "getTimeTaken"  << ","
		 << "getCounterTaken"  //<< ","
//		 << "getSensorPosition"  << ","
//		 << "getEncoderPoseTaken"
		 << "\n";
}

void Logger::log(ArSensorReading *readings)
{
	fout << readings->getPoseTaken().getX() << ","
		 << readings->getPoseTaken().getY() << ","
		 << readings->getPoseTaken().getTh() << ","
		 << readings->getX() << ","
		 << readings->getY() << ","
//		 << readings->getPose().getTh()  << ","
		 << readings->getRange()  << ","
		 << readings->getLocalX()  << ","
		 << readings->getLocalY()  << ","
		 << readings->getXTaken()  << ","
		 << readings->getYTaken()  << ","
		 << readings->getSensorX()  << ","
		 << readings->getSensorY()  << ","
		 << readings->getThTaken()  << ","
		 << readings->getSensorDX()  << ","
		 << readings->getSensorDY()  << ","
		 << readings->getSensorTh()  << ","
//		 << readings->getLocalPose().getTh()  << ","
//		 << readings->getPoseTaken().getTh()  << ","
//		 << readings->getTimeTaken().mSecSince()  << ","
		 << readings->getCounterTaken()  //<< ","
//		 << readings->getSensorPosition()  << ","
//		 << readings->getEncoderPoseTaken()
		 << "\n";
}

void Logger::close()
{
	fout.close();
}

void Logger::run()
{

}
