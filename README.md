<br>
<img src="Doc/images/GPUscript.png" width="500">

## The fastest way to build a lightning speed Vulkan App!

## **Big Picture**

GpuScript is a high-speed app engine and drawing library.

The primary technology used is Vulkan, which is a low-level graphics card API that is cross-platform and cross-hardware.

Vulkan is well known for being powerful but difficult to use.

We’ve spent years analyzing many different open source Vulkan frameworks and carefully considered their strengths and weaknesses.  Often:

* They are specific to only one platform, often Windows or Linux
* Are not tested regularly, so are out of date with the latest Vulkan builds
* They are draw platform generic (for example including OGL and Metal interfaces) which leads to a ton of complexity

So, we’ve done a mash-up of the very best Vulkan frameworks and adapted them to work well with:

* GLFW to simplify window management and events across platforms
* Windows, Mac, and Linux through tons of testing and native code (for example file open dialogues)
* CMake so that there is a simple standardized process to compile across platforms
* Well known shader repositories like Shadertoy so it’s easy to grab example code and experiment

The end result is a Vulkan platform that is very flexible for whatever you need:
* For the beginner there’s tons of tutorials and diagrams for a great out of box experience
* For the expert it’s easy to rip apart and change well tested, templated and documented code

GpuScript also includes a powerful adaptive user interface which has many features:

* It’s easy to make complex application layouts in a few lines of code
* Window pane management is simplified so it adaptively resizes based on constraints.
* Window panes can be: fixed, percentage based, floating and draggable
* Hierarchical panes can be contained within other panes, and tutorials show how to make standard UI patterns found in major applications


## **Documentation**

* [Install Guide (PC, Mac, and Ubuntu)](Doc/Install_Notes_Index.md)
* [Get Started & Tutorials](Doc/Tutorials_Index.md)
* [Code Diagram](Doc/Code_Big_Picture_Diagram.pdf)
* [User Interface Library](Doc/UILib_Starting_Functions.md)
* [FAQ](Doc/FAQ.md)
* [Vulkan Version Compatibility Table](Doc/Vulkan_Compatibility.md)
* [MIT License](LICENSE.txt)


<br><br>
<img src="Doc/images/DRComputing.png" width="500">

<br>

[![Analytics](https://ga-beacon.appspot.com/UA-154737984-2/DillonRidgeComputing/GPU-Script)](https://github.com/igrigorik/ga-beacon)