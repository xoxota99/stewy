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

void processCommands();

int shell_reader(char * data);

void shell_writer(char data);

int handleSet(int argc, char** argv);
int handleMSet(int argc, char** argv);
int handleHelp(int argc, char** argv);
int handleSetAll(int argc, char** argv);
int handleMSetAll(int argc, char** argv);
int handleDump(int argc, char** argv);
int handleReset(int argc, char** argv);
int handleDemo(int argc, char** argv);
int handleMoveTo(int argc, char** argv);
int handleLog(int argc, char** argv);

//===

const Command commands[] = {
  {handleHelp,    "help"},
  {handleHelp,    "?"},
  {handleSet,     "set"},
  {handleMSet,    "mset"},
  {handleSetAll,  "setall"},
  {handleMSetAll, "msetall"},
  {handleDump,    "dump"},
  {handleReset,   "reset"},
  {handleMoveTo,  "moveto"},
  {handleDemo,    "demo"},
  {handleLog,     "log"}
};

#endif //ENABLE_SERIAL_COMMANDS
#endif //__STU_COMMANDS_H__
