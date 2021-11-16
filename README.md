# [NoNameEngine](https://github.com/BooStarGamer/NoNameEngine)
## Team Members
### [Martí Buxeda](https://github.com/BooStarGamer)
### [Jordi Espriu](https://github.com/LordUnicorn31)
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
 - **Speed Up**
   - L-Alt: speed x2

### Save & Load
 - **Project:**
   - Save & Load the whole project using JSON, including preferences (camera pos as well, modules not yet)
   - Save/Load Shortcuts:
     - Ctrl + S: Save
     - Ctrl + L: Load
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
    - Select, from the Hierarchy, the Model3D, then Drag & Drop a file on the screen

### Model Creation
_Important: "Shape3D" class is the equivalent of "GameObject" class_
 - **Primitives:**
   - Locate: Create -> Primitives
   - Cube, Pyramid, Line, Cylinder, Plane
 - **Test:**
   - Locate: Create -> Test
   - Baker House, Warrior, Cube
 - **Delete Models:**
   - Delete Model:
     - Locate: Edit -> Delete Model
     - Shortcut: Supr
     - Select the model from the hierarchy
   - Delete All Models:
     - Locate: Edit -> Delete All Models
     - Shortcut: Shift + Supr

### View Options
- Modify the information the Engine shows
- **Geometry:**
  - Modify how geometry is seen
  - Locate: View -> Geometry
    - Edges: Shows the edges of the selected model
    - Normals: Shows the normals of the selected model

### Hierarchy
 - View all the shapes in the stage
 - Select a model to display:
   - Shape name
   - Transpose info (Position, Rotation, Scale)
   - Shape type
     - In case it has specific variables, show them
   - Mesh info
   - Texture Info
