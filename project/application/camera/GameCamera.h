#pragma once
#include <vector>
#include <memory>
#include <Camera.h>
#include <Vector3.h>
#include <CurveJsonLoader.h>

class GameCamera {
public:
    // 初期化
    void Initialize(Vector3 offset);
    // 更新処理
    void Update();

	// ベジェ曲線の制御点を設定
    Vector3 BezierInterpolate(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

    float GetTForDistance(float distance) const;
    
    void CalculateCurveLength();
    void UpdateObjectPosition();

private:
    CurveJsonLoader* Jsondata = nullptr;
    Camera* camera_ = nullptr;

    	
    Vector3 moveOffset_; // カメラの移動オフセット
    Vector3 bezierPos_;
    bool useFollowCamera_ = false; // カメラモード切替用フラグ
    std::vector<BezierPoint> bezierPoints; 
    float t = 0.0f;
    
    
	bool movefige; // ベジェ曲線に沿って移動するフラグ     
    int segmentIndex = 0; // 今は1つだけと仮定、複数ならループ管理
    // ヘッダーかクラス内に追加
    bool addedInitialOffset_ = false;      
    std::vector<float> curveLengths;  // 曲線を細分割した累積距離テーブル
    float totalCurveLength = 0.0f;    // 曲線全体の長さ
    float distanceAlongCurve = 0.0f;  // 現在の距離位置
	float speed = 0.2f; // 移動の速さ
public:

    // getter 
	Camera* Getcamera() { return camera_; }
    bool Getmovefige() { return movefige; }
    Vector3 GetbezierPos() { return bezierPos_; }

    // setter
    void Setmovefige(bool value) { movefige = value; }
    void SetbezierPos(Vector3 pos) { bezierPos_ = pos; }
};