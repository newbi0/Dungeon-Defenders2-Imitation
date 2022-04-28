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


Material WoodBoards_Desaturated_MAT {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "WoodBoards_1_COLOR.tga";
 }
}

Frame CoreGeo_part_17 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh CoreGeo_part_17 {
  12;
  -1691.99411;-128.000307;-1790.00474;,
  -2093.99488;-128.000307;-2050.00371;,
  -2093.99386;-128.000307;-1790.00499;,
  -1691.99488;-128.000307;-2050.00397;,
  -1535.99488;-286.000384;-2050.00294;,
  -1691.99411;-128.000307;-1790.00474;,
  -1535.99437;-286.000486;-1790.00397;,
  -1691.99488;-128.000307;-2050.00397;,
  -1535.99539;-448.00032;-2050.00294;,
  -1535.99437;-286.000486;-1790.00397;,
  -1535.99488;-448.00032;-1790.00397;,
  -1535.99488;-286.000384;-2050.00294;;
  6;
  3;0,1,2;,
  3;1,0,3;,
  3;4,5,6;,
  3;5,4,7;,
  3;8,9,10;,
  3;9,8,11;;

  MeshMaterialList {
   1;
   6;
   0,
   0,
   0,
   0,
   0,
   0;
   { WoodBoards_Desaturated_MAT }
  }

  DeclData {
   3;
   2;0;3;0;,
   1;0;5;0;,
   1;0;5;1;;
   84;
   0,
   1065353216,
   0,
   1073725440,
   3186081792,
   1054982144,
   1029488640,
   0,
   1065353216,
   0,
   1065369600,
   1065336832,
   1064296448,
   1064411136,
   0,
   1065353216,
   0,
   1073725440,
   1065336832,
   1064296448,
   1029488640,
   0,
   1065353216,
   0,
   1065369600,
   3186081792,
   1054982144,
   1064411136,
   1060514619,
   1060363465,
   3046254936,
   1065369600,
   3205292032,
   1048313856,
   1064411136,
   1060514619,
   1060363465,
   3046254936,
   1073725440,
   3186081792,
   1054982144,
   1029488640,
   1060514619,
   1060363466,
   3046254936,
   1073725440,
   3205292032,
   1048313856,
   1029488640,
   1060514607,
   1060363478,
   3053880934,
   1065369600,
   3186081792,
   1054982144,
   1064411136,
   1065353216,
   3058320787,
   3053195277,
   1065369600,
   3212812288,
   1027416064,
   1064411136,
   1065353216,
   3058320788,
   3053195275,
   1073725440,
   3205292032,
   1048313856,
   1029488640,
   1065353216,
   3058320791,
   3053195271,
   1073725440,
   3212812288,
   1027416064,
   1029488640,
   1065353216,
   3058320784,
   3053195281,
   1065369600,
   3205292032,
   1048313856,
   1064411136;
  }
 }
}