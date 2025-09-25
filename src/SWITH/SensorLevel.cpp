#include "Config.h"
#include <Arduino.h>
#include "SensorLevel.h"
SensorLevel::SensorLevel(String strN):LFsaAppl(strN) { }

void SensorLevel::run() { LevelView(); }

void SensorLevel::LevelView()
{
  levelStatus1 = digitalRead(gpioLevel1);
  levelStatus2 = digitalRead(gpioLevel2);
  levelStatus3 = digitalRead(gpioLevel3);
  if (!levelStatus1&&levelStatus2&&levelStatus3) fLevel=30;
  else if (levelStatus1&&!levelStatus2) fLevel=60;
  else if (!levelStatus3) fLevel=90;
  else if (levelStatus1&&levelStatus2&&levelStatus3) fLevel=0;
  else fLevel=-1;
}
