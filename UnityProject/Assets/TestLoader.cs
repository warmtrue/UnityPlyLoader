using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TestLoader : MonoBehaviour
{
    public string fileName;
    public Texture modelTextrue;

    // Use this for initialization
    void Start()
    {
        string filePath = System.IO.Path.Combine(Application.streamingAssetsPath, fileName);
        IntPtr plyIntPtr = PlyLoaderDll.LoadPly(filePath);

        Mesh mesh = new Mesh();
        mesh.vertices = PlyLoaderDll.GetVertices(plyIntPtr);
        mesh.uv = PlyLoaderDll.GetUvs(plyIntPtr);
        mesh.normals = PlyLoaderDll.GetNormals(plyIntPtr);
        mesh.colors32 = PlyLoaderDll.GetColors(plyIntPtr);
        mesh.SetIndices(PlyLoaderDll.GetIndexs(plyIntPtr), MeshTopology.Triangles, 0, true);
        mesh.name = "mesh";

        GameObject g = new GameObject();
        g.name = "meshNew";
        MeshFilter mf = g.AddComponent<MeshFilter>();
        mf.mesh = mesh;
        MeshRenderer mr = g.AddComponent<MeshRenderer>();
        mr.material = new Material(Shader.Find("Unlit/Texture"));
        mr.material.mainTexture = modelTextrue;

        PlyLoaderDll.UnLoadPly(plyIntPtr);
    }
}