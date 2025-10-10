# Quasar - 3D Rendering Engine

<p align="center">
  <img src="images/portfolio/motionBlur.png" width="80%"/>
  <br>
  <em>A Hero Render</em>
</p>

After spending months and casting trillions of rays, all the work has finally cumulated into "Quasar" - my very own Rendering Engine based on Path Tracing. The Hero Render is a testimony to our statement, a dragon - which symbolizes power, courage and authority.

This repository contains a c++ and python implementation for the rendering engine, albeit python is used just to upscale images, as a post processing effect, wherever required. In the next few sections, I will walk you through the feature set and will conclude by mentioning the plans for future.

## GLB / GLTF 

<p align="center">
  <img src="images/portfolio/sponzaAtrium.png" width="80%"/>
  <br>
  <em>Classic Sponza Atrium</em>
</p>

Creating complex scenes using basic shapes with code becomes cumbersome really fast. Loading models through GLB / GLTF was an envitable feature. Quasar uses Assimp for interpreting the GLB / GLTF file and load the geometry /  material information in memory. The read information is stored into an in-memory format that Quasar uses to render visuals on screen. 

As of now, everything is loaded as triangles, for materials GLTF format uses Principled BSDF approach with KHR extensions wherever needed. Quasar has the ability to work with:
- Diffuse Color
- Diffuse Texture
- Normal Map

Additionally, it can interpret UVs, Normals, Tangents and Bi-Tangents. The remaining values of Principled BSDF are a WIP, being implemented as Disney BRDF.

## Lights

In Quasar, there are two types of light sources:
- Ambient
- Sources casting lights

The engine itself supports Global Illumination, as a result of the Path Tracing algorithm, which means it will convey the physics of light upto a certain extent. In this case, the colors from different light sources get mixed and result into a 3rd color. The current implementation includes lights as a material, so all one has to do is apply a material to mesh.

<p align="center">
  <img src="images/portfolio/lights.png" width="80%"/>
  <br>
  <em>Metal Fox</em>
</p>