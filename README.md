# Quasar - 3D Rendering Engine

<p align="center">
  <img src="images/portfolio/motionBlur.png" width="100%"/>
  <br>
  <em>A Hero Render</em>
</p>

After spending months and casting **trillions** of rays, all the work has finally cumulated into "_Quasar_" - my very own **_Rendering Engine_** based on Path Tracing. The Hero Render is a testimony to that statement, a dragon - which symbolizes power, courage and authority.

This repository contains a **_C++_** and **_Python_** implementation for the rendering engine, albeit python is used just to upscale images, as a post processing effect, wherever required. In the next few sections ( each section has visuals ), I will walk you through the feature set and will conclude by mentioning the plans for future. 

**TIP:** Dark Mode would enhance the experience of reading, but ofcourse it's not a necessity!
**Note:** All images in this repository are rendered using Quasar.

## Index

- [GLB / GLTF](#glb--gltf)
- [Shading](#shading)
- [Lights](#lights)
  - [Source Light](#Source-Light)
  - [Ambient Light](#ambient-light)
- [Textures](#textures)
- [Motion Blur](#motion-blur)
- [Normal Map](#normal-map)
- [Volumetrics](#volumetrics)
- [Mesh Types and Transformations](#mesh-types-and-transformations)
- [Cornell Box](#cornell-box)
- [Material Showcase](#material-showcase)
- [Anti-Aliasing](#anti-aliasing)
- [Depth Of Field ( DOF )](#depth-of-field--dof-)
- [Field Of View ( FOV )](#field-Of-view--fov-)
- [Depth & SPP](#depth--spp)
- [Bounding Volume Hierarchies ( BVH )](#bounding-volume-hierarchies--bvh-)
- [Multi-Threading](#multi-threading)
- [UI](#ui)
- [Author's Note](#authors-note)
- [References](#References)

### GLB & GLTF 

<p align="center">
  <img src="images/portfolio/sponzaAtrium.png" width="100%"/>
  <br>
  <em>Classic Sponza Atrium</em>
</p>

Creating complex scenes using basic shapes and code becomes cumbersome really fast. **Loading** models through GLB / GLTF was an envitable feature. Quasar uses **_Assimp_** for interpreting the GLB / GLTF file and load the geometry /  material information in memory. The read information is stored into an in-memory format that Quasar uses to render visuals on screen. 

As of now, everything is loaded as **triangles**, for materials GLTF format uses **Principled BSDF** approach with _KHR_ extensions wherever needed. Quasar has the ability to work with:
- Diffuse Color
- Diffuse Texture
- Normal Map

Additionally, it can interpret UVs, Normals, Tangents and Bi-Tangents. The remaining values of Principled BSDF are a WIP ( present in repo but not used ), being implemented as **Disney BRDF**.

### Shading

<p align="center">
  <img src="images/portfolio/SmoothFlatShading.png" width="100%"/>
  <br>
  <em>Shading Types</em>
</p>

Lighting is mostly a play of **normals** and how we **reflect** the incoming ray. This leads to the following two cases:
- **Flat Shading**: Use vertex normals
- **Smooth Shading**: Use neighbouring normals 

Fortunately, Quasar is capable of working with both the situations. All one has to do is make sure the appropriate shading mode is applied before exporting GLB / GLTF.

### Lights

In Quasar, there are two types of light sources:
- Source Light
- Ambient Light

The engine itself supports **Global Illumination**, as a result of the Path Tracing algorithm, which means it will convey the **physics** of light upto a certain extent. The current implementation includes lights as a material, so all one has to do is apply a material to mesh.

#### Source Light

<p align="center">
  <img src="images/portfolio/lights.png" width="100%"/>
  <br>
  <em>Source Light</em>
</p>

In this case, Quad geometry is used to create the shape of light, and one can see that the colors from different light sources **blend** and result into a 3rd color. A direction conclusion of that effect is the change in the color of platform: In this case the platform appears to be white-ish in color but if you reference other images, you would see that the true color of platform is blueish.

#### Ambient Light

<p align="center">
  <img src="images/portfolio/metalNonMetal.png" width="100%"/>
  <br>
  <em>Ambient Light</em>
</p>

Ambient light is the base layer of **general illumination** that fills a space, providing the overall brightness and establishing the mood or atmosphere. This feature also implies that we don't need any explicit lights, as the ambient light will contribute enough to generate a visually appealing result, albeit that depends upon the configuration of the Ambient Light.

### Textures

<p align="center">
  <img src="images/portfolio/textures.png" width="100%"/>
  <br>
  <em>Textures</em>
</p>

Textures help us to add the intricacies to the materials, they are also an important part of material pipeline in Quasar. The reason being, even solid colours are implemented as Textures to maintain uniformity.

There are 2 types of textures in the pipeline:
- **Procedural:** Perlin Noise + Checkers
- **Image** Texture

Like any other library, Textures are loaded as a separate entity, then attached to the material to be used during Rendering. One important thing for Image Textures is **UV** coordinates, which can either be loaded through GLB / GLTF or computed on the fly using **barycentric** coordinates

### Motion Blur

<p align="center">
  <img src="images/portfolio/motionBlur.png" width="100%"/>
  <br>
  <em>Motion Blur</em>
</p>

Motion blur is a visual effect that simulates the streaking or blurring of moving objects in a scene, caused by the relative motion between the camera and objects during the exposure time of a frame. In our case, the camera always stays stable and the object moves, this movement is described by assigning two positions to our desired object: the start position and the end position

The Hero Render is a good example that encapsulates the entire idea, it also proves how true the implementation is to physical reality as it shows all the reflections of the surrounding light whilst being in motion.

### Normal Map

<p align="center">
  <img src="images/portfolio/normalNoNormal.png" width="100%"/>
  <br>
  <em>Normal Map</em>
</p>

Normal maps deserve their own section because of the sweet maths that goes behind them. _Tangents_, _Bi-Tangents_ and _Normals_ are the driving factor in this case. This orthogonal basis can either be loaded from GLB / GLTF or Quasar can generate them on the fly, just like UVs

A normal map is a texture map in 3D computer graphics that stores directional information in an RGB image to simulate fine surface detail, such as bumps, grooves, and scratches, without altering the model's actual geometry. It basically modifies how light reflects off of a surface, using the orthogonal basis described previously.

Normal maps are pretty crucial, because if properly configured thay bring us closer to realism. 

### Volumetrics

<p align="center">
  <img src="images/portfolio/info_2/latestHeroRender.png" width="70%"/>
  <br>
  <em>Volumetrics</em>
</p>

Volumetrics is the idea of simulating and rendering participating medium. Mediums that scatter, absorb or emit light as it travels through them. Examples like: Fog, Smoke, Dust and Most.

Quasar has implemented them in the form of a Texture / Material, one has to define the boundary of such a medium using a Mesh and then use the provided **Isotropic Material**, configure the density of the medium and voila! You have a Fog or mist or anything in between or outside depending upon the density. 

**Note:** In this website, most of the images with dark background have a volumetric effect, albeit with a very low density, mostly it being visible in dark mode, as white usually overshadows it.

### Mesh Types and Transformations

Quasar has a few in-built types of geometries, the Primitive geometry list can be extended further easily. Majorly there are two types of geometries:
- **Primitives** ( Sphere / Quad / Triangle )
- **Composites** ( Composites / Primitives )

Each of them are described by pure **mathematical** functions. These equations not only allow us to implement the geometry but also transform them using the following operations:
- Rotation
- Translation

These transformation implementations use a clever trick of transforming the ray to the **local coordinate** system, rather than updating the geometrical data representing the geomtry

**Note**: When reading the GLB, we just take the data and convert to our class of Triangles and Composites

### Cornell Box

<p align="center">
  <img src="images/portfolio/CornellBox.png" width="70%"/>
  <br>
  <em>Cornell Box</em>
</p>

The Cornell box is a test scene designed to evaluate the accuracy of rendering software by comparing a rendered image with a photograph of a real-world model under the same lighting conditions. Above is the Cornell Box Test Result for our Rendering Engine.

### Material Showcase

This section demonstrates different materials that exist, and how they interact with light ( with respect to physics ). Each Material implemented, takes care of two things: ***attenuation*** and ***scattering***. And depending upon just two factors we have created a bunch of materials ( kind of nuanced explanation ). A quick glance at list of materials that are present in the code base:
- Normal
- Diffuse 
- Solid
- Metal
- Dielectric

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/solid/render.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Solid - Flat-colored material,
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/diffuse/render.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Diffuse - A Lambertian surface that scatters light evenly in all directions.
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/normal/render.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Normal - Visualizes the normal vectors
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

#### Metal

Metal, as a material consists of two major properties: ***reflectance*** and ***color*** of the metal itself. The reflectance can also have a "***fuzz***" factor, just like in real life. Below you can see a progression of fuzz factor from shiny metal ball to almost diffuse metal ball ( acts just like a diffuse sphere material )

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/metal/fuzz0.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Metal - 0 fuzz
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/metal/fuzz0.5.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Metal 0.5 fuzz
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/metal/fuzz1.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Metal 1 fuzz
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

Just to showcase the "magic" of PathTracing, I have also created the following two scenarios:
- ***metal + metal reflections***: You can see the infinite reflection pattern on both the spheres ( just like real life )
- ***metal + dielectric reflections***: You can see the faint reflections of a glass sphere and also the glass sphere reflecting + refracting the metal sphere

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/metal/dielectricIntoMetalReflection.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Dielectric Reflecting Into Metal
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/metal/metalIntoMetalreflection.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Metal Reflecting Into Metal
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

####  Dielectric

***Dielectrics*** are materials that are weak conductors of electricty, like wool, water, sand, air etc.. Here we tried to simulate "***glass***" and that too with reflection, refraction and total internal reflection. You might feel that the glass balls are floating due to abscence of shadow but even in real life glass balls rarely have a dense shadow. If you focus at the bottom center of the sphere, you might see a faint patch of shadow

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/dielectric/dielctric.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Dielectric - RI: ( 1.00 / 1.33 )
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/dielectric/dielectricInsideDielectricReRender.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Dielectric Inside Dielectric - RI1 ( outer ): ( 1.50 ) + RI2 ( inner ): ( 1.00 / 1.50 )
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

### Anti Aliasing

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

Anti-Aliasing helps reduce the jagged, stair-step edges that can appear in digital images, especially along diagonal lines or curves. In the real world, visual information is continuous, but in a rendered image, we **approximate** it with a grid of discrete pixels. By taking multiple **samples per pixel** and **averaging the results**, anti-aliasing creates smoother transitions and more realistic gradients, resulting in a cleaner and more natural-looking image.

### Depth of Field ( DOF )

You already must have had experience with depth of Field, when you click photos or even watch vlogs on youtube. You might see that the speaker is sharp but the background is **blurred**, that is due to depth of field

The "Field" in "Depth of Field" is merely an area in which objects appear sharp, the "Depth" in "Depth of Field" is like "how deep is the field", and mostly we are interested in controlling this so called "Depth". Unfortunately for us, according to our current implementation, we wouldn't be able to control the "deepness" of the field, rather for us ~ it's a "field" where object will appear in focus but more like a "Plane" and what ever is in the plane region will appear sharp. Things further before or after the plane will get blurred.

Two things can control Depth of Field:
- **De-focus Angle:** If there is a blur section in an image, then how much to blur it
- **De-focus Distance:** Where to place the plane, at this distance everything will be sharp

##### Defocus Angle "How much to blur the blurry regions"

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

##### Defocus Distance "What region should appear sharp"

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/defocusDistance/render1.png" width="100%">
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

### Field Of View ( FOV )

Field of View controls how wide the camera can see. A smaller FOV gives a zoomed-in, narrow perspective, while a larger FOV creates a wide-angle view. We use **vFOV** here, but also hFOV would have been fine. In our case we supply the vFOV and the dimensions are automatically adjusted

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

### Depth & SPP

In ray tracing, depth defines the maximum number of times a ray is allowed to bounce (or recurse) before it's terminated. Each bounce represents a light interaction — such as reflection, refraction, or scattering.

This parameter is used to avoid infinite recursion and control render time. A ray that exceeds the depth is assumed to contribute no more light, and its color is set to black

- A **lower** depth results in faster renders but may miss deeper reflections or refractions.
- A **higher** depth produces more realistic images, especially for transparent or reflective materials, but is computationally more expensive.

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/maxDepth/maxdepth2samplerate500.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Depth 2 SPP 500
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/maxDepth/maxdepth10samplerate500.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Depth 10 SPP 500
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/maxDepth/maxdepth50samplerate500.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Depth 50 SPP 500
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

SPP ( Samples Per Pixel ) controls how many rays are shot per pixel to estimate its final color. Instead of casting just one ray, multiple rays with slight random offsets are averaged to reduce noise and achieve anti-aliasing. 

**Fun Fact:** Since SPP adds extra rays, the extra rays tend to be additional cost. DOF uses this additional cost to it's benefit and implements the blur effect

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/maxDepth/maxdepth50samplerate2.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Depth 50 SPP 2
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/maxDepth/maxdepth50samplerate50.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Depth 50 SPP 50
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/maxDepth/maxdepth50samplerate500.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Depth 50 SPP 500
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

Below you can see that low Depth + low SPP, would result into a grainy and dark image because not enough rays are spawned which results in bad sampling and low Depth causes less infoinformation to be gathered. In the right image, you can see that a image generated with good parameters would give a good anti-aliased + tinted reflections + smooth image

<div align="center">
<table>
  <tbody>
    <tr>
      <td >
        <img src="images/portfolio/info_1/maxDepth/maxdepth2samplerate2.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Max Depth 2 SPP 2
        </p>
      </td>
      <td>
        <img src="images/portfolio/info_1/BouncedReflections/render.png" width="100%">
        <p style="text-align: center; font-style: italic; font-size: 14px; color: #555;">
          Bounced Reflections
        </p>
      </td>
    </tr>
  </tbody>
</table>
</div>

### Bounding Volume Hierarchies ( BVH )

A BVH (Bounding Volume Hierarchy) is a tree structure used to accelerate ray tracing by organizing scene geometry into nested bounding volumes, enabling efficient ray-object intersection tests, which in turn translates directly to improved rendering time. Inorder to render all the feature set present in Quasar with complex scenes, in reasonable time, BVH is a very important. Earlier, without BVH, certain scenes took 12-14 hours, but with BVH that time came down to 1 hour.

### Multi Threading

The nature of graphics rendering is embarrassingly parallel, it's like "hey, this is the process to calculate the color of a pixel, can you apply it to millions of pixels"?

If we don't use this fact to our advantage then we are loosing a lot. That's why, I decided to add Multi-threading.

The Multi-Threading is managed by a thread pool, which allows us to have a lot more control over the threads themselves. In order to avoid blocking the main thread we spawn N threads, depending upon hardware concurrency, and then use N-1 of them for rendering and 1 thread for updating the UI, this process results into a seamless non-blocking workflow.

The idea is to spawn max pure hardware threads, divide the image into Tiles depending upon number of threads and a multiplier. Ask each thread to compute values for the assigned Tile, write into image buffer and save the buffer to file. Once a thread finishes rendering a Tile, it grabs the next tile randomly.

We do get a huge speed boost, anywhere from 5.5x to 24x ( upper limit being number of pure hardware threads ). As an example the volumetrics render of this repo, took around 1.4 hours to render, whereas without multi threading i believe it would have taken anywhere from 12 hours to 20 hours

The Volumetrics scene is massive and complex: 1000 spheres, caustics, Fog, 10k samples per pixel and 40 as depth for each recursed ray.

For other small scenes in here, i could see time going down from mins to seconds or hours to mins.

### UI

https://github.com/user-attachments/assets/c5a35ebc-be32-4be4-a020-530870df95fb

Qusar uses SFML to display the result into window, the result is refreshed every few milliseconds. It consists of auto-save which is triggered the moment rendering is done, if one wants to save manually then pressing "s" can do that. Additionally, it also displays some additional information towards the top-left cornerof the window

## Author's Note:

Quasar had a very hopeful conception, it was supposed to be a **CPU** based PathTracing Engine ~ I implemented it to deepen my knowledge of how exactly Rendering Engines work, eventually I started loosing a grip of that journey and thought about extending Quasar to **Vulkan** ( based on **Compute Shader** ), so I would be able to learn Vulkan ( coming from an **OpenGL** background ). But after spending more time, I have come to a conclusion to let Quasar be what it was meant to be, a CPU Rendering Engine, the only thing that is going to be added is: **Disney BRDF** ( WIP ).

As for my next journey, my knowledge about Path Tracing Engines is wayy better than what I had started with. The obvious next goal is to master **BRDFs**, **BTDFs**, **BSDFs** and **BSSDFs**, Quasar is going to teach me that and once done with it I will be working on a Vulkan implementation ( albiet more advanced )

## References:

- [ Depth Of Field ](https://cristianrosiu.github.io/posts/dof-blog/)
- [ Shape Intersection Function Theory ](https://iquilezles.org/articles/intersectors/)
- Advanced Global Illumination ( Book )
- [ Disney BRDF ](https://schuttejoe.github.io/post/disneybsdf/)
- [ UCSD College Lecture Resource ](https://cseweb.ucsd.edu/~tzli/cse272/wi2023/homework1.pdf)
- [ Ray Tracing In One Weekend ](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- [ Ray Tracing The Next Week ](https://raytracing.github.io/books/RayTracingTheNextWeek.html)
- [ PBRT: From Theory To Implementation ](https://pbr-book.org/3ed-2018/Materials/Bump_Mapping)
- [ Crash Course In BRDF ](https://boksajak.github.io/files/CrashCourseBRDF.pdf)
- [ Learn OpenGL ](https://learnopengl.com/Advanced-Lighting/Normal-Mapping)
- [ Normal Map Strength ](https://computergraphics.stackexchange.com/questions/5411/correct-way-to-set-normal-strength)
- [ UCSD College Lecture Resource ](https://cseweb.ucsd.edu/~tzli/cse272/wi2023/)

