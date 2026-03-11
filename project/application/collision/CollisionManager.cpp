//#include "CollisionManager.h"
//#include "Player.h"
//#include "Enemy.h"
//#include "BulletManager.h"
//#include "Collision.h"
//
//using namespace Collision;
//
//void CollisionManager::CheckAllCollisions()
//{
//    CheckBulletEnemy();
//    CheckEnemyBulletPlayer();
//    CheckEnemyPlayer();
//}
//
//void CollisionManager::CheckBulletEnemy()
//{
//    const auto& bullets = BulletManager::GetInstance()->GetPlayerBullets();
//
//    for (const auto& bullet : bullets) {
//
//        if (!bullet->IsActive()) continue;
//
//        for (auto& enemy : *enemies_) {
//
//            if (!enemy->IsActive()) continue;
//
//            if (IsOBBIntersect(bullet->GetOBB(), enemy->GetOBB())) {
//
//                bullet->SetInactive();
//                enemy->OnHit();
//                break;
//            }
//        }
//    }
//}
//
//void CollisionManager::CheckEnemyBulletPlayer()
//{
//    const auto& bullets = BulletManager::GetInstance()->GetEnemyBullets();
//
//    OBB playerOBB = player_->GetOBB();
//
//    for (const auto& bullet : bullets) {
//
//        if (!bullet->IsActive()) continue;
//
//        if (IsOBBIntersect(bullet->GetOBB(), playerOBB)) {
//
//            bullet->SetInactive();
//            player_->SetState(State::Dead);
//        }
//    }
//}
//
//void CollisionManager::CheckEnemyPlayer()
//{
//    OBB playerOBB = player_->GetOBB();
//
//    for (auto& enemy : *enemies_) {
//
//        if (!enemy->IsActive()) continue;
//
//        if (IsOBBIntersect(enemy->GetOBB(), playerOBB)) {
//
//            player_->SetState(State::Dead);
//        }
//    }
//}