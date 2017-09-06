#ifndef __ML_MESH_TYPE_H
#define __ML_MESH_TYPE_H

#include <vcg/complex/complex.h>

namespace vcg
{
    namespace vertex
    {
        template <class T> class Coord3m: public Coord<Point3f, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("Coord3m"));T::Name(name);}
        };

        template <class T> class Normal3m: public Normal<Point3f, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("Normal3m"));T::Name(name);}
        };

        template <class T> class CurvatureDirmOcf: public CurvatureDirOcf<CurvatureDirTypeOcf<float>, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("CurvatureDirmOcf"));T::Name(name);}
        };

        template <class T> class RadiusmOcf: public RadiusOcf<float, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("RadiusmOcf"));T::Name(name);}
        };

    }//end namespace vertex
    namespace face
    {
        template <class T> class Normal3m: public NormalAbs<Point3f, T> {
        public:  static void Name(std::vector<std::string> & name){name.push_back(std::string("Normal3m"));T::Name(name);}
        };

        template <class T> class CurvatureDirmOcf: public CurvatureDirOcf<CurvatureDirOcfBaseType<float>, T> {
        public:	static void Name(std::vector<std::string> & name){name.push_back(std::string("CurvatureDirdOcf"));T::Name(name);}
        };

    }//end namespace face
}//end namespace vcg

// Forward declarations needed for creating the used types
class CVertexO;
class CEdgeO;
class CFaceO;

// Declaration of the semantic of the used types
class CUsedTypesO: public vcg::UsedTypes < vcg::Use<CVertexO>::AsVertexType,
    vcg::Use<CEdgeO   >::AsEdgeType,
    vcg::Use<CFaceO  >::AsFaceType >{};


// The Main Vertex Class
// Most of the attributes are optional and must be enabled before use.
// Each vertex needs 40 byte, on 32bit arch. and 44 byte on 64bit arch.

class CVertexO  : public vcg::Vertex< CUsedTypesO,
    vcg::vertex::InfoOcf,           /*  4b */
    vcg::vertex::Coord3m,           /* 12b */
    vcg::vertex::BitFlags,          /*  4b */
    vcg::vertex::Normal3m,          /* 12b */
    vcg::vertex::Qualityf,          /*  4b */
    vcg::vertex::Color4b,           /*  4b */
    vcg::vertex::VFAdjOcf,          /*  0b */
    vcg::vertex::MarkOcf,           /*  0b */
    vcg::vertex::TexCoordfOcf,      /*  0b */
    vcg::vertex::CurvaturefOcf,     /*  0b */
    vcg::vertex::CurvatureDirmOcf,  /*  0b */
    vcg::vertex::RadiusmOcf         /*  0b */
>{
};


// The Main Edge Class
class CEdgeO : public vcg::Edge<CUsedTypesO,
    vcg::edge::BitFlags,          /*  4b */
    vcg::edge::EVAdj,
    vcg::edge::EEAdj
>{
};

// Each face needs 32 byte, on 32bit arch. and 48 byte on 64bit arch.
class CFaceO    : public vcg::Face<  CUsedTypesO,
    vcg::face::InfoOcf,              /* 4b */
    vcg::face::VertexRef,            /*12b */
    vcg::face::BitFlags,             /* 4b */
    vcg::face::Normal3m,             /*12b */
    vcg::face::QualityfOcf,          /* 0b */
    vcg::face::MarkOcf,              /* 0b */
    vcg::face::Color4bOcf,           /* 0b */
    vcg::face::FFAdjOcf,             /* 0b */
    vcg::face::VFAdjOcf,             /* 0b */
    vcg::face::CurvatureDirmOcf,     /* 0b */
    vcg::face::WedgeTexCoordfOcf     /* 0b */
> {};


class CMeshO    : public vcg::tri::TriMesh< vcg::vertex::vector_ocf<CVertexO>, vcg::face::vector_ocf<CFaceO> > {};

#endif
