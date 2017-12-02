#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include "Configuration.h"
#include "CodeLabels.h"
#include "C64KeyMaps.h"
#include "CD4051.h"
#include "RgbLed.h"
#include "KeyMatrix.h"

//Note: (Adam) Defines size of the project's keyboard matrix
#define ROW_COUNT 8
#define COLUMN_COUNT 8

//Note: (Adam) Enables additional serial output and debug code
#define _DEBUG

#endif