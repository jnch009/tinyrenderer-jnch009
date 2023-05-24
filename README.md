# tinyrenderer-jnch009

During my time at university I took Introduction to Computer Graphics which we were tasked to create a Software Renderer with a minimal amount of code provided to us. Starting from line rendering, 
to triangles, to color interpolation, transformations, camera work and ending off with lighting models (Flat, Gouroud and Phong). I've been looking to revisit this work since graduating back in 2019 as
I spent countless hours working on the renderer and managed to get some parts looking great, but others looking terrible/incomplete. I stumbled upon https://github.com/ssloy/tinyrenderer and was
pleasantly surprised that this is very similar to that course and wanted to experience it again and make it look better.

Some sample renders so far:

## Triangle rasterizer

![image](https://github.com/jnch009/tinyrenderer-jnch009/assets/13592249/74e55b21-033e-4316-a1d5-fd6232e5e285)

## Wireframe rendering 

| Bresenham  | Xiaolin Wu (AA) |
| ------------- | ------------- |
| ![image](https://github.com/jnch009/tinyrenderer-jnch009/assets/13592249/0cf6a8d5-bc08-41f1-bab3-3e1244a95c75)  | ![image](https://github.com/jnch009/tinyrenderer-jnch009/assets/13592249/5f7cd862-7a6d-4b42-8e56-48e13053028d)  |

## Line renderer

![image](https://github.com/jnch009/tinyrenderer-jnch009/assets/13592249/1fae3683-c7bd-45f8-b779-3fcb5756d730)

---

# Learnings:
## Triangle Rasterization
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

2. For scanline rendering <TODO continue to add to this>
    
    // Can we have two separate loops storing the unique points and then use the other loop to find the y coordinates that match
	// In order to do this we need to figure out which line has the steeper slope
	// The line with the steeper slope will obviously hit the limit faster

	// we don't need to find the steeper slope, in the line function above, I simply take all Vec2i where the y values are unique