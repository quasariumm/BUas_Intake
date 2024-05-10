# Sorry, we're broke
The intake game for BUas. The theme was 'collect'.

## Installation
You can install the game from the releases or clone and build it yourself.
When cloning, please use the following command to make sure that the SFML submodule will be installed as well: `git clone --recursive https://github.com/quasariumm/BUas_Intake.git`  
If you have already cloned it and are not seeing the SFML repository, you can update the submodules with `git submodules update --init --recursive`  

### Visual Studio
To build the game, follow these steps:
- Open the folder with Visual Studio and make sure it recognises the CMake configuration. Also, make sure you have the required modules installed to build with CMake.
- Select your preferred build configuration.
- Select "SorryWereBroke.exe" as the target.
- Build.

### Terminal/Command prompt
To build the game, do the following:
- Install cmake (if you haven't already)
- Make a `build` directory in the project directory (`cd` to the cloned folder and run `mkdir build` and `cd build`)
- Run `cmake ..`
- Finally, build the game using `cmake --build .`
- You can now run the game using `bin/SorryWereBroke` (on Windows `bin/SorryWereBroke.exe`)

## Controls
You can change the controls in the settings in the main menu. Note that you can't go back to the main menu once you have clicked _Play_. The default controls are:
- `R`: Rotate counterclockwise (in placement mode)
- `T`: Rotate clockwise (in placement mode)
- Hold `Left Shift`: Rotate slower (in placement mode)
- Hold `Left control`: Rotate faster (in placement mode)
- `Escape`: Cancel placement (puts the item back in inventory)
- `F`: Move (in edit mode)
- `G`: Delete (in edit mode)

NOTE: Moue buttons are not supported. If you do want to use the mouse, please rebind that mouse button to a supported key (See [https://www.sfml-dev.org/documentation/2.6.1/structsf_1_1Keyboard_1_1Scan.php](https://www.sfml-dev.org/documentation/2.6.1/structsf_1_1Keyboard_1_1Scan.php) for the supported keys).

## Specific file extensions
### .ql
These are the level files (Quasar Level). These files have the following headers:

#### [Tilemap]
The tilemap for the level in base 36. The value denotes an entry in a texture atlas (You count from left to right, top to bottom).   
`0` is empty  
`1` is the first wall  
`h` is the first pipe  
`n` is the first prop  
You can find the tilemaps in `res/sprites/tilemap[...].png`.  
Note that the level is 16 by 16 _units_ and that the tiles on the edges count as the walls. The tilemap gets drawn with an offset of `(-0.5 unit, -0.5 unit)` so that the walls are only half visible. 

#### [Inventory]
The items that the player gets. Each item is written on one line using the format `(ITEMID,COUNT)` where a bounce pad has ITEMID `0` and a booster ITEMID `1`

#### [MoneyBags]
Each following line represents a money bag. The format for the lines is `(X,Y)` where both `X` and `Y` can be floating point numbers.

#### [MoneyBagsNeeded]
On the line below you can put the required money bags.

#### [BouncyObjects]
Each following line represents a BouncyObject, a block that the ball bounces off. Every line has the format `(STARTX,STARTY) (ENDX,ENDY) COR (ORX,ORY)`.  
The start and end coordinates are in _units_. The BouncyObject spans from the top-left corner of the start tile to the bottom-right corner of the end tile.  
`COR` stands for _Coefficient of Restitution_ and is the factor that the speed of the ball gets multiplied by when it bounces off the object.  
`(ORX,ORY)` is the orientation of the object (always `(1,0)`).

### .qd
These are the dialogue files (Quasar Dialogue). To make a dialogue you can use the following commands, each on its own line:
- `SAY`: Shows text on the text bubble at the bottom of the screen. It uses a typewriter effect to make the text appear. This command is followed by a message (string) with quotation marks (`"<MESSAGE>"`).
- `TEXT`: Shows text in the center of the screen. This command is followed by a string with quotation marks.
- `WAIT`: Waits for the specified amount of seconds. This command is followed by a number that can be a floating point number.
- `ENABLE_DIALOGUE`: Enables the dialogue and makes it so that the text bubble is drawn on the screen. This command is run by default at the beginning of every dialogue (so no need to put it at the start).
- `CLEAR_DIALOGUE`: Disables the dialogue and makes it so that the text bubble is not drawn on the screen. You do have to put this command at the end of the dialogue.
- `CLEAR_TEXT`: Sets the string of the text to `""`, which makes the text invisible.

### .qconf
This is the config file for the game (Quasar CONFig).  
For the controls, please use the sf::Keyboard::Scan from [https://www.sfml-dev.org/documentation/2.6.1/structsf_1_1Keyboard_1_1Scan.php](https://www.sfml-dev.org/documentation/2.6.1/structsf_1_1Keyboard_1_1Scan.php)  
NOTE: Mouse buttons are not supported.

## Sources
CMakeLists.txt:
* [https://github.com/SFML/cmake-sfml-project/blob/master/CMakeLists.txt](https://github.com/SFML/cmake-sfml-project/blob/master/CMakeLists.txt)
* [https://github.com/SFML/SFML/issues/2289](https://github.com/SFML/SFML/issues/2289)

BouncePad sprite: Using [https://www.svgrepo.com/svg/324985/bounce-right](https://www.svgrepo.com/svg/324985/bounce-right)
Money bag sprite: [https://www.svgrepo.com/svg/234197/money-bag](https://www.svgrepo.com/svg/234197/money-bag) (Edited)
Booster sprite: Using [https://www.svgrepo.com/svg/470503/lightning](https://www.svgrepo.com/svg/470503/lightning) and [https://www.svgrepo.com/svg/533612/arrow-narrow-up](https://www.svgrepo.com/svg/533612/arrow-narrow-up)
Dialogue bar: [https://www.svgrepo.com/svg/356715/businessman](https://www.svgrepo.com/svg/356715/businessman) (Edited)
Background: [https://www.svgrepo.com/svg/338091/erlenmeyer-laboratory-lab-experiment-flask-science-medical](https://www.svgrepo.com/svg/338091/erlenmeyer-laboratory-lab-experiment-flask-science-medical) (Edited)

Key sound: [https://kbs.im/](https://kbs.im/) (Gateron Black Ink)
Wall bounce sound: [https://samplefocus.com/samples/soft-kick-f-159](https://samplefocus.com/samples/soft-kick-f-159) (Changed pitch and applied EQ)
All other sounds are made with my Arturia MicroFreak.

Font: [https://fonts.google.com/noto/specimen/Noto+Sans](https://fonts.google.com/noto/specimen/Noto+Sans)
