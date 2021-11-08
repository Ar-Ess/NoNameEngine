# [NoNameEngine](https://github.com/BooStarGamer/NoNameEngine)
## Team Members
### [Martí Buxeda](https://github.com/BooStarGamer)
### [Víctor Jara](https://github.com/Kerali)
## Description
A programed & a designer of the successful team NoNameBros, have created an astonishing engine that will blow your mind. 
NoNameEngine will help programmers developing games as famous as "Split Duty".
## Engine Features
### Camera Controls
 - **Orientation:**
   - Mouse: Move + L-Click 
   - Trackpad: Scroll vertically / horizontally
 - **Zoom:**
   - Mouse: L-Shift + Move + L-Click
   - Trackpad: L-Shift + Scroll vertically
 - **Movement**
   - W: Move forwards
   - S: Move Backwards
   - A: Move Left
   - D: Move Right
   - L-Shift + W: Move Upwards
   - L-Shift + S: Move Downwards

### Save & Load
 - **Project:**
   - Save & Load the whole project, including preferences (models not yet)
   - Locate: File -> Save File / Load File
 - **Preferences:**
   - Saves & Loads a preset of the preferences selected on the configuration window
   - Locate: File -> Configuration -> Options
   - In the "Options" menu, there is a "Default" option which resets your preferences to the default ones

### Load Models & Textures
- Any model/image extension supported (in code)
- Drag & Drop feature:
  - **Models:**
    - Supports FBX for now (will be extended on the future) 
  - **Textures:**
    - Supports any kind of image extension
- When Drag&Drop a texture, it reloads all models texture (future implementation: Drag&Drop texture for specific model)

### Model Creation
 - **Primitives:**
   - Locate: Create -> Primitives
   - Cube, Pyramid, Line, Cylinder, Plane
 - **Test:**
   - Locate: Create -> Test
   - Baker House, Warrior, Cube
 - **Delete Models:**
   - You can delete all the models, or just the first one loaded
   - Locate: Edit -> Delete / Delete All

### View Options
- Modify the information the Engine shows
- **Geometry:**
  - Modify how geometry is seen
  - Locate: View -> Geometry
    - Edges: Shows the edges of all models
    - Normals: Shows the normals of all models
