//#pragma once
//#include <CurveJsonLoader.h>
//
//namespace MyGame {
//    /// <summary>
//	/// レール上の距離を区間と進行度に変換するクラス
//    /// </summary>
//    class RailPath {
//    public: // メンバ関数
//        /// <summary>
//		/// ベジェ曲線の制御点をセットし、区間長と全体長を計算して保存する
//        /// </summary>
//        /// <param name="curve"></param>
//        void Build(const std::vector<BezierPoint>& curve);
//        /// <summary>
//		/// レール上の距離を区間と進行度に変換する 
//        /// </summary>
//        /// <param name="distance"></param>
//        /// <param name="index"></param>
//        /// <param name="t"></param>
//        void ConvertDistance(float distance, int& index, float& t);
//    private: // 内部関数
//        /// <summary>
//		/// 指定した区間の長さを計算する関数
//        /// </summary>
//        /// <param name="curve"></param>
//        /// <param name="index"></param>
//        /// <returns></returns>
//        float CalculateSegmentLength(const std::vector<BezierPoint>& curve, int index);
//        /// <summary>
//		/// 指定した区間と進行度に基づいて、レール上の位置をサンプリングする関数
//        /// </summary>
//        /// <param name="curve"></param>
//        /// <param name="index"></param>
//        /// <param name="t"></param>
//        /// <returns></returns>
//        MyEngine::Vector3 GetPoint(const std::vector<BezierPoint>& curve, int index, float t);
//	private: // メンバ変数
//        std::vector<float> segmentLengths_;
//        float totalLength_ = 0.0f;
//    public:
//		// getter
//		float GetTotalLength() const { return totalLength_; }
//    };
//}