#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#ifdef ENABLE_SERIAL_COMMANDS
#include <Shell.h>

typedef struct shell_command_entry Command;

//void setupCommandLine(int bps = 9600);
void processCommands();
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
#endif //__COMMANDS_H__
