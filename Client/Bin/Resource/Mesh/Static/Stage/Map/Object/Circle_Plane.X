xof 0303txt 0032
template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}

template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template VertexElement {
 <f752461c-1e23-48f6-b9f8-8350850f336f>
 DWORD Type;
 DWORD Method;
 DWORD Usage;
 DWORD UsageIndex;
}

template DeclData {
 <bf22e553-292c-4781-9fea-62bd554bdd93>
 DWORD nElements;
 array VertexElement Elements[nElements];
 DWORD nDWords;
 array DWORD data[nDWords];
}


Material DistanceFog_alt {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "flare_01_o.tga";
 }
}

Frame CirclePlane {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh CirclePlane {
  18;
  294.903987;0.000000;0.000000;,
  225.909606;0.000000;189.560589;,
  277.119104;0.000000;100.863098;,
  -147.451994;0.000000;255.394304;,
  -277.119104;0.000000;100.863098;,
  -225.909606;0.000000;189.560589;,
  -277.119104;0.000000;-100.863002;,
  51.209402;0.000000;-290.423808;,
  225.909402;0.000000;-189.560806;,
  277.118976;0.000000;-100.863296;,
  147.451802;0.000000;-255.394406;,
  -294.903987;0.000000;0.000000;,
  51.209501;0.000000;290.423706;,
  -51.209600;0.000000;290.423706;,
  147.451994;0.000000;255.394406;,
  -147.452109;0.000000;-255.394304;,
  -225.909606;0.000000;-189.560589;,
  -51.209702;0.000000;-290.423706;;
  16;
  3;0,1,2;,
  3;3,4,5;,
  3;3,6,4;,
  3;1,6,3;,
  3;1,7,6;,
  3;0,7,1;,
  3;0,8,7;,
  3;9,8,0;,
  3;7,8,10;,
  3;4,6,11;,
  3;12,3,13;,
  3;1,3,12;,
  3;1,12,14;,
  3;6,15,16;,
  3;6,7,15;,
  3;15,7,17;;

  MeshMaterialList {
   1;
   16;
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0;
   { DistanceFog_alt }
  }

  DeclData {
   2;
   2;0;3;0;,
   1;0;5;0;;
   90;
   0,
   1065353216,
   0,
   1056964608,
   939524096,
   0,
   1065353216,
   0,
   1043783680,
   1039106048,
   0,
   1065353216,
   0,
   1051222016,
   1022844928,
   0,
   1065353216,
   0,
   1032396800,
   1061158912,
   0,
   1065353216,
   0,
   1051222016,
   1064845312,
   0,
   1065353216,
   0,
   1043783680,
   1063387136,
   0,
   1065353216,
   0,
   1059831808,
   1064845312,
   0,
   1065353216,
   0,
   1065222144,
   1054048256,
   0,
   1065353216,
   0,
   1062354944,
   1039106048,
   0,
   1065353216,
   0,
   1059831808,
   1022844928,
   0,
   1065353216,
   0,
   1064222720,
   1048576000,
   0,
   1065353216,
   0,
   1056964608,
   1065353216,
   0,
   1065353216,
   0,
   1006174208,
   1054048256,
   0,
   1065353216,
   0,
   1006174208,
   1058414592,
   0,
   1065353216,
   0,
   1032396800,
   1048576000,
   0,
   1065353216,
   0,
   1064222720,
   1061158912,
   0,
   1065353216,
   0,
   1062354944,
   1063387136,
   0,
   1065353216,
   0,
   1065222144,
   1058414592;
  }
 }
}