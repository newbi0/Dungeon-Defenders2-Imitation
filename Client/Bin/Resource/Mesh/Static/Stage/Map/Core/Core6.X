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


Material PaintedStone_1_MAT {
 1.000000;1.000000;1.000000;1.000000;;
 10.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "PaintedStone_1.tga";
 }
}

Frame CoreGeo_part_06 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh CoreGeo_part_06 {
  13;
  1280.00000;-124.000499;-3328.00026;,
  1280.00026;-768.00045;-2303.99949;,
  1280.00000;-124.000397;-2304.00051;,
  1279.99962;-768.00051;-3328.00026;,
  511.99952;-124.000397;-3583.99949;,
  1023.99878;-768.00051;-3584.00026;,
  1023.99872;-124.000499;-3583.99974;,
  725.55782;-768.00051;-3584.00000;,
  511.99962;-768.00051;-3583.99949;,
  1023.99872;-124.000499;-3583.99974;,
  1279.99962;-768.00051;-3328.00026;,
  1280.00000;-124.000499;-3328.00026;,
  1023.99878;-768.00051;-3584.00026;;
  7;
  3;0,1,2;,
  3;1,0,3;,
  3;4,5,6;,
  3;7,5,4;,
  3;7,4,8;,
  3;9,10,11;,
  3;10,9,12;;

  MeshMaterialList {
   1;
   7;
   0,
   0,
   0,
   0,
   0,
   0,
   0;
   { PaintedStone_1_MAT }
  }

  DeclData {
   3;
   2;0;3;0;,
   1;0;5;0;,
   1;0;5;1;;
   91;
   3212836864,
   3033237288,
   691148358,
   3233595392,
   1085652992,
   1058127872,
   1065181184,
   3212836864,
   3033237288,
   691148358,
   3228876800,
   1088389120,
   1008762880,
   1008975872,
   3212836864,
   3033237288,
   691148358,
   3228876800,
   1085652992,
   1008762880,
   1065181184,
   3212836864,
   890807645,
   891289591,
   3233595392,
   1088389120,
   1058127872,
   1008975872,
   897552513,
   3038123074,
   1065353216,
   3236855808,
   1085652992,
   1065172992,
   1065181184,
   889192458,
   3041625895,
   1065353216,
   3234684928,
   1088389120,
   1060478976,
   1008975872,
   889192458,
   3041625895,
   1065353216,
   3234684928,
   1085652992,
   1060478976,
   1065181184,
   901529599,
   3030211093,
   1065353216,
   3235946496,
   1088389120,
   1063215104,
   1008975872,
   907636787,
   716634997,
   1065353216,
   3236855808,
   1088389120,
   1065172992,
   1008975872,
   3207922897,
   866101487,
   1060439318,
   3234684928,
   1085652992,
   1060478976,
   1065181184,
   3207922905,
   3029588586,
   1060439310,
   3233595392,
   1088389120,
   1058127872,
   1008975872,
   3207922890,
   886562700,
   1060439324,
   3233595392,
   1085652992,
   1058127872,
   1065181184,
   3207922911,
   3038898141,
   1060439304,
   3234684928,
   1088389120,
   1060478976,
   1008975872;
  }
 }
}