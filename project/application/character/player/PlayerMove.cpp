#include"PlayerMove.h"
#include <Input.h>

using namespace MyEngine;

namespace MyGame {

    void PlayerMove::Update(MyEngine::Transform& transform, float speedMultiplier) {
		if (Input::GetInstance()->Pushkey(DIK_D)) {
			transform.translate.x += speedMultiplier;
			transform.translate.y += speedMultiplier;
		}
			
		if (Input::GetInstance()->Pushkey(DIK_A)) {
			transform.translate.x -= speedMultiplier;
			transform.translate.y -= speedMultiplier;
		}
    } 
}