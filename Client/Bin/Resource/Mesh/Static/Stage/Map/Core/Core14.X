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

Frame CoreGeo_part_14 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh CoreGeo_part_14 {
  12;
  -2560.00102;-127.999898;891.14720;,
  -1279.99898;-127.999898;510.39440;,
  -2560.00102;-127.999898;509.99379;,
  -1279.99898;-127.999898;891.34880;,
  -510.99661;-127.999898;510.99680;,
  -1279.99898;-127.999898;1279.99923;,
  -509.99341;-127.999898;2560.00102;,
  -891.50067;-127.999898;1279.99898;,
  -891.14675;-127.999898;2560.00077;,
  -510.69558;-127.999898;1279.99872;,
  -510.99661;-127.999898;510.99680;,
  -1279.99898;-127.999898;1279.99923;;
  8;
  3;0,1,2;,
  3;1,0,3;,
  3;3,4,1;,
  3;3,5,4;,
  3;6,7,8;,
  3;7,6,9;,
  3;9,10,7;,
  3;7,10,11;;

  MeshMaterialList {
   1;
   8;
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
   0,
   1065353216,
   0,
   3237101568,
   1084702720,
   1051402240,
   1025998848,
   0,
   1065353216,
   0,
   3234209792,
   1092468736,
   1026670592,
   1058889728,
   0,
   1065353216,
   0,
   3234209792,
   1084702720,
   1026588672,
   1025998848,
   0,
   1065353216,
   0,
   3237101568,
   1092468736,
   1051410432,
   1058889728,
   0,
   1065353216,
   0,
   3234217984,
   1095385088,
   1026801664,
   1064681472,
   0,
   1065353216,
   0,
   3239026688,
   1092468736,
   1059168256,
   1058889728,
   0,
   1065353216,
   0,
   3243884544,
   1095385088,
   1064681472,
   1064681472,
   0,
   1065353216,
   0,
   3239026688,
   1093943296,
   1059790848,
   1053122560,
   0,
   1065353216,
   0,
   3243884544,
   1093943296,
   1059790848,
   1064681472,
   0,
   1065353216,
   0,
   3239026688,
   1095385088,
   1064673280,
   1053122560,
   0,
   1065353216,
   0,
   3234217984,
   1095385088,
   1064665088,
   1025941504,
   0,
   1065353216,
   0,
   3239026688,
   1092468736,
   1052655616,
   1053122560;
  }
 }
}