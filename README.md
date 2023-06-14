# tinyrenderer-jnch009

During my time at university I took Introduction to Computer Graphics which we were tasked to create a Software Renderer with a minimal amount of code provided to us. Starting from line rendering, 
to triangles, to color interpolation, transformations, camera work and ending off with lighting models (Flat, Gouroud and Phong). I've been looking to revisit this work since graduating back in 2019 as
I spent countless hours working on the renderer and managed to get some parts looking great, but others looking terrible/incomplete. I stumbled upon https://github.com/ssloy/tinyrenderer and was
pleasantly surprised that this is very similar to that course and wanted to experience it again and make it look better.

Some sample renders so far:

## Triangle rasterizer

<img src="https://github.com/jnch009/tinyrenderer-jnch009/assets/13592249/74e55b21-033e-4316-a1d5-fd6232e5e285" width="380">

## Wireframe rendering 

| Bresenham  | Xiaolin Wu (AA) |
| ------------- | ------------- |
| <img src=".vs/Wireframe no AA.png" width="400"> | <img src=".vs/Wireframe AA.png" width="380"> |

## Line renderer

<img src=".vs/Line renderer.png" width="380">

## Anti aliasing

<img src=".vs/Additive Blending attempt 2.png" width="380">

## Flat Shading renders

| Barycentric without hidden face removal | Barycentric with hidden face removal | With Textures |
| ------------- | ------------- | ------------- |
|<img src=".vs/Barycentric using cross and dot products.png" width="260"> | <img src=".vs/Hidden surface removal.png" width="330"> | <img src=".vs/Interpolated Textures.png" width="380">  |

---

# Learnings:
1. When integrating triangle rendering using barycentric coordinates, my initial attempt of creating a bounding box was by
using the entire canvas for points to check. Now if we're simply providing one triangle into the `barycentricPolygonRenderer` function
this is not too big of a deal since we only do one pass, but direct your attention to lines 98-107 in `main.cpp`. Notice that we iterate
through every face, get their coordinates and then render the corresponding triangle. You can see how inefficient this will be, especially 
if the entire canvas is just filled with triangles!!! At the worst case, the algorithm runtime is O(n^3) due to scanning the width + height 
which is O(n^2) for every single triangle => O(n^3). 
    
    After doing more research and thinking this through, I coded up a new strategy which intelligently takes the lowest X and Y coordinates 
of the vertex list provided and then calculates the maximum width/height from those points. From there I create a bounding box of points 
to check if they exist inside the triangle. The difference with the strategy here is the efficiency where instead of scanning the width + height
every time, we're only scanning a condensed region of pixels using the vertices. This would improve the worst case time complexity to O(n^2), where
the worst case would be the entire grid filled with triangles.

<hr/>

2. For scanline rendering <TODO continue to add to this>
    
    // Can we have two separate loops storing the unique points and then use the other loop to find the y coordinates that match
	// In order to do this we need to figure out which line has the steeper slope
	// The line with the steeper slope will obviously hit the limit faster

	// we don't need to find the steeper slope, in the line function above, I simply take all Vec2i where the y values are unique

<hr/>

3. For using barycentric coordinates to render triangles, one issue that was annoying me was
gaps appearing in the lighting render. I was not seeing the same issue occurring with scanline rendering and was puzzled. Turns out the problem was that because lines of a triangle might not always pass through each pixel exactly, you may end up with unlit pixels due to the fact that the barycentric coordinate is out of the range [0,1]. 

   | Triangle Raster  | Flat Shading |
   | ------------- | ------------- |
   | <img src=".vs/Screenshot%202023-05-27%20212256.png" width="380"> | <img src=".vs/Screenshot 2023-05-27 212836.png" width="400"> |

    What I decided to do was add a small margin of error (something like 0.003) to compare with the violated barycentric coordinate and if that coordinate was within the range then it would still be lit up. This was the result. The noticeable difference should be the top left of the triangle raster that the white pixel that was missing is now filled and in flat shading, you'll see the middle of the face has those black spots now filled in. It's not a perfect solution, but at least makes it comparable to rendering with scanline.

	# Barycentric
	| Triangle Raster  | Flat Shading |
   | ------------- | ------------- |
   | <img src=".vs/Screenshot 2023-05-27 213406.png" width="380"> | <img src=".vs/Screenshot 2023-05-27 213513.png" width="400"> |

    # Scanline
	| Triangle Raster  | Flat Shading |
   | ------------- | ------------- |
   | <img src=".vs/Screenshot Scanline triangle.png" width="380"> | <img src=".vs/Screenshot Scanline flat shading.png" width="400"> |

   ### Correction to #3
   The margin of error strategy does in fact mitigate the problem, it unfortunately violates the actual use case of barycentric coordinates. Namely, barycentric coordinates represent the location of a point inside of a simplex and if the 3 coordinates are within the range [0,1] then the point exists in the triangle. If any of the coordinates are found outside of the range, then it is NOT inside the triangle and therefore the pixel should not be lit. After doing a bit more research, I found this article https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates.html which helped clarify the usage of using cross and dot products to solve the problem. Taking the cross product of two edges or the cross product of one edge with a line to our point would give us the normal vector. If we take the dot product of these two vectors, we may get a negative value. A negative dot product means that the vectors are pointing in opposite directions. We can therefore conclude that the point does not exist on the triangle and therefore we don't draw a pixel.

   <img src=".vs/Barycentric using cross and dot products.png" width="380">

<hr/>

4. Interesting case study of Anti Aliasing. I've been concurrently going through a book called "Practical Shader Development" and learned about a concept called additive blending. The way it works is by combining colors of textures (need to check) in a scene to get a brighter effect. I had noticed that my Anti-Aliased algorithm looked a bit off where the colors of the starburst looked a bit off. 

    <img src=".vs/Xiaolin AA looks off.png" width="380">

    If you look carefully in the middle, you'll see that the AA effect looks a bit inconsistent from the horizontal line on the right. In the middle, another thing that was bothering me was the reduced brightness. I am attempting to replicate the effect from a past university course I took at Simon Fraser University. 

    <img src=".vs/AA from University.png" width="380">

    With the addition of additive blending, I think I have managed to resolve the issue to the best of my ability and the end result looks fairly close. The image from the university utilizes a different Anti-Aliasing algorithm however.

    <img src=".vs/Additive Blending attempt 1.png" width="380">

    UPDATE: The reason for the black spots in the middle is that `unsigned chars` do not get clamped by default. If you go past a certain value, then the `unsigned char` will get changed to an unintended value. I needed to clamp the values within [0,255]. Now the picture looks almost the same (slightly brighter in the middle).

    | University AntiAliasing | My attempt |
    | ------------- | ------------- |
    | <img src=".vs/AA from University.png" width="380"> | <img src=".vs/Additive Blending attempt 2.png" width="380"> |
    