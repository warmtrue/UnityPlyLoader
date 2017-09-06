#include "PlyLoader.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include <vcg/math/base.h>
#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <vcg/complex/algorithms/update/texture.h>
#include <vcg/complex/algorithms/attribute_seam.h>

using namespace vcg;

/////// FUNCTIONS NEEDED BY "UV WEDGE TO VERTEX" FILTER
inline void ExtractVertex(const CMeshO & srcMesh, const CMeshO::FaceType & f, int whichWedge, const CMeshO & dstMesh, CMeshO::VertexType & v)
{
	(void)srcMesh;
	(void)dstMesh;
	// This is done to preserve every single perVertex property
	// perVextex Texture Coordinate is instead obtained from perWedge one.
	v.ImportData(*f.cV(whichWedge));
	v.T() = f.cWT(whichWedge);
}

inline bool CompareVertex(const CMeshO & m, const CMeshO::VertexType & vA, const CMeshO::VertexType & vB)
{
	(void)m;
	return (vA.cT() == vB.cT());
}

PlyFileObject::PlyFileObject(const char* fileName)
{
	int mask = 0;
	tri::io::ImporterPLY<CMeshO>::LoadMask(fileName, mask);
	if (mask & tri::io::Mask::IOM_WEDGCOLOR)
		mask |= tri::io::Mask::IOM_FACECOLOR;

	Enable(mask);
	bool isWedgetexture = (mask & tri::io::Mask::IOM_WEDGTEXCOORD) != 0;
	tri::io::ImporterPLY<CMeshO>::Open(cm, fileName, mask, NULL);
	if (isWedgetexture)
	{
		// change per face uv to per vertex uv
		int vn = cm.vn;
		cm.vert.EnableTexCoord();
		tri::AttributeSeam::SplitVertex(cm, ExtractVertex, CompareVertex);
	}

	// read data 
	verts.clear();
	norms.clear();
	colors.clear();
	uvCoords.clear();
	faces.clear();

	for (int i = 0; i < cm.vert.size(); i++)
	{
		Point3f position = cm.vert[i].P();
		verts.push_back(position.X());
		verts.push_back(position.Y());
		verts.push_back(position.Z());
		if (cm.vert[i].IsNormalEnabled())
		{
			Point3f normal = cm.vert[i].N();
			norms.push_back(normal.X());
			norms.push_back(normal.Y());
			norms.push_back(normal.Z());
		}
		if (cm.vert[i].IsColorEnabled())
		{
			Color4b color = cm.vert[i].C();
			colors.push_back(color[0]);
			colors.push_back(color[1]);
			colors.push_back(color[2]);
			colors.push_back(color[3]);
		}
		if (cm.vert[i].IsTexCoordEnabled())
		{
			TexCoord2f uv = cm.vert[i].T();
			uvCoords.push_back(uv.u());
			uvCoords.push_back(uv.v());
		}
	}

	for (int i = 0; i < cm.fn; i++)
		for (int k = 0; k < 3; k++)
			faces.push_back((unsigned int)(cm.face[i].V(k) - &*cm.vert.begin()));
}

void PlyFileObject::Enable(int openingFileMask)
{
	if (openingFileMask & tri::io::Mask::IOM_VERTTEXCOORD)
		updateDataMask(MM_VERTTEXCOORD);
	if (openingFileMask & tri::io::Mask::IOM_WEDGTEXCOORD)
		updateDataMask(MM_WEDGTEXCOORD);
	if (openingFileMask & tri::io::Mask::IOM_VERTCOLOR)
		updateDataMask(MM_VERTCOLOR);
	if (openingFileMask & tri::io::Mask::IOM_FACECOLOR)
		updateDataMask(MM_FACECOLOR);
	if (openingFileMask & tri::io::Mask::IOM_VERTRADIUS) updateDataMask(MM_VERTRADIUS);
	if (openingFileMask & tri::io::Mask::IOM_CAMERA) updateDataMask(MM_CAMERA);
	if (openingFileMask & tri::io::Mask::IOM_VERTQUALITY) updateDataMask(MM_VERTQUALITY);
	if (openingFileMask & tri::io::Mask::IOM_FACEQUALITY) updateDataMask(MM_FACEQUALITY);
	if (openingFileMask & tri::io::Mask::IOM_BITPOLYGONAL) updateDataMask(MM_POLYGONAL);
}

void PlyFileObject::updateDataMask(int neededDataMask)
{
	if ((neededDataMask & MM_FACEFACETOPO) != 0)
	{
		cm.face.EnableFFAdjacency();
		tri::UpdateTopology<CMeshO>::FaceFace(cm);
	}
	if ((neededDataMask & MM_VERTFACETOPO) != 0)
	{
		cm.vert.EnableVFAdjacency();
		cm.face.EnableVFAdjacency();
		tri::UpdateTopology<CMeshO>::VertexFace(cm);
	}

	if ((neededDataMask & MM_WEDGTEXCOORD) != 0)
		cm.face.EnableWedgeTexCoord();
	if ((neededDataMask & MM_FACECOLOR) != 0)
		cm.face.EnableColor();
	if ((neededDataMask & MM_FACEQUALITY) != 0)
		cm.face.EnableQuality();
	if ((neededDataMask & MM_FACECURVDIR) != 0)
		cm.face.EnableCurvatureDir();
	if ((neededDataMask & MM_FACEMARK) != 0)
		cm.face.EnableMark();
	if ((neededDataMask & MM_VERTMARK) != 0)
		cm.vert.EnableMark();
	if ((neededDataMask & MM_VERTCURV) != 0)
		cm.vert.EnableCurvature();
	if ((neededDataMask & MM_VERTCURVDIR) != 0)
		cm.vert.EnableCurvatureDir();
	if ((neededDataMask & MM_VERTRADIUS) != 0)
		cm.vert.EnableRadius();
	if ((neededDataMask & MM_VERTTEXCOORD) != 0)
		cm.vert.EnableTexCoord();
}

__declspec(dllexport) PlyFileObject* LoadPly(const char* fileName)
{
	return new PlyFileObject(fileName);
}

__declspec(dllexport) void UnLoadPly(PlyFileObject* plyFile)
{
	delete plyFile;
}

__declspec(dllexport) float* GetPlyVerts(PlyFileObject* plyFile, unsigned int& count)
{
	if (plyFile == NULL)
		return NULL;

	count = (unsigned int)plyFile->verts.size();
	return plyFile->verts.data();
}

__declspec(dllexport) float* GetPlyNormals(PlyFileObject* plyFile, unsigned int& count)
{
	if (plyFile == NULL)
		return NULL;

	count = (unsigned int)plyFile->norms.size();
	return plyFile->norms.data();
}

__declspec(dllexport) unsigned char* GetPlyColors(PlyFileObject* plyFile, unsigned int& count)
{
	if (plyFile == NULL)
		return NULL;

	count = (unsigned int)plyFile->colors.size();
	return plyFile->colors.data();
}

__declspec(dllexport) unsigned int* GetPlyIndexs(PlyFileObject* plyFile, unsigned int& count)
{
	if (plyFile == NULL)
		return NULL;

	count = (unsigned int)plyFile->faces.size();
	return plyFile->faces.data();
}

__declspec(dllexport) float* GetPlyUvs(PlyFileObject* plyFile, unsigned int& count)
{
	if (plyFile == NULL)
		return NULL;

	count = (unsigned int)plyFile->uvCoords.size();
	return plyFile->uvCoords.data();
}