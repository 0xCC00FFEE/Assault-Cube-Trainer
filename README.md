# Assault Cube Trainer

This is a trainer for the open source game Assault Cube. 
The main purpose of publishing that project is to share some Reverse Code Engineering/C++ knowledge, and optimizing games protection mechanisms.

The currently supported features of the trainer include:
- Infinite Health
- Infinite Ammo
- Recoil disabled
- Automatic shooting for pistols
- Rapid Fire Enabled

The project also dissects the player's entity data structure, which helps understanding more about RCE, pointers and memory.

Please note that this is an external hack, which means all the features were implemented in a DLL file that should be injected into the game's process in order to be able to activate the available cheats. You can find the compiled DLL inside the DllProject/ directory of the current repository. 
You can use my Dll injector, or any other injector of choice to inject the DLL into the game's process.

Assault Cube's Official Website:
https://assault.cubers.net/

---------------------------------------------------------------