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