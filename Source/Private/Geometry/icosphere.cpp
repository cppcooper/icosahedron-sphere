#include "icosphere.h"
#include "core.h"
#include <array>


icosphere::icosphere(const icosphere &other){
    LOGINIT(DColor::Green);
    m_vertices = TArray<FVector>(other.m_vertices);
    m_uvmapping = TArray<FVector2D>(other.m_uvmapping);
    m_triangles = TArray<Triangle>(other.m_triangles);
}

icosphere::icosphere( uint8 subdivisions )
{
    LOGINIT(DColor::Green);
    make_icosphere( subdivisions );
}

icosphere::~icosphere()
{
    //nothing to do, the TArrays will clean up themselves
}

void FindUV( const FVector &normal, FVector2D &uv )
{
    const float &x = normal.X;
    const float &y = normal.Y;
    const float &z = normal.Z;
    float normalisedX = 0;
    float normalisedZ = -1;
    if( ((x * x) + (z * z)) > 0 )
    {
        normalisedX = sqrt( (x * x) / ((x * x) + (z * z)) );
        if( x < 0 )
        {
            logVeryVerboseC(Geometry,DColor::Yellow,false,"Inverting normalized X");
            normalisedX = -normalisedX;
        }
        normalisedZ = sqrt( (z * z) / ((x * x) + (z * z)) );
        if( z < 0 )
        {
            logVeryVerboseC(Geometry,DColor::Yellow,false,"Inverting normalized Z");
            normalisedZ = -normalisedZ;
        }
    }
    if( normalisedZ == 0 )
    {
        uv.X = ((normalisedX * PI) / 2);
    }
    else
    {
        uv.X = atan( normalisedX / normalisedZ );
        if( normalisedX < 0 )
        {
            uv.X = PI - uv.X;
        }
        if( normalisedZ < 0 )
        {
            uv.X += PI;
        }
    }
    if( uv.X < 0 )
    {
        uv.X += 2 * PI;
    }
    uv.X /= 2 * PI;
    uv.Y = (-y + 1) / 2;
    logVeryVerbose(Geometry,"\nnorm.x = %.15f\nnorm.y = %.15f\nnorm.z = %.15f\nnormalizedX = %.15f\nnormalizedZ = %.15f\nU = %.15f\nV = %.15f", x,y,z,normalisedX,normalisedZ,uv.X,uv.Y);
}

void icosphere::make_icosphere( uint8 subdivisions )
{
    logInfoC(Geometry,DColor::Cyan,true,"Making icosphere with (%d) subdivisions.", subdivisions);
    m_vertices.Reset( 16 );
    m_triangles.Reset( 20 );

    logInfo(Geometry, "Setting up icosahedron.");
    m_vertices.Append( icosahedron::vertices, ARRAY_COUNT( icosahedron::vertices ) );
    m_triangles.Append( icosahedron::triangles, ARRAY_COUNT( icosahedron::triangles ) );
    normalize(); //just to be sure
    for( int i = 0; i < subdivisions; ++i )
    {
        logInfo(Geometry,"Subdividing icosphere\niteration: %d\ncurrent vertex count: %d",i,m_vertices.Num());
        subdivide();
    }
    mapuv();
}

void icosphere::normalize()
{
    logInfoC(Geometry,DColor::Cyan,true,"Normalizing vertices.")
    for( FVector &each : m_vertices )
    {
        each.Normalize();
    }
    logInfoC(Geometry,DColor::Cyan,true,"Normalized vertices, sphere should be unit-sphere now.")
}

//todo: add log lines
uint32 icosphere::vertex_for_edge( uint32 vi1, uint32 vi2 )
{

    uint32 a = std::min( vi1, vi2 );
    uint32 b = std::max( vi1, vi2 );
    std::pair<uint32, uint32> key( a, b );

    auto inserted = lookup.insert( {key, m_vertices.Num()} );
    if( inserted.second )
    {
        FVector& edge0 = m_vertices[a];
        FVector& edge1 = m_vertices[b];
        auto point = edge0 + edge1;
        point.Normalize();
        m_vertices.Add( point );
    }

    return inserted.first->second;
}

//todo: add log lines
void icosphere::subdivide()
{
    TArray<Triangle> swap_sphere;
    swap_sphere.Reserve( m_triangles.Num() * 3 );

    for( auto&& triangle : m_triangles )
    {
        std::array<int, 3> mid;
        for( int edge = 0; edge < 3; ++edge )
        {
            mid[edge] = vertex_for_edge( triangle.vert[edge], triangle.vert[(edge + 1) % 3] );
        }

        swap_sphere.Add( {triangle.vert[0], mid[0], mid[2]} );
        swap_sphere.Add( {triangle.vert[1], mid[1], mid[0]} );
        swap_sphere.Add( {triangle.vert[2], mid[2], mid[1]} );
        swap_sphere.Add( {mid[0], mid[1], mid[2]} );
    }
    Swap( m_triangles, swap_sphere ); // no new memory needed
    lookup.clear();
}

void icosphere::mapuv()
{
    logInfoC(Geometry,DColor::Cyan,true,"Creating UV Mapping");
    m_uvmapping.Empty(m_vertices.Num());
    for( int32 i = 0; i < m_vertices.Num(); ++i )
    {
        FindUV( m_vertices[i], *(m_uvmapping.GetData() + i) );
    }
}
