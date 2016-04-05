Building PCSpim
===========================
In order to build PCSpim, you need Microsoft VC++ 4.1, or later.
Use a later compiler if you do not wish for Win3.x compatibility.
I typically build PCSpim with Visual Studio version 6, though I have build it
with VS.NET (Visual Studio version 7) [the solution files are PCSpim.VC7].

This source must be arranged as follows:

spim\
	src\		(the core spim source)
	PCSpim\		(the Windows source)

If you edit the paths of the project files, you may change this organization.

Once you have the source, load PCSpim.dsw into DevStudio, and build.


Registry Keys
=============
PCSpim uses the registry for storing its configuration information:

"HKEY_CURRENT_USER\Software\LarusStone\PCSpim\Settings\"

At setup time, a set of default configuration values are stored
in the registry at:

"HKEY_LOCAL_MACHINE\SOFTWARE\LarusStone\PCSpim\Default Settings\"

Registry Values
===============
These are the registry values that PCSpim uses under the "Settings" Key.
Note:  In the PCSpim.INI file in Win32s, these are value names in the "Settings" section.

"Bare Machine"
	BOOLEAN: 0 | 1
	Default: 0

"Accept Pseudo Instructions"
	BOOLEAN: 0 | 1
	Default: 1

"Load Trap Handler"
	BOOLEAN: 0 | 1
	Default: 1

"Mapped IO"
	BOOLEAN: 0 | 1
	Default: 1

"Quiet"
	BOOLEAN: 0 | 1
	Default: 0

"Trap File"
	String
	Default: $(InstallDir)\trap.han(dler)

