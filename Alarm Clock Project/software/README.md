# README

This is the template project folder for your lab. Feel free to modify the files in here, as long as they remain in this folder. This is where TAs will look for when running your project and/or inspecting your code for software quality.

## Lab 3 and 4

Please see Lab3.md and Lab4.md for further details. Please see BLYNK.md for setting up Blynk, connecting to it, or flashing new FW to your ESP8266 to talk to Blynk.

## Coding Style

## Creating a New Project

The first 6 steps have been done for you in this template repository. Click on the project file in your explorer to open it up.
1. Open up Keil.
2. Click on the 'Project' tab on the top toolbar.
3. Click on New uVision Project.
4. Create a new .uvproj file of the desired name (typically Lab1, Lab5, etc) in the SW folder.
5. Search for the 'TM4C123GH6PM' device in the 'Select Device for Target 'Target 1' window.
6. Press OK and then press OK again in the 'Manage Run-Time Environment' window. 

## Managing Project Files

1. Right click on the 'Target 1' item in the Project sidebar window. 
2. Click on 'Manage Project Items' in the submenu.
3. In the 'Files' column, click on the 'Add Files' button.
4. Add the implementation files (.C and .s files) required for the project dependencies into the project. This includes your main file. In particular, make sure you include the startup.s file, since this includes things like interrupt definitions.
5. Compiling the project will automagically link the implementation files to dependency header files. You can then view them from the project window after expanding the implementation file.

## Project Structure

We recommend the following project for Keil:
- doc: This folder contains documentation and READMEs for how to use your project.
- src: This folder contains the project main source file.
- inc: This folder contains the implementation files for external drivers and dependencies used in the project. 
- lib: This folder contains the implementation files for the drivers that YOU write!

This is different compared to the overall git repo project structure:
- resources: PDFs for development help
- hw: files for schematic capture and PCB layout, plus any accompanying diagrams for your HW setup
- sw: code that runs on the hardware
  - examples: Example Keil projects and code for each of your drivers. This could be unit tests or reference code instructing your customers (or your future self) how to appropriately use your drivers!
  - inc: This folder contains all the files for external drivers and dependencies used in the project.
  - lib: This folder contains all the files, organized by content, for the drivers that YOU write!
