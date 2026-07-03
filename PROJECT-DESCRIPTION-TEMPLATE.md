# Moonlit Portal

159/2020 - Nataša Blagojević  
The agent has been sent into space and is tasked with exploring it. 
After a long journey, he becomes tired and wants to return home. He lands on a floating island
where a mysterious portal is located. The player helps the agent reach the portal, while
a spaceship monitors the island and reacts to his movement.
<br> <br>
The scene contains a space environment with a floating island, an agent, a spaceship, 
an orbiting moon and skybox. Lighting is implemented using directional light and spotlight, while 
Bloom is used to emphasize glowing objects such as the moon, spaceship and agent.


## Controls

`W` -> move forward  
`S` -> move backward  
`A` -> move left  
`D` -> move right   
`UP` -> move up  
`DOWN` -> move down  
`SCROOL` -> zoom in/out
<br> <br>
`RIGHT` -> agent moves right  
`LEFT` -> agent moves left     
`SPACE` -> reset/show agent   
`E` -> start event sequence
<br> <br>
`B` -> enable/disable Bloom effect  
`F` -> switch between Phong and Blinn-Phong lighting \
`F2` -> open/close GUI  
`ESC` -> close the program

## Features

### Fundamental:

[X] Model with lighting  [directional light & spot]   
[X] Two types of lighting with customizable properties through GUI or actions
<br> <br>
The scene uses directional light as moonlight and spotlight as the spaceship light. 
The spotlight intensity can be adjusted through the GUI. During the event sequence, the spaceship
increases its spotlight intensity while the agent moves towards the portal.
<br> <br>
[X] `E` key pressed --- after 2 seconds ---> spaceship spotlight intensity increases --- after 2 more seconds
    ---> agent starts glowing --- after 2 more seconds ---> agent disappears/teleports through the portal 

Event sequence: \
Pressing `E` starts the event sequence. The agent automatically moves towards the portal.
After 2 seconds, the spaceship increases its spotlight intensity. After 4 seconds, the agent starts 
glowing more strongly. After 6 seconds, the agent disappears, simulating teleportation through the portal.

### Group A:

[ ] Frame-buffers with post-processing   
[ ] Off-screen Anti-Aliasing  
[ ] Parallax Mapping   
[X] Bloom with the use of HDR

### Group B:
[ ] Deferred Shading  
[ ] Point Shadows  
[ ] SSAO

### Engine improvement:

[X] Bloom 

## Models:

[//]: # ([Moon]&#40;https://drive.google.com/drive/folders/1LE0byH5idFZ96GbHG1oFkRb75evlOKE-?usp=sharing;  !!! must be downloaded and put it in the project [app/resources/modles/moon] !!! <br>)
[Moon](https://drive.google.com/drive/folders/1LE0byH5idFZ96GbHG1oFkRb75evlOKE-?usp=sharing) !!! download model from link and put it at path: app/resources/models/moon <br>
[Agent](https://sketchfab.com/3d-models/stylized-sci-fi-soldier-animated-9e19e517429c4077b800273890186456) <br>
[Space island](https://sketchfab.com/3d-models/interdimensional-zen-gallery-space-1bd9d26e53e044eea56a2837ffd75136) <br>
[Spaceship](https://sketchfab.com/3d-models/space-station-0da4a24e7edd49159737675ffcc06228)

## Textures

[Blue space skybox](https://opengameart.org/content/space-skyboxes-0)

{OTHER-RESOURCES}
