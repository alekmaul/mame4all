=============================================================================
MAME4ALL OPENDINGUX 1.1 (Mai 26, 2013) by Alekmaul (alekmaul@portabledev.com)
=============================================================================
Official web page for news, additional information :
http://www.portabledev.com/pages/dingoo/jeuxdev.-perso/mame4all_od.php

Based on :
----------

=============================================================
MAME4ALL 2.7 (April 30, 2012) by Franxis (franxism@gmail.com)
=============================================================


1. INTRODUCTION
---------------

It is a port of MAME 0.37b5 emulator by Nicola Salmoria for GP2X, WIZ and CAANOO portable consoles.
To see MAME license see the end of this document (chapter 18).
It emulates all arcade games supported by original MAME 0.37b5 plus some additional
games from newer MAME versions.

This version emulates 2270 different romsets.

Official web page for news, additional information and donations:
http://franxis.zxq.net/


2. CONTROLS
-----------

The emulator controls are the next ones:
- Joystick: Movement in pad, mouse and analog control of the four players. 
- Buttons B,X,A,Y,L,R: Buttons A,B,C,D,E,F.
- Buttons SELECT|I: Insert credits (UP+SELECT = 2P credits, RIGHT+SELECT = 3P credits, DOWN+SELECT = 4P credits).
- Buttons START|MENU|II: start (UP+START = 2P start, RIGHT+START = 3P start, DOWN+START = 4P start).
- Buttons (L+R)|LOCK: Pause.
- Buttons (L+R+START)|HOME: Exit to selection menu to select another game.
- Buttons L+(START|MENU|I): Show profiler.
- Buttons R+(SELECT|I): Show FPS.
- Buttons (START+SELECT)|(I+II): Access to the MAME menu.
- Buttons VolUp and VolDown: Increase / decrease sound volume.
- USB Joypads: Up to four USB Joypads are supported for multiplayer in the GP2X.

In the rotated video modes the following additional controls are available:
- GP2X: Buttons VolUp and VolDown: Buttons A and B.
- GP2X: Buttons VolUp and VolDown + START: Increase / decrease sound volume.
- WIZ: Button MENU: Button A.
- CAANOO: Buttons I and II: Buttons A and B.

NOTE: To type OK when MAME requires it, press LEFT and then RIGHT.

3. EMULATION OPTIONS
--------------------

After selecting a game on the list next configuration options are available:

- Clock:
In the GP2X: 66 to 300 MHz options are available. Performance of the emulator is better with bigger values.
200 MHz is the standard value. 250 MHz seems to run ok with all GP2X consoles (but batteries duration is reduced).
Use greater values at your own risk!.
In the WIZ: 200 to 900 MHz options are available. Performance of the emulator is better with bigger values.
533 MHz is the standard value. 700 MHz seems to run ok with all WIZ consoles (but batteries duration is reduced).
Use greater values at your own risk!.

- Video Depth:
It allows to configure the video depth. The possible values are:
Auto: The emulator uses the most suitable video depth.
8 bit: The emulator is forced to use 8 bit color.
16 bit: The emulator uses 16 bit color.

- Video Aspect:
The video aspect is configured with the following combination of options:
Normal: Nominal video resolution (320x240).
Scale: The game window is scaled to fill the entire screen.
4:3: The 4:3 aspect ratio is forced.
Border: A border is added to be able to see the entire screen in the TV.
Rotate: The window is rotated to be played with the GP2X.
TATE: The window is rotated to be played in a rotated TV.
Diagonal Fix: WIZ video diagonal line is fixed but with impact in the performance.

- Video Sync:
Normal: Single buffer and dirty buffer is used.
VSync: VSync activated.
DblBuf: Double buffer without dirty buffer.
OFF: No video synchronization, use manual frameskip.

- Frame-Skip:
Auto: The frameskip is adjusted automatically in real time.
0 to 11: The frameskip is manually adjusted. The selected frames each 12 are skipped.

- Sound:
The sound options are the following ones:
ON: The sound is activated. 11, 15, 22, 33 and 44 KHz sound mixing rates are available in both mono and stereo.
OFF: The sound is disabled.
Fast sound: Some tweaks are done to improve the performance (but with a bit worse sound quality).

- CPU Clock:
The clock of the CPUs can be adjusted from 50% to 200%. The nominal value is 100% and the CPU is emulated accurately.
The clock can be safely underclocked to about 80% to gain performance in almost all games (be careful because some
games could not run correctly). Use lower values to get more performance but probably several more games would not
run correctly. Also the clock can be overclocked up to 200% to avoid slowdowns in some Neo·Geo games.

- Audio Clock:
The clock of the audio CPUs can also be adjusted from 50% to 200%. The nominal value is 100%. It can be underclocked
and overclocked as well.

- CPU ASM Cores:
The following combinations of CPU ASM cores are available:
Cyclone: It is the M68000 ASM CPU core, faster than the C one. Enable it because it seems to have perfect compatibility.
DrZ80: It is the Z80 ASM CPU core, faster than the C one. The compatibility is very limited, use it if the desired game runs ok.
Cyclone+DrZ80: Both ASM CPU cores are activated. More faster.
Drz80(snd): DrZ80 is only used for the sound CPUs.
Cyclone+DrZ80(snd): Both ASM cores are activated, DRz80 only for the sound CPUs.
None: None of the ASM CPU cores are activated. Slower.
Note that this configuration is automatically adjusted in each game according to an internal compatibility list.

- RAM Tweaks:
ON: The RAM Tweaks are activated to gain some more performance. Some consoles have problems with them. Try it.
OFF: The RAM Tweaks are disabled to ensure the emulator run on every console.

- Cheats:
ON: The cheats are enabled. To access in game press SELECT+START and enter the "Cheats" menu.
OFF: The cheats are disabled.
Note: The high scores are not saved if cheats are enabled!!!.

- Volume:
Quiet: Quiet default volume.
Low: Low default volume.
Medium: Medium default volume.
Maximum: Maximum default volume.
Note that the default volume can be changed using the console volume controls.

- Auto-Fire:
To access the auto-fire configuration, during game press SELECT+START and enter the "Auto-Fire" menu.

- Press B to play the selected game or X to go back to game selection menu.

- In the game selection menu if L+R are pressed simultaneously, the frontend is ended.


4. INSTALLATION
---------------

mame.gpe    -> Frontend to select games
mame_vm.gpe -> Experimental launcher (it uses additional 64 MB of virtual memory) (*)
mame.png    -> Icon for the frontend
mame        -> MAME emulator
neomame     -> MAME emulator (Neo·Geo games)
mmuhack.o   -> MMU Hack Kernel Module (GP2X)
warm_2.6.24.ko -> MMU Hack Kernel Module (WIZ)
autorun.gpu -> Script for the auto-run
cheat.dat   -> Cheats definition file
hiscore.dat -> High Scores definition file
artwork/    -> Artwork directory
cfg/        -> MAME configuration files directory
frontend/   -> Frontend configuration files
hi/         -> High Scores directory
inp/        -> Game recordings directory
memcard/    -> Memory card files directory
nvram/      -> NVRAM files directory
roms/       -> ROMs directory
samples/    -> Samples directory
skins/      -> Frontend skins directory
snap/       -> Screen snapshots directory
sta/        -> Save states directory

(*) mame_vm.gpe uses 64 MB swap file in the root of SD card to be able to play the biggest games.
It is not recommended due to the maximum number of SD card writings (about 500.000). It will reduce
the life of your SD card. Use at your own risk!

5. SUPPORTED GAMES
------------------

There are 2270 different supported romsets. For more details, see "gamelist.txt" file.
Games have to be copied into the roms/ folder on the SD card.


6. ROM NAMES
------------

Folder names or ZIP file names are listed on "gamelist.txt" file.
Romsets have to be MAME 0.37b5 ones (July 2000).
Additionaly there are additional romsets from newer MAME versions.

Please use "clrmame.dat" file to convert romsets from other MAME versions to the ones used by
this version for GP2X, using ClrMAME Pro utility, available in next webpage:

http://mamedev.emulab.it/clrmamepro/

NOTE: File and directory names in Linux are case-sensitive. Put all file and directory names
using low case!.


7. SOUND SAMPLES
----------------

The sound samples are used to get complete sound in some of the oldest games.
They are placed into the 'samples' directory compressed into ZIP files.
The directory and the ZIP files are named using low case!.

The sound samples collection can be downloaded in the following link:
http://dl.openhandhelds.org/cgi-bin/gp2x.cgi?0,0,0,0,5,2511

You can also use "clrmame.dat" file with ClrMAME Pro utility to get the samples pack.


8. ARTWORK
----------

Artwork is used to improve the visualization for some of the oldest games. Download it here:
http://dl.openhandhelds.org/cgi-bin/gp2x.cgi?0,0,0,0,5,2512


9. ORIGINAL CREDITS
-------------------

- MAME 0.37b5 original version by Nicola Salmoria and the MAME Team (http://www.mame.net).

- Z80 emulator Copyright (c) 1998 Juergen Buchmueller, all rights reserved.

- M6502 emulator Copyright (c) 1998 Juergen Buchmueller, all rights reserved.

- Hu6280 Copyright (c) 1999 Bryan McPhail, mish@tendril.force9.net

- I86 emulator by David Hedley, modified by Fabrice Frances (frances@ensica.fr)

- M6809 emulator by John Butler, based on L.C. Benschop's 6809 Simulator V09.

- M6808 based on L.C. Benschop's 6809 Simulator V09.

- M68000 emulator Copyright 1999 Karl Stenerud.  All rights reserved.

- 80x86 M68000 emulator Copyright 1998, Mike Coates, Darren Olafson.

- 8039 emulator by Mirko Buffoni, based on 8048 emulator by Dan Boris.

- T-11 emulator Copyright (C) Aaron Giles 1998

- TMS34010 emulator by Alex Pasadyn and Zsolt Vasvari.

- TMS9900 emulator by Andy Jones, based on original code by Ton Brouwer.

- Cinematronics CPU emulator by Jeff Mitchell, Zonn Moore, Neil Bradley.

- Atari AVG/DVG emulation based on VECSIM by Hedley Rainnie, Eric Smith and Al Kossow.

- TMS5220 emulator by Frank Palazzolo.

- AY-3-8910 emulation based on various code snippets by Ville Hallik, Michael Cuddy,
  Tatsuyuki Satoh, Fabrice Frances, Nicola Salmoria.

- YM-2203, YM-2151, YM3812 emulation by Tatsuyuki Satoh.

- POKEY emulator by Ron Fries (rfries@aol.com). Many thanks to Eric Smith, Hedley Rainnie and Sean Trowbridge.

- NES sound hardware info by Jeremy Chadwick and Hedley Rainne.

- YM2610 emulation by Hiromitsu Shioya.


10. PORT CREDITS
----------------

- Port to GP2X, WIZ and CAANOO by Franxis (franxism@gmail.com) based on source code MAME 0.37b5 (dated on july 2000).

- TheGrimReaper (m_acky@hotmail.com) has colaborated with a lot of
  things since GP32 MAME 1.3, i.e. Vector graphics support, high scores,
  general frontend, frontend improvements, bugfixes, etc. Thank you!!!
  
- Pepe_Faruk (joserod@ya.com) has colaborated with new screen centering
  code. Also he has added some new supported games. Thank you!!!

- Reesy (drsms_reesy@yahoo.co.uk) has developed DrZ80 (Z80 ASM ARM core) and has helped
  a lot with core integration into MAME. He has also done several fixes to the Cyclone core.
  You are the best!!!
  
- Flubba (flubba@i-solutions.se) has done some optimizations and improvements to the DrZ80
  core. Thank you!!!

- Dave (dev@finalburn.com) has developed Cyclone (M68000 ASM ARM core). Big thanks to him.

- Notaz (notasas@gmail.com) have fixed some bugs in the Cyclone source code. He has also
  contributed with several useful code to the GP2X scene. Thanks!!!
  http://uosis.mif.vu.lt/~grig2790/Cyclone/
  http://notaz.gp2x.de/

- Chui (sdl_gp32@yahoo.es) has developed MAME4ALL, the MAME GP2X port for Dreamcast, Windows
  and Linux. Also he has done several optimizations aplicable to all targets.
  http://chui.dcemu.co.uk/mame4all.html

- Slaanesh (astaude@hotmail.com) has continued my work on MAME GP32 and he has done several
  improvements aplicable to all targets.
  http://users.bigpond.net.au/mame/gp32/

- GnoStiC (mustafa.tufan@gmail.com) has done the USB Joypad support using the library created
  by Puck2099.
  
- Sean Poyser (seanpoyser@gmail.com) has done interesting improvements in some drivers.
  For example the use of diagonals in Q*Bert or the use of the shoulder buttons in Tron.

- TTYman (ttyman@free.fr) has done the MAME GP2X port for the PSP portable console.
  http://ttyman.free.fr/

- Headoverheels (davega@euskalnet.net) has added some new games to MAME4ALL, and he has also
  done some optimizations to existing games.


11. DEVELOPMENT
---------------

April 30, 2012:
- Version 2.7: Improved MMU-HACK, solved CAANOO lock-ups, added assembler optimizations.

April 9, 2011:
- Version 2.6. Added WIZ TV-Out support. Added CAANOO port.

March 9, 2010:
- Version 2.5. Version for the RetroMadrid 2010 fair.

August 5, 2009:
- Version 2.4. Improvements in the GP2X WIZ port.

July 11, 2009:
- Version 2.3. Improvements in the GP2X WIZ port. More games added. Fixed controls.

June 13, 2009:
- Version 2.2. New timers system based on integers (instead of floats).

May 31, 2009:
- Version 2.1. Improvements in the GP2X WIZ port. Added Discrete Sound System.

May 16, 2009:
- Version 2.0. GP2X WIZ port added.

December 28, 2008:
- Version 1.6. Optimizations, executable for Neo·Geo games, ASM cores selected automatically.

November 15, 2008:
- Version 1.5. Optimizations and bug-fixes.

September 04, 2008:
- Version 1.4. New games added.

April 05, 2008:
- Version 1.3. Cheats and auto-fire.

March 18, 2008:
- Version 1.2. More bug fix.

March 16, 2008:
- Version 1.1. High scores and bug fix.

March 11, 2008:
- Version 1.0. First version.

Developed with:
- DevKitGP2X rc2 (http://sourceforge.net/project/showfiles.php?group_id=114505)
- GP2X minimal library SDK v0.A by Rlyeh (http://www.retrodev.info/)
- GpBinConv by Aquafish (www.multimania.com/illusionstudio/aquafish/)


12. KNOWN PROBLEMS
------------------

- Not perfect sound or incomplete in some games.

- Slow playability in modern games.

- Memory leaks. In case of errors, reset GP2X and try again please ;-).


13. TO BE IMPROVED
------------------

- Use second GP2X processor.

- Future HH development kit without Linux?.

- Improve sound.

- Improve speed.

- Update romsets to actual MAME ones, or update MAME version to actual one.

- Add support to more games.


14. THANKS TO
-------------

- Unai: Thanks Unai due to a lot of hours helping me with MAME, optimizations...

- Talfi: Friend who gives me the webspace for my MAME port for GP32.

- Chicho: Great friend with blind faith on my MAME port...

- Anarchy (gp32spain.com): Thanks, Anarchy, for the GP2X development unit you sent me to port MAME.

- EvilDragon (gp32x.de): Thanks, EvilDragon, for the WIZ development unit you sent me to port MAME.

- Gamepark Holdings: Thank you people for releasing GP2X console, as well as providing some GP2X development
units for programmers some weeks before GP2X official launch. One of them was finally mine through Anarchy
mediation (gp32spain.com). Also for releasing the WIZ console, as well as for sending me the development
unit through EvilDragon mediation (gp32x.de). Also thanks for sending the CAANOO.

- Rlyeh: Master of GP32 emulation... Creator of GP2X minimal library SDK.

- Hermes PS2R, god_at_hell: Thanks for the CpuCtrl library (to modify GP2X clock frequency and RAM timings).

- Kounch: Information about the TV-Out in GP2X, MMU Hack, etc. Thank you!.

- Antiriad: Thank you for the excelent artwork for GP32 version ;-).

- Zenzuke, Chipan, Dokesman, Enkonsierto, Quest, Sttraping and Sike for the frontend skins for GP2X and WIZ :-).

- Zaq121: Author of the alternative frontend for MAME GP32 and GP2X (FEMAMEGP2X) :).

- Baktery: Sound advices.

- Groepaz: More sound advices.

- Woogal: Big help with the games selection frontend... 

- Alien8: Programmer of GpMameLauncher utility.

- LDChen: Help with some ASM code for the vector graphics library.

- D_Skywalk : Multipac runs now due his help. He teached me a lot of GP32 programming.

- Locke : Advices, beta-testing...

- Sssuco, [MaD]: Thank you por providing me romsets of old MAME version, as well as DATs.

- Ron: Really mad man, but good old one ;-). MadriDC organizer.

- Ilarri: Cheers ;-).

- Fox68k: Good advices.

- DaveC: Searching bugs in my MAME port. Thanks ;-).

- Creature XL: Some testing with ASM for the video output.

- EvilDragon: Creator of MAMEGP-COPIER utility. Also moderator & news-poster of gp32x.com

- WarmFluffyUK: MAME GP32 Compatibility List: http://www.berzerk.co.uk/gp32/

- BobBorakovitz, frolik, Alan: GameProbe32 webpage: http://gameprobe32.blogspot.com/

- People on IRC-Hispano #retrodev beta-testing: Xenon, Mortimor, Nestruo, Dj_Syto, K-Teto,
Enkonsierto, Soup, joanvr, amkan, etc.

- People on EFNet IRC #gp2xdev: DJWillis, etc.

- More beta-testing: nullEX, Propeller, Ozius, etc.

- Richard Weeks: Thank you for a new interview about the MAME GP2X port:
http://mygp2x.com/blog_comment.asp?bi=246&m=12&y=2005&d=1&s=

- Keith: Thank you for a new interview: http://www.emulation64.com/spotlights/22/

- Hooka: Thank you for a great interview: http://www3.telus.net/public/hooka/

- Mark Rowley: Thank you for the interview for GamePark Magazine 7:
http://www.gp32x.de/cgi-bin/cfiles.cgi?0,0,0,0,2,630

- Gladiator: Thank you for the spanish interview, as well as the great GP32 article: 
http://www.viciojuegos.com/reportaje.jsp?idReportaje=131

- José Luis Martín Sánchez: Thanks for university project direction (the MAME port for the GP32).

- Puck2099: Thank you for all the information and the Pico library for the WIZ console.

- Orkie: Thank you for all the help with the WIZ and his Castor library.

- Squidge and NK: Thanks for the MMU hack library for the GP2X console.

- Exophase: Thank you for the MMU hack library for the WIZ console.

- Pickle and KungPhoo: Thank you for all the information about the sound in the WIZ console.

- Notaz: Thanks for the MMU Hack kernel module for the WIZ console.

- Zenzuke: Thanks for the WIZ artwork.

- Simon: Thanks for the CAANOO port.

- Thanks to all paypal donators: EvilDragon (www.gp32x.de), Federico Mazza, Nandove, Videogame Stuff, Denis Evans, Ricardo Cabello,
Elías Ballesteros, J.Antonio Serralvo Martín, bagmouse7, Suj, funkyferdy, Gieese, Vincent Cleaver, William Burnett, Bleeg,
Martin Dolphin, Ilarri, Glen Stones, Dr.Konami, Augusto Carlos Pérez Arriaza, Charles Box, Borochi, Kayday, George Tavoulareas,
Timofonic, Fabrice Canava, Redox, Javitotrader, remowilliams, Scott Contrera, Jinhyun Seo, Craigix (www.gp2x.co.uk), Shane Monroe,
Simon Beattie, Stefan Braunstein, DaveC, Colin Bradshaw, Dana Rodolico, Revod, Michael Evers, Riccardo Pizzi, Fosfy45, Dj Syto,
Rob Pittman, Stefan Mueller, Musa, Unai, Sascha Reuter, Globalwide Technologies Limited, Juan Rivera-Novoa, Mark Carin, SBock,
Julio Catalina Piedrahita, techFreak (www.gp2xtr.com), Darius Hardy, Charles Andre, Matt Brimelow, McOskar, Daniel PP Saurborn,
Picayuco, Kojote (www.pdroms.com), Knoxximus, Tony Watterson, Matthew Forman, naples39, NEO (www.elotrolado.net), Patrick Mettes,
Angel Molero Grueso, Lubidog, Smiths (www.emuholic.com), retromander, Ruben Villar, Snakefd99cb, Harkaitz, BZFrank, Sang Kim,
phoda, Caesaris, Furanchu, Selcuk Cegit, K-Navis, Estaño, Jeff Hong, Jasmot, Igboo, Sergio Onorati, Julien Perret, Cheap Impostor,
Gianluca Lazzari, Niche IP Solutions, Jason, Thomas Seban, Miq01, Paul Carter, Freddy Deniau, Mustafa Beciragic, Ian Rawlings,
Domenico Calcagno, pongplaya, Aruseman, Anarchy (www.hardcore-gamer.net), www.gp32spain.com, www.gp2xspain.com, Darkman, Chaos Engineer,
Ian Buxton, Martin M Pedersen, Philip Trottman, Gary Ross, Fat Agnus, Austin Holdsworth, Paul Johns, Gaterooze, Elizabeth Burrow, Godmil,
rooster, Dark_Warlock, Danilo Gadza, Gadget, Hando (www.gp32x.com), Gary Miller, AOJ, John Huxley, X-Code BirraMaster, Jorge Gavela Alvarez,
Halo9, b._.o._.b, James Perry, Pughead, Beb, Luis Fernando González Barreto, Frank Bukor, Oliver Lewandowski, Alberto Martin Martin,
Holger Lenz, Carlos Arozarena, Hobbes, Propeller, www.gp2xstore.com, Marc Pallante, David Cifre García, Jose Zanni, Jorge Salvador Corell,
Ralph Blazyczek, Tilman Oestereich, Steve Sims, Craig Ritchie, Anonymous, Malatesta, fmayosi, David Cifre García, Mauro González, Pedator,
José Rodríguez Reyes, Jack Wielink, amzg, Emilio Hernández, Rob ter Linden (www.gpark.nl), fpomata, Xenon, Abdullah Soylu, Stephan Bockhorst.


15. INTERESTING WEBPAGES ABOUT MAME
-----------------------------------

- http://www.mame.net/
- http://www.mameworld.net/
- http://www.marcianitos.org/


16. SOME OTHER INTERESTING WEBPAGES
-----------------------------------

- http://www.gp32x.com
- http://www.gp32spain.com
- http://www.emulatronia.com
- http://www.emulation64.com


17. SKINS
---------

The frontend graphic skin used in the emulator is located in next files:
skins/gp2xsplash.bmp   -> Intro screen for the GP2X (by Danibat).
skins/gp2xmenu.bmp     -> Screen to select games and options in the GP2X (by Danibat).
skins/wizsplash.bmp    -> Intro screen for the WIZ (by Zenzuke).
skins/wizmenu.bmp      -> Screen to select games and options in the WIZ (by Zenzuke).

Bitmaps have to be 320x240 pixels - 256 colors (8 bit). In the game selection screen, the
text is drawn with color 255 of the palette with a right-under border with color 0.

In skins/others/ folder there are other alternative skins, done by next people:
Zenzuke, Chipan, Dokesman, Enkonsierto, Quest, Sttraping, Sike, Danibat, Pedator,
Antiriad and Slaanesh. Thanks to everybody !.


18. MAME LICENSE
----------------

http://www.mame.net
http://www.mamedev.com

Copyright © 1997-2009, Nicola Salmoria and the MAME team. All rights reserved. 

Redistribution and use of this code or any derivative works are permitted provided
that the following conditions are met: 

* Redistributions may not be sold, nor may they be used in a commercial product or activity. 

* Redistributions that are modified from the original source must include the complete source
code, including the source code for all components used by a binary built from the modified
sources. However, as a special exception, the source code distributed need not include 
anything that is normally distributed (in either source or binary form) with the major 
components (compiler, kernel, and so on) of the operating system on which the executable
runs, unless that component itself accompanies the executable. 

* Redistributions must reproduce the above copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
