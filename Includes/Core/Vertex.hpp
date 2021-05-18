#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <vector>

namespace Core {

    //! Vertex format enums.
    enum class VertexFormat 
    {
        //! None
        None = 0,
        
        //! Position in 3D.
        Position3 = 1 << 0,

        //! Normal in 3D.
        Normal3 = 1 << 1,

        //! Texture coordinates in 2D.
        TexCoord2 = 1 << 2,

        //! Texture coordinates in 3D.
        TexCoord3 = 1 << 3,

        //! Color in RGBA (4D).
        Color4 = 1 << 4,

        //! Tangents in 4D.
        Tangent4 = 1 << 5,

        //! Last format
        Last = 1 << 6,

        //! Position (3D) and normal (3D).
        Position3Normal3 = Position3 | Normal3,

        //! Position (3D) and texture coordinates (2D).
        Position3TexCoord2 = Position3 | TexCoord2,

        //! Position (3D) and texture coordinates (3D).
        Position3TexCoord3 = Position3 | TexCoord3,

        //! Position (3D) and color in RGBA (4D).
        Position3Color4 = Position3 | Color4,

        //! Position (3D), normal (3D), and texture coordinates (2D).
        Position3Normal3TexCoord2 = Position3Normal3 | TexCoord2,

        //! Position (3D), normal (3D), and color in RGBA (4D).
        Position3Normal3Color4 = Position3Normal3 | Color4,

        //! Position (3D), texture coordinates (2D), and color in RGBA (4D).
        Position3TexCoord2Color4 = Position3TexCoord2 | Color4,

        //! Position (3D), normal (3D), texture coordinates (2D), and color in RGBA (4D).
        Position3Normal3TexCoord2Color4 = Position3Normal3TexCoord2 | Color4,

        //! Position (3D), normal (3D), and tangent coordinates (4D).
        Position3Normal3Tangent4 = Position3Normal3 | Tangent4,

        //! Position (3D), normal (3D), texture coordinates (2D), and tangent coordinates (4D).
        Position3Normal3TexCoord2Tangent4 = Position3Normal3TexCoord2 | Tangent4,

        //! Position (3D), normal (3D), texture coordinates (2D), color in RGBA (4D), and tangent coordinates (4D).
        Position3Normal3TexCoord2Color4Tangent4 = Position3Normal3TexCoord2Color4 | Tangent4,
    };

    //! Bit-wise operator for two vertex formats
    inline VertexFormat operator|(VertexFormat a, VertexFormat b)
    {
        return static_cast<VertexFormat>(static_cast<int>(a) | static_cast<int>(b));
    }

    //! Bit-wise operator for two vertex formats.
    inline void operator|=(VertexFormat& a, const VertexFormat& b)
    {
        a = static_cast<VertexFormat>(static_cast<int>(a) | static_cast<int>(b));
    }

    //! Bit-wise operator for two vertex formats.
    inline VertexFormat operator&(VertexFormat a, VertexFormat b) 
    {
        return static_cast<VertexFormat>(static_cast<int>(a) & static_cast<int>(b));
    }

    //!
    //! \brief      Collection of vertex helper functions.
    //!
    class VertexHelper
    {
     public:
        //! Returns number of floats for a single vertex with given vertex format.
        static std::size_t GetNumberOfFloats(VertexFormat format);

        //! Returns size of a single vertex with given format in bytes.
        static std::size_t GetSizeInBytes(VertexFormat format);
    };

}  

#endif //! End of Vertex.hpp