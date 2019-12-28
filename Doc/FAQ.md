```
88888888888         db           ,ad8888ba,    
88                 d88b         d8"'    `"8b   
88                d8'`8b       d8'        `8b  
88aaaaa          d8'  `8b      88          88  
88"""""         d8YaaaaY8b     88          88  
88             d8""""""""8b    Y8,    "88,,8P  
88            d8'        `8b    Y8a.    Y88P   
88           d8'          `8b    `"Y8888Y"Y8a 
```

-----------------------------------------------------------------------
## **I hear Vulkan is awesome fast.  How fast is it?**

In our internal tests where we re-wrote parts of other applications the increase is 2X to 50X faster.  And as GPUs get faster the increase will grow. Vulkan is a cross-platform API made by the GPU hardware vendors (mostly: Intel, AMD and NVidea).  So it's going to be the fastest and lowest level cross-platform option.

-----------------------------------------------------------------------
## **Is Vulkan growing? Is Vulkan is a good choice for the core of an app?**

Yes, Vulkan is growing, and yes, Vulkan is an excellent choice for an app core - both now and looking forward.

Because of the important historic role OpenGL took in cross-platform graphics, and the enthusiasm of the hardware vendors - Vulkan is in a very powerful position for growth and industry transformation in 2020. 

Here’s a detailed breakdown of major players in this market and how the world is shifting towards Vulkan as the graphics platform of the future:

### **AMD**

* In 2013, AMD created project [Mantle](https://en.wikipedia.org/wiki/Mantle_(API)), a leading-edge graphics API that pushes the entire industry forward
* It had huge improvements in both GPU performance and areas where CPU was the limiting factor
* In a case study, AMD announced the performance in Battlefield 4 was up to 319% faster in Mantle
* In 2015, AMD donated Mantle to the Khronos Group as the foundation of Vulkan, so a next-generation low-level API could standardize across the industry
* As a founder of Vulkan, AMD is a huge and enthusiastic supporter 

>Vulkan supports close-to-metal control enabling faster performance and better image quality. No other graphics API offers the same powerful combination of OS compatibility, rendering features, and hardware efficiency.
>
>[*-AMD Website*](https://www.amd.com/en/technologies/vulkan)

### **Intel**

* Intel is a very active member of the Vulkan community, and they have lots of excellent example code.  Part of their application code examples grew into the starting point for GpuScript.
* [Jason Ekstrand](https://www.youtube.com/watch?v=l3Hyd2sWSvA) is a Lead on the Intel Vulkan team and is a member of the Khronos Vulkan working group.  He has also done many excellent presentations on training people to use Vulkan.

>Just as parallelism and multithreaded programming paved the way for the performance strides of multicore CPUs, Vulkan APIs are poised to forge a future of multithreaded, cross-platform GPU programming, and high-performance rendering, regardless of the target device.
>
>[*-Intel Website*](https://software.intel.com/en-us/articles/vulkan-apis-for-unity)


### **NVIDIA**

NVIDIA has worked closely with the Khronos Group, the creators of Vulkan, throughout its development.  NVIDIA calls Vulkan, "The only cross-platform next-generation 3D API," and points out that: DirectX is Windows only and Metal is Apple only.  Vulkan is one API for all GPUs. See [presentation.](https://on-demand.gputechconf.com/siggraph/2015/presentation/SIG1501-Piers-Daniell.pdf)

>We have been using NVIDIA hardware and drivers on both Windows and Android for Vulkan development, and the reductions in CPU overhead have been impressive.
>
>*-John Carmack, Chief Technology Officer, Oculus*


### **Google**

In October 2019 Google announced at the [X.Org Developer's Conference](https://xdc2019.x.org/event/5/contributions/576/attachments/441/695/SwiftShader_Lightning_Talk.pdf), "We are all in with Vulkan."

* Vulkan is mandatory on Android Q/10, and future Google Phones.  This strategy is called "Vulkan Everywhere".
* Chrome has Vulkan support:
    * WebGPU has Vulkan backend
    * The core drawing libraries in Chrome (SKIA) have a Vulkan backend
* Fushia is the next generation Google OS for PCs, phones, tablets and IOT devices.  It's 100% Vulkan - Vulkan only for low-level drawing
* Google is embracing a Linux + Vulkan stack for gaming.  Stadia, Google’s cloud gaming service is 100% Vulkan on Linux   
* Google wrote SwiftShader to run Vulkan on multi-threaded CPUs so that it is 100% compatible on all hardware - even without a graphics driver or a graphics card

### **Valve**

Valve has funded and driven many important aspects of Vulkan becoming a full-fledged cross-platform API:
* They arranged for Vulkan to work simply on Mac by getting MoltenVK to be released Open Source.
* They fund LunarG who creates the main Vulkan SDK and debugging tools (used by GpuScript).
* They are active in bringing more Windows games to Linux through Vulkan, especially through the Steam gaming platform.

> We have an ongoing investment in making Vulkan a competitive and well-supported graphics API, as well as making sure it has first-class support on Linux platforms.
>
>[*-Valve*](https://www.pcgamesn.com/steam-machines/valve-remove-steam-machines)


### **Samsung**

Samsung has been involved with Vulkan the day it started, since they "believe in the importance of high-quality graphics on mobile devices."

Samsung reports: Vulkan offers the same environment for desktop and mobile GPUs, making the task of writing portable software much easier.

>Samsung and Epic Games teamed up to optimize the performance of global phenomenon Fortnite for Galaxy devices.  Specialized Vulkan API support and optimized memory usage improved performance on Galaxy smartphones.
>
>[*-Sangmin Lee From Samsung*](https://www.youtube.com/watch?v=0AWgPbjxjT8)

<br>

Here are neutral parties:

### **Apple**

Vulkan code can run on top of Metal (the Apple GPU SDK) using MoltenVK. Apple announced they will remove OpenGL from future macOS releases.

This inadvertently helps Vulkan, because all cross-platform GPU libraries need to target Vulkan or Metal if they want to keep macOS supported.

Apple is also very active and is one of the leaders in the WebGPU standard being developed for web browsers.

So, in the near future, WebGPU will also be an additional compile target for GpuScript to reach the Mac.  It is also possible to port the GpuScript library to target Metal directly, as it has a similar structure to Vulkan.

>This is Apple's draft proposal for the WebGPU API. It started as a mapping of Metal to JavaScript, but that won't be where it ends up. Not only are there some things in Metal that don't quite fit with Vulkan and D3D12, we also don't want to be tied to the Metal API.
>
>[*-Apple's WebGPU API Proposal*](https://github.com/gpuweb/proposals/blob/master/WebGPU-Apple/api-proposal.html)

<br>

***So big picture is there's a very unique "lightning-in-a-bottle" situation around Vulkan becoming the hardware and software standard for the GPU.  There's really no other candidate. Non-GPU computing is a 100X slower now, and will be 1000X, 10,000X, 100,000X, etc. slower in the near future.***

-----------------------------------------------------------------------
## **Why is this project called GPU Script, but from what I see only runs C++?**

Our goal is to simplify using Vulkan to the point is easy to script. We are in the middle of a JavaScript layer and have our eye on Python. Once we have these pieces up and running and well tested we will bring them into the build.

-----------------------------------------------------------------------
## **I don't get what you mean by SUI and TUI?  What does this mean?  I don't see it in other Vulkan projects.**

These are terms that help simplify the 2 major types of drawing pipelines:

1. A complete DIY approach where everything from the UBO to the shader is all custom code. We call this SUI or "Shader UI".
 
2. A quick approach to get stuff on the screen fast using drawing libray calls.  Things like: AddRectangle, AddLine, AddText, ect.  You just make the stuff you need and don't worry about the low-level stuff below.  We call this TUI or "Triangle Based UI Draw Library".

    Often it's useful to use a mix of both, so we made a framework where it's easy to mix them together. We have a more detailed explanation of this topic [here](Tutorials_SUIvsTUI.md). 

-----------------------------------------------------------------------
## **Can I use this to do 2D?  To make a desktop app?**

Yes, this is a great platform for 2D.  The Vulkan community is primarily oriented to games and 3D projects.  We've extended that base and added lots of tools and functions to also excel at 2D projects.

-----------------------------------------------------------------------
## **Can I use this to do 3D?  To make a game?**

Yes, since Vulkan is designed for 3D you can.  Just use the SUI Shader UI to pass 3D verticies, vertex colors and uv coordinates.  

We have a [tutorial](Tutorials_ShaderToy_Particles.md) showing how to run ShaderToy examples here which would help get you started.

-----------------------------------------------------------------------
## **What is the relationship between this code and ImGUI?**

This code started as a app framework for running multiple Vulkan pipelines at the same time (the current Shader UI framework).  As we were planning out a 2D triangle submission UI we experiemented with ImGUI and thought it was great.  So we've included it in the app framework.

ImGUI tends to specialize in overlay UI that pops up and changes dynamically.  It's also really good for lists of different type of data.  So it's part of our app and we recommend using it for those cases.

Our team has been doing over 20 years of UI programming so we had quite a few app development cases (esp for business apps) that were outside of how ImGUI was working.  The major scenario is where UI panes are designed in Illustrator or Photoshop and you want to make a pixel accurate replica in the UI.  ImGUI, like QT tends to lean towards automated layout.

So we added our own general drawing and UI Library found in UILib.cpp for this type of scenario.

So we like ImGUI, and consider it a sister product we've embedded.  You may find both libraries useful for different scenarios and call both.  In our documentation "Triangle UI" refers to the combination of ImGUI & UILib.

We also have some additional UI technology that we're thinking about integrating that could widen the scope even more.

-----------------------------------------------------------------------
## **I'm not familiar with immediate mode UI coding, how can I learn it?**

Immediate Mode UI programming is amazing, this is a great intro about the concepts:
https://www.youtube.com/watch?v=Z1qyvQsjK5Y&t=2s

The great power of immediate mode UI is only what is drawn is evaluated in code.

>
> *The fastest instruction is the one that is never executed.*
>
> -- **<cite>Michael Abrash</cite>**
>

It also changes the way you think about events.  Instead of thinking about triggers when IO events happening, you use state (InputStateData, DisplayStateData) to read what has changed.


However there is a UI idea that we consider beyond Immediate Mode - that is the UI built by fragment shaders.  


> *Learning to write graphics shaders is learning to leverage the
> GPU with its thousands of cores all running in parallel*
>
> -- **<cite>Omar Shehata</cite>**
>

Fragment shader UI has the potential to be mind-blowing fast because everything is calculated in parallel.  A good inspiration reference for this was made by Intel called [FastUIDraw](https://github.com/intel/fastuidraw).  Although it never took off the ideas are amazing and it was around 10X faster than any other UI approach:
https://github.com/intel/fastuidraw

<img src="images/faq_fastuidraw.jpg" width="500">

So the UI framework in this Example App should be a great starting point for doing this type of experiment.  Of course the basics of displaying chunks of textures are covered in our docs, but there is another potential UI frontier beyond that.  You may also want to look at [Shadertoy](https://www.shadertoy.com/), they have many fascinating experiments with UI and shader generated text: 
https://www.shadertoy.com/

-----------------------------------------------------------------------
## **Can you explain more about using State instead of Events?**

Ok to start big picture you can use events if you choose, all the GLFW event callbacks are in: [c_Window.cpp](../Include/Core/c_Window.cpp)

However, this is not a great for mouse and keyboard events because they can easily occur in the middle of drawing and get trampled.  When using the UILib you don't even need to think of this, for example calling:

```cpp
UILib::DragStart()
```

will return true for only one frame of drawing.  So you can do something more complicated like begin a drag rectangle.  And you're guaranteed to be aligned with the Vulkan draw.

Under the hood this is happening both in `PipelineManager::UpdateUIInputs` and ImGui. They make sure that a change value is only for one frame.  Either carrying over last value (mouse change) or buffering values (key change) and filling state.

<img src="images/faq_keystate.jpg" width="800">

-----------------------------------------------------------------------
## **How can I get started with shader coding?**

Check out this "Beginner's Guide to Coding Graphics Shaders"

https://gamedevelopment.tutsplus.com/tutorials/a-beginners-guide-to-coding-graphics-shaders--cms-23313

-----------------------------------------------------------------------
## **How does anti-aliasing work?**

For now the Triangle UI pipeline uses the AA features of ImGui.
This is done with a vertex color Anti-Alias Fringe.

**Note:** Anti-aliased filling requires points to be in clockwise order

-----------------------------------------------------------------------
## **Can I run this on iPhone / iPad?**

On iOS it works the same way as our Mac build.  Which is MoltenVK maps Vulkan into Metal calls: https://moltengl.com/moltenvk/

In fact the first 8 months of dev when this project started was on Mac.  Apple is currently accepting Vulkan-over-Metal apps in their store now as well: https://www.phoronix.com/scan.php?page=news_item&px=Apple-Lets-In-Updated-MVK-App

-----------------------------------------------------------------------
## **Can I run this on Android?**

Vulkan works really well on Android.  Both Google & Samsung are big supporters of Vulkan. We have only done some intial tests but here's the main site if you want to investigate porting it: https://developer.android.com/ndk/guides/graphics/index.html

-----------------------------------------------------------------------
## **Can I run this on the web?**

This code base currently can't run on the web.  But we are really interested in recent developments that could help do this. The biggest is ImGui is running in a browser: https://pbrfrat.com/post/imgui_in_browser.html

So it looks like this approach could be extended to our app framework. We have an interest in this but haven't done it yet.

We are also part of the WC3 GPU on the Web Community Group.  They are working towards an interface that will connect the web to "modern 3D graphics and computation capabilities".  From what we're seeing this will likely resemble Vulkan and would be a great target platform for GPU Script.

-----------------------------------------------------------------------
## **What about old hardware that won’t run Vulkan?**

Just recently there has been an amazing development in running Vulkan on hardware:
* With graphics cards and graphics drivers that don't support Vulkan 
* With no graphics card

The development is the open source Google project Swiftshader. SwiftShader is a high-performance CPU-based implementation of the Vulkan. It’s goal is to provide hardware independence for advanced 3D graphics.

What's cool is that SwiftShader is optimized to turn Vulkan into smart, multi-threaded CPU calls and improves the performance of games or 3D content on low-end machines.

-----------------------------------------------------------------------