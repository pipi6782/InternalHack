#include "Global.h"
#include "AimbotMgr.h"
#include "Character.h"

AimbotMgr* AimbotMgr::instance = nullptr;

void AimbotMgr::Create()
{
    if (instance == nullptr)
        instance = new AimbotMgr();
}

AimbotMgr* AimbotMgr::Get()
{
    return instance;
}

void AimbotMgr::Destroy()
{
    SafeDelete(instance);
}

void AimbotMgr::AddEntity(Character* entity)
{
    entities.push_back(entity);
    targetIndex = entities.size() - 1;
    target = entity;
}

void AimbotMgr::TraceEntity(Character* player)
{
    if (target == nullptr) return;

    //설정한 타겟이 이미 죽어있으면 타겟 변경 후 함수 종료
    if (target->health <= 0.0f)
    {
        ChangeTarget(rand() % 2 == 0 ? EChangeType::Prev : EChangeType::Next);
        return;
    }

    Vec3 playerLocation = player->position;
    Vec3 entityLocation = target->position;

    Vec3 rotation = FindRotation(player, target);

    player->rotation = rotation;    
}

Vec3 AimbotMgr::FindRotation(Character* from, Character* to)
{
    Vec3 direction = to->position - from->position;

    Vec3 result;
    float Yaw = atan2f(direction.y , direction.x) * (180.0f / D3DX_PI);
    float Pitch = atan2f((to->position - from->position).z,sqrt(direction.x * direction.x + direction.y * direction.y)) * (180.0f / D3DX_PI);
    result.x = Yaw + 90.0f;
    result.y = Pitch;
    result.z = 0.0f;

    return { result.x,result.y,result.z };
}

void AimbotMgr::ChangeTarget(EChangeType eType)
{
    int iChangeIndex = (int)eType;
    do
    {
        targetIndex += iChangeIndex;
        targetIndex %= entities.size();
        target = entities[targetIndex];
    } while (target->health <= 0);
}
