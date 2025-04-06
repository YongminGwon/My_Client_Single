Window W H FL FS
- This line declares that the SFML Window must be constructed with width W and height H, each of which will be integers.
 FL is the frame limit that the window should be set to, and FS will be an integer which specifies wheter to display the application in full-screen mode (1) or not (0).

 Font F S R G B
 - This line defines the font which is to be used to draw text
 for this program. The format of the line is as follows:
 Font File     F              std::string (it will have no spaces)
 Font Size     S              int
 RGB Color     (R, G, B)      int, int, int

 Player Specification:
 Player SR CR S FR FG FB OR OG OB OT V
 Shape Radius      SR         int
 Collision Radius  CR         int
 Speed             S          float
 Fill Color        FR,FG,FB   int, int, int
 Outliine Color    OR,OG,OB   int, int, int
 Outline Thickness OT         int
 Shape Vertices    V          int

 Enemy Specification:
 Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
 Shape Radius      SR         int
 Collision Radius  CR         int
 Min/Max Speed     SMIN, SMAX float

 Bullet Specification:
 Bullet SR CR S FR FG FB OR OG OB OT V L
 Shape Radius      SR         int
 Collision Radius  CR         int
 Speed             S          float
 Fill Color        FR, FG, FB int, int, int
 Outline Color     OR, OG, OB int, int, int
 Outline Thickness OT         int
 Shape Vertices    V          int
 Lifespan          L          int