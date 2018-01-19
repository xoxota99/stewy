#pragma once
#ifndef __STU_COMMANDS_H__
#define __STU_COMMANDS_H__
/*
   6dof-stewduino
   Copyright (C) 2018  Philippe Desrosiers

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef ENABLE_SERIAL_COMMANDS
#include <Shell.h>  //https://github.com/geekfactory/Shell

typedef struct shell_command_entry Command;

//void setupCommandLine(int bps = 9600);
void processCommands();
#pragma message ("shell_reader h")
int shell_reader(char * data);

void shell_writer(char data);

int handleSet(int argc, char** argv);
int handleHelp(int argc, char** argv);
int handleSetAll(int argc, char** argv);
int handleDump(int argc, char** argv);
int handleReset(int argc, char** argv);
int handleDemo(int argc, char** argv);
int handleMoveTo(int argc, char** argv);

//===

const Command commands[] = {
  {handleSet,     "set"},
  {handleSetAll,  "setall"},
  {handleDump,    "dump"},
  {handleReset,   "reset"},
  {handleMoveTo,  "moveTo"},
  {handleDemo,    "demo"},
  {handleHelp,    "help"},
  {handleHelp,    "?"}
};

#endif //ENABLE_SERIAL_COMMANDS
#endif //__STU_COMMANDS_H__
