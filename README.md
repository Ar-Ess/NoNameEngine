# [NoNameEngine](https://github.com/Ar-Ess/NoNameEngine)
## Team Members
### [Martí Buxeda](https://github.com/Ar-Ess)
### [Jordi Espriu](https://github.com/LordUnicorn31)
### [Víctor Jara](https://github.com/Kerali)
## Description
A programed & a designer of the successful team NoNameBros, have created an astonishing engine that will blow your mind. 
NoNameEngine will help programmers developing games as famous as "Split Duty".
## Engine Features

### Frustum
- Frustum added. The frustum movement works well, however the rotation of the frustum gives problems,
so we do not recommend rotating the frsutum.

## Mouse picking
- Mouse picking system added, which allows to select entities and acces their inspector information

## Bounding boxes
- Now all models have a visual bounding box which allows the user to see the transformation of the 
models. Right now the bounding boxes don't update, we are working on it.
 
### Camera Controls
 - **Orientation:**
   - Mouse: Move + L-Click 
   - Trackpad: Scroll vertically / horizontally
   - Wheel: Scroll vertically + L-Shift (Z axis)
 - **Zoom:**
   - Mouse: L-Shift + L-Click + Move Vertically
   - Trackpad: L-Shift + Scroll vertically
   - Mouse Wheel: Scroll vertically
 - **Movement**
   - W: Move Front
   - S: Move Back
   - A: Move Left
   - D: Move Right
   - L-Click + W: Move Forwards Cam Axis
   - L-Click + S: Move Backwards Cam Axis
   - L-Shift + W: Move Upwards
   - L-Shift + S: Move Downwards
 - **Speed Up**
   - L-Alt: speed x2

### Save & Load
 - **Project:**
   - Save & Load the whole project using JSON, including preferences
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
    - Only for Model3D shapes, not Primitives yet
    - Select, from the Hierarchy, the Model3D, then Drag & Drop a file on the screen

### Model Creation
_Important: "Shape3D" class is the equivalent of "GameObject" class_
 - **Primitives:**
   - Locate: Create -> Primitives
   - Cube, Pyramid, Line, Cylinder, Sphere, Plane
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
    - Edges: Shows the edges of the selected shape
    - Normals: Shows the normals of the selected shape
    - Solid: Shows the solid plane that defines the selected shape

### Hierarchy
 - View all the shapes in the stage
 - Select a model to display:
   - Shape name
   - Transpose info (Position, Rotation, Scale)
   - Shape type
     - In case it has specific variables, show them
   - Mesh info
   - Texture Info
