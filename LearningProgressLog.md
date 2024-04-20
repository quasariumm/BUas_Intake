# Learning/Progress Log

## Format
Normal text is just a normal log entry.  
_Italic_ text can give some context or indicate names.

## 2024-02-05
Struggles with letting SFML work with windows using cmake. Got a lot of errors. Realised that I forgot to link _sfml-graphics_.

## 2024-02-06
Started on the physics header file. Got to know some physics behind the bouncing of a ball.

## 2024-02-13
Brainstormed for the collision system. Went through a couple of iterations:
1. For `x` points on the ball, determine if the points are in between the sides by per side calculating the tangent and checking if the point is 'inside' the rectangle. (Trown away due to immense complexity)
2. For some number of points on each side, check the distance to the midpoint of the ball. If the distance is smaller than the radius of the ball, the ball collides with the object.
3. (Best) For every side, derive a formula of the format $$ax+by+c=0$$ and calculate the distance to the midpoint of the ball with the formula: $$\frac{|ax_0+by_0+c|}{\sqrt{a^2+b^2}}$$

## 2024-02-14
Finished the first version of the physics and collision system. Got to work on the drawing of the ball.  Got to know how the file system works and how to make prites and load textures from files.  

## 2024-02-15
Made a system to check collision for every object in the level. I made a `for (PhysicsObjects::Bouncyobject& object : bouncyObjectList)` loop, but forgot to use references, so I accidentally worked with copies.  Got some problems with the bouncing code. I realised that the documentation was incorrect. It stated _'this amounts to a clockwise rotation by phi'_, when in fact it was counterclockwise. What inevitably fixed the issue was converting my physics to 'real world' (so (0,1) is up, because in SFML (0,0) is in the top-left corner) and when updating the position using `+=` with x and `-=` with y to convert real world to SFML.

## 2024-02-17
Got to work on a tilemap loading system. I seperated the objects in both 'walls and props' (background objects) and 'pipes' (foreground). Also made the file map width and height 18, whilst the window is divided up into 17x17 'units'. _The walls take up 0.5 unit on either side of the screen, to maximise the available screen space._ It didn't work at first, later found out it was using the wrong offset.  
_Also, I moved the BouncyObject list to be handled in its own class in `include/level.hpp`._

## 2024-04-07
Realised that I've not updated this for very long.  
Learned that I have to commit more frequently, because until now I only commited once a whole feature was done instead of commiting per stable piece.  
Got to work on the placement of the bounce pads. All worked well, until it broke for some reason. The rotation stoped working. Fixed this by making a local variable global.  
Got the placement working fine, then the rotation was a fixed value all of the times. Still have no clue where that came from, because it has worked once.

## 2024-04-08
Fixed the rotation issue. Turned out that the constructor of `UserObjects::EditableObject` had the following order `..., const float unitSize, const float newRotation = 0, ...` and I gave those two arguments in the wrong order, leading to it rotating the object at a constant angle equal to the unit size.  
Got to work on the `PhysicsObject::BouncyObject` that is inside of the bounce pad.  
Made a money bag sprite.

## 2024-04-09
Fixed the fact that the angle at which the ball bounced from the BouncePad was wrong by setting the orientation's angle to `sf::degrees(90.f - newRotation)` instead of `sf::degrees(newRotation)`.  
Added money bags to the level file and implemented a system to read them from the file and draw them on the screen.

## 2024-04-10
Implemented the collision of the money bags. Tried to make them fall with no collision using threads, which didn't work.

## 2024-04-11
Made them fall. Standard threads worked while SFML threads didn't.  
Made a global source and header file and changed some variables in all files to the global ones. This makes it easier to access the most common parameters like the unit size or the game window

## 2024-04-12
Made the score label and made it add money once a money bag is hit. The size of the text is still a bit large.  
Changed the currency from euro to dollar to match the symbol on the money bag.

## 2024-04-13
Added a background to the score label. In the process I learned about how to deal with drawing an object that inherits from mutliple `sf::Drawable` classes.  
Made a run button to start the simulation. I also made it so that the simulation resets once the ball is unable to move on or is outside of the map due to some collision thing.

## 2024-04-14
Made a way to edit objects. Still need to make the inventory work and make it so that you can move objects.

## 2024-04-15
Made the inventory counts work. Every time you build an object, that item's value will decrease. It lead to a segmentation fault, that was caused bu the fact that I had not initialised a value in a constructor.  
Also made it so you can cancel building.

## 2024-04-16
Made a booster sprite and put it in the inventory.

## 2024-04-17
Finished the boosters by checking if the ball collides and updating the speed accordingly. I update the speed by getting the parallel velocity using $$ v_{perp}=\left|\vec{v}\right|*cos(φ) $$ where φ is the angle between the direction vector of the ball and the direction vector of the booster.

## 2024-04-18
Made level 1 and made it so that the money bags reset when the level resets.

## 2024-04-19
Fixed the `UIElements::TextLabel` object, because the text was not properly scaled.