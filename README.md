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

## Shading

<p align="center">
  <img src="images/portfolio/SmoothFlatShading.png" width="80%"/>
  <br>
  <em>Shading Types</em>
</p>

Lighting is mostly a play of normals and how we reflect the incoming ray. This leads to the following two cases:
- Flat Shading: Use vertex normals
- Smooth Shading: Use nighbouring normals 

Fortunately, Quasar is capable of working with both the situations. All one has to do is make sure the appropriate shading mode is applies before exporting GLB / GLTF.

## Lights

<p align="center">
  <img src="images/portfolio/lights.png" width="80%"/>
  <br>
  <em>Metal Fox</em>
</p>

In Quasar, there are two types of light sources:
- Ambient
- Sources casting lights

The engine itself supports Global Illumination, as a result of the Path Tracing algorithm, which means it will convey the physics of light upto a certain extent. In this case, the colors from different light sources get mixed and result into a 3rd color. The current implementation includes lights as a material, so all one has to do is apply a material to mesh.

## Textures

<p align="center">
  <img src="images/portfolio/textures.png" width="80%"/>
  <br>
  <em>Metal Fox</em>
</p>

Textures help us to add the intricacies to the materials, they are also an important part of material pipeline in Quasar. The reason being even solid colours are implemented as Textures to maintain uniformity.

There are 2 types of textures in the pipeline:
- Procedural: Perlin Noise + Checkers
- Image

Like any other library, Textures are loaded asna separate entity, then attached to the material to be used during Rendering. One important thing for Image Textures is UV coordinates, which can either be loaded through GLB / GLTF or computed on the fly using barycentric coordinates

## Motion Blur

<p align="center">
  <img src="images/portfolio/motionBlur.png" width="80%"/>
  <br>
  <em>Metal Fox</em>
</p>

Motion blur is a visual effect that simulates the streaking or blurring of moving objects in a scene, caused by the relative motion between the camera and objects during the exposure time of a frame. In our case, the camera always stays stable and the object moves, this movement is described by assigning two positions to our desired object: the start position and the end position

The Hero Render is a good example that encapsulates the entire idea, it also proves how true out implementation is to physical reality as it shows all the reflections of the surrounding light whilst being in motion.

## Bounding Volume Hierarchies ( BVH )

A BVH (Bounding Volume Hierarchy) is a tree structure used to accelerate ray tracing by organizing scene geometry into nested bounding volumes, enabling efficient ray-object intersection tests, which in turn translates directly to improved rendering time. Earlier, without BVH, certain scenes took 12-14 hours, but with BVH that time came down to 1 hour

## Ambient Light

<p align="center">
  <img src="images/portfolio/metalNonMetal.png" width="80%"/>
  <br>
  <em>Metal Fox</em>
</p>

Ambient light is the base layer of general illumination that fills a space, providing the overall brightness and establishing the mood or atmosphere. This feature also implies that we don't need any explicit light as the ambient light will contribute enough to generate a visually appealing result, albeit that depends upon the configuration of the Ambient Light.

## Normal Map

<p align="center">
  <img src="images/portfolio/normalNoNormal.png" width="80%"/>
  <br>
  <em>Metal Fox</em>
</p>

Normal maps deserve their own section because of the sweet maths that goes behind them. Tangents, Bi-Tangents and Normals are the driving factor in this case. This orthogonal basis can either be loaded from GLB / GLTF or Quasar can generate them on the fly, just like UVs

A normal map is a texture map in 3D computer graphics that stores directional information in an RGB image to simulate fine surface detail, such as bumps, grooves, and scratches, without altering the model's actual geometry. It basically modifies how light reflects off of a surface.

Normal maps are pretty crucial, because if properly configured thay bring us closer to realism. 

## Cornell Box

<p align="center">
  <img src="images/portfolio/CornellBox.png" width="80%"/>
  <br>
  <em>Metal Fox</em>
</p>

The Cornell box is a test scene designed to evaluate the accuracy of rendering software by comparing a rendered image with a photograph of a real-world model under the same lighting conditions.

Here's a visual for Cornell's Box rendered using Quasar.

## Volumetrics

<p align="center">
  <img src="images/portfolio/info_2/latestHeroRender.png" width="80%"/>
  <br>
  <em>Metal Fox</em>
</p>

Volumetrics is the idea of simulating and rendering participating medium. Mediums that scatter, absorb or emit light as it travels through them. Examples like: Fog, Smoke, Dust and Most.

Quasar has implemented them in the form of a Texture / Material, one has to define the boundary of such a medium using a Mesh and then use the provided Isotropic Material, configure the density of the medium and voila! You have a Fog or mist or anything in between or outside depending upon the density.

## Anti - Aliasing

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/antiAliasing/spp1-upscaled.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          SPP 1
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/antiAliasing/spp100-upscaled.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          SPP 100
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/antiAliasing/spp500-upscaled.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          SPP 500
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

Anti-Aliasing helps reduce the jagged, stair-step edges that can appear in digital images, especially along diagonal lines or curves. In the real world, visual information is continuous, but in a rendered image, we approximate it with a grid of discrete pixels. By taking multiple samples per pixel and averaging the results, anti-aliasing creates smoother transitions and more realistic gradients, resulting in a cleaner and more natural-looking image.

## Depth of Field ( DOF )

You already must have had experience with depth of Field, when you click photos or even watch vlogs on youtube. You might see that the speaker is sharp but the background is blurred, that is due to depth of field

The "Field" in "Depth of Field" is merely an area in which objects appear sharp, the "Depth" in "Depth of Field" is like "how deep is the field", and mostly we are interested in controlling this so called "Depth". Unfortunately for us, according to our current implementation ( based on book ), we wouldn't be able to control the "deepness" of the field, rather for us ~ it's a "field" where object will appear in focus but more like a "Plane" and what ever is in the plane region will appear sharp. Things further before or after the plane will get blurred.

Two things can control Depth of Field:
- De-focus Angle: If there is a blur section in an image, then how much to blur it
- De-focus Distance: When to place the plane, at this distance everything will be sharp

#### De-focus Angle "How much to blur the blurry regions"

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/defocusAngle/0.0.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Defocus Angle 1.0
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/defocusAngle/0.5.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Defocus Angle 0.5
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/defocusAngle/1.0.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Defocus Angle 1.0
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

#### De-focus Distance "What region should appear sharp"

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/dportfolio/info_1/defocusDistance/render1.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Defocus Distance
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/defocusDistance/render2.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Defocus Distance
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

## FOV

Field of View controls how wide the camera can see. A smaller FOV gives a zoomed-in, narrow perspective, while a larger FOV creates a wide-angle view. We use vFOV here, but also hFOV would have been fine. In our case we supply the vFOV and the dimensions are automatically adjusted

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/fov/20.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          FOV 20
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/fov/90.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          FOV 90
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>