# **GPU Script - Vulkan Version Compatibility**

### The following table shows which versions of Vulkan work with this project on multiple platforms.


| Version | Windows | Mac | Ubuntu | Comments |
|:-------:|:-------:|:---:|:------:|----------|
| 1.1.130.0 | NO | NO | NO | All Platforms: UniqueSurfaceKHR creation appears to have been regressed. See [discussion.](https://github.com/KhronosGroup/Vulkan-Hpp/issues/467). |
| 1.1.126.0 | NO | YES | YES | Windows: There are issues with vulkan.hpp loading the .dll. See [discussion.](https://github.com/KhronosGroup/Vulkan-Hpp/issues/454) |
| 1.1.121.2 | YES | - | - | |
| 1.1.121.1 | - | YES | YES | |
| 1.1.114.0 | YES | YES | YES | |
