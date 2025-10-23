#pragma once  
#include <Vector3.h>  
#include <cmath>  
#include <corecrt_math_defines.h>

/// <summary>  
/// 3x3行列を表すクラス  
/// </summary>  
class Matrix3x3 {  
public:  
  float m[3][3];  

  // コンストラクタ（ゼロ初期化）  
  Matrix3x3() {  
      for (int i = 0; i < 3; ++i)  
          for (int j = 0; j < 3; ++j)  
              m[i][j] = 0.0f;  
  }  

  /// <summary>  
  /// 指定した列にベクトルを設定する  
  /// </summary>  
  /// <param name="index">列インデックス（0～2）</param>  
  /// <param name="vec">設定するベクトル</param>  
  void SetColumn(int index, const Vector3& vec) {  
      if (index < 0 || index > 2) return; // 範囲外チェック  
      m[0][index] = vec.x;  
      m[1][index] = vec.y;  
      m[2][index] = vec.z;  
  }  

  /// <summary>  
  /// 回転行列をオイラー角（XYZ順）に変換する  
  /// </summary>  
  /// <returns>オイラー角を表すVector3</returns>  
  Vector3 ToEulerXYZ() const {  
      Vector3 euler;  
      if (m[2][0] < 1) {  
          if (m[2][0] > -1) {  
              euler.x = std::atan2(-m[2][1], m[2][2]);  
              euler.y = std::asin(m[2][0]);  
              euler.z = std::atan2(-m[1][0], m[0][0]);  
          } else {  
              // m[2][0] == -1  
              euler.x = std::atan2(m[0][1], m[1][1]);  
              euler.y = -M_PI / 2;  
              euler.z = 0;  
          }  
      } else {  
          // m[2][0] == 1  
          euler.x = std::atan2(m[0][1], m[1][1]);  
          euler.y = M_PI / 2;  
          euler.z = 0;  
      }  
      return euler;  
  }  
};