#include <Novice.h>
#include <imgui.h>
#include "Vector3Math.h"
#include "MatrixMath.h"
#include "MakeMatrix.h"
#include "Draw.h"

const char kWindowTitle[] = "LE2B_17_ナガイ_コハク_MT3_2_4 3次元の衝突判定(三角形と線)";

bool IsCollision(const Segment& segment, const Triangle& triangle);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//ウィンドウサイズ
	float kWindowWidth = 1280.0f;
	float kWindowHeight = 720.0f;

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };

	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	Segment segment{ {-0.45f,0.33f,0.0f},{0.5f,0.58f,0.0f} };

	Triangle triangle;

	triangle.vertices[0] = { 0.0f,1.0f,0.0f };
	triangle.vertices[1] = { 1.0f,0.0f,0.0f };
	triangle.vertices[2] = { -1.0f,0.0f,0.0f };

	Draw draw;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		if (ImGui::TreeNode("Camera")) {
			ImGui::DragFloat3("Translate", &cameraTranslate.x, 0.01f);
			ImGui::DragFloat3("Rotate", &cameraRotate.x, 0.01f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Segment")) {
			ImGui::DragFloat3("Origin", &segment.origin.x, 0.01f);
			ImGui::DragFloat3("Diff", &segment.diff.x, 0.01f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Triangle")) {
			ImGui::DragFloat3("Vertex0", &triangle.vertices[0].x, 0.01f);
			ImGui::DragFloat3("Vertex1", &triangle.vertices[1].x, 0.01f);
			ImGui::DragFloat3("vertex2", &triangle.vertices[2].x, 0.01f);
			ImGui::TreePop();
		}

		draw.Pipeline(cameraTranslate, cameraRotate, kWindowWidth, kWindowHeight);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		draw.DrawGrid();

		if (IsCollision(segment, triangle)) {
			draw.DrawLine(segment.origin, segment.diff, RED);
		} else {
			draw.DrawLine(segment.origin, segment.diff, WHITE);
		}

		draw.DrawTriangle(triangle, WHITE);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

bool IsCollision(const Segment& segment, const Triangle& triangle) {

	//三角形が含まれる平面を求める
	Vector3 v01 = Vector3Math::Subtract(triangle.vertices[1], triangle.vertices[0]);

	Vector3 v12 = Vector3Math::Subtract(triangle.vertices[2], triangle.vertices[1]);

	Vector3 v20 = Vector3Math::Subtract(triangle.vertices[0], triangle.vertices[2]);

	Vector3 normal = Vector3Math::Normalize(Vector3Math::Cross(v01, v12));

	float distance = Vector3Math::Dot(triangle.vertices[0], normal);

	//法線と線の内積を求める
	float dot = Vector3Math::Dot(normal, segment.diff);

	//垂直 = 並行であるので、衝突しているはずがない
	if (dot == 0.0f) {
		return false;
	}

	//tを求める
	float t = (distance - Vector3Math::Dot(segment.origin, normal)) / dot;

	//tの値と線の種類によって衝突しているかを判断する
	if (t >= 0.0f && t <= 1.0f) {

		Vector3 point = Vector3Math::Add(segment.origin, Vector3Math::Multiply(t, Vector3Math::Add(segment.origin, segment.diff)));

		Vector3 v0p = Vector3Math::Subtract(point, triangle.vertices[0]);
		Vector3 v1p = Vector3Math::Subtract(point, triangle.vertices[1]);
		Vector3 v2p = Vector3Math::Subtract(point, triangle.vertices[2]);

		Vector3 cross01 = Vector3Math::Normalize(Vector3Math::Cross(v01, v1p));
		Vector3 cross12 = Vector3Math::Normalize(Vector3Math::Cross(v12, v2p));
		Vector3 cross20 = Vector3Math::Normalize(Vector3Math::Cross(v20, v0p));

		if (Vector3Math::Dot(cross01, normal) >= 0.0f &&
			Vector3Math::Dot(cross12, normal) >= 0.0f &&
			Vector3Math::Dot(cross20, normal) >= 0.0f) {
			return true;
		}

	}

	return false;

}