# Icosphere for UE4
Takes an icosahedron, subdivides it N times to create a sphere-like geometry. While sub-dividing, the vertices are normalized to ensure a unit sphere is formed. An interface is provided to access the TArrays holding the indexed vertices for each triangle on the icosphere, and the vertices themselves. UV-mapping is dealt with, and normals are merely the vertices themselves (normalized if the sphere is scaled).

All instances of the PawnBase will create their own ProceduralMeshComponent, copy the vertices, triangles, and anything else it needs to from a shared instance of the icosphere. Each PawnBase will then make any modifications to the sphere using their local copy.

Code adapted from what is available here: https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/

dead-link safety: {author="Marius Elvert", date=2016, company="softwareschneiderei"}
