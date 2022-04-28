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


Material StoneTile4_MAT {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "StoneBlock_4_COLOR_v3_COLOR.tga";
 }
}

Frame CoreGeo_part_00 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh CoreGeo_part_00 {
  12;
  -3017.99475;-128.000000;-2496.00154;,
  -2968.00000;-128.000000;-2694.00243;,
  -3017.99501;-128.000000;-2694.00269;,
  -3033.98886;-128.000000;-1344.00346;,
  -3033.98938;-128.000000;-2496.00128;,
  -3017.99501;-128.000000;-1344.00397;,
  -2751.99514;-128.000102;-898.00486;,
  -2751.99616;-128.000000;-2880.00000;,
  -2967.99616;-128.000000;-2880.00000;,
  -3017.99014;-128.000102;-898.00499;,
  -2093.99296;-128.000192;-1536.00179;,
  -2093.99296;-128.000397;-2304.00102;;
  10;
  3;0,1,2;,
  3;3,0,4;,
  3;0,3,5;,
  3;6,0,5;,
  3;6,1,0;,
  3;6,7,1;,
  3;1,7,8;,
  3;6,5,9;,
  3;7,10,11;,
  3;10,7,6;;

  MeshMaterialList {
   1;
   10;
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
   { StoneTile4_MAT }
  }

  DeclData {
   3;
   2;0;3;0;,
   1;0;5;0;,
   1;0;5;1;;
   84;
   874272753,
   1065353216,
   857597870,
   1083564032,
   1080328192,
   1061994496,
   1023418368,
   856739596,
   1065353216,
   854545804,
   1085071360,
   1081090048,
   1063641088,
   1034575872,
   0,
   1065353216,
   0,
   1085071360,
   1080328192,
   1063641088,
   1023418368,
   0,
   1065353216,
   0,
   1057251328,
   1080090624,
   1047265280,
   1014128640,
   0,
   1065353216,
   0,
   1083564032,
   1080090624,
   1061994496,
   1014022144,
   0,
   1065353216,
   0,
   1057251328,
   1080328192,
   1047265280,
   1023418368,
   880625383,
   1065353216,
   858063992,
   3213647872,
   1083244544,
   1008975872,
   1050492928,
   877264257,
   1065353216,
   2992608403,
   1086480384,
   1083244544,
   1065181184,
   1050492928,
   0,
   1065353216,
   0,
   1086480384,
   1081090048,
   1065181184,
   1034575872,
   2851568741,
   1065353216,
   880841478,
   3213647872,
   1080328192,
   1008975872,
   1023418368,
   883512777,
   1065353216,
   2984943335,
   1067130880,
   1088241664,
   1013596160,
   1065181184,
   895388216,
   1065353216,
   3028986549,
   1082089472,
   1088241664,
   1065132032,
   1065181184;
  }
 }
}